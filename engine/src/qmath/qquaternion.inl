#pragma once
#include "qoperations.inl"
#include "qvector4.inl"
#include "qmatrix4.inl"
#include "qdefines.hh"

namespace qmath {

template <typename T>
struct Quaternion {
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

    static Quaternion<T> Identity();
    static Quaternion<T> New(T x, T y, T z, T w); 
    static Quaternion<T> Zero(); 
    static Quaternion<T> One(); 
    static bool Compare(const Quaternion<T> &v1, const Quaternion<T>& v2, float tolerance); 
    static T Normal(const Quaternion<T>& q);
    static T Dot(const Quaternion<T>& q1, const Quaternion<T>& q2);
    static Quaternion<T> FromAxisAngle(Vec3<T> axis, T angle, bool normalize);
    static Quaternion<float> Slerp(const Quaternion<float>& q1, const Quaternion<float>& q2, float percentage);
    
    Quaternion<T> GetNormalized();
    Quaternion<T> GetConjugate();
    Quaternion<T> GetInverse();

    void Normalize();
    void Inverse();

    Mat4<T> ToMat4();
    Mat4<T> ToRotationMatrix(Vec3<T> center);

    Quaternion<T>& operator= (Quaternion<T>& m1);
};

template <typename T>
Quaternion<T> 
Quaternion<T>::New(T x, T y, T z, T w) {
    Quaternion<T> out_vec;
    out_vec.x = x;
    out_vec.y = y;
    out_vec.z = z;
    out_vec.w = w;
    return out_vec;
}

template <typename T>
Quaternion<T>
Quaternion<T>::Zero() {
    Quaternion<T> out_vec;
    out_vec.x = (T)0;
    out_vec.y = (T)0;
    out_vec.z = (T)0;
    out_vec.w = (T)0;
    return out_vec;
}

template <typename T>
Quaternion<T>
Quaternion<T>::One() {
    return (Quaternion<T>) {
        (T)1,
        (T)1,
        (T)1,
        (T)1
    };
}

template <typename T>
Quaternion<T>&
Quaternion<T>::operator= (Quaternion<T>& other) {
    this.x = other.x;
    this.y = other.y;
    this.z = other.z;
    this.w = other.w;
    return this;
}

// FROM VEC4 //
// Operator Overloads
// Addition
template <typename T>
const Quaternion<T> operator+ (const Quaternion<T>& v1, const Quaternion<T>& v2) {
    return (Quaternion<T>) {
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z,
        v1.w + v2.w
    };
}

template <typename T>
const Quaternion<T> operator+= (Quaternion<T>& v1, const Quaternion<T>& v2) {
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    v1.w += v2.w;
    return v1;
}

// Subtraction
template <typename T>
const Quaternion<T> operator- (const Quaternion<T>& v1, const Quaternion<T>& v2) {
    return (Quaternion<T>) {
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z,
        v1.w - v2.w
    };
}

template <typename T>
const Quaternion<T> operator-= (Quaternion<T>& v1, const Quaternion<T>& v2) {
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    v1.w -= v2.w;
    return v1;
}

// Division 
template <typename T>
const Quaternion<T> operator/ (const Quaternion<T>& v1, const Quaternion<T>& v2) {
    return (Quaternion<T>) {
        v1.x / v2.x,
        v1.y / v2.y,
        v1.z / v2.z,
        v1.w / v2.w
    };
}

template <typename T>
const Quaternion<T> operator/ (const Quaternion<T>& v1, const float scalar) {
    return (Quaternion<T>) {
        v1.x / scalar,
        v1.y / scalar,
        v1.z / scalar,
        v1.w / scalar
    };
}

template <typename T>
const Quaternion<T> operator/= (Quaternion<T>& v1, const Quaternion<T>& v2) {
    v1.x /= v2.x;
    v1.y /= v2.y;
    v1.z /= v2.z;
    v1.w /= v2.w;
    return v1;
}

// Multiply by a scalar
template <typename T>
const Quaternion<T> operator* (const Quaternion<T>& v1, const float scalar) {
    return (Quaternion<T>) {
        v1.x * scalar,
        v1.y * scalar,
        v1.z * scalar,
        v1.w * scalar
    };
}

// Multiply quaternions
template <typename T>
Quaternion<T> operator* (const Quaternion<T>& q1, const Quaternion<T>& q2) {
    Quaternion<T> qout;

    qout.x = q1.x * q2.w +
             q1.y * q2.z - 
             q1.z * q2.y +
             q1.w * q1.x;

    qout.y = -q1.x * q2.z +
             q1.y * q2.w + 
             q1.z * q2.x +
             q1.w * q1.y;

    qout.z = q1.x * q2.y -
             q1.y * q2.x + 
             q1.z * q2.w +
             q1.w * q1.z;

    qout.w = -q1.x * q2.x -
             q1.y * q2.y - 
             q1.z * q2.z +
             q1.w * q1.w;

    return qout;
}


template <typename T>
Quaternion<T> 
Quaternion<T>::Identity() {
    return (Quaternion<T>) {
        (T)0,
        (T)0,
        (T)0,
        (T)1
    };
}

template <typename T>
T 
Quaternion<T>::Normal(const Quaternion<T>& q) {
    return qsqrt(
        q.x * q.x +
        q.y * q.y +
        q.z * q.z +
        q.w * q.w
    );
}

template <typename T>
T 
Quaternion<T>::Dot(const Quaternion<T>& q1, const Quaternion<T>& q2) {
    return q1.x * q2.x +
           q1.y * q2.y +
           q1.z * q2.z +
           q1.w * q2.w;
}

template <typename T>
Quaternion<T> 
Quaternion<T>::FromAxisAngle(Vec3<T> axis, T angle, bool normalize) {
    const float half_angle = 0.5f * angle;
    float s = qsin(half_angle);
    float c = qcos(half_angle);

    Quaternion<T> q = (Quaternion<T>) {
        s * axis.x,
        s * axis.y,
        s * axis.z,
        c
    };
    return q;
}

template <typename T>
Quaternion<float> 
Quaternion<T>::Slerp(const Quaternion<float>& q1, const Quaternion<float>& q2, float percentage) {
    Quaternion<float> out;

    Quaternion<float> v1 = q1.GetNormalized();
    Quaternion<float> v2 = q2.GetNormalized();

    float dot = Quaternion<float>::Dot(v1, v2);

    if (dot < (float)0) {
        v2.x = -v1.x;
        v2.y = -v1.y;
        v2.z = -v1.z;
        v2.w = -v1.w;
        dot = -dot;
    }

    const float DOfloat_floatHRESHOLD = 0.9995f;
    if (dot > DOfloat_floatHRESHOLD) {
        // If the inputs are too close, linearly interpolate them
        out = (Quaternion<float>) {
            v1.x + ((v2.x - v1.x) * percentage),
            v1.y + ((v2.y - v1.y) * percentage),
            v1.z + ((v2.z - v1.z) * percentage),
            v1.w + ((v2.w - v1.w) * percentage)
        };

        return out.GetNormalized();
    }

    // Since the dot is in range [0, DOT_THRESHOLD], acos is safe
    float theta_0 = qacos(dot);
    float theta = theta_0 * percentage;
    float sin_theta = qsin(theta);
    float sin_theta_0 = qsin(theta_0);

    float s0 = qcos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    return (Quaternion<T>) {
        (v1.x * s0) + (v2.x * s1),
        (v1.y * s0) + (v2.y * s1),
        (v1.z * s0) + (v2.z * s1),
        (v1.w * s0) + (v2.w * s1)
    };
}
    
template <typename T>
Quaternion<T> 
Quaternion<T>::GetNormalized() {
    T normal = this->Normal();
    return (Quaternion<T>) {
        q.x / normal,
        q.y / normal,
        q.z / normal,
        q.w / normal
    };
}

template <typename T>
Quaternion<T> 
Quaternion<T>::GetConjugate() {
    return (Quaternion<T>) {
        -q.x,
        -q.y,
        -q.z,
        q.w
    };
}

template <typename T>
Quaternion<T> 
Quaternion<T>::GetInverse() {
    return (this->GetConjugate).GetNormalized();
}

template <typename T>
void 
Quaternion<T>::Normalize() {

}

template <typename T>
void 
Quaternion<T>::Inverse() {

}

template <typename T>
Mat4<T> 
Quaternion<T>::ToMat4() {
    Mat4<T> matrix = Mat4<T>::Identity();

    Quaternion qn = this->GetNormalized();

    matrix.data[0] = (T)1 - (T)2 * qn.y * qn.y - (T)2 * qn.z * qn.z;
    matrix.data[1] = (T)2 * qn.x * qn.y - (T)2 * qn.z * qn.w;
    matrix.data[2] = (T)2 * qn.x * qn.x + (T)2 * qn.y * qn.w;

    matrix.data[4] = (T)2 * qn.x * qn.y + (T)2 * qn.z * qn.w;
    matrix.data[5] = (T)1 - (T)2 * qn.x * qn.x - (T)2 * qn.z * qn.w;
    matrix.data[6] = (T)2 * qn.y * qn.z - (T)2 * qn.x * qn.w;

    matrix.data[8] = (T)2 * qn.x * qn.z - (T)2 * qn.y * qn.w;
    matrix.data[9] = (T)2 * qn.y * qn.z + (T)2 * qn.x * qn.w;
    matrix.data[10] = (T)1 - (T)2 * qn.x * qn.x - (T)2 * qn.y * qn.y;

    return matrix;
}

template <typename T>
Mat4<T> 
Quaternion<T>::ToRotationMatrix(Vec3<T> center) {
    Mat4<T> out_matrix;

    T* o = out_matrix.data;
    o[0] = (this->x * this->x) - (this->y * this->y) - (this->z * this->z) + (this->w * this->w);
    o[1] = 2.0f * ((this->x * this->y) + (this->z * this->w));
    o[2] = 2.0f * ((this->x * this->z) - (this->y * this->w));
    o[3] = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];

    o[4] = 2.0f * ((this->x * this->y) - (this->z * this->w));
    o[5] = -(this->x * this->x) + (this->y * this->y) - (this->z * this->z) + (this->w * this->w);
    o[6] = 2.0f * ((this->y * this->z) + (this->x * this->w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];

    o[8] = 2.0f * ((this->x * this->z) + (this->y * this->w));
    o[9] = 2.0f * ((this->y * this->z) - (this->x * this->w));
    o[10] = -(this->x * this->x) - (this->y * this->y) + (this->z * this->z) + (this->w * this->w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];

    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;
    return out_matrix;
}

} // qmath