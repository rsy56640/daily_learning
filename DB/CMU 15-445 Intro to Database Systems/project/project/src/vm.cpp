#include "include/vm.h"

namespace DB::vm
{

    VM::VM()
    {
        storage_engine_.disk_manager_ = new disk::DiskManager{};
        storage_engine_.buffer_pool_manager_ = new buffer::BufferPoolManager{ storage_engine_.disk_manager_ };

        // read DB meta


        storage_engine_.disk_manager_->set_cur_page_id(db_meta_->cur_page_no_);

        // read table meta




    }


    VM::~VM()
    {
        delete db_meta_;
        for (auto&[k, v] : table_meta_)
            delete v;
    }


} // end namespace DB::vm