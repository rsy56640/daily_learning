#include "include/buffer_pool.h"
#include "include/debug_log.h"

namespace DB::buffer
{
    using page::Page;

    BufferPoolManager::BufferPoolManager(disk::DiskManager* disk_manager)
        :disk_manager_(disk_manager) {}


    page::Page* BufferPoolManager::FetchPage(page_id_t page_id)
    {
        // UNDONE: there might be UB
        // suppose you fetch a Page, but later evict.
        // you want to lock the Page,
        // however, in other execution, you fetch the Page again.
        // thus data might be corruped!!!
        //
        // HACK: maybe we can use some transcation level technique
        //       to avoid accessing the same page at the same time.
        //       But what if the page is evicted and accessed again in the same txn???
        // TODO: in the worst case, the handling of the same page should be serialized.
        debug::DEBUG_LOG(debug::BUFFER_FETCH, "BufferPoolManager::FetchPage() fetch %d\n", page_id);
        if (page_id == NOT_A_PAGE) return nullptr;
        Page* page_ptr = hash_lru_.find(page_id);
        if (page_ptr != nullptr && page_ptr->get_page_id() > 1000) {
            debug::DEBUG_LOG(debug::BUFFER_FETCH,
                "error: page_id = %d",
                page_ptr->get_page_id());
        }
        if (page_ptr != nullptr) return page_ptr;
        debug::DEBUG_LOG(debug::BUFFER_FETCH,
            "BufferPoolManager::FetchPage() does not fetch, read page %d\n",
            page_id);
        // UNDONE: if invalid page_id
        return nullptr;
        char buffer[page::PAGE_SIZE];
        disk_manager_->ReadPage(page_id, buffer);
        page_ptr = buffer_to_page(buffer);
        hash_lru_.insert(page_ptr->get_page_id(), page_ptr);
        page_ptr->ref();
        return page_ptr;
    }


    bool BufferPoolManager::FlushPage(page_id_t page_id) {
        debug::DEBUG_LOG(debug::BUFFER_FLUSH, "BufferPoolManager::FlushPage() %d\n", page_id);
        Page* page_ptr = hash_lru_.find(page_id, false); // `false` means no update lru.
        if (page_ptr == nullptr) return false;
        disk_manager_->WritePage(page_ptr->get_page_id(), page_ptr->get_data());
        page_ptr->unref();
        return true;
    }


    Page* BufferPoolManager::NewPage(PageInitInfo info)
    {
        const page_id_t page_id = disk_manager_->AllocatePage();

        debug::DEBUG_LOG(debug::BUFFER_NEW,
            "BufferPoolManager::NewPage() page_id = %d, page_t = %s\n",
            page_id, page::page_t_str[static_cast<int>(info.page_t)]);

        Page* page_ptr;
        switch (info.page_t)
        {
        case page_t_t::ROOT_INTERNAL:
        case page_t_t::ROOT_LEAF:
            page_ptr = new RootPage(this, info.page_t, page_id, info.parent_id, 0,
                disk_manager_, info.key_t, info.str_len, info.value_page_id, true);
            if (info.page_t == page_t_t::ROOT_LEAF) {
                static_cast<RootPage*>(page_ptr)->set_left_leaf(NOT_A_PAGE);
                static_cast<RootPage*>(page_ptr)->set_right_leaf(NOT_A_PAGE);
            }
            break;
        case page_t_t::INTERNAL:
            page_ptr = new InternalPage(page_t_t::INTERNAL, page_id, info.parent_id, 0,
                disk_manager_, info.key_t, info.str_len, true);
            break;
        case page_t_t::LEAF:
            page_ptr = new LeafPage(this, page_id, info.parent_id, 0,
                disk_manager_, info.key_t, info.str_len, info.value_page_id, true);
            static_cast<LeafPage*>(page_ptr)->set_left_leaf(NOT_A_PAGE);
            static_cast<LeafPage*>(page_ptr)->set_right_leaf(NOT_A_PAGE);
            break;
        case page_t_t::VALUE:
            page_ptr = new ValuePage(page_id, info.parent_id, 0,
                disk_manager_, true);
            break;
        default:
            page_ptr = nullptr;
            debug::ERROR_LOG("`BufferPoolManager::NewPage()` error");
        }
        hash_lru_.insert(page_id, page_ptr);
        page_ptr->ref();
        return page_ptr;
    }


    bool BufferPoolManager::DeletePage(page_id_t page_id) {
        debug::DEBUG_LOG(debug::BUFFER_DELETE, "BufferPoolManager::DeletePage() %d\n", page_id);
        return hash_lru_.erase(page_id);
    }


    Page* BufferPoolManager::buffer_to_page(const char(&buffer)[page::PAGE_SIZE]) {
        // TODO: BufferPoolManager::buffer_to_page()
        // read page-info metadata
        PageInitInfo info;
        Page* page_ptr = nullptr;

        return page_ptr;
    }


} // end namespace DB::buffer