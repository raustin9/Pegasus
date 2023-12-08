#include "qmemory.hh"
#include <iostream>
#include <memory>

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
    "STRING",
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

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;
    // printf("GOT HERE\n");

    // TODO: align memory
    // void* block = malloc(size);
    // QAllocator::Zero(block, size);
    void* block;
    block = std::calloc(count, size);
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

char* GetUsageString() {
    return "Test";
}