#pragma once
#include "defines.hh"
#include <cstdint>
#include <exception>
#include "core/qmemory.hh"

template <typename T>
class QAPI Vector {
public:
    Vector();
    Vector(memory_tag tag);
    ~Vector();

    T at(uint64_t index);
    T pop();
    void push(T obj);
    bool set(uint64_t index, T obj);
    uint64_t size();
    uint64_t capacity();

    T operator[] (int index); 
    
private:
    T* m_data;
    uint64_t m_length;
    uint64_t m_capacity;
};

template <typename T>
T
Vector<T>::operator[](int index) {
    return this->at(index);
}

// Accessors
template <typename T>
uint64_t
Vector<T>::size() {
    return m_length;
}

template <typename T>
uint64_t
Vector<T>::capacity() {
    return m_capacity;
}

// Constructor
template <typename T>
Vector<T>::Vector()
    : m_data(static_cast<T*>(QAllocator::Allocate(1, MEMORY_TAG_UNKNOWN))), m_length(0), m_capacity(1) 
{

}

template <typename T>
Vector<T>::Vector(memory_tag tag)
    : m_data(static_cast<T*>(QAllocator::Allocate(1, tag))), m_length(0), m_capacity(1) 
{

}

// Destructor
template <typename T>
Vector<T>::~Vector() {
    delete[] m_data;
}

// Get object at an index
template <typename T>
T
Vector<T>::at(uint64_t index) {
    if (index < m_length && index >= 0) {
        return m_data[index];
    }

    throw std::runtime_error("QVector: accessing invalid index");
    return m_data[0];
}

// Remove and return the last item in the list
template <typename T>
T
Vector<T>::pop() {
    if (index < m_length && index >= 0) {
        T = m_data[index];
        m_length--;
        return T;
    }

    throw std::runtime_error("QVector: accessing invalid index");
    return m_data[0];
}
// Set an index of the Vector to specified object
template <typename T>
bool
Vector<T>::set(uint64_t index, T obj) {
    if (index < m_length && index >= 0) {
        m_data[index] = obj;
    }

    return false;
}

template <typename T>
void
Vector<T>::push(T obj) {
    if (m_length == m_capacity) {
        T *new_arr = new T[m_length * 2];

        for (int i = 0; i < m_length; i++) {
            new_arr[i] = m_data[i];
        }

        delete[] m_data;
        m_data = new_arr;
        m_capacity = m_capacity * 2;
    }
    m_data[m_length] = obj;
    m_length++;
}