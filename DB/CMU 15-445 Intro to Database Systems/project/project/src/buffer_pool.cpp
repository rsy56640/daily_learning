#include "include/buffer_pool.h"

namespace DB::buffer
{
    using page::Page;

    BufferPoolManager::BufferPoolManager(
        disk::DiskManager* disk_manager,
        log::LogManager* log_manager)
        :
        disk_manager_(disk_manager),
        log_manager_(log_manager)
    {}


    page::Page* BufferPoolManager::FetchPage(page_id_t page_id)
    {
        Page* page_ptr = hash_lru_.find(page_id);
        if (page_ptr != nullptr) return page_ptr;
        char buffer[page::PAGE_SIZE];
        disk_manager_->ReadPage(page_id, buffer);
        page_ptr = page::buffer_to_page(buffer, disk_manager_);
        // UNDONE: insert failure or race ???
        hash_lru_.insert(page_ptr->get_page_id(), page_ptr);
        page_ptr->ref();
        return page_ptr;
    }


    bool BufferPoolManager::FlushPage(page_id_t page_id) {
        Page* page_ptr = hash_lru_.find(page_id, false); // `false` means no update lru.
        if (page_ptr == nullptr) return false;
        disk_manager_->WritePage(page_ptr->get_page_id(), page_ptr->get_data());
        page_ptr->unref();
        return true;
    }


    Page* BufferPoolManager::NewPage(page_t_t page_t) {
        const page_id_t page_id = disk_manager_->AllocatePage();
        Page* page_ptr = new Page(page_t, page_id, disk_manager_);
        hash_lru_.insert(page_id, page_ptr);
        page_ptr->ref();
        return page_ptr;
    }


    bool BufferPoolManager::DeletePage(page_id_t page_id) {
        return hash_lru_.erase(page_id);
    }


} // end namespace DB::buffer