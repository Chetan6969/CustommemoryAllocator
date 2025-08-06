// ======= allocator.cpp =======
#include "allocator.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <iomanip>

CustomAllocator::CustomAllocator() {
    memory_pool = new char[POOL_SIZE];
    free_list_head = reinterpret_cast<BlockHeader*>(memory_pool);

    free_list_head->is_free = true;
    free_list_head->block_count = TOTAL_BLOCKS;
    free_list_head->next = nullptr;
    free_list_head->alloc_id = 0;
    free_list_head->timestamp = 0;

    strategy = FIRST_FIT;
    allocation_counter = 0;
    total_allocated_bytes = 0;
    peak_usage_bytes = 0;
}

CustomAllocator::~CustomAllocator() {
    delete[] memory_pool;
}

size_t CustomAllocator::getBlockIndex(BlockHeader* block) const {
    return (reinterpret_cast<char*>(block) - memory_pool) / BLOCK_SIZE;
}

void* CustomAllocator::xmalloc(size_t size) {
    if (size == 0) return nullptr;

    size_t total_size = size + sizeof(BlockHeader);
    size_t blocks_needed = (total_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    BlockHeader* prev = nullptr;
    BlockHeader* current = free_list_head;
    BlockHeader* chosen = nullptr;
    BlockHeader* chosen_prev = nullptr;

    if (strategy == FIRST_FIT) {
        while (current) {
            if (current->is_free && current->block_count >= blocks_needed) {
                chosen = current;
                chosen_prev = prev;
                break;
            }
            prev = current;
            current = current->next;
        }
    } else if (strategy == BEST_FIT) {
        size_t smallest_fit = SIZE_MAX;
        while (current) {
            if (current->is_free && current->block_count >= blocks_needed &&
                current->block_count < smallest_fit) {
                smallest_fit = current->block_count;
                chosen = current;
                chosen_prev = prev;
            }
            prev = current;
            current = current->next;
        }
    }

    if (!chosen) {
        std::cout << "❌ Allocation failed: Not enough memory.\n";
        return nullptr;
    }

    if (chosen->block_count > blocks_needed) {
        BlockHeader* new_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(chosen) + blocks_needed * BLOCK_SIZE);
        new_block->is_free = true;
        new_block->block_count = chosen->block_count - blocks_needed;
        new_block->next = chosen->next;
        new_block->alloc_id = 0;
        new_block->timestamp = 0;

        chosen->next = new_block;
        chosen->block_count = blocks_needed;
    }

    chosen->is_free = false;
    chosen->alloc_id = ++allocation_counter;
    chosen->timestamp = std::time(nullptr);

    total_allocated_bytes += chosen->block_count * BLOCK_SIZE;
    if (total_allocated_bytes > peak_usage_bytes) {
        peak_usage_bytes = total_allocated_bytes;
    }

    size_t start_sector = getBlockIndex(chosen);
    size_t end_sector = start_sector + chosen->block_count - 1;

    std::cout << "\n✅ Allocation successful:\n";
    std::cout << "   ID: " << chosen->alloc_id
              << " | Blocks: " << chosen->block_count
              << " | Size: " << (chosen->block_count * BLOCK_SIZE) << " bytes\n";
    std::cout << "   Start Block: " << start_sector
              << " | End Block: " << end_sector
              << " | Timestamp: " << std::ctime(&chosen->timestamp);

    return reinterpret_cast<char*>(chosen) + sizeof(BlockHeader);
}

void CustomAllocator::xfree(void* ptr) {
    if (!ptr) return;

    BlockHeader* block = reinterpret_cast<BlockHeader*>(
        reinterpret_cast<char*>(ptr) - sizeof(BlockHeader));

    if (block->is_free) {
        std::cout << "[xfree] ⚠️ Double free detected for block ID " << block->alloc_id << "\n";
        return;
    }

    total_allocated_bytes -= block->block_count * BLOCK_SIZE;
    block->is_free = true;
    block->alloc_id = 0;
    block->timestamp = 0;

    if (block->next && block->next->is_free) {
        block->block_count += block->next->block_count;
        block->next = block->next->next;
    }
}

void CustomAllocator::memoryLeakCheck() {
    bool leak_found = false;
    BlockHeader* current = reinterpret_cast<BlockHeader*>(memory_pool);
    while (current) {
        if (!current->is_free) {
            leak_found = true;
            std::cout << "💧 Leak: ID " << current->alloc_id
                      << " | Size: " << (current->block_count * BLOCK_SIZE)
                      << " bytes | Blocks: " << current->block_count
                      << " | Start: Block " << getBlockIndex(current) << "\n";
        }
        current = current->next;
    }
    if (!leak_found) {
        std::cout << "✅ No memory leaks detected.\n";
    }
}

void CustomAllocator::printMemoryState() const {
    BlockHeader* current = reinterpret_cast<BlockHeader*>(memory_pool);
    size_t index = 0;
    while (current) {
        std::string status = current->is_free ? "Free" : "Allocated";
        size_t start_sector = getBlockIndex(current);
        size_t end_sector = start_sector + current->block_count - 1;

        std::cout << "[Block " << index++ << "] Status: " << status
                  << " | ID: " << current->alloc_id
                  << " | Size: " << (current->block_count * BLOCK_SIZE) << " bytes"
                  << " | Blocks: " << current->block_count
                  << "\n   Start Sector: " << start_sector
                  << " | End Sector: " << end_sector
                  << " | Time: " << (current->timestamp ? std::ctime(&current->timestamp) : "N/A");
        current = current->next;
    }
}

void CustomAllocator::setStrategy(AllocationStrategy new_strategy) {
    strategy = new_strategy;
    std::string name = (strategy == FIRST_FIT) ? "First Fit" :
                       (strategy == BEST_FIT) ? "Best Fit" : "Buddy System";
    std::cout << "\n🔄 Strategy changed to: " << name << "\n";
}

void CustomAllocator::defragment() {
    BlockHeader* current = reinterpret_cast<BlockHeader*>(memory_pool);
    bool defragged = false;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            std::cout << "🧱 Merging free blocks at sector " << getBlockIndex(current)
                      << " and " << getBlockIndex(current->next) << "\n";
            current->block_count += current->next->block_count;
            current->next = current->next->next;
            defragged = true;
        } else {
            current = current->next;
        }
    }
    std::cout << (defragged ? "🧹 Memory defragmentation complete.\n" : "ℹ️ No fragmentation found.\n");
}

void CustomAllocator::showAllocatorStats() const {
    std::cout << "📊 Allocator Stats:\n";
    std::cout << "   Total allocated bytes: " << total_allocated_bytes << "\n";
    std::cout << "   Peak usage bytes: " << peak_usage_bytes << "\n";
}

void CustomAllocator::findBlockByPointer(void* ptr) const {
    BlockHeader* current = reinterpret_cast<BlockHeader*>(memory_pool);
    while (current) {
        void* user_ptr = reinterpret_cast<char*>(current) + sizeof(BlockHeader);
        if (user_ptr == ptr) {
            std::cout << "🔍 Block found: Size: " << current->block_count * BLOCK_SIZE
                      << " bytes | Free: " << (current->is_free ? "Yes" : "No")
                      << " | ID: " << current->alloc_id << "\n";
            return;
        }
        current = current->next;
    }
    std::cout << "❌ Pointer not found in allocator.\n";
}

size_t CustomAllocator::largestFreeBlockSize() const {
    size_t max_size = 0;
    BlockHeader* current = reinterpret_cast<BlockHeader*>(memory_pool);
    while (current) {
        if (current->is_free) {
            max_size = std::max(max_size, current->block_count * BLOCK_SIZE);
        }
        current = current->next;
    }
    return max_size;
}

