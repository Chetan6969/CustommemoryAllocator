#ifndef CUSTOM_ALLOCATOR_H
#define CUSTOM_ALLOCATOR_H

#include <cstddef>
#include <ctime>
#include <cstdint>

const size_t BLOCK_SIZE = 32;       // bytes per block
const size_t POOL_SIZE = 1024 * 1024; // 1 MB memory pool
const size_t TOTAL_BLOCKS = POOL_SIZE / BLOCK_SIZE;

enum AllocationStrategy {
    FIRST_FIT,
    BEST_FIT
};

struct BlockHeader {
    bool is_free;
    size_t block_count;
    BlockHeader* next;

    // Extra metadata
    unsigned long alloc_id;  // unique allocation ID
    std::time_t timestamp;   // time of allocation
};

class CustomAllocator {
private:
    char* memory_pool;
    BlockHeader* free_list_head;
    AllocationStrategy strategy;

    // Allocation tracking
    unsigned long allocation_counter;
    size_t total_allocated_bytes;
    size_t peak_usage_bytes;

public:
    CustomAllocator();
    ~CustomAllocator();

    void* xmalloc(size_t size);
    void xfree(void* ptr);

    void memoryLeakCheck();
    void printMemoryState() const; // const-correct

    // New public methods
    void setStrategy(AllocationStrategy new_strategy);
    void defragment();
    void showAllocatorStats() const;
    void findBlockByPointer(void* ptr) const;
    size_t largestFreeBlockSize() const;
};

#endif

