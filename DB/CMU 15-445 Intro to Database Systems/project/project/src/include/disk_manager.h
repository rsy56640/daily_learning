#ifndef _DISK_MANAGER_H
#define _DISK_MANAGER_H
#include "page.h"
#include "env.h"
#include <fstream>
#include <string>
#include <atomic>

namespace DB::disk
{
    using page::page_id_t;
    using page::PAGE_SIZE;

    /*
     * record any DB file
     */
    class DiskManager
    {
    public:

        DiskManager(const std::string& file_name);
        ~DiskManager();

        page_id_t get_cut_page_id() const;

        // no validation on `page_id`
        // the length of `page_data` should not surpass `PAGE_SIZE`
        void WritePage(page_id_t page_id, const char *page_data);

        // no validation on `page_id`
        // the length of `page_data` should be enough to hold `PAGE_SIZE`
        void ReadPage(page_id_t page_id, char* page_data);

        void WriteLog(char *log_data, uint32_t size);

        // return true if read successfully
        bool ReadLog(char *log_data, uint32_t offset, uint32_t size);

        page_id_t AllocatePage();

        uint32_t log_size() const;

        DiskManager(const DiskManager&) = delete;
        DiskManager(DiskManager&&) = delete;
        DiskManager& operator=(const DiskManager&) = delete;
        DiskManager& operator=(DiskManager&&) = delete;

    private:
        std::atomic<page_id_t> cur_page_no_;

        // stream to write db file
        const std::string file_name_;
        std::fstream db_io_;

        // stream to write log file
        const std::string log_name_;
        std::fstream log_io_;
        uint32_t log_size_;

    }; // end class DiskManager

} // end namespace DB::disk

#endif // !_DISK_MANAGER_H