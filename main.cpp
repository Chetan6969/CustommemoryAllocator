#include "allocator.h"
#include <iostream>
#include <string>

void showMenu() {
    std::cout << "\n===== Custom Memory Allocator =====\n";
    std::cout << "1. Allocate memory\n";
    std::cout << "2. Free memory\n";
    std::cout << "3. Show memory state\n";
    std::cout << "4. Change allocation strategy\n";
    std::cout << "5. Defragment memory\n";
    std::cout << "6. Show allocator stats\n";
    std::cout << "7. Find block by pointer\n";
    std::cout << "8. Largest free block size\n";
    std::cout << "9. Memory leak check\n";
    std::cout << "0. Exit\n";
    std::cout << "===================================\n";
    std::cout << "Enter choice: ";
}

int main() {
    CustomAllocator allocator;
    void* allocated_ptrs[100] = {nullptr};
    int ptr_count = 0;

    int choice;
    while (true) {
        showMenu();
        std::cin >> choice;

        if (choice == 0) {
            allocator.memoryLeakCheck();
            break;
        }

        switch (choice) {
        case 1: {
            size_t size;
            std::cout << "Enter size to allocate: ";
            std::cin >> size;
            void* ptr = allocator.xmalloc(size);
            if (ptr) {
                allocated_ptrs[ptr_count++] = ptr;
                std::cout << "✅ Allocation successful! Pointer index: " << (ptr_count - 1) << "\n";
            }
            break;
        }
        case 2: {
            int idx;
            std::cout << "Enter pointer index to free: ";
            std::cin >> idx;
            if (idx >= 0 && idx < ptr_count && allocated_ptrs[idx]) {
                allocator.xfree(allocated_ptrs[idx]);
                allocated_ptrs[idx] = nullptr;
                std::cout << "✅ Freed pointer at index " << idx << "\n";
            } else {
                std::cout << "❌ Invalid index or pointer already freed.\n";
            }
            break;
        }
        case 3:
            allocator.printMemoryState();
            break;
        case 4: {
            int strat;
            std::cout << "Enter strategy (1 = First Fit, 2 = Best Fit): ";
            std::cin >> strat;
            allocator.setStrategy(strat == 2 ? BEST_FIT : FIRST_FIT);
            break;
        }
        case 5:
            allocator.defragment();
            break;
        case 6:
            allocator.showAllocatorStats();
            break;
        case 7: {
            int idx;
            std::cout << "Enter pointer index to find: ";
            std::cin >> idx;
            if (idx >= 0 && idx < ptr_count && allocated_ptrs[idx]) {
                allocator.findBlockByPointer(allocated_ptrs[idx]);
            } else {
                std::cout << "❌ Invalid index.\n";
            }
            break;
        }
        case 8:
            std::cout << "📏 Largest free block size: " 
                      << allocator.largestFreeBlockSize() << " bytes\n";
            break;
        case 9:
            allocator.memoryLeakCheck();
            break;
        default:
            std::cout << "❌ Invalid choice.\n";
            break;
        }
    }

    return 0;
}

