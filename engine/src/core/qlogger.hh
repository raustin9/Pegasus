#pragma once
#include "defines.hh"
#include <cstdint>

namespace qlogger 
{
    enum log_level : uint32_t {
        LOG_LEVEL_FATAL = 0,
        LOG_LEVEL_ERROR = 1,
        LOG_LEVEL_WARN = 2,
        LOG_LEVEL_INFO = 3,
        LOG_LEVEL_DEBUG = 4,
        LOG_LEVEL_TRACE = 5,
    };


    bool Initialize();
    void Shutdown();

    void QAPI Fatal(const char* message, ...);
    void QAPI Error(const char* message, ...);
    void QAPI Warn(const char* message, ...);
    void QAPI Info(const char* message, ...);
    void QAPI Debug(const char* message, ...);
    void QAPI Trace(const char* message, ...);
} // qlogger