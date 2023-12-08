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

    void Fatal(const char* message, ...);
    void Error(const char* message, ...);
    void Warn(const char* message, ...);
    void Info(const char* message, ...);
    void Debug(const char* message, ...);
    void Trace(const char* message, ...);
} // qlogger