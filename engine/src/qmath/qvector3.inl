#pragma once
#include "qoperations.inl"
#include "defines.hh"

namespace qmath {

template <typename T>
struct Vec4;

template <typename T>
struct Vec3 {
    union {
        struct {
            union {
                T x, r, s, u;
            };
            union {
                T y, g, t, v;
            };
            union {
                T z, b, p, w;
            };
        };
    };

    // Methods
    PINLINE T length_squared() {
        return this->x * this->x + this->y * this->y + this->z * this->z;
    }

    PINLINE T length() {
        return qsqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    }

    PINLINE void normalize() {
        const T length = this->length();
        this->x /= length;
        this->y /= length;
        this->z /= length;
    }

    PINLINE void normalized() {
        const T length = this->length();
        return (Vec3<T>) {
            v.x / length,
            v.y / length,
            v.z / length
        };
    }

    PINLINE Vec4<T> ToVec4(T w) {
        Vec4<T> v;
        v.x = this->x;
        v.y = this->y;
        v.z = this->z;
        v.w = w;
        return v;
    }
    
    // Static Members
    PINLINE static Vec3<T> New(T x, T y, T z) {
        Vec3<T> out_vec;
        out_vec.x = x;
        out_vec.y = y;
        out_vec.z = z;
        return out_vec;
    }

    PINLINE static Vec3<T> Zero() {
        Vec3<T> out_vec;
        out_vec.x = (T)0;
        out_vec.y = (T)0;
        out_vec.z = (T)0;
        return out_vec;
    }

    PINLINE static Vec3<T> One() {
        return (Vec3<T>) {
            (T)1,
            (T)1,
            (T)1
        };
    }

    PINLINE static Vec3<T> Up() {
        return (Vec3<T>) {
            (T)0,
            (T)1,
            (T)0
        };
    }

    PINLINE static Vec3<T> Down() {
        return (Vec3<T>) {
            (T)0,
            (T)-1,
            (T)0
        };
    }

    PINLINE static Vec3<T> Left() {
        return (Vec3<T>) {
            (T)-1,
            (T)0,
            (T)0
        };
    }

    PINLINE static Vec3<T> Right() {
        return (Vec3<T>) {
            (T)1,
            (T)0,
            (T)0
        };
    }

    PINLINE static Vec3<T> Forward() {
        return (Vec3<T>) {
            (T)0,
            (T)0,
            (T)-1
        }
    }

    PINLINE static Vec3<T> Backward() {
        return (Vec3<T>) {
            (T)0,
            (T)0,
            (T)1
        }
    }
    
    PINLINE static bool Compare(const Vec3<T> &v1, const Vec3<T>& v2, float tolerance) {
        if (qabs(v1.x - v2.x) > tolerance) {
            return false;
        }
        if (qabs(v1.y - v2.y) > tolerance) {
            return false;
        }
        if (qabs(v1.z - v2.z) > tolerance) {
            return false;
        }
        return true;
    }

    PINLINE static float Distance(const Vec3<T> &v1, const Vec3<T>& v2) {
        Vec3<T> t = (Vec3<T>) {
            v1.x - v2.x,
            v1.y - v2.y,
            v1.z - v2.z
        };
        return t.length();
    }

    PINLINE static T Dot(const Vec3<T>& v1, const Vec3<T>& v2) {
        T p = (T)0;
        p += v1.x * v2.x;
        p += v1.y * v2.y;
        p += v1.z * v2.z;
        return p;
    }

};


// Operator Overloads
// Addition
template <typename T>
const Vec3<T> operator+ (const Vec3<T>& v1, const Vec3<T>& v2) {
    return (Vec3<T>) {
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z
    };
}

template <typename T>
const Vec3<T> operator+= (Vec3<T>& v1, const Vec3<T>& v2) {
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

// Subtraction
template <typename T>
const Vec3<T> operator- (const Vec3<T>& v1, const Vec3<T>& v2) {
    return (Vec3<T>) {
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z
    };
}

template <typename T>
const Vec3<T> operator-= (Vec3<T>& v1, const Vec3<T>& v2) {
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return v1;
}

// Division 
template <typename T>
const Vec3<T> operator/ (const Vec3<T>& v1, const Vec3<T>& v2) {
    return (Vec3<T>) {
        v1.x / v2.x,
        v1.y / v2.y,
        v1.z / v2.z
    };
}

template <typename T>
const Vec3<T> operator/ (const Vec3<T>& v1, const float scalar) {
    return (Vec3<T>) {
        v1.x / scalar,
        v1.y / scalar,
        v1.z / scalar
    };
}

template <typename T>
const Vec3<T> operator/= (Vec3<T>& v1, const Vec3<T>& v2) {
    v1.x /= v2.x;
    v1.y /= v2.y;
    v1.z /= v2.z;
    return v1;
}

// Multiplication 
template <typename T>
const Vec3<T> operator* (const Vec3<T>& v1, const Vec3<T>& v2) {
    return (Vec3<T>) {
        v1.x * v2.x,
        v1.y * v2.y,
        v1.z * v2.z
    };
}

template <typename T>
const Vec3<T> operator* (const Vec3<T>& v1, const float scalar) {
    return (Vec3<T>) {
        v1.x * scalar,
        v1.y * scalar,
        v1.z * scalar
    };
}

template <typename T>
const Vec3<T> operator*= (Vec3<T>& v1, const Vec3<T>& v2) {
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    return v1;
}

template <typename T>
const Vec3<T> operator== (const Vec3<T>& v1, const Vec3<T>& v2) {
    return (
        v1.x == v2.x &&
        v1.y == v2.y &&
        v1.z == v2.z
    );
}

// Cross Product
template <typename T>
Vec3<T> operator% (const Vec3<T>& v1, const Vec3<T>& v2) {
    return (Vec3<T>) {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}


} // qmath