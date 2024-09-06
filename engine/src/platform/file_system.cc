#include "file_system.hh"
#include "core/qlogger.hh"
#include "core/qmemory.hh"

#include <sys/stat.h>
#include <string>
#include <fstream>
#include <iostream>

namespace QFilesystem {


bool
file_exists(
    const char* path
) {
    std::ifstream f(path);
    return f.good();
}


bool 
QFile::open(const char* path, file_modes mode, bool bin) {
    this->is_valid = false;

    int ios_flags = 0;
    if (mode & FILE_MODE_READ) {
        ios_flags |= std::ios::in;
    }

    if (mode & FILE_MODE_WRITE) {
        ios_flags |= std::ios::out;
    }

    if (bin) {
        ios_flags |= std::ios::binary;
    }

    // Open the file
    this->handle.open(path, ios_flags);
    if (!this->handle) {
        qlogger::Error("Error opening file %s", path);
        return false;
    }

    this->is_valid = true;
    return true;
}

void 
QFile::close() {
    if (this->handle.is_open()) {
        this->handle.close();
        this->is_valid = false;
        return;
    }

    qlogger::Warn("Attempted closing file that is not open");
}

bool 
QFile::read_line(char** line_buffer) {
    if (handle) {
        char buffer[32000];
        handle.getline(buffer, 32000, '\n');

        std::strcpy(*line_buffer, buffer);
        return true;
    }

    qlogger::Error("Failed to read line from file");
    return false;
}

bool 
QFile::write_line(const char* text) {
    if (handle.is_open()) {
        handle.write(text, std::strlen(text));
        if (!handle) {
            qlogger::Error("Failed to write line to file");
            return false;
        }
        handle.flush();
        return true;
    }

    qlogger::Warn("Tried to write line to file with invalid handle");
    return false;
}

bool 
QFile::read(uint64_t data_size, void* out_data, uint64_t& out_bytes_read) {
    if (handle.is_open() && out_data) {
        handle.read((char*)out_data, data_size);
        out_bytes_read = handle.gcount();

        if (out_bytes_read != data_size) {
            qlogger::Warn("Number of bytes read from file was not the same as specified");
            return false;
        }

        return true;
    }

    qlogger::Error("Tried reading from invalid file stream");
    return false;
}

bool 
QFile::read_all_bytes(uint8_t** out_bytes, uint64_t& out_bytes_read) {
    if (handle.is_open()) {
        handle.seekg(0, handle.end);
        size_t file_length = handle.tellg();
        handle.seekg(0, handle.beg);

        *out_bytes = static_cast<uint8_t*>(QAllocator::Allocate(file_length, sizeof(uint8_t), MEMORY_TAG_STRING));
        handle.read((char*)*out_bytes, file_length);
        out_bytes_read = handle.gcount();

        if (out_bytes_read != file_length) {
            qlogger::Error("Number of bytes reaed was not same as desired amount");
            return false;
        }

        return true;
    }

    qlogger::Error("Tried reading from invalid file stream");
    return false;
}

bool 
QFile::write(uint64_t data_size, const void* data, uint64_t& out_bytes_written) {
    if (handle.is_open()) {
        handle.write(static_cast<const char*>(data), data_size);
        if(!handle) {
            qlogger::Error("Bad write to file");
            return false;
        }

        handle.flush();
        return true;
    }
    
    qlogger::Error("Tried writing to invalid file stream");
    return false;
}

// bool 
// file_open(const char* path, file_modes mode, bool binary, file_handle& out_handle) {
//     out_handle.is_valid = false;
    
//     const char* mode_str;

    
// }

// void 
// file_close(file_handle& handle) {

// }

// bool 
// file_read_line(file_handle& handle, char** line_buf) {

// }

// bool 
// file_write_line(file_handle& handle, const char* text) {

// }

// bool 
// file_read(file_handle& handle, uint64_t data_size, void* out_data, uint64_t &out_bytes_read0) {

// }

// bool 
// file_read_all_bytes(file_handle& handle, uint8_t** out_bytes, uint64_t &out_bytes_read) {

// }

// bool 
// file_write(file_handle& handle, uint64_t data_size, const void* data, uint64_t& out_bytes_written) {

// }


} // QFilesystem