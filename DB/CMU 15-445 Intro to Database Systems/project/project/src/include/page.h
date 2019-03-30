// 
#ifndef _PAGE_H
#define _PAGE_H
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <string_view>
#include "env.h"

namespace DB::disk { class DiskManager; }
namespace DB::tree { class BTree; }
namespace DB::page
{

    constexpr uint32_t PAGE_SIZE = 1 << 10; // 1KB

    enum class page_t_t :uint32_t {
        FREE,             //
        ROOT,             // also `InternalPage`
        INTERNAL,         //
        LEAF,             //
        VALUE,            //
        //pointer_map,      //
        //lock_byte,        //
    };

    // use 32 bit integer to represent the page_id.
    // page_id must start from 1.
    using page_id_t = uint32_t;
    constexpr page_id_t NOT_A_PAGE = 0;


    struct offset {
        static constexpr uint32_t

            // Page
            PAGE_T = 0,
            PAGE_ID = 4,
            PARENT_PAGE_ID = 8,
            NENTRY = 12,

            // BTree Page
            KEY_T = 16,
            STR_LEN = 20,

            // Leaf Page
            VALUE_PAGE_ID = 24,

            // Value Page
            CUR = 16;

    }; // end class offset



    // parse buffer[PAGE_SIZE] into a Page*
    class Page;
    class BTreePage;
    Page* buffer_to_page(const char* buffer, disk::DiskManager* disk_manager);

    // NB: whenever someone holds the Page*, the Page* must be `ref()` before!!!
    class Page
    {
    public:

        Page(page_t_t, page_id_t, BTreePage*, uint32_t nEntry, disk::DiskManager*, bool isInit);

        virtual ~Page();

        void ref();

        void unref();

        page_t_t get_page_t() const noexcept;

        page_id_t get_page_id() const noexcept;

        char* get_data() noexcept;

        void set_dirty() noexcept;

        bool is_dirty() noexcept;

        // flush to disk if dirty.
        void flush();

        // return true if the lock is acquired.
        bool try_page_read_lock();
        bool try_page_write_lock();
        void page_read_unlock();
        void page_write_unlock();

        // update the all metadata into memory, for the later `flush()`.
        virtual void update_data() = 0;

        Page(const Page&) = delete;
        Page& operator=(const Page&) = delete;
        Page(Page&&) = delete;
        Page& operator=(Page&&) = delete;

    protected:
        disk::DiskManager * disk_manager_;
        const page_t_t page_t_;
        const page_id_t page_id_;
        BTreePage* parent_;
        uint32_t nEntry_;
        char data_[PAGE_SIZE];
        bool dirty_;
        bool discarded_;

    private:
        mutable std::shared_mutex rw_page_mutex_;
        std::atomic<uint32_t> ref_count_;
    };

    //////////////////////////////////////////////////////////////////////
    //////////////////////           util           //////////////////////
    //////////////////////////////////////////////////////////////////////

    uint32_t read_int(const char*);

    void write_int(char*, uint32_t value);

    //////////////////////////////////////////////////////////////////////
    //////////////////////       B+ Tree Page       //////////////////////
    //////////////////////////////////////////////////////////////////////

    constexpr uint32_t BTdegree = 8u; // nEntry is [BTdegree - 1, 2*BTdegree -1], [7, 15]

    enum class key_t_t :uint32_t {
        INTEGER,
        CHAR,
        VARCHAR,
    };
    constexpr uint32_t INVALID_OFFSET = PAGE_SIZE;
    constexpr uint32_t MAX_STR_LEN = 57u;


    // for ROOT, INTERNAL, LEAF
    class BTreePage :public Page {
    public:

        BTreePage(page_t_t, page_id_t, BTreePage*, uint32_t nEntry, disk::DiskManager*,
            key_t_t, uint32_t str_len, bool isInit);
        ~BTreePage();

        // the caller ensures that the BTreePage* won't be deleted when in this function,
        // that means the caller had better `ref()` the BTreePage*.
        // `ref()` the parent at first.
        //void set_parent(BTreePage*);

    protected:
        const key_t_t key_t_;
        const uint32_t str_len_;
        uint32_t * keys_;    // nEntry

    }; // end class BTreePage


    // for ROOT and INTERNAL
    class InternalPage :public BTreePage {
        friend class DB::tree::BTree;
    public:
        InternalPage(page_t_t, page_id_t, BTreePage*, uint32_t nEntry,
            disk::DiskManager*, key_t_t, uint32_t str_len = 0, bool isInit = false);
        virtual ~InternalPage();

        // update the all metadata into memory, for the later `flush()`.
        virtual void update_data();

    private:
        Page * * branch_;     // nEntry + 1

    }; // end class InternalPage



    // the ValuePage stores the corresponding value to the key in LeafPage.
    // ValuePage stores whole `char*` and does not care about the specific content.
    // the content must be suffixed with '\0',
    // besieds we add 1B prefix to mark the content for GC. :) which might not be implemented.
    // structure: mark(1B), content(<=57B), '\0'(1B)
    // the value-offset points to the first mark.
    enum class value_state :char { INUSED, OBSOLETE };


    struct ValueEntry {
        value_state value_state_;
        std::string_view content_;
    };

    class ValuePage :public Page {
    public:
        ValuePage(page_id_t, BTreePage*, uint32_t nEntry, disk::DiskManager*, bool isInit = false);

        // read ValueEntry at offset.
        ValueEntry read_content(uint32_t offset);

        // return offset at first mark.
        // we suppose the strlen <= 57B, so that the write ops won't exceed.
        uint32_t write_content(const char* content, uint32_t size);

        // update the all metadata into memory, for the later `flush()`.
        // *** in fact, do nothing ***
        virtual void update_data();

    private:
        uint32_t cur_; // denotes the next pos to write.

    }; // end class ValuePage



    // all value is stored in the corresponding ValuePage
    class LeafPage :public BTreePage {
        friend class DB::tree::BTree;
    public:
        LeafPage(page_id_t, BTreePage*, uint32_t nEntry, disk::DiskManager*, key_t_t,
            uint32_t str_len = 0, bool isInit = false);
        virtual ~LeafPage();

        // update the all metadata into memory, for the later `flush()`.
        virtual void update_data();

    private:
        ValuePage * value_page_;
        uint32_t* values_; // points to value-offset

    }; // end class LeafPage


} // end namespace DB::page

#endif // !_PAGE_H