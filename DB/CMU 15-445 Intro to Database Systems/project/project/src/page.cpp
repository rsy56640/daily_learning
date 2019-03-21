#include "include/page.h"
#include "include/disk_manager.h"
#include <cstring>

namespace DB::page
{
    // TODO: buffer_to_page
    Page* buffer_to_page(const char* buffer, disk::DiskManager* disk_manager) {
        return new Page(page_t_t::free, NOT_A_PAGE, disk_manager);
    }


    Page::Page(page_t_t page_t, page_id_t page_id, disk::DiskManager* disk_manager)
        :
        disk_manager_(disk_manager),
        page_t_(page_t),
        page_id_(page_id),
        ref_count_(0),
        dirty_(false)
    {
        std::memset(data_, 0, PAGE_SIZE);
        // UNDONE: page data??? mark??
    }

    void Page::ref() {
        ref_count_++;
    }

    void Page::unref() {
        if (--ref_count_ == 0) {

        }
        // TODO: page unref -> 0
    }

    page_t_t Page::get_page_t() const noexcept {
        return page_t_;
    }

    page_id_t Page::get_page_id() const noexcept {
        return page_id_;
    }

    char* Page::get_data() noexcept {
        return data_;
    }

    void Page::set_dirty() noexcept {
        dirty_ = true;
    }

    bool Page::is_dirty() noexcept {
        return dirty_;
    }

    bool Page::try_lock_page() {
        return page_mutex_.try_lock();
    }

    Page::~Page() {
        // TODO: destruct Page
        // if(dirty_) flush

    }



} // end namespace DB::page