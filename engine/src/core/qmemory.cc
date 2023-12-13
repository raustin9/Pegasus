#include "qmemory.hh"
#include "platform/platform.hh"
#include "core/qlogger.hh"
#include <iostream>
#include <memory>
#include <cstdlib>

struct memory_stats {
    uint64_t total_allocated;
    uint64_t tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

struct memory_state {
    memory_stats stats;
    uint64_t num_allocs;
};

static memory_state* state_ptr = nullptr;
// static memory_stats stats {};
// static memory_stats *stats = nullptr;

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "ARRAY      ",
    "LINEAR_ALLOCATOR ",
    "DARRAY     ",
    "DICT       ",
    "RING_QUEUE ",
    "BST        ",
    "STRING     ",
    "APPLICATION",
    "JOB        ",
    "TEXTURE    ",
    "MAT_ISNT   ",
    "RENDERER   ",
    "GAME       ",
    "TRANSFORM  ",
    "ENTITY     ",
    "ENTITY_NODE",
    "SCENE      ",
};

void 
QAllocator::Initialize(uint64_t& memory_requirements, void* state) {
    memory_requirements = sizeof(memory_state);
    if (state == nullptr) {
        return;
    }

    state_ptr = new (static_cast<memory_state*>(state)) memory_state;
    state_ptr->num_allocs = 0;
    QAllocator::Zero(&state_ptr->stats, sizeof(state_ptr->stats));
}

void 
QAllocator::Shutdown() {
    state_ptr = nullptr;
}

void*
QAllocator::Allocate(uint64_t count, uint64_t size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        qlogger::Warn("Allocating using unknown tag");
    }

    if (state_ptr != nullptr) {
        state_ptr->stats.total_allocated += size * count;
        state_ptr->stats.tagged_allocations[tag] += size * count;
        state_ptr->num_allocs++;
    } else {
        qlogger::Warn("QAllocator::Allocate attempted allocation before memory subsystem is initialized");
    }

    // TODO: align memory
    // void* block = malloc(size);
    // QAllocator::Zero(block, size);
    void* block;
    // block = std::calloc(count, size);
    block = Platform::Allocate(size * count, false);
    if (!block) {
        throw std::bad_alloc{}; 
    }

    Platform::ZeroMem(block, count * size);

    return block;
}

template <typename T> void
QAllocator::Delete(T* block, uint64_t size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        qlogger::Warn("Deallocating using unknown tag");
    }

    state_ptr->stats.total_allocated -= size;
    state_ptr->stats.tagged_allocations[tag] -= size;

    // TODO: align memory
    delete[] block;
    
    // free(block);
}

void 
QAllocator::Free(void* block, uint64_t size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        qlogger::Warn("Deallocating using unknown tag");
    }

    if (state_ptr != nullptr) {
        state_ptr->stats.total_allocated -= size;
        state_ptr->stats.tagged_allocations[tag] -= size;
    }

    // TODO: align memory
    free(block);
}

void*
QAllocator::Zero(void* block, uint64_t size) {
    return memset(block, 0, size);
}

void*
QAllocator::Copy(void* dst, const void* source, uint64_t size) {
    return memcpy(dst, source, size);
}

void*
QAllocator::Set(void* dst, int32_t value, uint64_t size) {
    return memset(dst, value, size);
}

std::string 
QAllocator::GetUsageString(){
    const uint64_t gib = 1024 * 1024 * 1024;
    const uint64_t mib = 1024 * 1024;
    const uint64_t kib = 1024;

    char buffer[8000] = "System memory use (tagged):\n";
    uint64_t offset = strlen(buffer);
    
    for (uint64_t i = 0; i < MEMORY_TAG_MAX_TAGS; i++) {
        char unit[4] = "Xib";
        float amount = 1.0f;

        // Check for which unit we should be using
        if (state_ptr->stats.tagged_allocations[i] >= gib) {
            unit[0] = 'G';
            amount = state_ptr->stats.tagged_allocations[i] / (static_cast<float>(gib));
        } else if (state_ptr->stats.tagged_allocations[i] >= mib) {
            unit[0] = 'M';
            amount = state_ptr->stats.tagged_allocations[i] / (static_cast<float>(mib));
        } else if (state_ptr->stats.tagged_allocations[i] >= kib) {
            unit[0] = 'K';
            amount = state_ptr->stats.tagged_allocations[i] / (static_cast<float>(kib));
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = static_cast<float>(state_ptr->stats.tagged_allocations[i]);
        }

        int32_t length = snprintf(buffer + offset, 8000, " %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }

    std::string out_string = buffer;
    return out_string;
}

uint64_t
QAllocator::AllocationCount() {
    return (state_ptr != nullptr) ? state_ptr->num_allocs : 0;
}