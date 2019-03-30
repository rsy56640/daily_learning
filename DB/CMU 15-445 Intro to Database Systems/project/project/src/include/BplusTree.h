#ifndef _BPLUSTREE_H
#include "page.h"

namespace DB::disk { class DiskManager; }
namespace DB::tree
{
    using namespace page;

    class BTree
    {
    public:
        using Key = uint32_t; // direct value for `INTEGER`, offset for `VARCHAR`
        using Value = char*;
        using base_ptr = BTreePage * ;
        using link_type = InternalPage * ;
        using leaf_type = LeafPage * ;

    public:
        BTree(disk::DiskManager*, key_t_t, uint32_t str_len = 0);
        ~BTree();
        BTree(const BTree&) = delete;
        BTree(BTree&&) = delete;
        BTree& operator=(const BTree&) = delete;
        BTree& operator=(BTree&&) = delete;





    private:

        void BT_create();

        // Page* has been `ref()` before return.
        BTreePage * allocate_node(page_t_t);

        bool key_comp(uint32_t key1, uint32_t key2); // pageid ??

    private:
        disk::DiskManager * disk_manager_;
        const key_t_t key_t_;
        const uint32_t str_len_;
        base_ptr root;

    }; // end class BTree


} // end namespace DB::tree

#endif // !_BPLUSTREE_H