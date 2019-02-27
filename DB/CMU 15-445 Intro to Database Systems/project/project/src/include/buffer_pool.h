#ifndef _BUFFER_POOL_H
#include "hash_lru.h"

namespace DB::buffer
{

    class BufferPoolManager
    {
    public:

    private:

        Hash_LRU hash_lru_;

    }; // end class BufferPoolManager


} // end namespace DB::buffer

#endif // !_BUFFER_POOL_H
