#ifndef _TABLE_H
#define _TABLE_H
#include "disk_manager.h"
#include "log_manager.h"
#include "buffer_pool.h"

namespace DB::table
{

    // so many interfaces




    class StorageEngine
    {
    public:



    private:

        disk::DiskManager* disk_manager_;
        buffer::BufferPoolManager* buffer_pool_manager_;
        log::LogManager* log_manager_;
        //LockManager* lock_manager_;
        //TransactionManager *transaction_manager_;
    };


    class Table 
    {



    };



} // end namespace DB::table

#endif // !_TABLE_H
