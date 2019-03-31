#include "include/BplusTree.h"
#include "include/disk_manager.h"
#include "include/vm.h"
#include "include/page.h"
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




    /////////////////////////////// private implementation ///////////////////////////////


    void BTree::BT_create(OpenTableInfo openTableInfo) {
        if (openTableInfo.isInit)
        {
            PageInitInfo info;
            info.page_t = page_t_t::ROOT;
            info.parent_id = NOT_A_PAGE;
            info.key_t = key_t_;
            info.str_len = str_len_;
            root_ = allocate_node(info);
            root_->set_dirty();
        }
        else
        {
            root_ = fetch_node(openTableInfo.root_id);
        }
        // root has been `ref()`.
    }


    void BTree::search(const KeyEntry& kEntry, SearchInfo& info) const
    {
        info.leaf_id = NOT_A_PAGE;
        if (size_ == 0) return;
        root_->page_read_lock();
        doSearch(root_, kEntry, info);
        root_->page_read_unlock();
    }


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
        base_ptr child = static_cast<base_ptr>(storage_engine_->buffer_pool_manager_
            ->FetchPage(static_cast<link_ptr>(node)->branch_[index]));
        child->page_read_lock();
        doSearch(child, kEntry, info);
        child->page_read_unlock();
        child->unref();
    }


    BTreePage* BTree::fetch_node(page_id_t page_id) {
        return static_cast<base_ptr>(storage_engine_->buffer_pool_manager_->FetchPage(page_id));
    }


    BTreePage* BTree::allocate_node(PageInitInfo info) {
        BTreePage* page_ptr =
            static_cast<base_ptr>(storage_engine_->buffer_pool_manager_->NewPage(info));
        return page_ptr;
    }


    // opeartion:
    //            1. create 2 Internal nodes, L and R
    //            2. move root/branch[0...7] into L, root/branch[8...14] into R
    //            3. move root[7] to root[0], adjust the relation
    void BTree::split_root()
    {
        std::shared_lock<std::shared_mutex> lg(range_query_lock_);
        root_->page_write_lock();

        PageInitInfo info;

        // step 1: create L and R
        info.page_t = page_t_t::INTERNAL;
        info.parent_id = root_->get_page_id();
        info.key_t = key_t_;
        info.str_len = str_len_;
        base_ptr L = allocate_node(info);
        base_ptr R = allocate_node(info);

        // step 2: move root/branch[0...7] into L, root/branch[8...14] into R
        KeyEntry kEntry;

        for (uint32_t index = 0; index <= KEY_MIDEIUM; index++)
        {
            kEntry = root_->read_key(index);
            root_->erase_key(index);
            L->insert_key(index, kEntry);
            static_cast<link_ptr>(L)->branch_[index] = static_cast<link_ptr>(root_)->branch_[index];
        }
        L->nEntry_ = KEY_MIDEIUM + 1;
        L->parent_id_ = root_->get_page_id();
        L->set_dirty();

        for (uint32_t index = KEY_MIDEIUM + 1; index < MAX_KEY_SIZE; index++)
        {
            kEntry = root_->read_key(index);
            root_->erase_key(index);
            R->insert_key(index, kEntry);
            static_cast<link_ptr>(R)->branch_[index] = static_cast<link_ptr>(root_)->branch_[index];
        }
        R->nEntry_ = KEY_MIDEIUM;
        R->parent_id_ = root_->get_page_id();
        R->set_dirty();

        // step 3: move root[7] to root[0], adjust the relation



        root_->nEntry_ = 1;
        for (uint32_t index = 0; index < MAX_KEY_SIZE; index++)
            static_cast<link_ptr>(root_)->branch_[index] = NOT_A_PAGE;
        static_cast<link_ptr>(root_)->branch_[0] = L->get_page_id();

        root_->page_write_unlock();
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