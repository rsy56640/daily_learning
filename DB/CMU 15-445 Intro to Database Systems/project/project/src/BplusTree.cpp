#include "include/BplusTree.h"
#include "include/disk_manager.h"
#include "include/vm.h"
#include "include/page.h"
#include "include/debug_log.h"
#include <cstring>

namespace DB::tree
{

    using namespace ::DB::page;

    // root is always resides in memory.
    // we don't care whether the root is in buffer-pool.
    // No one else may access root Page by using root_id!!!
    BTree::BTree(OpenTableInfo openTableInfo, vm::StorageEngine* storage_engine,
        key_t_t key_t, uint32_t str_len)
        :storage_engine_(storage_engine), buffer_pool_(storage_engine->buffer_pool_manager_),
        key_t_(key_t), str_len_(str_len), root_(nullptr), size_(0)
    {
        BT_create(openTableInfo);
    }

    BTree::~BTree() {
        root_->unref();
    }

    uint32_t BTree::size() const { return size_; }

    ValueEntry BTree::find(const KeyEntry& kEntry) const
    {
        std::shared_lock<std::shared_mutex> lg(range_query_lock_);

        ValueEntry vEntry;
        BTree::SearchInfo info;
        search(kEntry, info);
        leaf_ptr leaf = static_cast<leaf_ptr>(buffer_pool_->FetchPage(info.leaf_id));
        leaf->page_read_lock();
        if (info.leaf_id == NOT_A_PAGE || key_compare(kEntry, leaf, info.key_index) != 0)
            vEntry.value_state_ = value_state::OBSOLETE;
        else
            leaf->read_value(info.key_index, vEntry);
        leaf->page_read_unlock();
        leaf->unref();
        return vEntry;
    }


    // return: `INSERT_NOTHING`, the key exists, do nothing.
    //         `INSERT_KV`,      no such key exists, and insert k-v.
    uint32_t BTree::insert(const KVEntry& kEntry)
    {
        std::shared_lock<std::shared_mutex> lg(range_query_lock_);

        root_->page_write_lock();
        // try to split full root
        if (root_->nEntry_ == MAX_KEY_SIZE)
            split_root();
        // `ROOT_LEAF` insertion
        if (root_->get_page_t() == page_t_t::ROOT_LEAF)
        {
            // TODO: ROOT_LEAF insertion



            root_->page_write_unlock();
            return;
        }
        root_->page_write_unlock();

        root_->page_read_lock();
        // TODO: BTree::insert()


        root_->page_read_unlock();
    }


    /////////////////////////////// private implementation ///////////////////////////////


    // `ROOT_LEAF` when tree is initilized at the first time
    void BTree::BT_create(OpenTableInfo openTableInfo) {
        if (openTableInfo.isInit)
        {
            PageInitInfo info;
            info.page_t = page_t_t::ROOT_LEAF;
            info.parent_id = NOT_A_PAGE;
            info.key_t = key_t_;
            info.str_len = str_len_;
            info.value_page_id = NOT_A_PAGE;
            root_ = allocate_node(info);
            root_->set_dirty();
        }
        else
        {
            root_ = fetch_node(openTableInfo.root_id);
        }
        // root has been `ref()`.
    }


    // do nothing if BTree is empty.
    void BTree::search(const KeyEntry& kEntry, SearchInfo& info) const
    {
        info.leaf_id = NOT_A_PAGE;
        if (size_ == 0) return;
        root_->page_read_lock();
        doSearch(root_, kEntry, info);
        root_->page_read_unlock();
    }


    // return the min leaf.key that *** KeyEntry <= leaf.key ***
    void BTree::doSearch(base_ptr node, const KeyEntry& kEntry, SearchInfo& info) const
    {
        uint32_t index = 0;
        for (; index < node->nEntry_; index++)
            if (key_compare(kEntry, node, index) <= 0)
                break;

        // now (kEntry <= key[index]) or (index == n)
        if (index == node->nEntry_)
            return; // all key < kEntry, so no result.

        // if at Leaf
        if (node->get_page_t() == page_t_t::LEAF)
        {
            if (key_compare(kEntry, node, index) == 0) {
                info.leaf_id = node->get_page_id();
                info.key_index = index;
            }
            return;
        }

        // root is Internal and (kEntry <= key[index])
        // search recursively into branch[index]
        base_ptr child = fetch_node(node, index);
        child->page_read_lock();
        doSearch(child, kEntry, info);
        child->page_read_unlock();
        child->unref();
    }


    BTreePage* BTree::fetch_node(page_id_t page_id) const {
        return static_cast<base_ptr>(storage_engine_->buffer_pool_manager_->FetchPage(page_id));
    }


    BTreePage* BTree::fetch_node(base_ptr node, uint32_t index) const {
        return static_cast<base_ptr>(storage_engine_->buffer_pool_manager_
            ->FetchPage(static_cast<link_ptr>(node)->branch_[index]));
    }


    BTreePage* BTree::allocate_node(PageInitInfo info) const {
        BTreePage* page_ptr =
            static_cast<base_ptr>(storage_engine_->buffer_pool_manager_->NewPage(info));
        return page_ptr;
    }


    // the only way to increase the height.
    // called when root is full.
    // opeartion:
    //          I. root is LEAF
    //              1. create 2 LEAF, L and R
    //              2. move root.k-v[0..7] into L, root.k-v[8..14] into R
    //              3. change root to INTERNAL
    //              4. set root.k[0] = root.k[7], set branch, adjust the relation
    //          II. root is INTERNAL
    //              1. create 2 INTERNAL, L and R
    //              2. 1) move root.k[0..6] + root.br[0..7] into L
    //                 2) move root.k[8..14] + root.br[8..15] into R
    //              3. set root.k[0] = root.k[7], set branch, adjust the relation
    void BTree::split_root()
    {
        // ROOT is full LEAF
        if (root_->page_t_ == page_t_t::ROOT_LEAF)
        {
            // step 1: create 2 LEAF, L and R
            PageInitInfo info;
            info.page_t = page_t_t::LEAF;
            info.parent_id = root_->get_page_id();
            info.key_t = key_t_;
            info.str_len = str_len_;
            info.value_page_id = NOT_A_PAGE;
            info.previous_page_id = NOT_A_PAGE;
            info.next_page_id = NOT_A_PAGE;
            leaf_ptr L = static_cast<leaf_ptr>(allocate_node(info));
            leaf_ptr R = static_cast<leaf_ptr>(allocate_node(info));

            // step 2: move root.k - v[0..7] into L, root.k - v[8..14] into R
            KeyEntry kEntry;
            ValueEntry vEntry;
            for (uint32_t index = 0; index <= KEY_MIDEIUM; index++) // [0..7]
            {
                kEntry = root_->read_key(index);
                static_cast<root_ptr>(root_)->read_value(index, vEntry);
                L->insert_key(index, kEntry);
                L->insert_value(index, vEntry);
                if (index != KEY_MIDEIUM) root_->erase_key(index); // Do not erase key[7]
                static_cast<root_ptr>(root_)->erase_value(index);
            }
            L->nEntry_ = KEY_MIDEIUM + 1;
            L->set_dirty();

            for (uint32_t index = 0; index < KEY_MIDEIUM; index++) // [0..6]
            {
                const uint32_t root_index = index + KEY_MIDEIUM + 1; // [8..14]
                kEntry = root_->read_key(root_index);
                static_cast<root_ptr>(root_)->read_value(root_index, vEntry);
                R->insert_key(index, kEntry);
                R->insert_value(index, vEntry);
                root_->erase_key(root_index);
                static_cast<root_ptr>(root_)->erase_value(root_index);
            }
            R->nEntry_ = KEY_MIDEIUM;
            R->set_dirty();

            L->set_right_leaf(R->get_page_id());
            R->set_left_leaf(L->get_page_id());

            // step 3: change root to INTERNAL
            root_->page_t_ = page_t_t::ROOT_INTERNAL;

            // step 4: set root.k[0] = root.k[7], set branch, adjust the relation
            root_->nEntry_ = 1;
            root_->keys_[0] = root_->keys_[KEY_MIDEIUM];
            static_cast<root_ptr>(root_)->branch_[0] = L->get_page_id();
            static_cast<root_ptr>(root_)->branch_[1] = R->get_page_id();
            root_->set_dirty();

            L->unref();
            R->unref();

        } // end root is LEAF

        // ROOT is full INTERNAL
        else
        {
            // step 1: create L and R
            PageInitInfo info;
            info.page_t = page_t_t::INTERNAL;
            info.parent_id = root_->get_page_id();
            info.key_t = key_t_;
            info.str_len = str_len_;
            link_ptr L = static_cast<link_ptr>(allocate_node(info));
            link_ptr R = static_cast<link_ptr>(allocate_node(info));

            //step 2: 1) move root.k[0..6] + root.br[0..7] into L
            //        2) move root.k[8..14] + root.br[8..15] into R
            KeyEntry kEntry;

            for (uint32_t index = 0; index < KEY_MIDEIUM; index++) // [0..6]
            {
                kEntry = root_->read_key(index);
                root_->erase_key(index);
                L->insert_key(index, kEntry);
                L->branch_[index] = static_cast<root_ptr>(root_)->branch_[index];
            }
            L->branch_[KEY_MIDEIUM] = static_cast<root_ptr>(root_)->branch_[KEY_MIDEIUM];
            L->nEntry_ = KEY_MIDEIUM;
            L->set_dirty();

            for (uint32_t index = 0; index < KEY_MIDEIUM; index++) // [0..6]
            {
                const uint32_t root_index = index + KEY_MIDEIUM + 1; // [8..14]
                kEntry = root_->read_key(root_index);
                root_->erase_key(root_index);
                R->insert_key(index, kEntry);
                R->branch_[index] = static_cast<root_ptr>(root_)->branch_[root_index];
            }
            R->branch_[KEY_MIDEIUM] = static_cast<root_ptr>(root_)->branch_[KEY_MIDEIUM + KEY_MIDEIUM + 1];
            R->nEntry_ = KEY_MIDEIUM;
            R->set_dirty();

            // step 3: set root.k[0] = root.k[7], set branch, adjust the relation
            root_->nEntry_ = 1;
            root_->keys_[0] = root_->keys_[KEY_MIDEIUM]; // set the offset.
            static_cast<root_ptr>(root_)->branch_[0] = L->get_page_id();
            static_cast<root_ptr>(root_)->branch_[1] = R->get_page_id();
            root_->set_dirty();

            L->unref();
            R->unref();

        } // end root is INTERNAL

    } // end function split_root();


    // REQUIREMENT: caller should hold the write-lock of both `node` and `L`.
    // node is non-full parent, split node.branch[index].
    // invoke `split_internal()` or `split_leaf()` on top of the page_t_t of `L`.
    void BTree::split(link_ptr node, uint32_t split_index, base_ptr L) const
    {
        switch (L->get_page_t())
        {
        case page_t_t::INTERNAL:
            split_internal(node, split_index, static_cast<link_ptr>(L));
            return;
        case page_t_t::LEAF:
            split_leaf(node, split_index, static_cast<leaf_ptr>(L));
            return;
        default:
            debug::ERROR_LOG("`BTree::split()` page_t invalid");
            return;
        }
    }


    // node is non-full parent, split node.branch[index].
    // operation:
    //              1. create 1 INTERNAL R, call node.branch[index] as L
    //              2. move L.k[8..14] into R.k[0..6], L.br[8..15] into R.br[0..7]
    //              3. shift node.k/br to right
    //              4. move L.k[7] upto node.k[index], set node.br[index+1] = R
    void BTree::split_internal(link_ptr node, uint32_t split_index, link_ptr L) const
    {
        // step 1: create 1 INTERNAL R, call node.branch[index] as L
        PageInitInfo info;
        info.page_t = page_t_t::INTERNAL;
        info.parent_id = node->get_page_id();
        info.key_t = key_t_;
        info.str_len = str_len_;
        link_ptr R = static_cast<link_ptr>(allocate_node(info));

        // step 2: move L.k[8..14] into R.k[0..6], L.br[8..15] into R.br[0..7]
        KeyEntry kEntry;
        ValueEntry vEntry;
        for (uint32_t R_index = 0; R_index < KEY_MIDEIUM; R_index++) // [0..6]
        {
            const uint32_t L_index = R_index + KEY_MIDEIUM + 1; // [8..14]
            kEntry = L->read_key(L_index);
            R->insert_key(R_index, kEntry);
            L->erase_key(L_index);
            R->branch_[R_index] = L->branch_[L_index];
        }
        R->branch_[KEY_MIDEIUM] = L->branch_[MAX_BRANCH_SIZE - 1];
        L->nEntry_ = KEY_MIDEIUM;
        R->nEntry_ = KEY_MIDEIUM;
        L->set_dirty();
        R->set_dirty();

        // step 3: shift node.k/br to right
        node->nEntry_++;
        for (uint32_t index = node->nEntry_ - 1; index > split_index; index--)
        {
            node->keys_[index] = node->keys_[index - 1];
            node->branch_[index + 1] = node->branch_[index];
        }

        // step 4: move L.k[7] upto node.k[index], set node.br[index+1] = R
        kEntry = L->read_key(KEY_MIDEIUM);
        node->insert_key(split_index, kEntry);
        L->erase_key(KEY_MIDEIUM);
        node->branch_[split_index + 1] = R->get_page_id();
        node->set_dirty();

        R->unref();

    }


    // node is non-full parent, split node.branch[index].
    // operation:
    //              1. create 1 LEAF R, call node.branch[index] as L
    //              2. move L.k-v[8..14] into R.k-v[0..6], adjust relation
    //              3. shift node.k/br to right
    //              4. set node.k[index] = L.k[7], set node.br[index+1] = R
    void BTree::split_leaf(link_ptr node, uint32_t split_index, leaf_ptr L) const
    {
        // step 1: create 1 LEAF R, call node.branch[index] as L
        PageInitInfo info;
        info.page_t = page_t_t::LEAF;
        info.parent_id = node->get_page_id();
        info.key_t = key_t_;
        info.str_len = str_len_;
        info.value_page_id = NOT_A_PAGE;
        info.previous_page_id = NOT_A_PAGE;
        info.next_page_id = NOT_A_PAGE;
        leaf_ptr R = static_cast<leaf_ptr>(allocate_node(info));

        // step 2: move L.k-v[8..14] into R.k-v[0..6], adjust relation
        KeyEntry kEntry;
        ValueEntry vEntry;
        for (uint32_t R_index = 0; R_index < KEY_MIDEIUM; R_index++) // [0..6]
        {
            const uint32_t L_index = R_index + KEY_MIDEIUM + 1; // [8..14]
            kEntry = L->read_key(L_index);
            L->read_value(L_index, vEntry);
            R->insert_key(R_index, kEntry);
            R->insert_value(R_index, vEntry);
            L->erase_key(L_index);
            L->erase_value(L_index);
        }
        L->nEntry_ = KEY_MIDEIUM + 1;
        R->nEntry_ = KEY_MIDEIUM;
        const page_id_t next = L->get_right_leaf();
        L->set_right_leaf(R->get_page_id());
        R->set_left_leaf(L->get_page_id());
        if (next != NOT_A_PAGE) {
            leaf_ptr next_leaf = static_cast<leaf_ptr>(fetch_node(next));
            next_leaf->page_write_lock();
            next_leaf->set_left_leaf(R->get_page_id());
            next_leaf->page_write_unlock();
            next_leaf->unref();
        }
        R->set_right_leaf(next);
        L->set_dirty();
        R->set_dirty();

        // step 3: shift node.k/br to right
        node->nEntry_++;
        for (uint32_t index = node->nEntry_ - 1; index > split_index; index--)
        {
            node->keys_[index] = node->keys_[index - 1];
            node->branch_[index + 1] = node->branch_[index];
        }

        // step 4: set node.k[index] = L.k[7], set node.br[index+1] = R
        kEntry = L->read_key(KEY_MIDEIUM);
        node->insert_key(split_index, kEntry);
        node->branch_[split_index + 1] = R->get_page_id();
        node->set_dirty();

        R->unref();

    }


    // node is non-full, non-root, insert if LEAF, else go down recursively.
    // return: `INSERT_NOTHING`, the key exists, do nothing.
    //         `INSERT_KV`,      no such key exists, and insert k-v.
    uint32_t BTree::INSERT_NONFULL(base_ptr node, const KVEntry& kvEntry, bool hold_node_write_lock)
    {
        if (!hold_node_write_lock)
            node->page_write_lock();

        // insert if node is `LEAF`
        if (node->get_page_t() == page_t_t::LEAF)
        {


        } // end insert into LEAF

        // recursively go down 
        else
        {

        } // end 

        node->page_write_unlock();

    }



    // return:
    // < 0, if KeyEntry < keys[index]
    // = 0, if KeyEntry = keys[index]
    // > 0, if KeyEntry > keys[index]
    int32_t BTree::key_compare(const KeyEntry& kEntry, const base_ptr node, uint32_t key_index) const
    {
        if (key_t_ == key_t_t::INTEGER)
        {
            return kEntry.key_int - node->keys_[key_index];
        }
        else
        {
            const KeyEntry key = node->read_key(key_index);
            return kEntry.key_str.compare(key.key_str);
        }
    }


} // end namespace DB::tree