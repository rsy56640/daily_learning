#ifndef _VM_H
#define _VM_H
#include "disk_manager.h"
#include "log_manager.h"
#include "buffer_pool.h"
#include "BplusTree.h"
#include "thread_pool.h"
#include "page.h"
#include <future>
#include <unordered_map>

namespace DB::tree { class BTree; }

namespace DB::vm
{

    // so many interfaces




    struct StorageEngine
    {
        disk::DiskManager* disk_manager_;
        buffer::BufferPoolManager* buffer_pool_manager_;
    };


    class Table
    {

    };


    // used as a temporary table.
    // implementation might be stream ?? with some sync mechanism.
    class VitrualTable {


    };


    class VM
    {
    public:

        VM();
        ~VM();

        void flush();


        template<typename F, typename... Args>[[nodiscard]]
            std::future<std::invoke_result_t<F, Args...>> register_task(F&& f, Args&& ...args) {
            return thread_pool_.register_for_execution(f, args...);
        }



    private:

        util::ThreadsPool thread_pool_;
        StorageEngine storage_engine_;
        page::DBMetaPage* db_meta_;
        std::unordered_map<std::string, page::TableMetaPage*> table_meta_;
    };



} // end namespace DB::vm

#endif // !_VM_H