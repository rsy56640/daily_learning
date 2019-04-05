#ifndef _DEBUG_LOG_H
#define _DEBUG_LOG_H
#include <string>
#include <cstdio>
#include "page.h"

namespace DB::debug
{

    static const char* debug_output = "./debug_output.txt";

    void ERROR_LOG(const char*);
    void DEBUG_LOG(const char*);

    template<typename ...Arg>
    void DEBUG_LOG(Arg... args) {
        std::printf(args...);
    }

    template<typename ...Arg>
    void ERROR_LOG(Arg... args) {
        std::printf(args...);
    }

    void debug_page(const page::page_id_t, buffer::BufferPoolManager*);
    void debug_leaf(const page::LeafPage* leaf);
    void debug_internal(const page::InternalPage* link);
    void debug_value(const page::ValuePage*);
    void debug_root(const page::RootPage*);

} // end namespace DB::debug

#endif // !_DEBUG_LOG_H