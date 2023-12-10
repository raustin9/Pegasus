#include "linear_allocator_tests.hh"
#include "../test_manager.hh"
#include "../expect.hh"

#include <memory/qlinear_allocator.hh>
#include <defines.hh>

bool linear_allocator_should_create_and_destroy() {
    qmemory::QLinearAllocator alloc;
    alloc.Create(sizeof(uint64_t), NULL);

    expect_should_not_be(0, alloc.memory);
    expect_should_be(sizeof(uint64_t), alloc.total_size);
    expect_should_be(0, alloc.allocated);

    alloc.Destroy();

    expect_should_be(0, alloc.memory);
    expect_should_be(0, alloc.total_size);
    expect_should_be(0, alloc.allocated);

    return true;

}

void
linear_allocator_register_tests(TestManager& manager) {
    manager.Register(linear_allocator_should_create_and_destroy, "linear allocator should create and destroy");
}