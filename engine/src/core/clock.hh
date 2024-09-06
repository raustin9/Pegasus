#pragma once
#include "defines.hh"


namespace qtime {

struct clock {
    double start_time;
    float elapsed_time;

    void update();
    void start();
    void stop();
};

}