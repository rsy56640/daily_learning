#include "include/page.h"
#include "include/disk_manager.h"
#include "include/buffer_pool.h"
#include "include/debug_log.h"
#include <cstring>
#include <vector>

namespace DB::page
{

    // TODO: all ctor/dtor/update_data !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Page::Page(page_t_t page_t, page_id_t page_id, page_id_t parent_id, uint32_t nEntry,
        disk::DiskManager* disk_manager, bool isInit)
        :
        disk_manager_(disk_manager),
        page_t_(page_t),
        page_id_(page_id),
        parent_id_(parent_id),
        nEntry_(nEntry),
        ref_count_(0),
        dirty_(false),
        discarded_(false)
    {
        disk_manager_->ReadPage(page_id_, data_);
        if (!isInit) {
            write_int(data_ + offset::PAGE_T, static_cast<uint32_t>(page_t_));
            write_int(data_ + offset::PAGE_ID, page_id_);
            if (parent_id_ == NOT_A_PAGE)
                write_int(data_ + offset::PARENT_PAGE_ID, NOT_A_PAGE);
            else
                write_int(data_ + offset::PARENT_PAGE_ID, parent_id_);
            write_int(data_ + offset::NENTRY, nEntry_);
            dirty_ = true;
        }
    }

    Page::~Page() {

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
    void Page::page_read_lock() {
        while (!(try_page_read_lock()));
    }
    void Page::page_write_lock() {
        while (!(try_page_write_lock()));
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

    uint32_t read_short(const char* data) {
        uint32_t
            u0 = static_cast<unsigned char>(data[0]),
            u1 = static_cast<unsigned char>(data[1]);
        uint32_t uval = u0 | (u1 << 8);
        return uval;
    }

    void write_short(char* data, uint32_t value) {
        data[0] = value;
        data[1] = value >> 8;
    }


    //////////////////////////////////////////////////////////////////////
    //////////////////////       B+ Tree Page       //////////////////////
    //////////////////////////////////////////////////////////////////////

    //
    // BTree Page
    //
    BTreePage::BTreePage(page_t_t page_t, page_id_t page_id, page_id_t parent_id, uint32_t nEntry,
        disk::DiskManager* disk_manager, key_t_t key_t, uint32_t str_len, bool isInit)
        :Page(page_t, page_id, parent_id, nEntry, disk_manager, isInit),
        key_t_(key_t), str_len_(str_len)//, last_offset_(PAGE_SIZE)
    {
        keys_ = new int32_t[BTNodeKeySize];
        if (!isInit) {
            write_short(data_ + offset::KEY_T, static_cast<uint32_t>(key_t_));
            write_short(data_ + offset::STR_LEN, str_len_);
        }
    }

    BTreePage::~BTreePage()
    {
        delete[] keys_;
    }


    void BTreePage::insert_key(uint32_t index, const KeyEntry& kEntry)
    {
        if (key_t_ == key_t_t::INTEGER)
        {
            keys_[index] = kEntry.key_int;
        }
        else // key is str.
        {
            // find a `OBSOLETE` record.
            uint32_t keyStrIndex = 0;
            for (; keyStrIndex < BTNodeKeySize; keyStrIndex++)
                if (data_[KEY_STR_START + keyStrIndex * KEY_STR_BLOCK]
                    == static_cast<char>(key_str_state::OBSOLETE))
                    break;

            if (keyStrIndex == BTNodeKeySize)
                debug::ERROR_LOG("`BTreePage::insert_key()`, might forget erase key somewhere");

            const uint32_t offset = KEY_STR_START + keyStrIndex * KEY_STR_BLOCK;
            keys_[index] = offset; // update key-index
            data_[offset] = static_cast<char>(key_str_state::INUSED);
            const uint32_t min_size =
                MAX_STR_LEN > kEntry.key_str.size() ? kEntry.key_str.size() : MAX_STR_LEN;
            std::memcpy(data_ + offset + 1, kEntry.key_str.c_str(), min_size);
        }
        dirty_ = true;
    }


    void BTreePage::erase_key(uint32_t index)
    {
        // if key is (VAR)CHAR attached at the end.
        if (key_t_ != key_t_t::INTEGER)
        {
            const uint32_t offset = keys_[index];
            data_[offset] = static_cast<char>(key_str_state::OBSOLETE);
        }
        dirty_ = true;
    }


    KeyEntry BTreePage::read_key(uint32_t index) const
    {
        KeyEntry kEntry;
        kEntry.key_t = key_t_;
        const uint32_t offset = keys_[index];
        if (data_[offset + KEY_STR_BLOCK - 1] != '\0')
            kEntry.key_str = std::string(data_ + offset + 1, KEY_STR_BLOCK - 1);
        else
            kEntry.key_str = std::string(data_ + offset + 1);
        return kEntry;
    }


    //
    // InternalPage
    //
    InternalPage::InternalPage(page_t_t page_t, page_id_t page_id, page_id_t parent_id, uint32_t nEntry,
        disk::DiskManager* disk_manager, key_t_t key_t, uint32_t str_len, bool isInit)
        :BTreePage(page_t, page_id, parent_id, nEntry, disk_manager, key_t, str_len, isInit)
    {
        branch_ = new page_id_t[BTNodeBranchSize];
    }

    InternalPage::~InternalPage()
    {
        delete[] branch_;
    }

    void InternalPage::update_data()
    {
        // TODO: InternalPage::update_data();
    }


    //
    // ValuePage
    //
    ValuePage::ValuePage(page_id_t page_id, page_id_t parent_id, uint32_t nEntry,
        disk::DiskManager* disk_manager, bool isInit)
        :Page(page_t_t::VALUE, page_id, parent_id, nEntry, disk_manager, isInit)//, cur_(offset::CUR)
    {
        if (!isInit) {
            // write_int(data_ + offset::CUR, offset::CUR);
        }
    }

    void ValuePage::read_content(uint32_t offset, ValueEntry& vEntry) const
    {
        vEntry.value_state_ = static_cast<value_state>(data_[offset]);
        std::memcpy(vEntry.content_, data_ + offset + 1, TUPLE_BLOCK_SIZE - 1);
    }

    void ValuePage::update_content(uint32_t offset, const ValueEntry& vEntry)
    {
        if (vEntry.value_state_ != value_state::INUSED)
            debug::ERROR_LOG("`ValuePage::update_content()` input data invalid.");
        data_[offset] = static_cast<char>(vEntry.value_state_);
        std::memcpy(data_ + offset + 1, vEntry.content_, MAX_TUPLE_SIZE);
    }

    uint32_t ValuePage::write_content(const ValueEntry& vEntry)
    {
        dirty_ = true;
        // find an obsolete record.
        uint32_t index = 0;
        for (; index < BTNodeKeySize; index++)
            if (data_[index * TUPLE_BLOCK_SIZE] == static_cast<char>(value_state::OBSOLETE))
                break;
        const uint32_t offset = index * TUPLE_BLOCK_SIZE;
        data_[offset] = static_cast<char>(value_state::INUSED);
        std::memcpy(data_ + offset + 1, vEntry.content_, MAX_TUPLE_SIZE);
        return offset;
    }

    void ValuePage::erase_block(uint32_t offset) {
        data_[offset] = static_cast<char>(value_state::OBSOLETE);
        dirty_ = true;
    }

    void ValuePage::update_data() {}



    //
    // LeafPage
    //
    LeafPage::LeafPage(buffer::BufferPoolManager* buffer_pool, page_id_t page_id, page_id_t parent_id, uint32_t nEntry,
        disk::DiskManager* disk_manager, key_t_t key_t, uint32_t str_len, page_id_t value_page_id, bool isInit)
        :BTreePage(page_t_t::LEAF, page_id, parent_id, nEntry,
            disk_manager, key_t, str_len, isInit), value_page_id_(value_page_id)
    {
        PageInitInfo info;
        info.page_t = page_t_t::VALUE;
        info.parent_id = this->get_page_id();
        if (isInit) // new
        {
            value_page_ = static_cast<ValuePage*>(buffer_pool->NewPage(info));
            value_page_id_ = value_page_->get_page_id();
            buffer_pool->DeletePage(value_page_id_);
            // now value_page has excatly *** 1 ref count ***.
            write_int(data_ + offset::VALUE_PAGE_ID, value_page_id_);
        }
        else // exist
        {
            value_page_ = static_cast<ValuePage*>(buffer_pool->FetchPage(value_page_id_));
            buffer_pool->DeletePage(value_page_id_);
            // now value_page has excatly *** 1 ref count ***.
        }
    }

    LeafPage::~LeafPage() {
        value_page_->unref(); // ref 1->0
    }

    void LeafPage::read_value(uint32_t index, ValueEntry& vEntry) const
    {
        value_page_->read_content(values_[index], vEntry);
        if (vEntry.value_state_ != value_state::INUSED)
            debug::ERROR_LOG("`LeafPage::read_value()` read corrupted value.");
    }

    void LeafPage::insert_value(uint32_t index, const ValueEntry& vEntry)
    {
        values_[index] = value_page_->write_content(vEntry);
        dirty_ = true;
    }

    void LeafPage::erase_value(uint32_t index) {
        value_page_->erase_block(values_[index]);
    }

    void LeafPage::update_value(uint32_t index, const ValueEntry& vEntry) {
        value_page_->update_content(values_[index], vEntry);
    }

    void LeafPage::set_left_leaf(page_id_t previous_page_id) { previous_page_id_ = previous_page_id; }
    void LeafPage::set_right_leaf(page_id_t next_page_id) { next_page_id_ = next_page_id; }
    page_id_t LeafPage::get_left_leaf() const { return previous_page_id_; }
    page_id_t LeafPage::get_right_leaf() const { return next_page_id_; }

    void LeafPage::update_data()
    {
        // TODO: LeafPage::update_data();
    }


} // end namespace DB::page