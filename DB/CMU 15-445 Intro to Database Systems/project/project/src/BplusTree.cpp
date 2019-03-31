#include "include/BplusTree.h"
#include "include/disk_manager.h"
#include "include/vm.h"
#include "include/page.h"
#include <cstring>

namespace DB::tree
{

    using namespace ::DB::page;

    BTree::BTree(vm::StorageEngine* storage_engine, key_t_t key_t, uint32_t str_len)
        :storage_engine_(storage_engine), key_t_(key_t), str_len_(str_len),
        root(nullptr), size_(0)
    {
        BT_create();
    }

    BTree::~BTree() {
        root->unref();
    }


    ValueEntry BTree::find(const KeyEntry& kEntry) const
    {
        ValueEntry vEntry;
        BTree::SearchInfo info = search(kEntry);
        if (info.leaf == nullptr || key_compare(info.leaf, info.key_index, kEntry) != 0)
            vEntry.value_state_ = value_state::OBSOLETE;
        else
            info.leaf->read_value(info.key_index, vEntry);
        return vEntry;
    }




    /////////////////////////////// private implementation ///////////////////////////////


    void BTree::BT_create() {
        PageInitInfo info;
        info.page_t = page_t_t::ROOT;
        info.parent = nullptr;
        info.key_t = key_t_;
        info.str_len = str_len_;
        root = allocate_node(info);
        // root has been `ref()`.
    }


    BTree::SearchInfo BTree::search(const KeyEntry&) const {
        SearchInfo info;
        if (size_ == 0) {
            info.leaf = nullptr;
            return info;
        }
        // TODO: BTree::search
        return info;
    }


    BTreePage* BTree::fetch_node(page_id_t page_id) {
        return static_cast<base_ptr>(storage_engine_->buffer_pool_manager_->FetchPage(page_id));
    }


    BTreePage* BTree::allocate_node(PageInitInfo info) {
        BTreePage* page_ptr =
            static_cast<base_ptr>(storage_engine_->buffer_pool_manager_->NewPage(info));
        return page_ptr;
    }



    // return:
    // < 0, if KeyEntry < keys[index]
    // = 0, if KeyEntry = keys[index]
    // > 0, if KeyEntry > keys[index]
    int32_t BTree::key_compare(const base_ptr node, uint32_t key_index, const KeyEntry& kEntry) const
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