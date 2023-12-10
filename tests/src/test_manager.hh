#pragma once
#include <defines.hh>
#include <cstdint>
#include <vector>
#include <functional>

#define BYPASS 2

// typedef uint8_t (*PFN_test)();
typedef std::function<bool()> PFN_test;

struct test_entry {
    PFN_test func;
    char* desc;
};

class TestManager {
public:
    TestManager();
    ~TestManager();

    void Register(PFN_test, char* desc);
    void RunTests();

    std::vector<test_entry> tests;
};