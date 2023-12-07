#pragma once
#include "qoperations.inl"
#include "qvector3.inl"
#include "defines.hh"

namespace qmath {

template <typename T>
struct Vec4 {
    union {
        struct {
            union {
                T x, r, s;
            };
            union {
                T y, g, t;
            };
            union {
                T z, b, p;
            };
            union {
                T w, a, q;
            };
        };
    };

    // Methods
    T length_squared(); 

    T length(); 

    void normalize(); 

    Vec4<T> normalized(); 

    // Static Members
    static Vec4<T> New(T x, T y, T z, T w); 

    static Vec4<T> Zero(); 

    static Vec4<T> One(); 
    
    static bool Compare(const Vec4<T> &v1, const Vec4<T>& v2, float tolerance); 

    static float Distance(const Vec4<T> &v1, const Vec4<T>& v2); //  {

    Vec3<T> ToVec3(); 

    T Dot(
        T a0, T a1, T a2, T a3,
        T b0, T b1, T b2, T b3
    ); 

};


// Operator Overloads
// Addition
template <typename T>
const Vec4<T> operator+ (const Vec4<T>& v1, const Vec4<T>& v2) {
    return (Vec4<T>) {
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z,
        v1.w + v2.w
    };
}

template <typename T>
const Vec4<T> operator+= (Vec4<T>& v1, const Vec4<T>& v2) {
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    v1.w += v2.w;
    return v1;
}

// Subtraction
template <typename T>
const Vec4<T> operator- (const Vec4<T>& v1, const Vec4<T>& v2) {
    return (Vec4<T>) {
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z,
        v1.w - v2.w
    };
}

template <typename T>
const Vec4<T> operator-= (Vec4<T>& v1, const Vec4<T>& v2) {
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    v1.w -= v2.w;
    return v1;
}

// Division 
template <typename T>
const Vec4<T> operator/ (const Vec4<T>& v1, const Vec4<T>& v2) {
    return (Vec4<T>) {
        v1.x / v2.x,
        v1.y / v2.y,
        v1.z / v2.z,
        v1.w / v2.w
    };
}

template <typename T>
const Vec4<T> operator/ (const Vec4<T>& v1, const float scalar) {
    return (Vec4<T>) {
        v1.x / scalar,
        v1.y / scalar,
        v1.z / scalar,
        v1.w / scalar
    };
}

template <typename T>
const Vec4<T> operator/= (Vec4<T>& v1, const Vec4<T>& v2) {
    v1.x /= v2.x;
    v1.y /= v2.y;
    v1.z /= v2.z;
    v1.w /= v2.w;
    return v1;
}

// Multiplication 
template <typename T>
const Vec4<T> operator* (const Vec4<T>& v1, const Vec4<T>& v2) {
    return (Vec4<T>) {
        v1.x * v2.x,
        v1.y * v2.y,
        v1.z * v2.z,
        v1.w * v2.w
    };
}

template <typename T>
const Vec4<T> operator* (const Vec4<T>& v1, const float scalar) {
    return (Vec4<T>) {
        v1.x * scalar,
        v1.y * scalar,
        v1.z * scalar,
        v1.w * scalar
    };
}

template <typename T>
const Vec4<T> operator*= (Vec4<T>& v1, const Vec4<T>& v2) {
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    v1.w *= v2.w;
    return v1;
}

template <typename T>
const Vec4<T> operator== (const Vec4<T>& v1, const Vec4<T>& v2) {
    return (
        v1.x == v2.x &&
        v1.y == v2.y &&
        v1.z == v2.z &&
        v1.w == v2.w
    );
}

//
// 
// METHODS
// 
//

template <typename T>
T 
Vec4<T>::length_squared() {
    return this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w;
}

template <typename T>
T 
Vec4<T>::length() {
    return qsqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
}

template <typename T>
void 
Vec4<T>::normalize() {
    const T length = this->length();
    this->x /= length;
    this->y /= length;
    this->z /= length;
    this->w /= length;
}

template <typename T>
Vec4<T> 
Vec4<T>::normalized() {
    const T length = this->length();
    return (Vec4<T>) {
        v.x / length,
        v.y / length,
        v.z / length,
        v.w / length
    };
}

template <typename T>
Vec4<T> 
Vec4<T>::New(T x, T y, T z, T w) {
    Vec4<T> out_vec;
    out_vec.x = x;
    out_vec.y = y;
    out_vec.z = z;
    out_vec.w = w;
    return out_vec;
}

template <typename T>
Vec4<T>
Vec4<T>::Zero() {
    Vec4<T> out_vec;
    out_vec.x = (T)0;
    out_vec.y = (T)0;
    out_vec.z = (T)0;
    out_vec.w = (T)0;
    return out_vec;
}

template <typename T>
Vec4<T>
Vec4<T>::One() {
    return (Vec4<T>) {
        (T)1,
        (T)1,
        (T)1,
        (T)1
    };
}

template <typename T> 
bool
Vec4<T>::Compare(const Vec4<T> &v1, const Vec4<T>& v2, float tolerance) {
    if (qabs(v1.x - v2.x) > tolerance) {
        return false;
    }
    if (qabs(v1.y - v2.y) > tolerance) {
        return false;
    }
    if (qabs(v1.z - v2.z) > tolerance) {
        return false;
    }
    if (qabs(v1.w - v2.w) > tolerance) {
        return false;
    }
    return true;
}

template <typename T>
float 
Vec4<T>::Distance(const Vec4<T> &v1, const Vec4<T>& v2) {
    Vec4<T> t = (Vec4<T>) {
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z,
        v1.w - v2.w
    };
    return t.length();
}

template <typename T>
Vec3<T> 
Vec4<T>::ToVec3() {
    return Vec3<T> {
        x,y,z
    };
}

template <typename T>
T 
Vec4<T>::Dot(
    T a0, T a1, T a2, T a3,
    T b0, T b1, T b2, T b3
) {
    return  a0 * b0 +
            a1 * b1 + 
            a2 * b2 +
            a3 * b3;
}

} // qmath