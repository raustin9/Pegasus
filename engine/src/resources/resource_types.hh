#pragma once

#include "qmath/qmath.hh"

struct texture {
    uint32_t id;
    uint32_t width;
    uint32_t h;
    uint8_t channel_count;
    bool has_transparency;
    uint32_t generation;
    void* internal_data;
};