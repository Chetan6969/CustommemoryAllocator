#include "allocator.h"
#include <iostream>
#include <vector>

int main() {
    CustomAllocator allocator;
    std::vector<void*> allocatedPointers;

    while (true) {
        std::cout << "\n🔧 ===== Custom Memory Allocator Menu ===== 🔧\n";
        std::cout << "1️⃣  Allocate Memory\n";
        std::cout << "2️⃣  Free Memory\n";
        std::cout << "3️⃣  Show Allocator State\n";
        std::cout << "4️⃣  Check for Memory Leaks\n";
        std::cout << "5️⃣  Exit\n";
        std::cout << "➡️  Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
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
            case 2: {
                std::cout << "📤 Enter index of pointer to free (0 - " << allocatedPointers.size()-1 << "): ";

		// in this code snippet that is written below, is for checking the which index to be freed
		std::cout << "🔢\n Active Allocations:\n";
		for (size_t i = 0; i < allocatedPointers.size(); ++i) {
	            if (allocatedPointers[i] != nullptr) {
		        std::cout << "🔹 Index " << i << " -> " << allocatedPointers[i] << "\n";
		    }
		}

                size_t index;
                std::cin >> index;
                if (index < allocatedPointers.size() && allocatedPointers[index]) {
                    allocator.xfree(allocatedPointers[index]);
                    std::cout << "✅ Memory at index " << index << " freed.\n";
                    allocatedPointers[index] = nullptr;
                } else {
                    std::cout << "❌ Invalid index or already freed.\n";
                }
                break;
            }
            case 3:
                allocator.printMemoryState();
                break;
            case 4:
                allocator.memoryLeakCheck();
                break;
            case 5:
                std::cout << "👋 Exiting...\n";
                return 0;
            default:
                std::cout << "❗ Invalid choice. Try again.\n";
        }
    }
}

