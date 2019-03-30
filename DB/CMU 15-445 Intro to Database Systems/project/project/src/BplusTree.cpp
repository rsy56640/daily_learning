#include "include/BplusTree.h"
#include "include/disk_manager.h"

namespace DB::tree
{

    BTree::BTree(disk::DiskManager* disk_manager, key_t_t key_t, uint32_t str_len)
        :disk_manager_(disk_manager), key_t_(key_t), str_len_(str_len), root(nullptr)
    {
        BT_create();
    }

    BTree::~BTree() {
        root->unref();
    }



    /////////////////////////////// private implementation ///////////////////////////////

    void BTree::BT_create() {
        root = allocate_node(page_t_t::ROOT, nullptr);
        root->ref();
    }

    BTreePage* BTree::allocate_node(page_t_t page_t, BTreePage* parent) {
        const page_id_t page_id = disk_manager_->AllocatePage();
        BTreePage* page_ptr;
        switch (page_t)
        {
        case page_t_t::ROOT:
        case page_t_t::INTERNAL:
            page_ptr = new InternalPage(page_t, page_id, parent, 0, disk_manager_, key_t_, str_len_, true);
            break;
        case page_t_t::LEAF:
            page_ptr = new LeafPage(page_id, parent, 0, disk_manager_, key_t_, str_len_, true);
        default:
            return nullptr;
        }
        page_ptr->ref();
        return page_ptr;
    }

} // end namespace DB::tree