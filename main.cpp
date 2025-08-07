// ======= main.cpp =======
// Entry point for Custom Memory Allocator Demo
// This program demonstrates memory allocation techniques using a custom allocator

#include "allocator.h"
#include <iostream>
#include <string>

// Function to display the interactive menu
void showMenu() {
    std::cout << "\n========== Custom Memory Allocator ==========\n";
    std::cout << " 1. Allocate memory\n";
    std::cout << " 2. Free memory\n";
    std::cout << " 3. Show memory state\n";
    std::cout << " 4. Change allocation strategy\n";
    std::cout << " 5. Defragment memory\n";
    std::cout << " 6. Show allocator stats\n";
    std::cout << " 7. Find block by pointer\n";
    std::cout << " 8. Largest free block size\n";
    std::cout << " 9. Memory leak check\n";
    std::cout << " 0. Exit\n";
    std::cout << "=============================================\n";
    std::cout << "  Enter your choice: ";
}

int main() {
    // Create an instance of our custom memory allocator
    CustomAllocator allocator;

    // Array to keep track of all allocated pointers
    void* allocated_ptrs[100] = {nullptr};
    int ptr_count = 0; // Counter to assign index to pointers

    int choice;

    // Menu-driven loop
    while (true) {
        showMenu();
        std::cin >> choice;

        // Option 0: Exit and perform memory leak check
        if (choice == 0) {
            allocator.memoryLeakCheck(); // Check for unreleased memory
            break;
        }

        // Switch-case for menu handling
        switch (choice) {
        case 1: { // Allocate memory
            size_t size;
            std::cout << " Enter size to allocate (in bytes): ";
            std::cin >> size;

            void* ptr = allocator.xmalloc(size); // Custom allocation
            if (ptr) {
                allocated_ptrs[ptr_count++] = ptr; // Track allocated pointer
                std::cout << " Memory allocated. Pointer Index: " << (ptr_count - 1) << "\n";
            } else {
                std::cout << " Allocation failed.\n";
            }
            break;
        }

        case 2: { // Free memory
            int idx;
            std::cout << " Enter pointer index to free: ";
            std::cin >> idx;

            if (idx >= 0 && idx < ptr_count && allocated_ptrs[idx]) {
                allocator.xfree(allocated_ptrs[idx]);  // Free the memory
                allocated_ptrs[idx] = nullptr;         // Mark as freed
                std::cout << " Freed memory at index " << idx << "\n";
            } else {
                std::cout << " Invalid index or pointer already freed.\n";
            }
            break;
        }

        case 3: // Show current state of memory blocks
            std::cout << "\n Showing memory state:\n";
            allocator.printMemoryState();
            break;

        case 4: { // Change allocation strategy
            int strat;
            std::cout << "   Select strategy:\n";
            std::cout << "    1 = First Fit\n";
            std::cout << "    2 = Best Fit\n";
            std::cout << "    3 = Buddy System\n";
            std::cout << "  Enter strategy number: ";
            std::cin >> strat;

            if (strat >= 1 && strat <= 3)
                allocator.setStrategy(static_cast<AllocationStrategy>(strat - 1));
            else
                std::cout << "  Invalid strategy selected.\n";
            break;
        }

        case 5: // Defragment memory to merge adjacent free blocks
            std::cout << "  Running defragmentation...\n";
            allocator.defragment();
            break;

        case 6: // Show memory statistics
            std::cout << "\n Allocator Statistics:\n";
            allocator.showAllocatorStats();
            break;

        case 7: { // Find block based on pointer index
            int idx;
            std::cout << " Enter pointer index to find: ";
            std::cin >> idx;

            if (idx >= 0 && idx < ptr_count && allocated_ptrs[idx]) {
                allocator.findBlockByPointer(allocated_ptrs[idx]);
            } else {
                std::cout << " Invalid index.\n";
            }
            break;
        }

        case 8: // Show largest available free memory block
            std::cout << "  Largest free block size: "
                      << allocator.largestFreeBlockSize() << " bytes\n";
            break;

        case 9: // Manually trigger memory leak check
            std::cout << " Running memory leak check...\n";
            allocator.memoryLeakCheck();
            break;

        default:
            std::cout << " Invalid choice. Please select between 0 - 9.\n";
            break;
        }
    }

    return 0;
}

