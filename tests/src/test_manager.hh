#pragma once
#include <defines.hh>
#include <cstdint>
#include <vector>
#include <functional>

#define BYPASS 2

// typedef uint8_t (*PFN_test)();
typedef std::function<uint8_t()> PFN_test;
#define TRUE 1
#define FALSE 0

struct test_entry {
    PFN_test func;
    const char* desc;
};

class TestManager {
public:
    TestManager();
    ~TestManager();

    void Register(PFN_test, const char* desc);
    void RunTests();

    std::vector<test_entry> tests;
};