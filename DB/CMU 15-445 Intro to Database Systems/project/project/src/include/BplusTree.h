#ifndef _BPLUSTREE_H
#define _BPLUSTREE_H
#include "page.h"
#include <atomic>
#include <shared_mutex>

namespace DB::disk { class DiskManager; }
namespace DB::vm { class StorageEngine; }
namespace DB::buffer { class BufferPoolManager; }
namespace DB::tree
{
    using namespace ::DB::page;

    struct KVEntry {
        KeyEntry kEntry;
        ValueEntry vEntry; // tuple's max size is 67B 
    };

    // used for init B+Tree
    struct OpenTableInfo {
        bool isInit;
        page_id_t root_id;
    };

    constexpr uint32_t MAX_KEY_SIZE = BTNodeKeySize;
    constexpr uint32_t MAX_BRANCH_SIZE = BTNodeKeySize;
    constexpr uint32_t MIN_KEY_SIZE = BTdegree - 1;
    constexpr uint32_t KEY_MIDEIUM = MAX_KEY_SIZE >> 1;

    // NB: in the B+Tree, the root page should be `ref()` in the whole execution,
    //     while other pages are fetched from buffer-pool whenever used.
    // NB: 
    //     ROOT is empty => tree s empty.
    //     Internal won't be empty.
    //     LEAF     won't be empty.
    // Internal Page structure:
    //      [branch, key, ..., branch, key]      // no branch after the last key!!!
    //      branch <= key < branch <= key < ...
    //
    //
    class BTree
    {
    public:
        using Key = uint32_t; // direct value for `INTEGER`, offset for `VARCHAR`
        using Value = char*;
        using base_ptr = BTreePage * ;
        using link_ptr = InternalPage * ;
        using leaf_ptr = LeafPage * ;

        static constexpr uint32_t
            INSERT_NOTHING = 0,
            INSERT_KV = 1,
            ERASE_NOTHING = 0,
            ERASE_KV = 1;

    public:
        BTree(OpenTableInfo, vm::StorageEngine* storage_engine, key_t_t, uint32_t str_len = 0);
        ~BTree();
        BTree(const BTree&) = delete;
        BTree(BTree&&) = delete;
        BTree& operator=(const BTree&) = delete;
        BTree& operator=(BTree&&) = delete;

        uint32_t size() const;

        // return state: `OBSOLETE` denotes no such key exists.
        //               `INUSED` ensures the value cotent is 
        ValueEntry find(const KeyEntry&) const;

        // return: `INSERT_NOTHING`, the key exists, do nothing.
        //         `INSERT_KV`,      no such key exists, and insert k-v.
        uint32_t insert(const KVEntry&);

        // return: `ERASE_NOTHING`, no such key exists.
        //         `ERASE_KV`,      the key-value has been erased.
        uint32_t erase(const KeyEntry&);

    private:

        void BT_create(OpenTableInfo);

        // do nothing if BTree is empty.
        // return the min leaf.key that *** KeyEntry <= leaf.key ***
        struct SearchInfo {
            page_id_t leaf_id = NOT_A_PAGE;
            uint32_t key_index = 0;
        };
        void search(const KeyEntry&, SearchInfo&) const;
        // if tree is empty, return the only leaf_id.
        void doSearch(base_ptr root, const KeyEntry&, SearchInfo&) const;

        // Page* has been `ref()` before return.
        // `unref()` the Page after use ! ! !
        BTreePage* fetch_node(page_id_t page_id);

        // Page* has been `ref()` before return.
        // `unref()` the Page after use ! ! !
        BTreePage* allocate_node(PageInitInfo);

        // the only way to increase the height.
        // called when root is full.
        // opeartion:
        //            1. create 2 Internal nodes, L and R
        //            2. move root/branch[0...7] into L, root/branch[8...14] into R
        //            3. move root[7] to root[0], adjust the relation
        void split_root();
        // TODO: split_root



        // return:
        // < 0, if KeyEntry < keys[index]
        // = 0, if KeyEntry = keys[index]
        // > 0, if KeyEntry > keys[index]
        int32_t key_compare(const KeyEntry&, const base_ptr, uint32_t key_index) const;


    private:
        vm::StorageEngine * storage_engine_;
        buffer::BufferPoolManager* buffer_pool_; // this is in StorageEngine, here for convenience
        const key_t_t key_t_;
        const uint32_t str_len_;
        base_ptr root_;
        std::atomic<uint32_t> size_;
        mutable std::shared_mutex range_query_lock_; // hold write-lock when do range query,
                                                     // otherwise hole read-lock.

    }; // end class BTree


} // end namespace DB::tree

#endif // !_BPLUSTREE_H