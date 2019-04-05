#ifdef _xjbDB_test_BPLUSTREE_
#include "test.h"
#include <map>
#include <cstring>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <algorithm>
#include <ctime>
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

    printf("--------------------- test begin ---------------------\n");

    const char* s[] =
    {
        "str0: hello world",
        "str1: qwer",
        "str2: The dog barks at the cat.",
        "str3: WTF!!!!",
        "str4: gkdgkd",
        "str5: LeetCode Completed",
        "str6: Distributed Database",
        "str7: abcdefghijk",
        "str8: hhhhhh",
        "str9: xjbDB ok",
    };

    const char* valueState[2] = { "OBSOLETE", "INUSED" };

    OpenTableInfo info;
    info.isInit = true;
    std::shared_ptr<disk::DiskManager> disk_manager = std::make_shared<disk::DiskManager>("test");
    std::shared_ptr<vm::StorageEngine> storage_engine = std::make_shared<vm::StorageEngine>();
    std::shared_ptr<buffer::BufferPoolManager> buffer_pool_manager = std::make_shared<buffer::BufferPoolManager>(disk_manager.get());
    storage_engine->disk_manager_ = disk_manager.get();
    storage_engine->buffer_pool_manager_ = buffer_pool_manager.get();
    BTree bt(info, storage_engine.get(), page::key_t_t::INTEGER);

    KeyEntry key;
    key.key_t = page::key_t_t::INTEGER;
    ValueEntry value;
    value.value_state_ = value_state::INUSED;

    KVEntry kv = { key ,value };

    constexpr int key_test_size = 1000;
    constexpr int debug_page_size = 25;
    constexpr int key_test_find = 100;
    constexpr int rand_seed = 19280826;
    srand(rand_seed);

    std::vector<int> keys(key_test_size);
    for (int i = 0; i < key_test_size; i++)
        keys[i] = i;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(keys.begin(), keys.end(), g);

    for (int i : keys)
    {
        key.key_int = i;
        set(value, s[i % (sizeof(s) / sizeof(const char*))]);
        bt.insert({ key , value });
        cout << i << endl;
    }

    bt.debug();
    //for (int i = 1; i < debug_page_size; i++)
    //    bt.debug_page(i);


    for (int i = 0; i < key_test_find; i++)
    {
        cout << endl;
        key.key_int = ((rand() % key_test_size) + key_test_size) % key_test_size;
        value = bt.find(key);
        cout << key.key_int << ": " << valueState[static_cast<int>(value.value_state_)]
            << " " << value.content_ << endl;
    }

    printf("--------------------- test end ---------------------\n");

}

#endif // _xjbDB_test_BPLUSTREE_
