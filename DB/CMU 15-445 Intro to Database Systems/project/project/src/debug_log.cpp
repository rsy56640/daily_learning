#include "include/debug_log.h"
#include <iostream>

namespace DB::debug
{

    void ERROR_LOG(const char* msg)
    {
        std::cout << "Error Message: " << msg << std::endl;
    }

    void DEBUG_LOG(const char* msg)
    {
        std::cout << "Debug Message: " << msg << std::endl;
    }


} // end namespace DB::debug