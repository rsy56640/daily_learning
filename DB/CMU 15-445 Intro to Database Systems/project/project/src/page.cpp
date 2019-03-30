#include "include/page.h"
#include "include/disk_manager.h"
#include <cstring>

namespace DB::page
{
    // TODO: buffer_to_page
    Page* buffer_to_page(const char* buffer, disk::DiskManager* disk_manager) {
        //return new Page(page_t_t::FREE, NOT_A_PAGE, disk_manager);
        return nullptr;
    }

    // TODO: all ctor/dtor/update_data !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Page::Page(page_t_t page_t, page_id_t page_id, BTreePage* parent, uint32_t nEntry,
        disk::DiskManager* disk_manager, bool isInit)
        :
        disk_manager_(disk_manager),
        page_t_(page_t),
        page_id_(page_id),
        parent_(parent),
        nEntry_(nEntry),
        ref_count_(0),
        dirty_(false),
        discarded_(false)
    {
        parent_->ref(); // !!!
        disk_manager_->ReadPage(page_id_, data_);
        if (!isInit) {
            write_int(data_ + offset::PAGE_T, static_cast<uint32_t>(page_t_));
            write_int(data_ + offset::PAGE_ID, page_id_);
            write_int(data_ + offset::PARENT_PAGE_ID, parent_->get_page_id());
            write_int(data_ + offset::NENTRY, nEntry_);
            dirty_ = true;
        }
    }

    Page::~Page() {
        parent_->unref();
    }

    void Page::ref() {
        ref_count_++;
    }

    void Page::unref() {
        if (--ref_count_ == 0) {
            if (dirty_) {
                this->update_data();
                flush();
            }
        }
        delete this;
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

    void Page::flush() {
        if (dirty_)
            disk_manager_->WritePage(page_id_, data_);
        dirty_ = false;
    }

    bool Page::try_page_read_lock() {
        return rw_page_mutex_.try_lock_shared();
    }
    bool Page::try_page_write_lock() {
        return rw_page_mutex_.try_lock();
    }
    void Page::page_read_unlock() {
        rw_page_mutex_.unlock_shared();
    }
    void Page::page_write_unlock() {
        rw_page_mutex_.unlock();
    }


    //////////////////////////////////////////////////////////////////////
    //////////////////////           util           //////////////////////
    //////////////////////////////////////////////////////////////////////

    uint32_t read_int(const char* data) {
        // This prevents buf[i] from being promoted to a signed int.
        uint32_t
            u0 = static_cast<unsigned char>(data[0]),
            u1 = static_cast<unsigned char>(data[1]),
            u2 = static_cast<unsigned char>(data[2]),
            u3 = static_cast<unsigned char>(data[3]);
        uint32_t uval = u0 | (u1 << 8) | (u2 << 16) | (u3 << 24);
        return uval;
    }

    void write_int(char* data, uint32_t value) {
        data[0] = value;
        data[1] = value >> 8;
        data[2] = value >> 16;
        data[3] = value >> 24;
    }

    //////////////////////////////////////////////////////////////////////
    //////////////////////       B+ Tree Page       //////////////////////
    //////////////////////////////////////////////////////////////////////

    //
    // BTree Page
    //
    BTreePage::BTreePage(page_t_t page_t, page_id_t page_id, BTreePage* parent, uint32_t nEntry,
        disk::DiskManager* disk_manager, key_t_t key_t, uint32_t str_len, bool isInit)
        :Page(page_t, page_id, parent, nEntry, disk_manager, isInit), key_t_(key_t), str_len_(str_len)
    {
        keys_ = new uint32_t[(BTdegree << 1) - 1];
        if (!isInit) {
            write_int(data_ + offset::KEY_T, static_cast<uint32_t>(key_t_));
            write_int(data_ + offset::STR_LEN, str_len_);
        }
    }

    BTreePage::~BTreePage()
    {
        delete[] keys_;
    }


    //
    // InternalPage
    //
    InternalPage::InternalPage(page_t_t page_t, page_id_t page_id, BTreePage* parent, uint32_t nEntry,
        disk::DiskManager* disk_manager, key_t_t key_t, uint32_t str_len, bool isInit)
        :BTreePage(page_t, page_id, parent, nEntry, disk_manager, key_t, str_len, isInit)
    {
        branch_ = new Page*[BTdegree << 1];
    }

    InternalPage::~InternalPage()
    {
        for (uint32_t i = 0; i < nEntry_; i++)
            branch_[i]->unref();
        delete[] branch_;
    }

    void InternalPage::update_data()
    {
        // TODO: InternalPage::update_data();
    }


    //
    // ValuePage
    //
    ValuePage::ValuePage(page_id_t page_id, BTreePage* parent, uint32_t nEntry,
        disk::DiskManager* disk_manager, bool isInit)
        :Page(page_t_t::VALUE, page_id, parent, nEntry, disk_manager, isInit), cur_(offset::CUR)
    {
        if (!isInit) {
            write_int(data_ + offset::CUR, offset::CUR);
        }
    }

    ValueEntry ValuePage::read_content(uint32_t offset)
    {
        ValueEntry vEntry;
        vEntry.value_state_ = static_cast<value_state>(data_[offset]);
        vEntry.content_ = std::string_view(data_ + offset + 1);
        return vEntry;
    }

    uint32_t ValuePage::write_content(const char* content, uint32_t size)
    {
        if (size > MAX_STR_LEN) return INVALID_OFFSET;
        data_[cur_] = static_cast<char>(value_state::INUSED);
        memcpy(data_ + cur_ + 1, content, size);
        data_[cur_ + size + 1] = '\0';
        uint32_t ret_offset = cur_;
        cur_ += size + 2;
        write_int(data_ + offset::CUR, cur_); // update `cur_`.
        dirty_ = true;
        return ret_offset;
    }

    void ValuePage::update_data() {}



    //
    // LeafPage
    //
    LeafPage::LeafPage(page_id_t page_id, BTreePage* parent, uint32_t nEntry,
        disk::DiskManager* disk_manager, key_t_t key_t, uint32_t str_len, bool isInit)
        :BTreePage(page_t_t::LEAF, page_id, parent, nEntry, disk_manager, key_t, str_len, isInit)
    {
        value_page_ = new ValuePage(disk_manager_->AllocatePage(), this, 0, disk_manager_);
        value_page_->ref();
        if (!isInit) {
            write_int(data_ + offset::VALUE_PAGE_ID, value_page_->get_page_id());
        }
    }

    LeafPage::~LeafPage() {
        value_page_->unref();
    }

    void LeafPage::update_data()
    {
        // TODO: LeafPage::update_data();
    }


} // end namespace DB::page