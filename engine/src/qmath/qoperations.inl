#pragma once
#include <cmath>

namespace qmath {

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