#include "test_manager.hh"
#include "memory/linear_allocator_tests.hh"
#include <core/qlogger.hh>

int main(void) {
    TestManager manager = TestManager();

    // TODO: Register tests
    linear_allocator_register_tests(manager);

    qlogger::Debug("Starting tests...");

    // Execute tests
    manager.RunTests();

    return 0;
}