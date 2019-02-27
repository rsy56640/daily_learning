#ifndef _HASH_TABLE_H
#include "page.h"
#include <deque>
#include <list>
#include <iterator>
#include <memory>
#include <utility>
#include <mutex>
#include <shared_mutex>
#include <atomic>

namespace DB::buffer
{
    using namespace page;

    struct PageListHandle {
        page_id_t page_id_;
        Page* page_;
        uint32_t ref_ = 0;              // TODO: thread-safe ???
        PageListHandle* prev_hash_ = nullptr;
        PageListHandle* next_hash_ = nullptr;
        PageListHandle* prev_ = nullptr;
        PageListHandle* next_ = nullptr;
        PageListHandle(page_id_t page_id, Page* page) :page_id_(page_id), page_(page) {}
        void ref() { page_->ref(); ref_++; }
        void unref() { page_->unref(); if (--ref_ == 0) delete this; }
    };

    struct PageList {
        mutable std::mutex mutex_;
        PageListHandle head_;             // dummy node
        uint32_t size_;
        PageList();
        void append(PageListHandle*);
        void remove(PageListHandle*);
        Page* find(page_id_t page_id) const;
        ~PageList();
    };


    /*
     * key   :  page_id
     * value :  Page*
     *
     * The implementation is thread safe.
     */
     // TODO: who should evict the page that has 0 ref?
    class Hash_LRU
    {
        // hash function = (magic * key) & (bucket_num_ - 1);
        static constexpr uint32_t init_bucket = 1 << 5;
        static constexpr uint32_t magic = 769;

        static constexpr uint32_t max_bucket = 1 << 10;
        static constexpr uint32_t  lru_init_size = 1 << 7;

        using bucket_it = std::list<Page*>::iterator;

    public:

        Hash_LRU();

        // return false if the key does exist.
        bool insert(page_id_t, Page*);

        // return false if the key does exist, 
        // after this call, the corresponding value of that page_id must be the Page* given.
        bool insert_or_assign(page_id_t, Page*);

        // bool  : whether the page exist.
        // return the corresponding Page if 
        //     1. the key does exist.
        //     2. the mutex is acquired by this thread.
        // return { nullptr, true } if 
        //     the Page does exist but the lock is acquired by other thread.
        std::pair<Page*, bool> find(page_id_t) const;

        // return true on success,
        // after this call, the hashtable does not contain such key.
        bool erase(page_id_t);

        // return approximate size.
        uint32_t size() const;


    private:
        uint32_t bucket_num_;
        std::deque<PageList> buckets_;
        PageListHandle lru_head_;                   // dummy node
        std::atomic<uint32_t> size_;
        std::atomic<uint32_t> lru_max_size_;
        mutable std::shared_mutex shared_mutex_;    // mutex for rehashing
        std::list<std::pair<page_id_t, bucket_it>> lru_cache_;
        std::mutex lru_mutex_;

        uint32_t hash(page_id_t) const noexcept;

        // `bucket_num_` and `lru_max_size_` will be multiplied by 2.
        // the hash table will be locked during rehashing.
        void rehash();



    }; // end class Hash_LRU

} // end namespace DB::buffer

#endif // !_HASH_TABLE_H
