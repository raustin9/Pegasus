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


    /**
     * @brief Set the memory requirement's value to the space needed to allocate the logger system state
     *     If state is nullptr, then we only set memory requirements and return
     *     Call twice, once to get the requirement. Second to initialize the system
     * @return true if we initialized. False if not
    */
    bool Initialize(uint64_t& memory_requirement, void* state);

    /**
     * @brief Set the state to a nullptr
    */
    void Shutdown(void* state);

    void QAPI Fatal(const char* message, ...);
    void QAPI Error(const char* message, ...);
    void QAPI Warn(const char* message, ...);
    void QAPI Info(const char* message, ...);
    void QAPI Debug(const char* message, ...);
    void QAPI Trace(const char* message, ...);
} // qlogger