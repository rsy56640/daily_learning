#ifndef _VM_H
#define _VM_H
#include "disk_manager.h"
#include "log_manager.h"
#include "buffer_pool.h"
#include "BplusTree.h"

namespace DB::tree { class BTree; }

namespace DB::vm
{

    // so many interfaces




    struct StorageEngine
    {

        disk::DiskManager* disk_manager_;
        buffer::BufferPoolManager* buffer_pool_manager_;
        //log::LogManager* log_manager_;
        //LockManager* lock_manager_;
        //TransactionManager *transaction_manager_;
    };


    class Table
    {



    };



} // end namespace DB::vm

#endif // !_VM_H