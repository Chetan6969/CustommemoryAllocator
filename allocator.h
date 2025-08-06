#ifndef CUSTOM_ALLOCATOR_H
#define CUSTOM_ALLOCATOR_H

#include <cstddef>

// Block + Pool size
const size_t BLOCK_SIZE = 1024; // 1KB
const size_t TOTAL_BLOCKS = 2048; // 2MB = 2048 * 1024
const size_t POOL_SIZE = BLOCK_SIZE * TOTAL_BLOCKS;

// Metadata header for each block
struct BlockHeader {
    bool is_free;
    size_t block_count;
    BlockHeader* next;
};

// Custom Memory Allocator class
class CustomAllocator {
private:
    char* mem_pool;
    BlockHeader* free_list_head;

public:
    CustomAllocator();
    ~CustomAllocator();

    void* xmalloc(size_t size);      // Custom malloc
    void xfree(void* ptr);           // Custom free
    void memoryLeakCheck();          // Leak detection
    void printMemoryState();         // Debug/State UI

    // Future extensions here (e.g. allocation strategy switch)
};

#endif

