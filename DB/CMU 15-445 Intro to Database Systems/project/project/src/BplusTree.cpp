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
    uint32_t BTree::insert(const KVEntry& kvEntry)
    {
        std::shared_lock<std::shared_mutex> lg(range_query_lock_);

        root_->page_write_lock();

        // try to split full root
        if (root_->nEntry_ == MAX_KEY_SIZE)
            split_root();

        // find index such that kEntry <= node.k[index]
        uint32_t index = 0;
        for (; index < root_->nEntry_; index++)
            if (key_compare(kvEntry.kEntry, root_, index) <= 0)
                break;

        //
        // `ROOT_LEAF` insertion
        //
        if (root_->get_page_t() == page_t_t::ROOT_LEAF)
        {
            uint32_t insert_return;
            // if equal, return `INSERT_NOTHING`. (note to validate index)
            if (index != root_->nEntry_ && // kEntry > all root.keys, insert!!!
                key_compare(kvEntry.kEntry, root_, index) == 0)
                insert_return = INSERT_NOTHING;

            // move the index to right, then insert(++BT.size), release write-lock.
            else {
                size_++; // increase B+Tree size
                root_ptr leaf = static_cast<root_ptr>(root_);
                leaf->nEntry_++;
                // also valid when B+Tree is empty :)
                for (uint32_t i = leaf->nEntry_ - 1; i > index; i--) {
                    leaf->keys_[i] = leaf->keys_[i - 1];
                    leaf->values_[i] = leaf->values_[i - 1];
                }
                leaf->insert_key(index, kvEntry.kEntry);
                leaf->insert_value(index, kvEntry.vEntry);
                insert_return = INSERT_KV;
                leaf->set_dirty();
            }

            root_->page_write_unlock();
            return insert_return;

        } // end `ROOT_LEAF` insertion.


        //
        // root is INTERNAL
        //
        // hold write-lock of root.br[index]
        base_ptr node = fetch_node(static_cast<root_ptr>(root_)->branch_[index]);
        node->page_write_lock();

        // if need to split root.br[index], then split.
        if (node->nEntry_ == MAX_KEY_SIZE)
            split(static_cast<root_ptr>(root_), index, node);

        // release write-lock of root, then hold the read-lock of root.
        root_->page_write_unlock();
        root_->page_read_lock();

        // recursively go down then release read-lock.
        uint32_t insert_return = INSERT_NONFULL(node, kvEntry);
        root_->page_read_unlock();

        node->unref();
        return insert_return;

    } // end function `BTree::insert(kv)`


      // return: `ERASE_NOTHING`, no such key exists.
      //         `ERASE_KV`,      the key-value has been erased.
      // operation:
      //          I. root is ROOT_LEAF
      //              1. directly delete if exists.
      //          II. root is ROOT_INTERNAL
      //              1.  a. root.nEntry = 1, call it (the founded one) as child.
      //                     1) child is LEAF (remember to release root write-locks)
      //                          1. child.nEntry > MIN_KEY, directly delete. (return)
      //                          2. child.nEntry = MIN_KEY
      //                              1] other-child.nEntry > MIN_KEY
      //                                  if delete child.key successfully,
      //                                  steal 1 k-v from other-child.
      //                              2] other-child.nEntry = MIN_KEY
      //                                  *** This is the only case root -> ROOT_LEAF ***
      //                                  change root to ROOT_LEAF.
      //                                  steal the 2 childs' k-v.
      //                                  delete 2 childs.
      //                     2) child is INTERNAL
      //                          1. child.nEntry > MIN_KEY
      //                              find K_index, recusively go down (from child).
      //                          2. child.nEntry = MIN_KEY
      //                              1] other-child.nEntry > MIN_KEY
      //                                  *steal* one key/branch
      //                              2] other-child.nEntry = MIN_KEY
      //                                  *merge* 2 childs into root.
      //                  b. root.nEntry > 1
      //                     find K_index, recusively go down.
    uint32_t BTree::erase(const KeyEntry& kEntry)
    {
        root_ptr root = static_cast<root_ptr>(root_);
        root->page_write_lock();
        uint32_t erase_return;

        // I.root is ROOT_LEAF
        if (root->get_page_t() == page_t_t::ROOT_LEAF)
        {
            // 1. directly delete if exists.
            uint32_t index = 0;
            for (; index < root->nEntry_; index++)
                if (key_compare(kEntry, root, index) == 0)
                    break;
            if (index == root->nEntry_)
                erase_return = ERASE_NOTHING;
            else
            {
                erase_return = ERASE_KV;
                root->erase_key(index);
                root->erase_value(index);
                root->nEntry_--;
                for (uint32_t i = index; i < root->nEntry_; i++)
                {
                    root->keys_[i] = root->keys_[i + 1];
                    root->values_[i] = root->values_[i + 1];
                }
                root->set_dirty();
            }

            root_->page_write_unlock();

        } // end root is ROOT_LEAF


        // II.root is ROOT_INTERNAL
        else
        {
            // 1.  a. root.nEntry = 1, call it (the founded one) as child.
            if (root->nEntry_ == 1)
            {
                bool child_L = true;
                base_ptr child, other_child;
                if (key_compare(kEntry, root, 0) <= 0) {
                    child = fetch_node(root->branch_[0]);
                    other_child = fetch_node(root->branch_[1]);
                }
                else {
                    child_L = false;
                    other_child = fetch_node(root->branch_[0]);
                    child = fetch_node(root->branch_[1]);
                }
                child->page_write_lock();


                // 1) child is LEAF (remember to release root write-locks)
                if (child->get_page_t() == page_t_t::LEAF)
                {
                    leaf_ptr child_leaf = static_cast<leaf_ptr>(child);
                    leaf_ptr other_child_leaf = static_cast<leaf_ptr>(other_child);

                    // 1. child.nEntry > MIN_KEY, directly delete. (return)
                    if (child_leaf->nEntry_ > MIN_KEY_SIZE)
                    {
                        uint32_t K_index = 0;
                        for (; K_index < child_leaf->nEntry_; K_index++)
                            if (key_compare(kEntry, child_leaf, K_index) == 0)
                                break;
                        if (K_index == child_leaf->nEntry_)
                            erase_return = ERASE_NOTHING;
                        else
                        {
                            erase_return = ERASE_KV;
                            child_leaf->erase_key(K_index);
                            child_leaf->erase_value(K_index);
                            child_leaf->nEntry_--;
                            for (uint32_t i = K_index; i < child_leaf->nEntry_; i++)
                            {
                                child_leaf->keys_[i] = child_leaf->keys_[i + 1];
                                child_leaf->values_[i] = child_leaf->values_[i + 1];
                            }
                            child_leaf->set_dirty();
                        }
                    } // end child.nEntry > MIN_KEY, directly delete. (return)


                    // 2. child.nEntry = MIN_KEY
                    else
                    {
                        other_child->page_write_lock();

                        // 1] other-child.nEntry > MIN_KEY
                        if (other_child_leaf->nEntry_ > MIN_KEY_SIZE)
                        {
                            // if delete child.key successfully, steal 1 k-v from other-child.
                            uint32_t K_index = 0;
                            for (; K_index < child_leaf->nEntry_; K_index++)
                                if (key_compare(kEntry, child_leaf, K_index) == 0)
                                    break;
                            if (K_index == child_leaf->nEntry_)
                                erase_return = ERASE_NOTHING;
                            else // successfully, steal 1 k-v from other-child.
                            {
                                erase_return = ERASE_KV;
                                child_leaf->erase_key(K_index);
                                child_leaf->erase_value(K_index);
                                child_leaf->nEntry_--;

                                // steal from right.k-v[0]
                                if (child_L)
                                {
                                    // shift left
                                    for (uint32_t i = K_index; i < child_leaf->nEntry_; i++)
                                    {
                                        child_leaf->keys_[i] = child_leaf->keys_[i + 1];
                                        child_leaf->values_[i] = child_leaf->values_[i + 1];
                                    }

                                    // steal right.k-v[0]
                                    KeyEntry kEntry_steal = other_child_leaf->read_key(0);
                                    ValueEntry vEntry_steal;
                                    other_child_leaf->read_value(0, vEntry_steal);
                                    child_leaf->insert_key(child_leaf->nEntry_, kEntry_steal);
                                    child_leaf->insert_value(child_leaf->nEntry_, vEntry_steal);
                                    child_leaf->nEntry_++;

                                    // shift left
                                    other_child_leaf->erase_key(0);
                                    other_child_leaf->erase_value(0);
                                    other_child_leaf->nEntry_--;
                                    for (uint32_t i = 0; i < other_child_leaf->nEntry_; i++)
                                    {
                                        other_child_leaf->keys_[i] = other_child_leaf->keys_[i + 1];
                                        other_child_leaf->values_[i] = other_child_leaf->values_[i + 1];
                                    }

                                } // end steal from right.k-v[0]

                                // steal from left.k-v[nEntry-1]
                                else
                                {
                                    // shift right
                                    for (uint32_t i = K_index; i > 0; i--)
                                    {
                                        child_leaf->keys_[i] = child_leaf->keys_[i - 1];
                                        child_leaf->values_[i] = child_leaf->values_[i - 1];
                                    }

                                    // steal left.k-v[nEntry-1]
                                    const uint32_t L_nEntry = other_child_leaf->nEntry_;
                                    KeyEntry kEntry_steal = other_child_leaf->read_key(L_nEntry - 1);
                                    ValueEntry vEntry_steal;
                                    other_child_leaf->read_value(L_nEntry - 1, vEntry_steal);
                                    child_leaf->insert_key(0, kEntry_steal);
                                    child_leaf->insert_value(0, vEntry_steal);
                                    child_leaf->nEntry_++;

                                    // shift left
                                    other_child_leaf->erase_key(L_nEntry - 1);
                                    other_child_leaf->erase_value(L_nEntry - 1);
                                    other_child_leaf->nEntry_--;

                                } // end steal from left.k-v[nEntry-1]

                                other_child_leaf->set_dirty();
                                child_leaf->set_dirty();

                            } // end steal 1 k-v from other-child

                        } // end other-child.nEntry > MIN_KEY


                        // 2] other-child.nEntry = MIN_KEY
                        else
                        {
                            // *** This is the only case root -> ROOT_LEAF ***
                            // change root to ROOT_LEAF.
                            // steal the 2 childs' k-v.
                            // delete 2 childs.
                            leaf_ptr L, R;
                            if (child_L) {
                                L = child_leaf;
                                R = other_child_leaf;
                            }
                            else {
                                L = other_child_leaf;
                                R = child_leaf;
                            }

                            root->page_t_ = page_t_t::ROOT_LEAF;

                            // delete root.k[0]
                            root->erase_key(0);

                            // L.k-v[0..6] to root.k-v[0..6], R.k-v[0..6] to root.k-v[7-13]
                            KeyEntry kEntry;
                            ValueEntry vEntry;
                            for (uint32_t index = 0; index < MIN_KEY_SIZE; index++)
                            {
                                kEntry = L->read_key(index);
                                L->read_value(index, vEntry);
                                root->insert_key(index, kEntry);
                                root->insert_value(index, vEntry);
                                L->erase_key(index);    // maybe ununsed
                                L->erase_value(index);  // maybe ununsed

                                kEntry = R->read_key(index);
                                R->read_value(index, vEntry);
                                root->insert_key(index + MIN_KEY_SIZE, kEntry);
                                root->insert_value(index + MIN_KEY_SIZE, vEntry);
                                R->erase_key(index);    // maybe ununsed
                                R->erase_value(index);  // maybe ununsed
                            }

                            root->nEntry_ = MIN_KEY_SIZE << 1;

                            // directly delete
                            uint32_t K_index = 0;
                            for (; K_index < root->nEntry_; K_index++)
                                if (key_compare(kEntry, root, K_index) == 0)
                                    break;
                            if (K_index == root->nEntry_)
                                erase_return = ERASE_NOTHING;
                            else
                            {
                                erase_return = ERASE_KV;
                                root->erase_key(K_index);
                                root->erase_value(K_index);
                                root->nEntry_--;
                                for (uint32_t i = K_index; i < root->nEntry_; i++)
                                {
                                    root->keys_[i] = root->keys_[i + 1];
                                    root->values_[i] = root->values_[i + 1];
                                }
                            }

                            // UNDONE: how to delete the page??? GC ???
                            L->set_dirty();
                            R->set_dirty();

                        } // end other-child.nEntry = MIN_KEY (aka. merge to ROOT_LEAF)

                        other_child->page_write_unlock();

                    } // end child.nEntry = MIN_KEY

                    child->page_write_unlock();
                    root_->page_write_unlock();

                } // end child is LEAF (remember to release root write-locks)


                // 2) child is INTERNAL
                else
                {
                    link_ptr child_link = static_cast<link_ptr>(child);
                    link_ptr other_child_link = static_cast<link_ptr>(other_child);

                    // 1. child.nEntry > MIN_KEY
                    if (child_link->nEntry_ > MIN_KEY_SIZE)
                    {
                        // find K_index, recusively go down (from child).
                        uint32_t K_index = 0;
                        for (; K_index < child_link->nEntry_; K_index++)
                            if (key_compare(kEntry, child_link, K_index) <= 0)
                                break;
                        base_ptr child_child = fetch_node(child_link->branch_[K_index]);
                        erase_return = ERASE_NONMIN(child, K_index, child_child, kEntry);
                        child_child->unref();

                    } // end child.nEntry > MIN_KEY


                    // 2. child.nEntry = MIN_KEY
                    else
                    {
                        other_child_link->page_write_lock();

                        // 1] other-child.nEntry > MIN_KEY
                        if (other_child_link->nEntry_ > MIN_KEY_SIZE)
                        {
                            // *steal* one key/branch

                            // child is left, steal right.k-br[0]
                            if (child_L)
                            {
                                link_ptr L = child_link, R = other_child_link;

                                KeyEntry kEntry_steal = R->read_key(0);
                                L->insert_key(MIN_KEY_SIZE, kEntry_steal);
                                L->nEntry_++;
                                L->branch_[L->nEntry_] = R->branch_[0];

                                // shift left
                                R->erase_key(0);
                                R->nEntry_--;
                                for (uint32_t i = 0; i < R->nEntry_; i++)
                                {
                                    R->keys_[i] = R->keys_[i + 1];
                                    R->branch_[i] = R->branch_[i + 1];
                                }
                                R->branch_[R->nEntry_] = R->branch_[R->nEntry_ + 1];

                            } // end child is left, steal right.k-br[0]

                            // child is right, steal left.k[nEntry-1], br[nEntry]
                            else
                            {
                                link_ptr L = other_child_link, R = child_link;

                                // shift right
                                R->nEntry_++;
                                R->branch_[R->nEntry_] = R->branch_[R->nEntry_ - 1];
                                for (uint32_t i = R->nEntry_ - 1; i > 0; i--)
                                {
                                    R->keys_[i] = R->keys_[i - 1];
                                    R->branch_[i] = R->branch_[i - 1];
                                }

                                // steal left.k[nEntry-1], br[nEntry]
                                KeyEntry kEntry_steal = L->read_key(L->nEntry_ - 1);
                                R->insert_key(0, kEntry_steal);
                                R->branch_[0] = L->branch_[L->nEntry_];
                                L->erase_key(L->nEntry_ - 1);
                                L->nEntry_--;

                                L->set_dirty();
                                R->set_dirty();

                            } // end child is right, steal left.k[nEntry-1], br[nEntry]

                            // hold child write-lock
                            other_child->page_write_unlock();

                            // find K_index, recusively go down.
                            uint32_t K_index = 0;
                            for (; K_index < child_link->nEntry_; K_index++)
                                if (key_compare(kEntry, child_link, K_index) <= 0)
                                    break;
                            base_ptr child_child = fetch_node(child_link->branch_[K_index]);
                            erase_return = ERASE_NONMIN(child, K_index, child_child, kEntry);
                            child_child->unref();

                        } // end other-child.nEntry > MIN_KEY


                        // 2] other-child.nEntry = MIN_KEY
                        else
                        {
                            // *merge* 2 childs into root.
                            link_ptr L, R;
                            if (child_L) {
                                L = child_link;
                                R = other_child_link;
                            }
                            else {
                                L = other_child_link;
                                R = child_link;
                            }

                            // move root.k[0] to root.k[7]
                            root_->keys_[KEY_MIDEIUM] = root->keys_[0];

                            // move L.k[0..6] into root.k[0..6], L.br[0..7] into root.br[0..7]
                            KeyEntry keyEntry_merge;
                            for (uint32_t i = 0; i < KEY_MIDEIUM; i++)
                            {
                                keyEntry_merge = L->read_key(i);
                                root->insert_key(i, keyEntry_merge);
                                L->erase_key(i);
                                root->branch_[i] = L->branch_[i];
                            }
                            root->branch_[KEY_MIDEIUM] = L->branch_[KEY_MIDEIUM];

                            // move R.k[0..6] into root.k[8..14], R.br[0..7] into root.br[8..15]
                            for (uint32_t i = 0; i < KEY_MIDEIUM; i++)
                            {
                                keyEntry_merge = R->read_key(i);
                                root->insert_key(i + KEY_MIDEIUM + 1, keyEntry_merge);
                                R->erase_key(i);
                                root->branch_[i + KEY_MIDEIUM + 1] = R->branch_[i];
                            }
                            root->branch_[MAX_KEY_SIZE] = R->branch_[MAX_KEY_SIZE];

                            root->nEntry_ = MAX_KEY_SIZE;

                            // how to delete ???
                            child->page_write_unlock();
                            other_child->page_write_unlock();

                            // find K_index, recusively go down.
                            uint32_t K_index = 0;
                            for (; K_index < root->nEntry_; K_index++)
                                if (key_compare(kEntry, root, K_index) <= 0)
                                    break;
                            base_ptr root_child = fetch_node(root->branch_[K_index]);
                            // hold root write-lock
                            erase_return = ERASE_NONMIN(root, K_index, root_child, kEntry);
                            root_child->unref();

                        } // end other-child.nEntry = MIN_KEY


                    } // end child.nEntry = MIN_KEY

                } // end child is INTERNAL

                child->unref();
                other_child->unref();

            } // end root.nEntry = 1, call it (the founded one) as child.

            // b. root.nEntry > 1
            else
            {
                // find K_index, recusively go down.
                uint32_t K_index = 0;
                for (; K_index < root->nEntry_; K_index++)
                    if (key_compare(kEntry, root, K_index) <= 0)
                        break;
                base_ptr root_child = fetch_node(root->branch_[K_index]);
                // hold root write-lock
                erase_return = ERASE_NONMIN(root, K_index, root_child, kEntry);
                root_child->unref();
            }

        } // end root is ROOT_INTERNAL

        return erase_return;

    } // end function `BTree::erase(k)`


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


    // REQUIREMENT: caller should hold the write-lock of node, callee unlock.
    //              node is non-full, non-root, insert if LEAF, else go down recursively.
    // return: `INSERT_NOTHING`, the key exists, do nothing.
    //         `INSERT_KV`,      no such key exists, and insert k-v.
    // operation:
    //          I:  node is LEAF
    //              1. find index such that kEntry <= node.k[index]
    //              2. if equal, return `INSERT_NOTHING`. (note to validate index)
    //              3. move the index to right, then insert(++BT.size), release write-lock.
    //          II:  node is INTERNAL
    //              1. find index such that kEntry <= node.k[index]
    //              2. hold write-lock of node.br[index]
    //                 if need to split node.br[index], then split.
    //              3. release write-lock of node, then hold the read-lock of node.
    //              4. recursively go down then release read-lock.
    uint32_t BTree::INSERT_NONFULL(base_ptr node, const KVEntry& kvEntry)
    {
        // step 1: find index such that kEntry <= node.k[index]
        uint32_t index = 0;
        for (; index < node->nEntry_; index++)
            if (key_compare(kvEntry.kEntry, node, index) <= 0)
                break;

        uint32_t insert_return;

        // node is `LEAF`, insert and return.
        if (node->get_page_t() == page_t_t::LEAF)
        {
            // step 2: if equal, return `INSERT_NOTHING`. (note to validate index)
            if (index != node->nEntry_ && // kEntry > all node.keys, insert!!!
                key_compare(kvEntry.kEntry, node, index) == 0)
                insert_return = INSERT_NOTHING;

            // step 3: move the index to right, then insert(++BT.size), release write-lock.
            else {
                size_++; // increase B+Tree size
                leaf_ptr leaf = static_cast<leaf_ptr>(node);
                leaf->nEntry_++;
                // also valid when index is out of range :), since `index == leaf->nEntry_-1`
                for (uint32_t i = leaf->nEntry_ - 1; i > index; i--) {
                    leaf->keys_[i] = leaf->keys_[i - 1];
                    leaf->values_[i] = leaf->values_[i - 1];
                }
                leaf->insert_key(index, kvEntry.kEntry);
                leaf->insert_value(index, kvEntry.vEntry);
                insert_return = INSERT_KV;
                leaf->set_dirty();
            }

            node->page_write_unlock();

        } // end insert into LEAF


        // node is `INTERNAL`, recursively go down.
        else
        {
            // step 2: hold write-lock of node.br[index]
            //         if need to split node.br[index], then split.
            base_ptr child = fetch_node(static_cast<link_ptr>(node)->branch_[index]);
            child->page_write_lock();
            if (child->nEntry_ == MAX_KEY_SIZE)
                split(static_cast<link_ptr>(node), index, child);

            // step 3: release write-lock of node, then hold the read-lock of node.
            node->page_write_unlock();
            node->page_read_lock();

            // step 4: recursively go down then release read-lock.
            insert_return = INSERT_NONFULL(child, kvEntry);
            node->page_read_unlock();

            child->unref();

        } // end recursively go down

        return insert_return;

    }




    uint32_t BTree::ERASE_NONMIN(base_ptr node, uint32_t index, base_ptr child, const KeyEntry& kEntry)
    {



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