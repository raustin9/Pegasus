#pragma once
#include "stdafx.hh"
#include <cstdint>
#include <memory>
#include <sys/types.h>


class Platform {
public:
    Platform(std::string name, uint32_t width, uint32_t height);
    void create_window();
    bool pump_messages();

    #ifdef Q_PLATFORM_LINUX
    // LINUX WINDOWING
    Display* display;
    Atom wm_delete_window;
    bool should_quit;
    void handle_x11_event(XEvent& event);

    #elif defined(Q_PLATFORM_WINDOWS)
    #endif // end platform specific

    std::string name;
    uint32_t width;
    uint32_t height;
};
