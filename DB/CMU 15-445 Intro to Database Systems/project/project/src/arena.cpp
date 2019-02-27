// refer to https://github.com/google/leveldb/blob/master/util/arena.cc
#include "include/arena.h"

namespace DB::buffer
{

    Arena::Arena() :alloc_ptr_(nullptr), memory_usage_(0), alloc_bytes_remaining_(0) {}


    Arena::~Arena() {
        const uint32_t size = blocks_.size();
        for (uint32_t i = 0; i < size; i++)
            delete[] blocks_[i];
    }


    char* Arena::AllocateFallback(uint32_t bytes) {
        if (bytes > Arena::kBlockSize / 4) {
            // Object is more than a quarter of our block size.  Allocate it separately
            // to avoid wasting too much space in leftover bytes.
            char* result = AllocateNewBlock(bytes);
            return result;
        }

        // We waste the remaining space in the current block.
        alloc_ptr_ = AllocateNewBlock(Arena::kBlockSize);
        alloc_bytes_remaining_ = Arena::kBlockSize;

        char* result = alloc_ptr_;
        alloc_ptr_ += bytes;
        alloc_bytes_remaining_ -= bytes;
        return result;
    }


    char* Arena::AllocateNewBlock(uint32_t block_bytes) {
        char* result = new char[block_bytes];
        blocks_.push_back(result);
        memory_usage_.store(MemoryUsage() + block_bytes + sizeof(char*));
        return result;
    }

} // end namespace DB::buffer