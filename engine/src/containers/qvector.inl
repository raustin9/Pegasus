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
    Vector(const Vector& v);
    ~Vector();

    T at(uint64_t index) const;
    T pop();
    void push(T obj);
    bool set(uint64_t index, T obj);
    uint64_t size() const;
    uint64_t capacity() const;
    memory_tag tag() const;
    T* data() const;

    T operator[] (int index) const; 
    Vector<T>& operator= (const Vector<T>& v2);
    
private:
    T* m_data;
    uint64_t m_length;
    uint64_t m_capacity;
    memory_tag m_tag;
};

// 
// Operator Overloads
//
template <typename T> Vector<T>& 
Vector<T>::operator= (const Vector<T>& other) { return *this; }

template <typename T> T
Vector<T>::operator[](int index) const { return this->at(index); }

//
// Accessors
//
template <typename T> T*
Vector<T>::data() const { return m_data; }

template <typename T> uint64_t
Vector<T>::size() const { return m_length; }

template <typename T> memory_tag
Vector<T>::tag() const { return m_tag; }

template <typename T> uint64_t
Vector<T>::capacity() const { return m_capacity; }

//
// Constructors
// 

// Constructor without memory tag
template <typename T>
Vector<T>::Vector()
    : m_data(
        new (QAllocator::Allocate(1, sizeof(T), MEMORY_TAG_UNKNOWN)) T[1]
    ), 
    m_length(0), 
    m_capacity(1),
    m_tag(MEMORY_TAG_UNKNOWN)
{

}

// Constructor specifying memory tag
template <typename T>
Vector<T>::Vector(memory_tag tag)
    : m_data(
        new (QAllocator::Allocate(1, sizeof(T), tag)) T[1]
    ), 
    m_length(0), 
    m_capacity(1),
    m_tag(tag)
{

}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector<T>& v) 
    : m_data(
        new (static_cast<T*>(QAllocator::Allocate(1, sizeof(T), v.tag())))
        T[1]
    ), 
    m_length(0), 
    m_capacity(1),
    m_tag(v.tag()) 
{
    for (uint64_t i = 0; i < v.size(); i++) {
        this->push(v[i]);
    }
}

//
// Destructor
//

// Destructor
template <typename T>
Vector<T>::~Vector() {
    QAllocator::Free((void*)m_data, sizeof(T) * m_capacity, m_tag);
    // delete[] m_data;
}

//
// METHODS
//

// Get object at an index
template <typename T> T
Vector<T>::at(uint64_t index) const {
    if (index < m_length && index >= 0) {
        return (T)m_data[index];
    }

    throw std::runtime_error("QVector: accessing invalid index");
    return m_data[0];
}

// Remove and return the last item in the list
template <typename T> T
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
template <typename T> bool
Vector<T>::set(uint64_t index, T obj) {
    if (index < m_length && index >= 0) {
        m_data[index] = obj;
    }

    return false;
}

// Append an item to the end of the Vector<>
template <typename T> void
Vector<T>::push(T obj) {
    if (m_length == m_capacity) { 
        // T *new_arr = new T[m_length * 2];
        T *buf = (T*)QAllocator::Allocate(m_length * 2, sizeof(T), m_tag);
        T *new_arr = new (buf) T[m_length * 2];

        for (int i = 0; i < m_length; i++) {
            new_arr[i] = (T)m_data[i];
        }
        
        QAllocator::Free(m_data, m_length * sizeof(T), m_tag);
        m_data = new_arr;
        m_capacity = m_capacity * 2;
    }
    m_data[m_length] = obj;
    m_length++;
}