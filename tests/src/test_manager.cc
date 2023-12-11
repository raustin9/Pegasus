#include "test_manager.hh"

#include <containers/qvector.inl>
#include <core/qlogger.hh>
#include <chrono>

TestManager::TestManager() {
}

TestManager::~TestManager() {

}

void
TestManager::Register(PFN_test test, const char* desc) {
    test_entry e {};
    e.func = test;
    e.desc = desc;
    tests.push_back(e);
}

void
TestManager::RunTests() {
    size_t passed = 0;
    size_t failed = 0;
    size_t skipped = 0;

    size_t count = tests.size();

    float total_time = 0;
    for (size_t i = 0; i < count; i++) {
        auto test_start = std::chrono::high_resolution_clock::now();
        uint8_t result = tests[i].func();
        auto test_finish = std::chrono::high_resolution_clock::now();
        float test_duration = std::chrono::duration<float, std::chrono::seconds::period>(test_finish - test_start).count();

        if (result == TRUE) {
            passed++;
        } else if (result == BYPASS) {
            qlogger::Warn("[SKIPPED]: %s", tests[i].desc);
            ++skipped;
        } else {
            qlogger::Error("[FAILED]: %s", tests[i].desc);
            ++failed;
        }

        char status[20];
        const char* format = 
            failed ? "*** %d FAILED ***" : "*** SUCCESS ***";
        sprintf(status, format, failed);
        total_time += test_duration;
        qlogger::Info("Executed %d of %d (skipped %d) %s (%.6f sec / %.6f sec total)", i+1, count, skipped, status, test_duration, total_time);
    }
    
    qlogger::Info("Results: %d passed. %d failed. %d skipped. Took %.6f seconds", passed, failed, skipped, total_time);
}