#include "qmemory.hh"
#include "platform/platform.hh"
#include <iostream>
#include <memory>
#include <cstdlib>

struct memory_stats {
    uint64_t total_allocated;
    uint64_t tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static memory_stats stats {};

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "ARRAY      ",
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
QAllocator::Initialize() {
    QAllocator::Zero(&stats, sizeof(stats));
}

void 
QAllocator::Shutdown() {

}

void*
QAllocator::Allocate(uint64_t count, uint64_t size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        std::cout << "WARN: Allocating using unknown tag" << std::endl;
    }

    stats.total_allocated += size * count;
    stats.tagged_allocations[tag] += size * count;
    // printf("GOT HERE\n");

    // TODO: align memory
    // void* block = malloc(size);
    // QAllocator::Zero(block, size);
    void* block;
    // block = std::calloc(count, size);
    block = Platform::Allocate(size * count, false);
    if (!block) {
        throw std::bad_alloc{}; 
    }

    return block;
}

void 
QAllocator::Free(void* block, uint64_t size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        std::cout << "WARN: Deallocating using unknown tag" << std::endl;
    }

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

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
        if (stats.tagged_allocations[i] >= gib) {
            unit[0] = 'G';
            amount = stats.tagged_allocations[i] / (static_cast<float>(gib));
        } else if (stats.tagged_allocations[i] >= mib) {
            unit[0] = 'M';
            amount = stats.tagged_allocations[i] / (static_cast<float>(mib));
        } else if (stats.tagged_allocations[i] >= kib) {
            unit[0] = 'K';
            amount = stats.tagged_allocations[i] / (static_cast<float>(kib));
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = static_cast<float>(stats.tagged_allocations[i]);
        }

        int32_t length = snprintf(buffer + offset, 8000, " %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }

    std::string out_string = buffer;
    return out_string;
}