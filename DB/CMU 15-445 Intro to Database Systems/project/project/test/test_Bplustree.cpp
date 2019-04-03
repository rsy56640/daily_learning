#ifdef _xjbDB_test_BPLUSTREE_
#include "test.h"
#include <map>
#include <cstring>
#include <iostream>
#include <memory>
using namespace DB::tree;
using namespace std;

void set(ValueEntry& vEntry, const char* s)
{
    const int size = strlen(s);
    std::memset(vEntry.content_, 0, sizeof(char)*MAX_TUPLE_SIZE);
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

    OpenTableInfo info;
    info.isInit = true;
    std::shared_ptr<disk::DiskManager> disk_manager = std::make_shared<disk::DiskManager>("test");
    std::shared_ptr<vm::StorageEngine> storage_engine = std::make_shared<vm::StorageEngine>();
    std::shared_ptr<buffer::BufferPoolManager> buffer_pool_manager = std::make_shared<buffer::BufferPoolManager>(disk_manager.get());
    storage_engine->buffer_pool_manager_ = buffer_pool_manager.get();
    BTree bt(info, storage_engine.get(), page::key_t_t::INTEGER);

    KeyEntry key;
    key.key_t = page::key_t_t::INTEGER;
    ValueEntry value;
    value.value_state_ = value_state::INUSED;

    KVEntry kv = { key ,value };


    for (int i = 0; i < 100; i++)
    {
        key.key_int = i;
        set(value, s[i % 4]);
        bt.insert({ key , value });
        cout << i << endl;
    }



    for (int i = 0; i < 10; i++)
    {
        key.key_int = i;
        value = bt.find(key);
        cout << i << ": " << valueState[static_cast<int>(value.value_state_)] << " "
            << value.content_ << endl;
    }

    printf("----------------- test ---------------------\n");
    
}

#endif // _xjbDB_test_BPLUSTREE_
