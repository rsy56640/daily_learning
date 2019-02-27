#include "include/hash_lru.h"
#include <chrono>

namespace DB::buffer
{

    /****************  implementation for class PageList  ****************/

    PageList::PageList() :size_(0), head_(NOT_A_PAGE, nullptr) {}

    void PageList::append(PageListHandle* handle) {
        size_++;
        handle->ref();
        PageListHandle* old = head_.next_hash_;
        if (old == nullptr) {
            head_.next_hash_ = handle; handle->prev_hash_ = &head_;
        }
        else {
            head_.next_hash_ = handle; handle->prev_hash_ = &head_;
            handle->next_hash_ = old; old->prev_hash_ = handle;
        }
    }

    void PageList::remove(PageListHandle* handle) {
        size_--;
        PageListHandle* prev = handle->prev_hash_;
        PageListHandle* next = handle->next_hash_;
        prev->next_hash_ = next;
        if (next != nullptr) next->prev_hash_ = prev;
        handle->unref();
    }

    Page* PageList::find(page_id_t page_id) const {
        PageListHandle* it = head_.next_hash_;
        while (it != nullptr) {
            if (it->page_id_ == page_id) return it->page_;
            it = it->next_hash_;
        }
        return nullptr;
    }

    PageList::~PageList() {
        PageListHandle* it = head_.next_hash_;
        while (it != nullptr) {
            PageListHandle* temp = it;
            it = it->next_hash_;
            temp->unref();
        }
    }


    /****************  implementation for class Hash_LRU  ****************/

    using namespace std::literals::chrono_literals;

    Hash_LRU::Hash_LRU() :
        bucket_num_(Hash_LRU::init_bucket),
        lru_head_(NOT_A_PAGE, nullptr),
        buckets_(bucket_num_),
        size_(0),
        lru_max_size_(lru_init_size)
    {}

    // TODO
    bool Hash_LRU::insert(page_id_t page_id, Page* page)
    {
        std::shared_lock<std::shared_mutex> lock(shared_mutex_);


        return false;
    }

    // TODO
    bool Hash_LRU::insert_or_assign(page_id_t page_id, Page* page)
    {
        std::shared_lock<std::shared_mutex> lock(shared_mutex_);
        return false;
    }

    // TODO
    std::pair<Page*, bool> Hash_LRU::find(page_id_t page_id) const
    {
        std::shared_lock<std::shared_mutex> lock(shared_mutex_);

        uint32_t _hash = hash(page_id);
        const PageList& page_list = buckets_[_hash];

        Page* _page_ptr = nullptr;
        bool try_lock = false;

        // find the page and try to acquire the lock
        {
            std::lock_guard<std::mutex> lg(page_list.mutex_);

            _page_ptr = page_list.find(page_id);

            // return if no such page.
            if (_page_ptr == nullptr)
                return { nullptr, false };
            // do not return immediately.
            if (_page_ptr->try_lock()) { try_lock = true; };
        }

        // update LRU
        {


        }

        if (try_lock)
            return { _page_ptr, true };
        else
            return { nullptr, true };
    }

    // TODO
    bool Hash_LRU::erase(page_id_t page_id)
    {
        std::shared_lock<std::shared_mutex> lock(shared_mutex_);
        return false;
    }


    uint32_t Hash_LRU::size() const {
        return size_.load();
    }


    /*
     * The read of `bucket_num_` does not contend with modifying it in `rehash()`.
     * Becase the modification must acquire the write-lock,
     * thus any find/insert/erase call will block.
     */
    inline uint32_t Hash_LRU::hash(page_id_t page_id) const noexcept {
        return (Hash_LRU::magic * page_id) & (bucket_num_ - 1);
    }

    // TODO
    void Hash_LRU::rehash()
    {
        // must immediately acquire the write-lock
        std::lock_guard<std::shared_mutex> lg(shared_mutex_);

        if (bucket_num_ >= Hash_LRU::max_bucket)
            return;

    }

} // end namespace DB::buffer