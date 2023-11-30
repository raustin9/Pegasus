#pragma once
// #include "renderer/vulkan/vkcommon.hh"
#include "core/input.hh"
#include "core/events.hh"
#include "platform/platform_timer.hh"
// #include "linux_timer.hh"
#include "stdafx.hh"
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <chrono>

class Platform {
public:
    Platform(std::string name, uint32_t width, uint32_t height);
    static bool Startup(std::string name, uint32_t width, uint32_t height);
    static void Shutdown();

    static void create_window();
    static void destroy_window();
    static bool pump_messages();
    // static bool create_vulkan_surface(VKCommonParameters &params);
    static void set_title(std::string title);
    static std::chrono::time_point<std::chrono::high_resolution_clock> get_current_time();

    // WINDOWING INFO
#ifdef Q_PLATFORM_LINUX
    // LINUX WINDOWING
    Display* display;
    Window handle;
    Atom wm_delete_window;
    static void handle_x11_event(XEvent& event);

#elif defined(Q_PLATFORM_WINDOWS)
    // WINDOWS WINDOWING SHIT
    HWND hWindow;
    HINSTANCE hInstance;
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif // END PLATFORM SPECIFIC WINDOWING INFO

    bool should_quit;
    std::string name;
    uint32_t width;
    uint32_t height;

private:
    static Keys _translateKey(uint32_t); // used to standardize keyboard input
                                  // across the different platforms
};
