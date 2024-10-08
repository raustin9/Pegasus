#pragma once
#include "defines.hh"
#include <cstdint>
#include <string>

enum memory_tag : uint32_t {
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_LINEAR_ALLOCATOR,
    MEMORY_TAG_DARRAY,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_TAGS,
};

class QAPI QAllocator {
    public:
        static void Initialize(uint64_t& memory_requirements, void* state);
        static void Shutdown();

        static uint64_t AllocationCount();
        static void* Allocate(uint64_t count, uint64_t size, memory_tag tag);
        static void  Free(void* block, uint64_t size, memory_tag tag);
        static void* Zero(void* block, uint64_t size);
        static void* Copy(void* dst, const void* source, uint64_t size);
        static void* Set(void* dst, int32_t value, uint64_t size);
        template <typename T> static void Delete(T* block, uint64_t size, memory_tag tag);
        static std::string GetUsageString();
};