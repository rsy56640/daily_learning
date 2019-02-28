#include "include/page.h"

namespace DB::page
{

    Page::Page(page_t_t page_t, page_id_t page_id)
        :page_t_(page_t), page_id_(page_id), ref_count_(0), dirty_(false) {}

    void Page::ref() {
        ref_count_++;
    }

    void Page::unref() {
        --ref_count_ == 0;
        // UNDONE: page unref -> 0
    }

    bool Page::try_lock_page() {
        return page_mutex_.try_lock();
    }

    Page::~Page() {
        // TODO: destruct Page
        // if(dirty_) flush

    }



} // end namespace DB::page