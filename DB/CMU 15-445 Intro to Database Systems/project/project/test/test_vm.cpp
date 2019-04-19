#ifdef _xjbDB_test_VM_
#include "test.h"
#include <map>
#include <cstring>
#include <iostream>
using namespace DB::tree;
using namespace std;

void set(ValueEntry& vEntry, const char* s)
{
    const int size = strlen(s);
    std::memcpy(vEntry.content_, s, size > MAX_TUPLE_SIZE ? MAX_TUPLE_SIZE : size);
}

void test()
{

    const char* s[] =
    {
        "hello world",
        "qwer",
        "The dog braks at the cat.",
        "WTF!!!!",
    };

    const char* valueState[2] = { "OBSOLETE", "INUSED" };

    disk::DiskManager* disk_manager = new disk::DiskManager{ "test" };
    buffer::BufferPoolManager* buffer_pool_manager = new buffer::BufferPoolManager{ disk_manager };




    delete disk_manager;
    delete buffer_pool_manager;

}

#endif // _xjbDB_test_VM_