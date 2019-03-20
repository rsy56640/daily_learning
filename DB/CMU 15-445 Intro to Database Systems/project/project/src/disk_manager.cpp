#include "include/disk_manager.h"
#include "include/debug_log.h"
#include <cstring>

namespace DB::disk
{

    DiskManager::DiskManager(const std::string& file_name)
        :
        cur_page_no_(page::NOT_A_PAGE),
        file_name_(file_name + ".xjbDB"),
        log_name_(file_name + "_log.xjbDB"),
        log_size_(0)
    {
        db_io_.open(file_name,
            std::ios::binary | std::ios::in | std::ios::out);
        // directory or file does not exist
        if (!db_io_.is_open()) {
            db_io_.clear();
            // create a new file
            db_io_.open(file_name,
                std::ios::binary | std::ios::trunc | std::ios::out);
            db_io_.close();
            // reopen with original mode
            db_io_.open(file_name,
                std::ios::binary | std::ios::in | std::ios::out);
        }

        log_io_.open(log_name_,
            std::ios::binary | std::ios::in | std::ios::app | std::ios::out);
        // directory or file does not exist
        if (!log_io_.is_open()) {
            log_io_.clear();
            // create a new file
            log_io_.open(log_name_,
                std::ios::binary | std::ios::trunc | std::ios::app | std::ios::out);
            log_io_.close();
            // reopen with original mode
            log_io_.open(log_name_,
                std::ios::binary | std::ios::in | std::ios::app | std::ios::out);
        }
    }


    DiskManager::~DiskManager()
    {
        db_io_.close();
        log_io_.close();
    }


    void DiskManager::WritePage(page_id_t page_id, const char *page_data)
    {
        db_io_.seekp(page_id * PAGE_SIZE);
        db_io_.write(page_data, PAGE_SIZE);
        if (db_io_.bad())
        {
            std::string error_log;
            error_log.reserve(23 + file_name_.size() + 13 + 10);
            error_log += "I/O writing error in \"" + file_name_
                + "\", page_id: " + std::to_string(page_id);
            debug::DEBUG_LOG(error_log.c_str());
            return;
        }
        db_io_.flush();
    }


    void DiskManager::ReadPage(page_id_t page_id, char *page_data)
    {
        db_io_.seekg(page_id * PAGE_SIZE);
        db_io_.read(page_data, PAGE_SIZE);
        const uint32_t read_count = db_io_.gcount();
        if (read_count < PAGE_SIZE)
        {
            std::string error_log;
            error_log.reserve(30 + file_name_.size() + 13 + 10);
            error_log += "Read less than PAGE_SIZE in \"" + file_name_
                + "\", page_id: " + std::to_string(page_id);
            debug::DEBUG_LOG(error_log.c_str());
            memset(page_data + read_count, 0, PAGE_SIZE - read_count);
        }
    }


    void DiskManager::WriteLog(char *log_data, uint32_t size)
    {
        log_io_.write(log_data, size);
        if (db_io_.bad())
        {
            std::string error_log;
            error_log.reserve(23 + file_name_.size());
            error_log += "Log writing error in \"" + file_name_;
            debug::DEBUG_LOG(error_log.c_str());
            return;
        }
        log_io_.flush();
        log_size_ += size;
    }


    bool DiskManager::ReadLog(char *log_data, uint32_t offset, uint32_t size)
    {
        log_io_.seekg(offset);
        log_io_.read(log_data, size);
        // if log file ends before reading "size"
        const uint32_t read_count = log_io_.gcount();
        if (read_count < size) {
            log_io_.clear();
            memset(log_data + read_count, 0, size - read_count);
        }
        log_io_.seekp(0, std::ios_base::end);
        return true;
    }


    page_id_t DiskManager::AllocatePage() { return ++cur_page_no_; }


    uint32_t DiskManager::log_size() const { return log_size_; }


} // end namespace DB::disk