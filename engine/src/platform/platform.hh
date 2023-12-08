#pragma once
#include "core/input.hh"
#include "core/events.hh"
#include "platform/platform_timer.hh"
#include "renderer/vulkan/vulkan_types.hh"
#include "stdafx.hh"
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <chrono>
#include <cstdint>

class Platform {
public:
    Platform(std::string name, uint32_t width, uint32_t height);
    static bool Startup(std::string name, uint32_t width, uint32_t height);
    static void Shutdown();

    static void* Allocate(uint64_t size, bool aligned);
    static void  Free(void* block, bool aligned);
    static void* ZeroMem(void* block, uint64_t size);
    static void* CopyMem(void* dst, const void* src, uint64_t size);
    static void* SetMem(void* dst, int32_t value, uint64_t size);

    static void ConsoleWrite(const char* message, uint8_t color);
    static void ConsoleError(const char* message, uint8_t color);

    static void create_window();
    static void destroy_window();
    static bool pump_messages();
    static bool create_vulkan_surface(VKContext& context);
    static void set_title(std::string title);
    static void get_vulkan_extensions(std::vector<const char*>& exts);
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
