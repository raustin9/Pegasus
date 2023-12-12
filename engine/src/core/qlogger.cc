#include "qlogger.hh"
#include "qmemory.hh"
#include "platform/platform.hh"
#include <stdarg.h>
#include <cstdint>

namespace qlogger 
{
    struct logger_system_state {
        bool initialized;
    };

    static logger_system_state* state_ptr;

    bool 
    Initialize(uint64_t& memory_requirement, void* state) {
        memory_requirement = sizeof(logger_system_state);
        if (state == nullptr) {
            return false;
        }
        // TODO: be able to configure files and other outputs
        state_ptr = static_cast<logger_system_state*>(state);
        state_ptr->initialized = true;
        return true;
    }

    // Release the state pointer. 
    // This memory is owned somewhere else
    void 
    Shutdown(void* state) {
        state_ptr = nullptr;
        // TODO: likely close file descriptors
        return;
    }

    void log_output(log_level level, const char* message, va_list args) {
        const char* level_strings[6] = {
            "[FATAL]",
            "[ERROR]",
            "[WARN]",
            "[INFO]",
            "[DEBUG]",
            "[TRACE]",
        };

        bool is_error = level > LOG_LEVEL_WARN;

        // Masive buffer to avoid runtime allcoations that are slow
        constexpr size_t message_length = 32000;
        char error_message[message_length];
        QAllocator::Zero(error_message, message_length);

        // Format the message
        vsnprintf(error_message, message_length, message, args);
        // __builtin_va_list arg_ptr;
        // va_start(arg_ptr, message);
        // vsnprintf(error_message, message_length, message, arg_ptr);
        // va_end(arg_ptr);

        // Prepend the error message level to the message string
        char out_message[message_length];
        QAllocator::Zero(out_message, message_length);
        sprintf(out_message, "%s%s\n", level_strings[level], error_message);

        // Print the error message
        if (is_error) {
            Platform::ConsoleError(out_message, level);
        } else {
            Platform::ConsoleWrite(out_message, level);
        }
    }

    void 
    Fatal(const char* message, ...) {
        va_list args;
        va_start(args, message);
        log_output(LOG_LEVEL_FATAL, message, args);
        va_end(args);
    }

    void 
    Error(const char* message, ...) {
        va_list args;
        va_start(args, message);
        log_output(LOG_LEVEL_ERROR, message, args);
        va_end(args);
    }

    void 
    Warn(const char* message, ...) {
        va_list args;
        va_start(args, message);
        log_output(LOG_LEVEL_WARN, message, args);
        va_end(args);
    }

    void 
    Info(const char* message, ...) {
        va_list args;
        va_start(args, message);
        log_output(LOG_LEVEL_INFO, message, args);
        va_end(args);
    }

    void 
    Debug(const char* message, ...) {
        va_list args;
        va_start(args, message);
        log_output(LOG_LEVEL_DEBUG, message, args);
        va_end(args);
    }

    void 
    Trace(const char* message, ...) {
        va_list args;
        va_start(args, message);
        log_output(LOG_LEVEL_TRACE, message, args);
        va_end(args);
    }

} // logger