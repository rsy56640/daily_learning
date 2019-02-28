#ifndef _PAGE_H
#include "env.h"
#include <mutex>
#include <atomic>

namespace DB::page
{

    struct page_t_t {
        enum {
            free,             //
            tree,             //
            pointer_map,      //
            lock_byte,        //
        };
    };

    /*
     * use 32 bit integer to represent the page_id.
     * page_id must start from 1.
     */
    using page_id_t = uint32_t;
    static constexpr page_id_t NOT_A_PAGE = 0;


    /*
     *
     */
    class Page
    {

    public:

        Page(page_t_t, page_id_t);

        void ref();

        void unref();

        // return true if the lock is acquired.
        bool try_lock_page();

        ~Page();

    private:
        const page_t_t page_t_;
        const page_id_t page_id_;
        mutable std::mutex page_mutex_;
        std::atomic<uint32_t> ref_count_;
        bool dirty_;


    };

} // end namespace DB::page

#endif // !_PAGE_H
