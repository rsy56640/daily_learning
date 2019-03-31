#ifndef _BPLUSTREE_H
#define _BPLUSTREE_H
#include "page.h"

namespace DB::disk { class DiskManager; }
namespace DB::vm { class StorageEngine; }
namespace DB::tree
{
    using namespace ::DB::page;

    struct KVEntry {
        KeyEntry kEntry;
        ValueEntry vEntry; // tuple's max size is 67B 
    };



    using page::BTdegree;         // = 8, nEntry is [BTdegree - 1, 2*BTdegree -1], [7, 15]
    using page::BTNodeKeySize;    // = (BTdegree << 1) - 1 = 15;
    using page::BTNodeBranchSize; // = BTdegree << 1 = 16;

    // NB: in the B+Tree, the root page should be `ref()` in the whole execution,
    //     while other pages are fetched from buffer-pool whenever used.
    class BTree
    {
    public:
        using Key = uint32_t; // direct value for `INTEGER`, offset for `VARCHAR`
        using Value = char*;
        using base_ptr = BTreePage * ;
        using link_ptr = InternalPage * ;
        using leaf_ptr = LeafPage * ;

    public:
        BTree(vm::StorageEngine* storage_engine, key_t_t, uint32_t str_len = 0);
        ~BTree();
        BTree(const BTree&) = delete;
        BTree(BTree&&) = delete;
        BTree& operator=(const BTree&) = delete;
        BTree& operator=(BTree&&) = delete;

        uint32_t size() const;

        // return state: `OBSOLETE` denotes no such key exists.
        //               `INUSED` ensures the value cotent is 
        ValueEntry find(const KeyEntry&) const;

        // return: 0, no such key exists. 
        //         1, the key exists, and the value has been substitute.
        uint32_t insert(const KVEntry&);

        // return: 0, no such key exists.
        //         1, the key-value has been erased.
        uint32_t erase(const KeyEntry&);

    private:

        void BT_create();

        // return nullptr(leaf) if BTree is empty.
        // return the max key that leaf.key <= KeyEntry.
        struct SearchInfo {
            leaf_ptr leaf; // maybe nullptr
            uint32_t key_index;
        };
        SearchInfo search(const KeyEntry&) const;

        // Page* has been `ref()` before return.
        // `unref()` the Page after use ! ! !
        BTreePage* fetch_node(page_id_t page_id);

        // Page* has been `ref()` before return.
        // `unref()` the Page after use ! ! !
        BTreePage* allocate_node(PageInitInfo);


        // return:
        // < 0, if KeyEntry < keys[index]
        // = 0, if KeyEntry = keys[index]
        // > 0, if KeyEntry > keys[index]
        int32_t key_compare(const base_ptr, uint32_t key_index, const KeyEntry&) const;



    private:
        vm::StorageEngine * storage_engine_;
        const key_t_t key_t_;
        const uint32_t str_len_;
        base_ptr root;
        uint32_t size_;

    }; // end class BTree


} // end namespace DB::tree

#endif // !_BPLUSTREE_H