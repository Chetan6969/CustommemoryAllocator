#include "allocator.h"
#include <iostream>
#include <cmath>
#include <cstring>

CustomAllocator::CustomAllocator() {
    mem_pool = new char[POOL_SIZE];
    strategy = FIRST_FIT; // default strategy

    initPool();
}

void CustomAllocator::initPool() {
    // Initialize the single free block covering the entire pool
    free_list_head = reinterpret_cast<BlockHeader*>(mem_pool);
    free_list_head->is_free = true;
    free_list_head->block_count = TOTAL_BLOCKS;
    free_list_head->next = nullptr;
    free_list_head->prev = nullptr;
    free_list_head->size = 0;
    free_list_head->type = LARGE_BLOCK;
    free_list_head->index = 0;

    // Initially, all memory is "large block" category
    free_small = nullptr;
    free_medium = nullptr;
    free_large = free_list_head;
}

CustomAllocator::~CustomAllocator() {
    delete[] mem_pool;
}

BlockType CustomAllocator::getBlockType(size_t size) {
    if (size <= 256) return SMALL_BLOCK;
    if (size <= 512) return MEDIUM_BLOCK;
    return LARGE_BLOCK;
}

void CustomAllocator::setStrategy(AllocationStrategy strat) {
    strategy = strat;
}

BlockHeader* CustomAllocator::findBlock(size_t size) {
    size_t required_blocks = static_cast<size_t>(
        std::ceil((double)size / BLOCK_SIZE)
    );

    BlockHeader* best = nullptr;
    BlockHeader* current = free_list_head;

    if (strategy == FIRST_FIT) {
        while (current) {
            if (current->is_free && current->block_count >= required_blocks) {
                return current;
            }
            current = current->next;
        }
    } else if (strategy == BEST_FIT) {
        size_t min_diff = SIZE_MAX;
        while (current) {
            if (current->is_free && current->block_count >= required_blocks) {
                size_t diff = current->block_count - required_blocks;
                if (diff < min_diff) {
                    min_diff = diff;
                    best = current;
                }
            }
            current = current->next;
        }
        return best;
    }

    return nullptr;
}

void CustomAllocator::removeFromFreeList(BlockHeader* block) {
    if (block->prev) block->prev->next = block->next;
    else free_list_head = block->next;

    if (block->next) block->next->prev = block->prev;
}

void CustomAllocator::addToFreeList(BlockHeader* block) {
    block->is_free = true;
    block->next = free_list_head;
    block->prev = nullptr;
    if (free_list_head) free_list_head->prev = block;
    free_list_head = block;
}

void CustomAllocator::splitBlock(BlockHeader* block, size_t size) {
    size_t required_blocks = static_cast<size_t>(
        std::ceil((double)size / BLOCK_SIZE)
    );

    if (block->block_count > required_blocks) {
        BlockHeader* new_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(block) + required_blocks * BLOCK_SIZE
        );

        new_block->is_free = true;
        new_block->block_count = block->block_count - required_blocks;
        new_block->next = block->next;
        new_block->prev = block;
        new_block->size = 0;
        new_block->type = LARGE_BLOCK;
        new_block->index = block->index + required_blocks;

        if (block->next) block->next->prev = new_block;
        block->next = new_block;
        block->block_count = required_blocks;
    }
}

void* CustomAllocator::xmalloc(size_t size) {
    if (size == 0) return nullptr;

    BlockType type = getBlockType(size);
    BlockHeader* block = findBlock(size);

    if (!block) {
        std::cout << "[xmalloc] ❌ Allocation failed. Not enough memory.\n";
        return nullptr;
    }

    splitBlock(block, size);
    block->is_free = false;
    block->size = size;
    block->type = type;

    return reinterpret_cast<char*>(block) + sizeof(BlockHeader);
}

void CustomAllocator::xfree(void* ptr) {
    if (!ptr) return;

    BlockHeader* block = reinterpret_cast<BlockHeader*>(
        reinterpret_cast<char*>(ptr) - sizeof(BlockHeader)
    );

    block->is_free = true;

    mergeFreeBlocks();
}

void CustomAllocator::mergeFreeBlocks() {
    BlockHeader* current = free_list_head;
    while (current && current->next) {
        char* end_of_current = reinterpret_cast<char*>(current) + (current->block_count * BLOCK_SIZE);
        if (current->is_free && current->next->is_free &&
            reinterpret_cast<char*>(current->next) == end_of_current) {

            current->block_count += current->next->block_count;
            current->next = current->next->next;
            if (current->next) current->next->prev = current;
        } else {
            current = current->next;
        }
    }
}

void CustomAllocator::defragment() {
    std::cout << "[Defragment] 🔄 Defragmenting memory...\n";
    mergeFreeBlocks();
}

void CustomAllocator::memoryLeakCheck() {
    BlockHeader* current = free_list_head;
    int leak_count = 0;

    std::cout << "\n📦 Memory Leak Report:\n";
    std::cout << "---------------------------------------------\n";

    while (current) {
        if (!current->is_free) {
            void* ptr = reinterpret_cast<void*>(reinterpret_cast<char*>(current) + sizeof(BlockHeader));
            std::cout << "🚨 Leaked Block at " << ptr
                      << " | Blocks: " << current->block_count
                      << " | Size: " << current->size
                      << " | Type: " << (current->type == SMALL_BLOCK ? "Small" :
                                        current->type == MEDIUM_BLOCK ? "Medium" : "Large")
                      << "\n";
            leak_count++;
        }
        current = current->next;
    }

    if (leak_count == 0)
        std::cout << "✅ No memory leaks detected.\n";

    std::cout << "---------------------------------------------\n";
}

void CustomAllocator::printMemoryState() {
    BlockHeader* current = free_list_head;
    std::cout << "\n📊 Current Memory State:\n";
    std::cout << "-----------------------------------------------------------\n";
    std::cout << "| Block Addr | Blocks | Size    | Status    | Type   | Idx |\n";
    std::cout << "-----------------------------------------------------------\n";

    while (current) {
        std::cout << "| "
                  << reinterpret_cast<void*>(current) << " | "
                  << current->block_count << "      | "
                  << current->block_count * BLOCK_SIZE << " B | "
                  << (current->is_free ? "Free      " : "Allocated ")
                  << " | "
                  << (current->type == SMALL_BLOCK ? "Small " :
                      current->type == MEDIUM_BLOCK ? "Medium" : "Large ")
                  << " | "
                  << current->index
                  << " |\n";
        current = current->next;
    }

    std::cout << "-----------------------------------------------------------\n";
}

