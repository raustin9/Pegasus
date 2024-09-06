#include "clock.hh"
#include "platform/platform.hh"

namespace qtime {

void
clock::start() {
    start_time = Platform::get_absolute_time();
    elapsed_time = 0;
}

void
clock::update() {
    if (start_time != 0) {
        elapsed_time = Platform::get_absolute_time() - start_time;
    } 
}

void
clock::stop() {
    start_time = 0; 
}

}
