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

    PageListHandle* PageList::find(page_id_t page_id) const {
        PageListHandle* it = head_.next_hash_;
        while (it != nullptr) {
            if (it->page_id_ == page_id) return it;
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
        buckets_(bucket_num_),
        size_(0),
        lru_head_(NOT_A_PAGE, nullptr),
        lru_size_(0),
        lru_max_size_(lru_init_size)
    {
        lru_head_.next_lru_ = &lru_head_;
        lru_head_.prev_lru_ = &lru_head_;
    }


    bool Hash_LRU::insert(page_id_t page_id, Page* page)
    {
        std::shared_lock<std::shared_mutex> lock(shared_mutex_);

        uint32_t _hash = hash(page_id);
        PageList& page_list = buckets_[_hash];

        PageListHandle* _page_handle = nullptr;
        PageListHandle* newHandle = nullptr;

        {
            std::lock_guard<std::mutex> lg(page_list.mutex_);
            _page_handle = page_list.find(page_id);
            if (_page_handle != nullptr)
                return false;

            newHandle = new PageListHandle(page_id, page);
            page_list.append(newHandle);

            std::lock_guard<std::mutex> lg(lru_mutex_);
            lru_size_++;
            lru_append(newHandle);
        }

        return true;
    }


    bool Hash_LRU::insert_or_assign(page_id_t page_id, Page* page)
    {
        std::shared_lock<std::shared_mutex> lock(shared_mutex_);

        uint32_t _hash = hash(page_id);
        PageList& page_list = buckets_[_hash];

        PageListHandle* _page_handle = nullptr;
        PageListHandle* newHandle = nullptr;

        {
            std::lock_guard<std::mutex> lg(page_list.mutex_);
            _page_handle = page_list.find(page_id);
            if (_page_handle != nullptr)
            {
                _page_handle->page_ = page;
                return false;
            }

            newHandle = new PageListHandle(page_id, page);
            page_list.append(newHandle);

            std::lock_guard<std::mutex> lg(lru_mutex_);
            lru_size_++;
            lru_append(newHandle);
        }

        return true;
    }


    Page* Hash_LRU::find(page_id_t page_id)
    {
        std::shared_lock<std::shared_mutex> lock(shared_mutex_);

        uint32_t _hash = hash(page_id);
        const PageList& page_list = buckets_[_hash];

        PageListHandle* _page_handle = nullptr;

        // find the page and try to acquire the lock
        {
            std::lock_guard<std::mutex> lg(page_list.mutex_);

            _page_handle = page_list.find(page_id);

            // return if no such page.
            if (_page_handle->page_ == nullptr)
                return nullptr;

            _page_handle->page_->ref();

            std::lock_guard<std::mutex> lg(lru_mutex_);
            lru_update(_page_handle);
        }

        return _page_handle->page_;
    }


    bool Hash_LRU::erase(page_id_t page_id)
    {
        std::shared_lock<std::shared_mutex> lock(shared_mutex_);

        uint32_t _hash = hash(page_id);
        PageList& page_list = buckets_[_hash];

        PageListHandle* _page_handle = nullptr;

        // remove handle from page_list
        {
            std::lock_guard<std::mutex> lg(page_list.mutex_);
            _page_handle = page_list.find(page_id);

            if (_page_handle == nullptr)
                return false;

            page_list.remove(_page_handle);

            std::lock_guard<std::mutex> lg(lru_mutex_);

            // If the handle is *evicted* in other thread
            // after we remove it from page_list but before remove it from LRU,
            // we check the whether the handle is in LRU.
            if (_page_handle->in_lru_ == false)
                return true;

            lru_size_--;
            lru_remove(_page_handle);
        }

        return true;
    }


    uint32_t Hash_LRU::size() const {
        return size_.load(std::memory_order_relaxed);
    }


    /*
     * The read of `bucket_num_` does not contend with modifying it in `rehash()`.
     * Becase the modification must acquire the write-lock,
     * thus any find/insert/erase call will block.
     */
    inline uint32_t Hash_LRU::hash(page_id_t page_id) const noexcept {
        return (Hash_LRU::magic * page_id) & (bucket_num_ - 1);
    }

    // TODO: rehash
    /*
     * Do not lock LRU !!!
     *
     */
    void Hash_LRU::rehash()
    {
        // must immediately acquire the write-lock
        std::lock_guard<std::shared_mutex> lg(shared_mutex_);

        if (bucket_num_ >= Hash_LRU::max_bucket)
            return;

    }


    void Hash_LRU::lru_append(PageListHandle* handle)
    {
        handle->ref();
        handle->in_lru_ = true;
        PageListHandle* second = lru_head_.next_lru_;
        lru_head_.next_lru_ = handle; handle->prev_lru_ = &lru_head_;
        handle->next_lru_ = second; second->prev_lru_ = handle;
    }


    void Hash_LRU::lru_update(PageListHandle* handle)
    {
        PageListHandle* prev = handle->prev_lru_;
        PageListHandle* next = handle->next_lru_;

        if (prev == &lru_head_) return;

        prev->next_lru_ = next; next->prev_lru_ = prev;

        PageListHandle* second = lru_head_.next_lru_;

        lru_head_.next_lru_ = handle; handle->prev_lru_ = &lru_head_;
        handle->next_lru_ = second; second->prev_lru_ = handle;
    }


    void Hash_LRU::lru_remove(PageListHandle* handle)
    {
        PageListHandle* prev = handle->prev_lru_;
        PageListHandle* next = handle->next_lru_;
        prev->next_lru_ = next; next->prev_lru_ = prev;
        handle->in_lru_ = false;
        handle->unref();
    }


    void Hash_LRU::lru_evict()
    {
        PageListHandle* victim = lru_head_.prev_lru_;
        PageListHandle* last = victim->prev_lru_;
        lru_head_.prev_lru_ = last; last->next_lru_ = &lru_head_;
        victim->in_lru_ = false;
        victim->unref();
    }


} // end namespace DB::buffer