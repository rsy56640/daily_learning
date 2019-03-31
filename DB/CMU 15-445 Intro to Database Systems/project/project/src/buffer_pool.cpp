#include "include/buffer_pool.h"
#include "include/debug_log.h"

namespace DB::buffer
{
    using page::Page;

    BufferPoolManager::BufferPoolManager(disk::DiskManager* disk_manager)
        :disk_manager_(disk_manager) {}


    page::Page* BufferPoolManager::FetchPage(page_id_t page_id)
    {
        Page* page_ptr = hash_lru_.find(page_id);
        if (page_ptr != nullptr) return page_ptr;
        char buffer[page::PAGE_SIZE];
        disk_manager_->ReadPage(page_id, buffer);
        page_ptr = buffer_to_page(buffer);
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


    Page* BufferPoolManager::NewPage(PageInitInfo info) {
        const page_id_t page_id = disk_manager_->AllocatePage();
        Page* page_ptr;
        switch (info.page_t)
        {
        case page_t_t::ROOT:
        case page_t_t::INTERNAL:
            page_ptr = new InternalPage(info.page_t, page_id, info.parent, 0,
                disk_manager_, info.key_t, info.str_len, true);
            break;
        case page_t_t::LEAF:
            page_ptr = new LeafPage(this, page_id, info.parent, 0,
                disk_manager_, info.key_t, info.str_len, true);
            break;
        case page_t_t::VALUE:
            page_ptr = new ValuePage(page_id, info.parent, 0,
                disk_manager_, true);
            break;
        default:
            page_ptr = nullptr;
            debug::DEBUG_LOG("`BufferPoolManager::NewPage()` error");
        }
        hash_lru_.insert(page_id, page_ptr);
        page_ptr->ref();
        return page_ptr;
    }


    bool BufferPoolManager::DeletePage(page_id_t page_id) {
        return hash_lru_.erase(page_id);
    }


    Page* BufferPoolManager::buffer_to_page(const char(&buffer)[page::PAGE_SIZE]) {
        // TODO: BufferPoolManager::buffer_to_page()
        // read page-info metadata
        return nullptr;
    }


} // end namespace DB::buffer