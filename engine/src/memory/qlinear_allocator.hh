#pragma once
#include "defines.hh"
#include <cstdint>

namespace qmemory {
    // Linear Allocator
    struct QAPI QLinearAllocator {
        uint64_t total_size;
        uint64_t allocated;
        void* memory;
        bool owns_memory;

        void Create(uint64_t total_size, void* memory);
        void Destroy();

        void* Allocate(uint64_t size);
        void FreeAll();
    };
}