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


    constexpr uint32_t MIN_KEY_SIZE = BTdegree - 1;         // 7
    constexpr uint32_t MAX_KEY_SIZE = BTNodeKeySize;        // 15
    constexpr uint32_t MIN_BRANCH_SIZE = BTdegree;          // 8
    constexpr uint32_t MAX_BRANCH_SIZE = BTNodeBranchSize;  // 16
    constexpr uint32_t MIN_LEAF_SIZE = MIN_KEY_SIZE;        // 7
    constexpr uint32_t MAX_LEAF_SIZE = MAX_KEY_SIZE;        // 15
    constexpr uint32_t KEY_MIDEIUM = MAX_KEY_SIZE >> 1;     // 7 in [0...6] [7] [8...14]

    // NB: in the B+Tree, the root page should be `ref()` in the whole execution,
    //     while other pages are fetched from buffer-pool whenever used.
    // Internal Page structure:
    //      [branch, key, ..., branch, key, branch]
    //      branch <= key < branch <= key < ... < branch <= key < branch
    class BTree
    {
    public:
        using Key = uint32_t; // direct value for `INTEGER`, offset for `VARCHAR`
        using Value = char*;
        using base_ptr = BTreePage * ;
        using link_ptr = InternalPage * ;
        using leaf_ptr = LeafPage * ;
        using root_ptr = RootPage * ;

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
        void doSearch(base_ptr node, const KeyEntry&, SearchInfo&) const;

        // Page* has been `ref()` before return.
        // `unref()` the Page after use ! ! !
        BTreePage* fetch_node(page_id_t page_id) const;
        BTreePage* fetch_node(base_ptr node, uint32_t index) const;

        // Page* has been `ref()` before return.
        // `unref()` the Page after use ! ! !
        BTreePage* allocate_node(PageInitInfo) const;

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
        void split_root();

        // REQUIREMENT: caller should hold the write-lock of both `node` and `L`.
        // node is non-full parent, split node.branch[index].
        // invoke `split_internal()` or `split_leaf()` on top of the page_t_t of `L`.
        void split(link_ptr node, uint32_t split_index, base_ptr L) const;

        // node is non-full parent, split node.branch[index].
        // node should hold the write lock.
        // operation:
        //              1. create 1 INTERNAL R, call node.branch[index] as L
        //              2. move L.k[8..14] into R.k[0..6], L.br[8..15] into R.br[0..7]
        //              3. shift node.k/br to right
        //              4. move L.k[7] upto node.k[index], set node.br[index+1] = R
        void split_internal(link_ptr node, uint32_t index, link_ptr L) const;

        // node is non-full parent, split node.branch[index].
        // node should hold the write lock.
        // operation:
        //              1. create 1 LEAF R, call node.branch[index] as L
        //              2. move L.k-v[8..14] into R.k-v[0..6], adjust relation
        //              3. shift node.k/br to right
        //              4. set node.k[index] = L.k[7], set node.br[index+1] = R
        void split_leaf(link_ptr node, uint32_t split_index, leaf_ptr L) const;

        // node is non-full, non-root, insert if LEAF, else go down recursively.
        // return: `INSERT_NOTHING`, the key exists, do nothing.
        //         `INSERT_KV`,      no such key exists, and insert k-v.
        uint32_t INSERT_NONFULL(base_ptr node, const KVEntry&, bool hold_node_write_lock);



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