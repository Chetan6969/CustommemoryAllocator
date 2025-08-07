// ======= allocator.h =======
#ifndef CUSTOM_ALLOCATOR_H
#define CUSTOM_ALLOCATOR_H

#include <cstddef>
#include <ctime>
#include <cstdint>

const size_t BLOCK_SIZE = 1024;                 // 1KB
const size_t POOL_SIZE = 2 * 1024 * 1024;       // 2MB
const size_t TOTAL_BLOCKS = POOL_SIZE / BLOCK_SIZE;

enum AllocationStrategy {
    FIRST_FIT,
    BEST_FIT,
    BUDDY_FIT,
};

struct BlockHeader {
    bool is_free;
    size_t block_count;
    BlockHeader* next;

    unsigned long alloc_id;
    std::time_t timestamp;
};

class CustomAllocator {
private:
    char* memory_pool;
    BlockHeader* free_list_head;
    AllocationStrategy strategy;
    void* splitBlock(BlockHeader* block, size_t blocks);
    void* allocate(size_t blocks);


    unsigned long allocation_counter;
    size_t total_allocated_bytes;
    size_t peak_usage_bytes;

    size_t getBlockIndex(BlockHeader* block) const;

public:
    CustomAllocator();
    ~CustomAllocator();

    void* xmalloc(size_t size);
    void xfree(void* ptr);

    void memoryLeakCheck();
    void printMemoryState() const;

    void setStrategy(AllocationStrategy new_strategy);
    void defragment();
    void showAllocatorStats() const;
    void findBlockByPointer(void* ptr) const;
    size_t largestFreeBlockSize() const;
};

#endif // CUSTOM_ALLOCATOR_H

