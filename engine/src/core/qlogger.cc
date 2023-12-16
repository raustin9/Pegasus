#include "qlogger.hh"
#include "qmemory.hh"
#include "platform/platform.hh"
#include "platform/file_system.hh"
#include <stdarg.h>
#include <cstdint>
#include <string>

namespace qlogger 
{
    struct logger_system_state {
        QFilesystem::QFile log_file_handle;
    };

    static logger_system_state* state_ptr;

    void 
    append_to_log_file(std::string message) {
        if (state_ptr && state_ptr->log_file_handle.is_valid) {
            uint64_t length = message.length();
            uint64_t written = 0;
            if (!state_ptr->log_file_handle.write(length, (void*)message.data(), written)) {
                Platform::ConsoleError("ERROR: unable to write to console.log\n", LOG_LEVEL_ERROR);
            }
        }
    }

    bool 
    Initialize(uint64_t& memory_requirement, void* state) {
        memory_requirement = sizeof(logger_system_state);
        if (state == nullptr) {
            return false;
        }
        // TODO: be able to configure files and other outputs
        state_ptr = new (static_cast<logger_system_state*>(state)) logger_system_state;

        if (!state_ptr->log_file_handle.open("console.log", QFilesystem::FILE_MODE_WRITE, false)) {
            Platform::ConsoleError("ERRROR: Unable to open console.log for writing", LOG_LEVEL_ERROR);
            return false;
        }
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
        // TODO: Split into a different thread once threading 
        //    and job assignment is available
        const char* level_strings[6] = {
            "[FATAL]",
            "[ERROR]",
            "[WARN]",
            "[INFO]",
            "[DEBUG]",
            "[TRACE]",
        };

        bool is_error = level > LOG_LEVEL_WARN;

        // Massive buffer to avoid runtime allcoations that are slow
        constexpr size_t message_length = 32000;
        char error_message[message_length];
        QAllocator::Zero(error_message, message_length);

        // Format the message
        vsnprintf(error_message, message_length, message, args);

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

        append_to_log_file(out_message);
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