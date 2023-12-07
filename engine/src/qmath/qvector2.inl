#pragma once
#include "qoperations.inl"
#include "defines.hh"

namespace qmath {

template <typename T>
struct Vec2 {
    union {
        struct {
            union {
                T x, r, s, u;
            };
            union {
                T y, g, t, v;
            };
        };
    };

    // Methods
    PINLINE T length_squared() {
        return this->x * this->x + this->y * this->y;
    }

    PINLINE T length() {
        return qsqrt(this->x * this->x + this->y * this->y);
    }

    PINLINE void normalize() {
        const T length = this->length();
        this->x /= length;
        this->y /= length;
    }

    PINLINE void normalized() {
        const T length = this->length();
        return (Vec2<T>) {
            v.x / length,
            v,y / length
        };
    }

    // Static Members
    PINLINE static Vec2<T> New(T x, T y) {
        Vec2<T> out_vec;
        out_vec.x = x;
        out_vec.y = y;
        return out_vec;
    }

    PINLINE static Vec2<T> Zero() {
        Vec2<T> out_vec;
        out_vec.x = (T)0;
        out_vec.y = (T)0;
        return out_vec;
    }

    PINLINE static Vec2<T> One() {
        return (Vec2<T>) {
            (T)1,
            (T)1
        };
    }

    PINLINE static Vec2<T> Up() {
        return (Vec2<T>) {
            (T)0,
            (T)1
        };
    }

    PINLINE static Vec2<T> Down() {
        return (Vec2<T>) {
            (T)1,
            (T)1
        };
    }

    PINLINE static Vec2<T> Left() {
        return (Vec2<T>) {
            (T)-1,
            (T)0
        };
    }

    PINLINE static Vec2<T> Right() {
        return (Vec2<T>) {
            (T)1,
            (T)0
        };
    }

    PINLINE static bool Compare(const Vec2<T> &v1, const Vec2<T>& v2, float tolerance) {
        if (qabs(v1.x - v2.x) > tolerance) {
            return false;
        }
        if (qabs(v1.y - v2.y) > tolerance) {
            return false;
        }
        return true;
    }

    PINLINE static float Distance(const Vec2<T> &v1, const Vec2<T>& v2) {
        Vec2<T> t = (Vec2<T>) {
            v1.x - v2.x,
            v1.y - v2.y
        };
        return t.length();
    }

};


// Operator Overloads
// Addition
template <typename T>
const Vec2<T> operator+ (const Vec2<T>& v1, const Vec2<T>& v2) {
    return (Vec2<T>) {
        v1.x + v2.x,
        v1.y + v2.y
    };
}

template <typename T>
const Vec2<T> operator+= (Vec2<T>& v1, const Vec2<T>& v2) {
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}

// Subtraction
template <typename T>
const Vec2<T> operator- (const Vec2<T>& v1, const Vec2<T>& v2) {
    return (Vec2<T>) {
        v1.x - v2.x,
        v1.y - v2.y
    };
}

template <typename T>
const Vec2<T> operator-= (Vec2<T>& v1, const Vec2<T>& v2) {
    v1.x -= v2.x;
    v1.y -= v2.y;
    return v1;
}

// Division 
template <typename T>
const Vec2<T> operator/ (const Vec2<T>& v1, const Vec2<T>& v2) {
    return (Vec2<T>) {
        v1.x / v2.x,
        v1.y / v2.y
    };
}

template <typename T>
const Vec2<T> operator/= (Vec2<T>& v1, const Vec2<T>& v2) {
    v1.x /= v2.x;
    v1.y /= v2.y;
    return v1;
}

// Division 
template <typename T>
const Vec2<T> operator* (const Vec2<T>& v1, const Vec2<T>& v2) {
    return (Vec2<T>) {
        v1.x * v2.x,
        v1.y * v2.y
    };
}

template <typename T>
const Vec2<T> operator*= (Vec2<T>& v1, const Vec2<T>& v2) {
    v1.x *= v2.x;
    v1.y *= v2.y;
    return v1;
}

template <typename T>
const Vec2<T> operator== (const Vec2<T>& v1, const Vec2<T>& v2) {
    return (
        v1.x == v2.x &&
        v1.y == v2.y
    );
}


}