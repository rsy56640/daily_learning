// The design doc: https://github.com/rsy56640/xjbDB/tree/master/doc
#ifndef _PAGE_H
#define _PAGE_H
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <string>
#include <unordered_map>
#include "env.h"

namespace DB::disk { class DiskManager; }
namespace DB::tree { class BTree; }
namespace DB::buffer { class BufferPoolManager; }
namespace DB::page
{

    class Page;
    Page* buffer_to_page(const char(&buffer)[page::PAGE_SIZE]);
    class DBMetaPage;
    class TableMetaPage;
    class InternalPage;
    class ValuePage;
    class LeafPage;
    class RootPage;
    DBMetaPage* parse_DBMetaPage(const char(&buffer)[page::PAGE_SIZE]);
    TableMetaPage* parse_TableMetaPage(const char(&buffer)[page::PAGE_SIZE]);
    InternalPage* parse_InternalPage(const char(&buffer)[page::PAGE_SIZE]);
    ValuePage* parse_ValuePage(const char(&buffer)[page::PAGE_SIZE]);
    LeafPage* parse_LeafPage(const char(&buffer)[page::PAGE_SIZE]);
    RootPage* parse_RootPage(const char(&buffer)[page::PAGE_SIZE]);

    constexpr uint32_t PAGE_SIZE = 1 << 10; // 1KB

    enum class page_t_t :uint32_t {
        DB_META,
        TABLE_META,

        ROOT_INTERNAL,
        ROOT_LEAF,
        INTERNAL,
        LEAF,
        VALUE,
    };

    static const char* page_t_str[] = {
        "DB_META",
        "TABLE_META",

        "ROOT_INTERNAL",
        "ROOT_LEAF",
        "INTERNAL",
        "LEAF",
        "VALUE",
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

            // DB meta
            CUR_PAGE_NO = 8,
            TABLE_NUM = 12,
            TABLE_NAME_STR_START = 256,

            // Table meta
            BT_ROOT_ID = 8,
            DEFAULT_VALUE_PAGE_ID = 12,


            // BTree Page / Value Page
            PARENT_PAGE_ID = 8,
            NENTRY = 12,

            // BTree Page
            KEY_T = 16,
            STR_LEN = 18,

            // Leaf Page
            VALUE_PAGE_ID = 20,
            PREVIOUS_PAGE_ID = 24,
            NEXT_PAGE_ID = 28;

    }; // end class offset



    class BTreePage;
    // NB: whenever someone holds the Page*, the Page* must be `ref()` before!!!
    class Page
    {
        friend class ::DB::tree::BTree;
    public:

        Page(page_t_t, page_id_t, disk::DiskManager*, bool isInit);

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
        void page_read_lock();
        void page_write_lock();
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
        page_t_t page_t_; // fundamentally const, but ROOT may violate the rule.
        const page_id_t page_id_;
        char data_[PAGE_SIZE];
        bool dirty_;
        bool discarded_;

    private:
        mutable std::shared_mutex rw_page_mutex_;
        std::atomic<uint32_t> ref_count_;
    };



    //
    // DBMetaPage
    //

    enum str_state :char { OBSOLETE, INUSED };

    class DBMetaPage :public Page {
    public:

        static constexpr uint32_t TABLE_NAME_STR_BLOCK = 25;
        static constexpr uint32_t MAX_TABLE_NAME_STR = 24;
        static constexpr uint32_t MAX_TABLE_NUM = 30;

        DBMetaPage(page_t_t, page_id_t, disk::DiskManager*, bool isInit,
            uint32_t cur_page_no, uint32_t table_num);
        ~DBMetaPage();

        page_id_t find_table(const std::string&);

        bool create_table(page_id_t, const std::string&);

        bool drop_table(const std::string&);

        virtual void update_data();

    public:
        uint32_t cur_page_no_;
        uint32_t table_num_;
        uint32_t* table_page_ids_;
        uint32_t* table_name_offset_;
        std::unordered_map<std::string, page_id_t> table_name2id_;
    };



    //
    // TableMetaPage
    //
    enum col_t_t { INTEGER, CHAR, VARCHAR };
    enum constraint_t_t { PK = 1, FK = 2, NOT_NULL = 4, DEFAULT = 8, };
    class TableMetaPage : public Page {
    public:
        static constexpr uint32_t TABLE_NAME_STR_BLOCK = 25;
        static constexpr uint32_t MAX_TABLE_NAME_STR = 24;
        static constexpr uint32_t MAX_COLUMN_NUM = 15;

        struct ColumnInfo {
            uint32_t col_name_offset_;
            col_t_t col_t_;
            uint32_t str_len_;      // used when col_t_ = `CHAR` or `VARCHAR`
            constraint_t_t constraint_t_;
            uint32_t other_value_;  // table_id     if constraint_t_ = `FK`
                                    // value_offset if constraint_t_ = `DEFAULT`
            bool isPK() const noexcept { return constraint_t_ & constraint_t_t::PK; }
            bool isFK() const noexcept { return constraint_t_ & constraint_t_t::FK; }
            bool isNOT_NULL() const noexcept { return constraint_t_ & constraint_t_t::NOT_NULL; }
            bool isDEFAULT() const noexcept { return constraint_t_ & constraint_t_t::DEFAULT; }
        };

        TableMetaPage(buffer::BufferPoolManager* buffer_pool, page_t_t, page_id_t,
            disk::DiskManager*, bool isInit,
            page_id_t BT_root_id, uint32_t col_num, page_id_t default_value_page_id);

        ~TableMetaPage();

        bool is_PK(const std::string& col_name) const;
        bool is_FK(const std::string& col_name) const;
        bool is_not_null(const std::string& col_name) const;
        bool is_default_col(const std::string& col_name) const;
        ValueEntry get_default_value(const std::string& col_name) const;


    public:
        const page_id_t BT_root_id_;
        const uint32_t col_num_;
        const page_id_t default_value_page_id_;
        ValuePage* value_page_;
        ColumnInfo* cols_;
        std::unordered_map<std::string, ColumnInfo*> col_name2col_;
    };

    //////////////////////////////////////////////////////////////////////
    //////////////////////           util           //////////////////////
    //////////////////////////////////////////////////////////////////////

    uint32_t read_int(const char*);

    void write_int(char*, uint32_t value);

    uint32_t read_short(const char*);

    void write_short(char*, uint16_t value);

    char read_char(const char*);

    void write_char(char*, char c);

    //////////////////////////////////////////////////////////////////////
    //////////////////////       B+ Tree Page       //////////////////////
    //////////////////////////////////////////////////////////////////////

    constexpr uint32_t BTdegree = 8u; // nEntry is [BTdegree - 1, 2*BTdegree -1], [7, 15]
    constexpr uint32_t BTNodeKeySize = (BTdegree << 1) - 1;
    constexpr uint32_t BTNodeBranchSize = BTdegree << 1;

    enum class key_t_t :uint32_t {
        INTEGER,
        CHAR,
        VARCHAR,
    };
    constexpr uint32_t INVALID_OFFSET = PAGE_SIZE;
    constexpr uint32_t MAX_STR_LEN = 57u;


    struct KeyEntry {
        key_t_t key_t;
        int32_t key_int;
        std::string key_str; // <=57B
    };


    // if key is (VAR)CHAR, the key is stored the offset to the real content.
    // all contents are organized as blocks, each block is 58B.
    // block structure: mark(1B), content(<=57B)
    // the 1st block starts at 152u.
    constexpr uint32_t KEY_STR_BLOCK = 58u;
    constexpr uint32_t KEY_STR_START = 152u;

    // for ROOT, INTERNAL, LEAF
    class BTreePage :public Page {
        friend class ::DB::tree::BTree;
    public:

        page_id_t get_parent_id() const noexcept;
        uint32_t get_nEntry() const noexcept;
        void set_parent_id(page_id_t) noexcept;
        key_t_t get_key_t() const noexcept;
        uint32_t get_str_len() const noexcept;

        // insert key at `index`.
        void insert_key(uint32_t index, const KeyEntry&);

        // erase key at `index`.
        // *** the caller should later changes keys[index] and values[index] ***
        void erase_key(uint32_t index);

        // called when key_t is (VAR)CHAR
        KeyEntry read_key(uint32_t index) const;

        int32_t* keys_;    // nEntry // WTF, if in protected, BTree can not access this.

    protected:
        BTreePage(page_t_t, page_id_t, page_id_t parent_id, uint32_t nEntry, disk::DiskManager*,
            key_t_t, uint32_t str_len, bool isInit);
        ~BTreePage();

        page_id_t parent_id_;
        uint32_t nEntry_;
        const key_t_t key_t_;
        const uint32_t str_len_;

        // uint32_t last_offset_; // used for CHAR-key, insert ** from bottom to up **,
                                  // denotes the last front offset, initialized as `PAGE_SIZE`.
                                  // It means data_[last_offset_-1] == '\0' for the next key-str.
                                  // *** not record on disk, recovered when read this page. ***

    }; // end class BTreePage


    struct PageInitInfo {
        page_t_t page_t;
        page_id_t parent_id;
        key_t_t key_t;              // used for BTreePage
        uint32_t str_len;           // used for BTreePage
        page_id_t value_page_id;    // used for LeafPage, ignore if `isInit == true`
        page_id_t previous_page_id; // used for LeafPage, ignore if `isInit == true`
        page_id_t next_page_id;     // used for LeafPage, ignore if `isInit == true`
    };


    // for INTERNAL
    class InternalPage :public BTreePage {
        friend class ::DB::tree::BTree;
    public:
        InternalPage(page_t_t, page_id_t, page_id_t parent_id, uint32_t nEntry,
            disk::DiskManager*, key_t_t, uint32_t str_len, bool isInit);
        virtual ~InternalPage();

        // update the all metadata into memory, for the later `flush()`.
        virtual void update_data();

    public:
        page_id_t * branch_;     // nEntry + 1

    }; // end class InternalPage



    // the ValuePage stores the corresponding value to the key in LeafPage.
    // ValuePage stores whole `char*` and does not care about the specific content.
    // record structure: mark(1B), content(<=67B)
    // each record is 68B :), so it's trivial to handle.
    // 15 * 68 < PAGE_SIZE
    // the state mark is used when deleted, and when do inserttion, find the `OBSOLETE` entry.
    enum class value_state :char { OBSOLETE, INUSED };
    constexpr uint32_t MAX_TUPLE_SIZE = 67u;
    constexpr uint32_t TUPLE_BLOCK_SIZE = 68u;

    struct ValueEntry {
        value_state value_state_ = value_state::OBSOLETE;
        char content_[MAX_TUPLE_SIZE] = { 0 };        // 67B
    };

    // NB: *** tuple size <= 67B ***
    // NO '\0' at end !!!
    class ValuePage :public Page {
    public:
        ValuePage(page_id_t, page_id_t, uint32_t nEntry, disk::DiskManager*, bool isInit);

        // read ValueEntry at `offset`.
        void read_content(uint32_t offset, ValueEntry&) const;

        // update content at `offset`
        void update_content(uint32_t offset, const ValueEntry&);

        // return offset of  the block.
        uint32_t write_content(const ValueEntry&);

        // set the block value_state to `OBSOLETE`.
        void erase_block(uint32_t offset);

        // update the all metadata into memory, for the later `flush()`.
        // *** in fact, do nothing ***
        virtual void update_data();

    public:
        page_id_t parent_id_;
        uint32_t nEntry_;

    }; // end class ValuePage



    // all value is stored in the corresponding ValuePage
    class LeafPage :public BTreePage {
        friend class ::DB::tree::BTree;
        friend class BTreePage;
    public:
        LeafPage(buffer::BufferPoolManager*, page_id_t, page_id_t parent_id, uint32_t nEntry,
            disk::DiskManager*, key_t_t, uint32_t str_len, page_id_t value_page_id, bool isInit);
        virtual ~LeafPage();

        // read the value record into ValueEntry
        void read_value(uint32_t index, ValueEntry&) const;

        // insert value at `index`.
        void insert_value(uint32_t index, const ValueEntry&);

        // erase value-str in value page corrsponding to keys[index].
        void erase_value(uint32_t index);

        // update value at `index`.
        void update_value(uint32_t index, const ValueEntry&);

        void set_left_leaf(page_id_t);
        void set_right_leaf(page_id_t);
        page_id_t get_left_leaf() const;
        page_id_t get_right_leaf() const;

        // update the all metadata into memory, for the later `flush()`.
        virtual void update_data();

    public:
        ValuePage * value_page_;
        page_id_t value_page_id_;
        page_id_t previous_page_id_, next_page_id_;
        uint32_t* values_; // points to offset of the value-blocks.

    }; // end class LeafPage




    class RootPage :public InternalPage {
        friend class ::DB::tree::BTree;
        friend class BTreePage;
    public:
        RootPage(buffer::BufferPoolManager*, page_t_t, page_id_t parent_id, page_id_t,
            uint32_t nEntry, disk::DiskManager*, key_t_t, uint32_t str_len, page_id_t value_page_id, bool isInit);
        virtual ~RootPage();

        // read the value record into ValueEntry
        void read_value(uint32_t index, ValueEntry&) const;

        // insert value at `index`.
        void insert_value(uint32_t index, const ValueEntry&);

        // erase value-str in value page corrsponding to keys[index].
        void erase_value(uint32_t index);

        // update value at `index`.
        void update_value(uint32_t index, const ValueEntry&);

        void set_left_leaf(page_id_t);
        void set_right_leaf(page_id_t);
        page_id_t get_left_leaf() const;
        page_id_t get_right_leaf() const;

        // update the all metadata into memory, for the later `flush()`.
        virtual void update_data();

    private:
        // for ROOT_LEAF
        ValuePage * value_page_;
        page_id_t value_page_id_;
        uint32_t* values_; // points to offset of the value-blocks.
        page_id_t previous_page_id_, next_page_id_;

    }; // end class RootPage


} // end namespace DB::page

#endif // !_PAGE_H