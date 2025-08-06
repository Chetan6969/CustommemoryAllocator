#include "allocator.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

void showStats(const CustomAllocator &allocator) {
    allocator.printMemoryState();
}

int main() {
    CustomAllocator allocator;
    std::vector<void*> allocatedPointers;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    while (true) {
        std::cout << "\n🔧 ===== Custom Memory Allocator Menu ===== 🔧\n";
        std::cout << "1️⃣  Allocate Memory\n";
        std::cout << "2️⃣  Free Memory\n";
        std::cout << "3️⃣  Show Allocator State\n";
        std::cout << "4️⃣  Check for Memory Leaks\n";
        std::cout << "5️⃣  Change Allocation Strategy (First Fit / Best Fit)\n";
        std::cout << "6️⃣  Defragment Memory\n";
        std::cout << "7️⃣  Show Allocation Stats\n";
        std::cout << "8️⃣  Stress Test Allocator\n";
        std::cout << "9️⃣  Find Block by Pointer\n";
        std::cout << "0️⃣  Exit\n";
        std::cout << "➡️  Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: { // Allocate
                std::cout << "📥 Enter size in bytes to allocate: ";
                size_t size;
                std::cin >> size;
                void* ptr = allocator.xmalloc(size);
                if (ptr) {
                    std::cout << "✅ Allocated " << size << " bytes at address: " << ptr << "\n";
                    allocatedPointers.push_back(ptr);
                }
                break;
            }
            case 2: { // Free
                if (allocatedPointers.empty()) {
                    std::cout << "⚠️ No active allocations.\n";
                    break;
                }
                std::cout << "📤 Enter index of pointer to free (0 - " << allocatedPointers.size()-1 << "):\n";
                std::cout << "🔢 Active Allocations:\n";
                for (size_t i = 0; i < allocatedPointers.size(); ++i) {
                    if (allocatedPointers[i] != nullptr) {
                        std::cout << "🔹 Index " << i << " -> " << allocatedPointers[i] << "\n";
                    }
                }
                size_t idx;
                std::cin >> idx;
                if (idx < allocatedPointers.size() && allocatedPointers[idx] != nullptr) {
                    allocator.xfree(allocatedPointers[idx]);
                    allocatedPointers[idx] = nullptr;
                    std::cout << "✅ Freed pointer at index " << idx << "\n";
                } else {
                    std::cout << "❌ Invalid index or already freed.\n";
                }
                break;
            }
            case 3: // Show state
                allocator.printMemoryState();
                break;
            case 4: // Leak check
                allocator.memoryLeakCheck();
                break;
            case 5: { // Change strategy
                std::cout << "Select Allocation Strategy:\n1️⃣ First Fit\n2️⃣ Best Fit\nChoice: ";
                int strat;
                std::cin >> strat;
                allocator.setStrategy(strat == 2 ? BEST_FIT : FIRST_FIT);
                std::cout << "✅ Strategy changed.\n";
                break;
            }
            case 6: // Defragment
                allocator.defragment();
                break;
            case 7: { // Stats
                allocator.showAllocatorStats();
                break;
            }
            case 8: { // Stress test
                std::cout << "🚀 Running stress test...\n";
                for (int i = 0; i < 50; ++i) {
                    size_t size = (std::rand() % 1024) + 1;
                    void* ptr = allocator.xmalloc(size);
                    if (ptr) allocatedPointers.push_back(ptr);
                    if (i % 3 == 0 && !allocatedPointers.empty()) {
                        size_t idx = std::rand() % allocatedPointers.size();
                        if (allocatedPointers[idx] != nullptr) {
                            allocator.xfree(allocatedPointers[idx]);
                            allocatedPointers[idx] = nullptr;
                        }
                    }
                }
                std::cout << "✅ Stress test complete.\n";
                break;
            }
            case 9: { // Find block
                std::cout << "Enter pointer address (in hex format, e.g. 0x7ffee1234): ";
                void* queryPtr;
                std::cin >> queryPtr;
                allocator.findBlockByPointer(queryPtr);
                break;
            }
            case 0:
                std::cout << "👋 Exiting...\n";
                return 0;
            default:
                std::cout << "❌ Invalid choice.\n\n";
        }
    }
}

