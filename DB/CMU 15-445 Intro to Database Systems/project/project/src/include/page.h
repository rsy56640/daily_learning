// each page is of 1KB size,
// ______  ______  ______  ______
// | 1KB | | 1KB | | 1KB | | 1KB |
//  ~~~~~~  ~~~~~~  ~~~~~~  ~~~~~~
// the 0th page is special, in which stores the metadata of the whole file.
// page_no starts from 1.
//
//
//
//
//
//
#ifndef _PAGE_H
#define _PAGE_H
#include <mutex>
#include <atomic>
#include "env.h"

namespace DB::disk { class DiskManager; }
namespace DB::page
{

    inline constexpr std::size_t PAGE_SIZE = 1 << 10; // 1KB

    enum class page_t_t {
        free,             //
        tree,             //
        pointer_map,      //
        lock_byte,        //
    };

    // use 32 bit integer to represent the page_id.
    // page_id must start from 1.
    using page_id_t = uint32_t;
    inline constexpr page_id_t NOT_A_PAGE = 0;


    // parse buffer[PAGE_SIZE] into a Page*
    //
    class Page;
    Page* buffer_to_page(const char* buffer, disk::DiskManager* disk_manager);


    class Page
    {

    public:

        Page(page_t_t, page_id_t, disk::DiskManager* disk_manager);

        void ref();

        void unref();

        page_t_t get_page_t() const noexcept;

        page_id_t get_page_id() const noexcept;

        char* get_data() noexcept;

        void set_dirty() noexcept;

        bool is_dirty() noexcept;

        // return true if the lock is acquired.
        bool try_lock_page();

        ~Page();

    private:
        disk::DiskManager * disk_manager_;
        const page_t_t page_t_;
        const page_id_t page_id_;
        mutable std::mutex page_mutex_;
        char data_[PAGE_SIZE];
        std::atomic<uint32_t> ref_count_;
        bool dirty_;

    };

} // end namespace DB::page

#endif // !_PAGE_H