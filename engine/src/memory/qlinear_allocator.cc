#include "qlinear_allocator.hh"
#include "core/qmemory.hh"
#include "core/qlogger.hh"
/**
 * Implementation for linear allocator
*/

namespace qmemory {

void
QLinearAllocator::Create(uint64_t total_size, void* memory) {
    this->total_size = total_size;
    this->allocated = 0;
    this->owns_memory = (memory == nullptr);

    if (memory) {
        this->memory = memory;
    } else {
        this->memory = QAllocator::Allocate(1, total_size, MEMORY_TAG_LINEAR_ALLOCATOR);
    }
}

void 
QLinearAllocator::Destroy() {

    if (this->owns_memory && this->memory) {
        QAllocator::Free(this->memory, this->total_size, MEMORY_TAG_LINEAR_ALLOCATOR);
        this->memory = nullptr;
    } else {
        this->memory = nullptr;
    }

    this->total_size = 0;
    this->allocated = 0;
    this->owns_memory = false;
}

void* 
QLinearAllocator::Allocate(uint64_t size) {
    if (this->memory) {
        if (this->allocated + size > this->total_size) {
            uint64_t remaining = this->total_size - this->allocated;
            qlogger::Error("QLinearAllocator::Allocate(): tried to allocate %llu bytes with only %llu remaining", size, remaining);

            return nullptr;
        }
    
        void* block = static_cast<uint8_t*>(this->memory) + this->allocated;
        this->allocated += size;
        return block;
    }

    qlogger::Error("QLinearAllocator::Allocate(): memory not initialized when called");
    return nullptr;
}

void 
QLinearAllocator::FreeAll() {
    if (this->memory) {
        this->allocated = 0;
        QAllocator::Zero(this->memory, this->total_size);
    }
}


} // qmemory