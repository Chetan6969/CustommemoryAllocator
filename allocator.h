#ifndef CUSTOM_ALLOCATOR_H
#define CUSTOM_ALLOCATOR_H

#include <cstddef>

// Constants
const size_t BLOCK_SIZE = 1024;         // 1KB per block
const size_t TOTAL_BLOCKS = 2048;       // 2MB total
const size_t POOL_SIZE = BLOCK_SIZE * TOTAL_BLOCKS;

// Allocation strategy
enum AllocationStrategy {
    FIRST_FIT,
    BEST_FIT
};

// Block categories (segregated free lists)
enum BlockType {
    SMALL_BLOCK,   // <= 256B
    MEDIUM_BLOCK,  // 257–512B
    LARGE_BLOCK    // 513–1024B
};

// Metadata header for each block
struct BlockHeader {
    bool is_free;          // free/allocated flag
    size_t block_count;    // number of contiguous blocks
    BlockHeader* next;     // next in free list
    BlockHeader* prev;     // previous in free list
    size_t size;           // requested size
    BlockType type;        // small/medium/large
    int index;             // block index in pool
};

// Custom Memory Allocator class
class CustomAllocator {
private:
    char* mem_pool;                // memory pool
    BlockHeader* free_list_head;   // generic free list

    // Segregated free lists
    BlockHeader* free_small;
    BlockHeader* free_medium;
    BlockHeader* free_large;

    AllocationStrategy strategy;   // first fit / best fit

    // Internal helpers
    void initPool();
    BlockType getBlockType(size_t size);
    BlockHeader* findBlock(size_t size);
    void removeFromFreeList(BlockHeader* block);
    void addToFreeList(BlockHeader* block);
    void splitBlock(BlockHeader* block, size_t size);
    void mergeFreeBlocks();

public:
    CustomAllocator();
    ~CustomAllocator();

    // Core functions
    void* xmalloc(size_t size);
    void xfree(void* ptr);

    // New features
    void setStrategy(AllocationStrategy strat);
    void defragment();
    void memoryLeakCheck();
    void printMemoryState();
};

#endif

