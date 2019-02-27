#include "include/page.h"

namespace DB::page
{

    Page::Page(page_t_t page_t, page_id_t page_id)
        :page_t_(page_t), page_id_(page_id), ref_count_(0), dirty_(false) {}

    void Page::ref() {
        std::lock_guard<std::mutex> lg(mutex_);
        ref_count_++;
    }

    void Page::unref() {
        std::lock_guard<std::mutex> lg(mutex_);
        --ref_count_ == 0;
        // TODO: page unref -> 0
    }

    bool Page::try_lock() {
        return mutex_.try_lock();
    }

    Page::~Page() {
        // TODO
        // if(dirty_) flush

    }



} // end namespace DB::page