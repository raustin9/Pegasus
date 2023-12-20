#pragma once
#include <iostream>
#include "qoperations.inl"
#include "qdefines.hh"
#include "qvector3.inl"

namespace qmath {

// template <typename T>
// struct Vec3;

template <typename T>
struct Mat4 {
    T data[16];

    Mat4();
    Mat4(const Mat4<T>& m);

    static Mat4<T> Identity();

    static Mat4<float> Orthographic(
        float left, float right,
        float bottom, float top,
        float near_clip, float far_clip
    );

    void Print();

    static uint64_t Size() { return 16 * sizeof(T); }

    static Mat4<float> Perspective(float fov_radians, float aspect_ratio, float near_clip, float far_clip);

    static Mat4<T> LookAt(Vec3<T> position, Vec3<T> target, Vec3<T> up);

    static Mat4<T> GetTransposed(const Mat4<T>& matrix);
    void Transpose();

    static Mat4<T> GetInverse(const Mat4<T>& matrix);
    void Invert();

    static Mat4<T> GetTranslation(Vec3<T> position);

    static Mat4<T> Scale(Vec3<T> scale);

    static Mat4<T> EulerX(T radians);
    static Mat4<T> EulerY(T radians);
    static Mat4<T> EulerZ(T radians);
    static Mat4<T> EulerXYZ(T xrad, T yrad, T zrad);

    static Vec3<T> Forward(const Mat4<T>& matrix);
    static Vec3<T> Backward(const Mat4<T>& matrix);
    static Vec3<T> Up(const Mat4<T>& matrix);
    static Vec3<T> Down(const Mat4<T>& matrix);
    static Vec3<T> Left(const Mat4<T>& matrix);
    static Vec3<T> Right(const Mat4<T>& matrix);

    Mat4<T>& operator= (const Mat4<T>& m1);
};

template <typename T>
Mat4<T>::Mat4() {
    for(uint32_t i = 0; i < 16; i++) {
        data[i] = 0;
    }
}

// Copy Constructor
template <typename T>
Mat4<T>::Mat4(const Mat4<T>& m) {
    this->data[0] = m.data[0];
    this->data[1] = m.data[1];
    this->data[2] = m.data[2];
    this->data[3] = m.data[3];

    this->data[4] = m.data[4];
    this->data[5] = m.data[5];
    this->data[6] = m.data[6];
    this->data[7] = m.data[7];

    this->data[8] = m.data[8];
    this->data[9] = m.data[9];
    this->data[10] = m.data[10];
    this->data[11] = m.data[11];

    this->data[12] = m.data[12];
    this->data[13] = m.data[13];
    this->data[14] = m.data[14];
    this->data[15] = m.data[15];
}

template <typename T>
void
Mat4<T>::Print() {
    std::cout << " " << data[0] << " " << data[1] << " " << data[2] << " " << data[3] << std::endl;
    std::cout << " " << data[4] << " " << data[5] << " " << data[6] << " " << data[7] << std::endl;
    std::cout << " " << data[8] << " " << data[9] << " " << data[10] << " " << data[11] << std::endl;
    std::cout << " " << data[12] << " " << data[13] << " " << data[14] << " " << data[15] << std::endl;
    return;
}

template <typename T>
Mat4<T>&
Mat4<T>::operator=(const Mat4<T>& m2) {
    this->data[0] = m2.data[0];
    this->data[1] = m2.data[1];
    this->data[2] = m2.data[2];
    this->data[3] = m2.data[3];

    this->data[4] = m2.data[4];
    this->data[5] = m2.data[5];
    this->data[6] = m2.data[6];
    this->data[7] = m2.data[7];

    this->data[8] = m2.data[8];
    this->data[9] = m2.data[9];
    this->data[10] = m2.data[10];
    this->data[11] = m2.data[11];

    this->data[12] = m2.data[12];
    this->data[13] = m2.data[13];
    this->data[14] = m2.data[14];
    this->data[15] = m2.data[15];

    return *this;
}


template <typename T>
Mat4<T> operator* (const Mat4<T>& m1, const Mat4<T>& m2) {
    Mat4<T> out = Mat4<T>::Identity();

    const T* m1_data = m1.data;
    const T* m2_data = m2.data;
    T* dst_data = out.data;

    // This is the algorithm the code follows. My implementation is just unrolled
    // TODO: Test this and compare which is faster
    // T dst_data[16];
    // size_t k = 0;
    // for (size_t i = 0; i < 4; i++) {
    //     for (size_t j = 0; j < 4; j++) {
    //         dst_data[k] = 
    //             m1_data[0] * m2_data[0 + j] +
    //             m1_data[1] * m2_data[4 + j] +
    //             m1_data[2] * m2_data[8 + j] +
    //             m1_data[3] * m2_data[12 + j];
    //         k++;
    //     }

    //     m1_data += 4;
    // }

    // for (size_t i = 0; i < 16; i++)
    //     out.data[i] = dst_data[i];

    dst_data[0] = 
        m1_data[0] * m2_data[0 + 0] +
        m1_data[1] * m2_data[4 + 0] +
        m1_data[2] * m2_data[8 + 0] +
        m1_data[3] * m2_data[12 + 0]
    ;

    dst_data[1] = 
        m1_data[0] * m2_data[0 + 1] +
        m1_data[1] * m2_data[4 + 1] +
        m1_data[2] * m2_data[8 + 1] +
        m1_data[3] * m2_data[12 + 1]
    ;
    
    dst_data[2] = 
        m1_data[0] * m2_data[0 + 2] +
        m1_data[1] * m2_data[4 + 2] +
        m1_data[2] * m2_data[8 + 2] +
        m1_data[3] * m2_data[12 + 2]
    ;

    dst_data[3] = 
        m1_data[0] * m2_data[0 + 3] +
        m1_data[1] * m2_data[4 + 3] +
        m1_data[2] * m2_data[8 + 3] +
        m1_data[3] * m2_data[12 + 3]
    ;

    dst_data[4] = 
        m1_data[4] * m2_data[0 + 0] +
        m1_data[5] * m2_data[4 + 0] +
        m1_data[6] * m2_data[8 + 0] +
        m1_data[7] * m2_data[12 + 0]
    ;

    dst_data[5] = 
        m1_data[4] * m2_data[0 + 1] +
        m1_data[5] * m2_data[4 + 1] +
        m1_data[6] * m2_data[8 + 1] +
        m1_data[7] * m2_data[12 + 1]
    ;
    
    dst_data[6] = 
        m1_data[4] * m2_data[0 + 2] +
        m1_data[5] * m2_data[4 + 2] +
        m1_data[6] * m2_data[8 + 2] +
        m1_data[7] * m2_data[12 + 2]
    ;

    dst_data[7] = 
        m1_data[4] * m2_data[0 + 3] +
        m1_data[5] * m2_data[4 + 3] +
        m1_data[6] * m2_data[8 + 3] +
        m1_data[7] * m2_data[12 + 3]
    ;
    
    dst_data[8] = 
        m1_data[8] * m2_data[0 +   0] +
        m1_data[9] * m2_data[4 +   0] +
        m1_data[10] * m2_data[8 +  0] +
        m1_data[11] * m2_data[12 + 0]
    ;
    
    dst_data[9] = 
        m1_data[8] * m2_data[0 +   1] +
        m1_data[9] * m2_data[4 +   1] +
        m1_data[10] * m2_data[8 +  1] +
        m1_data[11] * m2_data[12 + 1]
    ;
    
    dst_data[10] = 
        m1_data[8] * m2_data[0 +   2] +
        m1_data[9] * m2_data[4 +   2] +
        m1_data[10] * m2_data[8 +  2] +
        m1_data[11] * m2_data[12 + 2]
    ;
    
    dst_data[11] = 
        m1_data[8] * m2_data[0 +   3] +
        m1_data[9] * m2_data[4 +   3] +
        m1_data[10] * m2_data[8 +  3] +
        m1_data[11] * m2_data[12 + 3]
    ;
    
    dst_data[12] = 
        m1_data[12] * m2_data[0 +  0] +
        m1_data[13] * m2_data[4 +  0] +
        m1_data[14] * m2_data[8 +  0] +
        m1_data[15] * m2_data[12 + 0]
    ;
    
    dst_data[13] = 
        m1_data[12] * m2_data[0 +  1] +
        m1_data[13] * m2_data[4 +  1] +
        m1_data[14] * m2_data[8 +  1] +
        m1_data[15] * m2_data[12 + 1]
    ;
    
    dst_data[14] = 
        m1_data[12] * m2_data[0 +  2] +
        m1_data[13] * m2_data[4 +  2] +
        m1_data[14] * m2_data[8 +  2] +
        m1_data[15] * m2_data[12 + 2]
    ;
    
    dst_data[15] = 
        m1_data[12] * m2_data[0 +  3] +
        m1_data[13] * m2_data[4 +  3] +
        m1_data[14] * m2_data[8 +  3] +
        m1_data[15] * m2_data[12 + 3]
    ;

    return out;
}
    
// Static Members
template <typename T>
Mat4<T>
Mat4<T>::Identity() {
    Mat4<T> out;
    out.data[0] = (T)1;
    out.data[1] = 0;
    out.data[2] = 0;
    out.data[3] = 0;
        
    out.data[4] = 0;
    out.data[5] = (T)1;
    out.data[6] = 0;
    out.data[7] = 0;
       
    out.data[8] = 0;
    out.data[9] = 0;
    out.data[10] = (T)1;
    out.data[11] = 0;
      
    out.data[12] = 0;
    out.data[13] = 0;
    out.data[14] = 0;
    out.data[15] = (T)1;

    return out;
}

template <typename T>
Mat4<float> 
Mat4<T>::Orthographic(
    float left, float right,
    float bottom, float top,
    float near_clip, float far_clip
) {
    Mat4<float> out = Matr<float>::Identity();

    float lr = 1.f / (left - right);
    float bt = 1.f / (bottom - top);
    float nf = 1.f / (near_clip - far_clip);

    out.data[0]  = -2.f * lr;
    out.data[5]  = -2.f * bt;
    out.data[10] = 2.f  * nf;

    out.data[12] = (left + right) * lr;
    out.data[13] = (top + bottom) * bt;
    out.data[14] = (far_clip + near_clip) * nf;
    return out;
}

template <typename T>
Mat4<float> 
Mat4<T>::Perspective(float fov_radians, float aspect_ratio, float near_clip, float far_clip) {
    float fov_half_tan = qtan(fov_radians * 0.5f);
    Mat4<float> out;
    for (size_t i = 0; i < 16; i++) {
        out.data[i] = 0;
    }
    out.data[0] = 1.0f / (aspect_ratio * fov_half_tan);
    out.data[5] = 1.0f / fov_half_tan;
    out.data[10] = -((far_clip + near_clip) / (far_clip - near_clip));
    out.data[11] = -1.f;
    out.data[14] = -((2.f * far_clip * near_clip) / (far_clip - near_clip));
    return out;
}

template <typename T>
Mat4<T> 
Mat4<T>::LookAt(Vec3<T> position, Vec3<T> target, Vec3<T> up) {
    Mat4<T> out;
    Vec3<T> z_axis;
    z_axis.x = target.x - position.x;
    z_axis.y = target.y - position.y;
    z_axis.z = target.z - position.z;

    z_axis.normalize();
    Vec3<float> x_axis = z_axis % up;
    x_axis.normalize();
    Vec3<float> y_axis = x_axis % z_axis;

    out.data[0] = x_axis.x;
    out.data[1] = y_axis.x;
    out.data[2] = -z_axis.x;
    out.data[3] = 0;
    out.data[4] = x_axis.y;
    out.data[5] = y_axis.y;
    out.data[6] = -z_axis.y;
    out.data[7] = 0;
    out.data[8] = x_axis.z;
    out.data[9] = y_axis.z;
    out.data[10] = -z_axis.z;
    out.data[11] = 0;
    out.data[12] = -Vec3<T>::Dot(x_axis, position);
    out.data[13] = -Vec3<T>::Dot(y_axis, position);
    out.data[14] = Vec3<T>::Dot(z_axis, position);
    out.data[15] = 1.0f;

    return out;
}

template <typename T>    
Mat4<T> 
Mat4<T>::GetTransposed(const Mat4<T>& matrix) {
    Mat4<T> out = Mat4<T>::Identity();

    out.data[0] = matrix.data[0];
    out.data[1] = matrix.data[4];
    out.data[2] = matrix.data[8];
    out.data[3] = matrix.data[12];
    
    out.data[4] = matrix.data[1];
    out.data[5] = matrix.data[5];
    out.data[6] = matrix.data[9];
    out.data[7] = matrix.data[13];
    
    out.data[8] = matrix.data[2];
    out.data[9] = matrix.data[6];
    out.data[10] = matrix.data[10];
    out.data[11] = matrix.data[14];
    
    out.data[12] = matrix.data[3];
    out.data[13] = matrix.data[7];
    out.data[14] = matrix.data[11];
    out.data[15] = matrix.data[15];
    return out;
}

// TODO: This can be more efficient but this works for now
template <typename T>
void 
Mat4<T>::Transpose() {
    Mat4<T> t = Mat4<T>::GetTransposed(*this);
    *this = t;
    // Mat4<T> out = Mat4<T>::Identity();

    // out.data[0] = this->data[0];
    // out.data[1] = this->data[4];
    // out.data[2] = this->data[8];
    // out.data[3] = this->data[12];
    
    // out.data[4] = this->data[1];
    // out.data[5] = this->data[5];
    // out.data[6] = this->data[9];
    // out.data[7] = this->data[13];
    
    // out.data[8] = this->data[2];
    // out.data[9] = this->data[6];
    // out.data[10] = this->data[10];
    // out.data[11] = this->data[14];
    
    // out.data[12] = this->data[3];
    // out.data[13] = this->data[7];
    // out.data[14] = this->data[11];
    // out.data[15] = this->data[15];

    // this->data[0 ] = out.data[0 ];
    // this->data[1 ] = out.data[1 ];
    // this->data[2 ] = out.data[2 ];
    // this->data[3 ] = out.data[3 ];
    // this->data[4 ] = out.data[4 ];
    // this->data[5 ] = out.data[5 ];
    // this->data[6 ] = out.data[6 ];
    // this->data[7 ] = out.data[7 ];
    // this->data[8 ] = out.data[8 ];
    // this->data[9 ] = out.data[9 ];
    // this->data[10] = out.data[10];
    // this->data[11] = out.data[11];
    // this->data[12] = out.data[12];
    // this->data[13] = out.data[13];
    // this->data[14] = out.data[14];
    // this->data[15] = out.data[15];
}

template <typename T>
Mat4<T>
Mat4<T>::GetInverse(const Mat4<T>& matrix) {
    const T* m = matrix.data;

    T t0 = m[10] * m[15];
    T t1 = m[14] * m[11];
    T t2 = m[6] * m[15];
    T t3 = m[14] * m[7];
    T t4 = m[6] * m[11];
    T t5 = m[10] * m[7];
    T t6 = m[2] * m[15];
    T t7 = m[14] * m[3];
    T t8 = m[2] * m[11];
    T t9 = m[10] * m[3];
    T t10 = m[2] * m[7];
    T t11 = m[6] * m[3];
    T t12 = m[8] * m[13];
    T t13 = m[12] * m[9];
    T t14 = m[4] * m[13];
    T t15 = m[12] * m[5];
    T t16 = m[4] * m[9];
    T t17 = m[8] * m[5];
    T t18 = m[0] * m[13];
    T t19 = m[12] * m[1];
    T t20 = m[0] * m[9];
    T t21 = m[8] * m[1];
    T t22 = m[0] * m[5];
    T t23 = m[4] * m[1];

    Mat4<T> out_matrix;
    T* o = out_matrix.data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    T d = (T)1 / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return out_matrix;
}

template <typename T>
void
Mat4<T>::Invert() {
    Mat4<T> tmatrix = Mat4<T>::Identity();
    for (size_t i = 0; i < 16; i++)
        tmatrix.data[i] = this->data[i];

    *this = Mat4<T>::GetInverse(tmatrix);
    // *this = tmatrix;
}

template <typename T>
Mat4<T> 
Mat4<T>::GetTranslation(Vec3<T> position) {
    Mat4<T> matrix = Mat4<T>::Identity();
    matrix.data[12] = position.x;
    matrix.data[13] = position.y;
    matrix.data[14] = position.z;
    return matrix;
}

template <typename T>
Mat4<T>
Mat4<T>::Scale(Vec3<T> scale) {
    Mat4<T> matrix = Mat4<T>::Identity();
    matrix.data[0] = scale.x;
    matrix.data[5] = scale.y;
    matrix.data[10] = scale.z;
    return matrix;
}

template <typename T>
Mat4<T>
Mat4<T>::EulerX(T radians) {
    Mat4<T> matrix = Mat4<T>::Identity();
    T c = qcos(radians);
    T s = qsin(radians);

    matrix.data[5] = c;
    matrix.data[6] = s;
    matrix.data[9] = -s;
    matrix.data[10] = c;
    return matrix;
}

template <typename T>
Mat4<T>
Mat4<T>::EulerY(T radians) {
    Mat4<T> matrix = Mat4<T>::Identity();
    T c = qcos(radians);
    T s = qsin(radians);

    matrix.data[0] = c;
    matrix.data[2] = -s;
    matrix.data[8] = s;
    matrix.data[10] = c;
    return matrix;
}

template <typename T>
Mat4<T>
Mat4<T>::EulerZ(T radians) {
    Mat4<T> matrix = Mat4<T>::Identity();
    T c = qcos(radians);
    T s = qsin(radians);

    matrix.data[0] = c;
    matrix.data[1] = s;
    matrix.data[4] = -s;
    matrix.data[5] = c;
    return matrix;
}

template <typename T>
Mat4<T>
Mat4<T>::EulerXYZ(T xrad, T yrad, T zrad) {
    Mat4<T> rx = Mat4<T>::EulerX(xrad);
    Mat4<T> ry = Mat4<T>::EulerY(xrad);
    Mat4<T> rz = Mat4<T>::EulerZ(xrad);

    Mat4<T> matrix = rx * ry;
    matrix = matrix * rz;
    return out_matrix;
}

template <typename T>
Vec3<T>
Mat4<T>::Forward(const Mat4<T>& matrix) {
    Vec3<T> forward;
    forward.x = -matrix.data[2];
    forward.y = -matrix.data[6];
    forward.z = -matrix.data[10];
    forward.Normalize();
    return forward;
}

template <typename T>
Vec3<T>
Mat4<T>::Backward(const Mat4<T>& matrix) {
    Vec3<T> forward;
    forward.x = matrix.data[2];
    forward.y = matrix.data[6];
    forward.z = matrix.data[10];
    forward.Normalize();
    return forward;
}

template <typename T>
Vec3<T>
Mat4<T>::Up(const Mat4<T>& matrix) {
    Vec3<T> forward;
    forward.x = -matrix.data[1];
    forward.y = -matrix.data[5];
    forward.z = -matrix.data[9];
    forward.Normalize();
    return forward;
}

template <typename T>
Vec3<T>
Mat4<T>::Down(const Mat4<T>& matrix) {
    Vec3<T> forward;
    forward.x = matrix.data[1];
    forward.y = matrix.data[5];
    forward.z = matrix.data[9];
    forward.Normalize();
    return forward;
}

template <typename T>
Vec3<T>
Mat4<T>::Left(const Mat4<T>& matrix) {
    Vec3<T> forward;
    forward.x = -matrix.data[0];
    forward.y = -matrix.data[4];
    forward.z = -matrix.data[8];
    forward.Normalize();
    return forward;
}

} // qmath