#include "allocator.h"
#include <iostream>
#include <cmath>
#include <cstring>

CustomAllocator::CustomAllocator() {
    mem_pool = new char[POOL_SIZE];
    free_list_head = reinterpret_cast<BlockHeader*>(mem_pool);

    free_list_head->is_free = true;
    free_list_head->block_count = TOTAL_BLOCKS;
    free_list_head->next = nullptr;
}

CustomAllocator::~CustomAllocator() {
    delete[] mem_pool;
}

void* CustomAllocator::xmalloc(size_t size) {
    size_t required_blocks = std::ceil((double)size / BLOCK_SIZE);
    BlockHeader* current = free_list_head;

    while (current) {
        if (current->is_free && current->block_count >= required_blocks) {
            // Split block if needed
            if (current->block_count > required_blocks) {
                BlockHeader* new_block = reinterpret_cast<BlockHeader*>(
                    reinterpret_cast<char*>(current) + required_blocks * BLOCK_SIZE
                );

                new_block->is_free = true;
                new_block->block_count = current->block_count - required_blocks;
                new_block->next = current->next;

                current->next = new_block;
                current->block_count = required_blocks;
            }

            current->is_free = false;
            return reinterpret_cast<char*>(current) + sizeof(BlockHeader);
        }

        current = current->next;
    }

    std::cout << "[xmalloc] ❌ Allocation failed. Not enough memory.\n";
    return nullptr;
}

void CustomAllocator::xfree(void* ptr) {
    if (!ptr) return;

    BlockHeader* block = reinterpret_cast<BlockHeader*>(
        reinterpret_cast<char*>(ptr) - sizeof(BlockHeader)
    );
    block->is_free = true;

    // Merge adjacent free blocks
    BlockHeader* current = free_list_head;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            current->block_count += current->next->block_count;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
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
                      << " | Size: " << current->block_count * BLOCK_SIZE << " bytes\n";
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
    std::cout << "---------------------------------------------\n";
    std::cout << "| Block Addr | Blocks | Size    | Status    |\n";
    std::cout << "---------------------------------------------\n";

    while (current) {
        std::cout << "| "
                  << reinterpret_cast<void*>(current) << " | "
                  << current->block_count << "      | "
                  << current->block_count * BLOCK_SIZE << " B | "
                  << (current->is_free ? "Free      " : "Allocated ")
                  << "|\n";
        current = current->next;
    }

    std::cout << "---------------------------------------------\n";
}

