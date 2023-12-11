#include "linear_allocator_tests.hh"
#include "../test_manager.hh"
#include "../expect.hh"

#include <memory/qlinear_allocator.hh>
#include <defines.hh>

uint8_t linear_allocator_should_create_and_destroy() {
    qmemory::QLinearAllocator alloc;
    alloc.Create(sizeof(uint64_t), NULL);

    expect_should_not_be(0, alloc.memory);
    expect_should_be(sizeof(uint64_t), alloc.total_size);
    expect_should_be(0, alloc.allocated);

    alloc.Destroy();

    expect_should_be(0, alloc.memory);
    expect_should_be(0, alloc.total_size);
    expect_should_be(0, alloc.allocated);

    return TRUE;
}

uint8_t linear_allocator_multi_allocation_all_space() {
    uint64_t max_allocs = 1024;
    qmemory::QLinearAllocator alloc;
    alloc.Create(sizeof(uint64_t) * max_allocs, nullptr);

    // Multiple allocations - full
    void* block;
    for (uint64_t i = 0; i < max_allocs; i++) {
        block = alloc.Allocate(sizeof(uint64_t));
        expect_should_not_be(0, block);
        expect_should_be(sizeof(uint64_t) * (i+1), alloc.allocated);
    }

    alloc.Destroy();
    return true;
}

uint8_t linear_allocator_multi_allocation_over_allocate() {
    uint64_t max_allocs = 1024;
    qmemory::QLinearAllocator alloc;
    alloc.Create(sizeof(uint64_t) * max_allocs, nullptr);

    // Multiple allocations - full
    void* block;
    for (uint64_t i = 0; i < max_allocs; i++) {
        block = alloc.Allocate(sizeof(uint64_t));
        expect_should_not_be(0, block);
        expect_should_be(sizeof(uint64_t) * (i+1), alloc.allocated);
    }

    qlogger::Debug("Following error is meant to be caused by test");
    block = alloc.Allocate(sizeof(uint64_t));
    expect_should_be(0, block);
    expect_should_be(sizeof(uint64_t) * max_allocs, alloc.allocated);

    alloc.Destroy();
    return true;
}

uint8_t linear_allocator_multi_allocation_all_space_then_free() {
    uint64_t max_allocs = 1024;
    qmemory::QLinearAllocator alloc;
    alloc.Create(sizeof(uint64_t) * max_allocs, nullptr);

    void* block;
    for (uint64_t i = 0; i < max_allocs; i++) {
        block = alloc.Allocate(sizeof(uint64_t));
        expect_should_not_be(0, block);
        expect_should_be(sizeof(uint64_t) * (i+1), alloc.allocated);
    }

    alloc.FreeAll();
    expect_should_be(0, alloc.allocated);

    alloc.Destroy();
    return true;
}

uint8_t linear_allocator_simple_allocation_all_space() {
    qmemory::QLinearAllocator alloc;
    alloc.Create(sizeof(uint64_t), nullptr);

    void* block = alloc.Allocate(sizeof(uint64_t));
    expect_should_not_be(0, block);
    expect_should_be(sizeof(uint64_t), alloc.allocated);

    alloc.Destroy();
    return true;
}

void
linear_allocator_register_tests(TestManager& manager) {
    manager.Register(linear_allocator_should_create_and_destroy, "linear allocator should create and destroy");
    manager.Register(linear_allocator_multi_allocation_all_space, "linear allocator multi alloc for all space");
    manager.Register(linear_allocator_simple_allocation_all_space, "linear allocator single alloc for all space");
    manager.Register(linear_allocator_multi_allocation_all_space_then_free, "linear allocator allocated should be 0 after FreeAll");
    manager.Register(linear_allocator_multi_allocation_over_allocate, "linear allocator try to over allocate");
}