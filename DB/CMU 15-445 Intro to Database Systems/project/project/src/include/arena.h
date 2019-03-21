// refer to https://github.com/google/leveldb/blob/master/util/arena.h
#ifndef _ARENA_H
#define _ARENA_H
#include <deque>
#include <atomic>

namespace DB::buffer
{

    class Arena
    {

    public:

        static constexpr uint32_t kBlockSize = 4096;

        Arena();
        ~Arena();
        Arena(const Arena&) = delete;
        Arena& operator=(const Arena&) = delete;

        // Return a pointer to a newly allocated memory block of "bytes" bytes.
        char* Allocate(uint32_t bytes);

        // Returns an estimate of the total memory usage of data allocated
        // by the arena.
        uint64_t MemoryUsage() const { return memory_usage_.load(std::memory_order_relaxed); }


    private:

        char* alloc_ptr_;                       // current block allocation position.
        uint32_t alloc_bytes_remaining_;        // current block remaining.
        std::deque<char*> blocks_;              // Array of new[] allocated memory blocks.
        std::atomic<uint64_t>  memory_usage_;   // Total memory usage of the arena.

        char* AllocateFallback(uint32_t bytes);

        char* AllocateNewBlock(uint32_t block_bytes);

    }; // end class Arena


    inline char* Arena::Allocate(uint32_t bytes) {
        // The semantics of what to return are a bit messy if we allow
        // 0-byte allocations, so we disallow them here (we don't need
        // them for our internal use).
        if (bytes <= alloc_bytes_remaining_) {
            char* result = alloc_ptr_;
            alloc_ptr_ += bytes;
            alloc_bytes_remaining_ -= bytes;
            return result;
        }
        return AllocateFallback(bytes);
    }

} // end namespace DB::buffer

#endif // !_ARENA_H
