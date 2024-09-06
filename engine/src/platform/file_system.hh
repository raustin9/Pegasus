#pragma once
#include "defines.hh"
#include <cstdint>
#include <fstream>

namespace QFilesystem {


// Access modes for files
enum file_modes : uint32_t {
    FILE_MODE_READ = 0x1,
    FILE_MODE_WRITE = 0x2,
};

// Holds a handle to a file
struct QAPI QFile {
    // void* handle;
    std::fstream handle;
    bool is_valid;

    bool open(const char* path, file_modes mode, bool binary);
    void close();
    bool read_line(char** buffer);
    bool write_line(const char* text);
    bool read(uint64_t data_size, void* out_data, uint64_t& out_bytes_read);
    bool read_all_bytes(uint8_t** out_bytes, uint64_t& out_bytes_read);
    bool write(uint64_t data_size, const void* data, uint64_t& out_bytes_written);
};

/**
 * Checks if a file with the given path exists
 * @param path the path of the file to be checked
 * @returns true if it exists, false otherwise
*/
QAPI bool file_exists(const char* path);

/**
 * Attempt to open file located at the path
 * @param path the path of the file meant to be opened
 * @param mode flags specifying the mode of file access
 * @param binary indicates if the file should be opened in a binary mode
 * @param out_handle pointer to a file_handle that holds the handle information
 * @returns True if opened successfully, false otherwise
*/
// QAPI bool file_open(const char* path, file_modes mode, bool binary, file_handle& out_handle);
// QAPI void file_close(file_handle& handle);
// QAPI bool file_read_line(file_handle& handle, char** line_buf);
// QAPI bool file_write_line(file_handle& handle, const char* text);
// QAPI bool file_read(file_handle& handle, uint64_t data_size, void* out_data, uint64_t &out_bytes_read0);
// QAPI bool file_read_all_bytes(file_handle& handle, uint8_t** out_bytes, uint64_t &out_bytes_read);
// QAPI bool file_write(file_handle& handle, uint64_t data_size, const void* data, uint64_t& out_bytes_written);
} // QFilesystem