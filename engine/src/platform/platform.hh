#pragma once
#include "renderer/vkcommon.hh"
#include "core/input.hh"
#include "core/events.hh"
#include "linux_timer.hh"
#include "stdafx.hh"
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <chrono>

class Platform {
public:
    Platform(std::string name, uint32_t width, uint32_t height, EventHandler &eh);
    void create_window();
    void destroy_window();
    bool pump_messages();
    bool create_vulkan_surface(VKCommonParameters &params);
    void set_title(std::string title);
    std::chrono::time_point<std::chrono::high_resolution_clock> get_current_time();


    // WINDOWING INFO
#ifdef Q_PLATFORM_LINUX
    // LINUX WINDOWING
    Display* display;
    Window handle;
    Atom wm_delete_window;
    bool should_quit;
    void handle_x11_event(XEvent& event);

#elif defined(Q_PLATFORM_WINDOWS)
    // WINDOWS WINDOWING SHIT
#endif // END PLATFORM SPECIFIC WINDOWING INFO

    std::string name;
    uint32_t width;
    uint32_t height;

private:
    InputHandler m_inputHandler;

    Keys _translateKey(uint32_t); // used to standardize keyboard input
                                  // across the different platforms
};
