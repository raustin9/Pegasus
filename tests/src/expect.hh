#pragma once
#include <core/qlogger.hh>
#include <qmath/qmath.hh>

#define expect_should_be(expected, actual)  \
    if (actual != expected) { \
        qlogger::Error("Tests: Expected %lld, but got %lld. File %s:%d", expected, actual, __FILE__, __LINE__); \
        return FALSE; \
    }


#define expect_should_not_be(expected, actual)  \
    if (actual == expected) { \
        qlogger::Error("Tests: Expected %d != %d, but they are equal. File %s:%d", expected, actual, __FILE__, __LINE__); \
        return FALSE; \
    }

#define expect_float_to_be(expected, actual)  \
    if (qmath::qabs(expected - actual) > 0.001f) { \
        qlogger::Error("Tests: Expected %f but got %f. File %s:%d", expected, actual, __FILE__, __LINE__); \
        return FALSE; \
    }

#define expect_float_not_to_be(expected, actual)  \
    if (qmath::qabs(expected - actual) <= 0.001f) { \
        qlogger::Error("Tests: Expected %f != %f but they are equal. File %s:%d", expected, actual, __FILE__, __LINE__); \
        return FALSE; \
    }

#define expect_to_be_true(actual)  \
    if (actual != true) { \
        qlogger::Error("Tests: Expected true but got: false. File %s:%d", __FILE__, __LINE__); \
        return FALSE; \
    }

#define expect_to_be_false(actual)  \
    if (actual != false) { \
        qlogger::Error("Tests: Expected false but got: true. File %s:%d", __FILE__, __LINE__); \
        return FALSE; \
    }