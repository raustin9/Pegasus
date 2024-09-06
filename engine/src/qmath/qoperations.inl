#pragma once
#include "qdefines.hh"
#include <cmath>

namespace qmath {

inline float
deg_to_rad(int32_t deg) {
    return (
        static_cast<float>(Q_PI / 180) * deg
    );
}

template <typename T>
T qsqrt(T num) {
    return std::sqrt(num);
}

template <typename T>
T qabs(T num) {
    return std::abs(num);
}

template<typename T>
T qtan(T num) {
    return std::tan(num);
}

template<typename T>
T qsin(T num) {
    return std::sin(num);
}

template<typename T>
T qcos(T num) {
    return std::cos(num);
}

template<typename T>
T qacos(T num) {
    return std::acos(num);
}


} // qmath