/*
 * platform_linux.cc
 *
 * This holds the linux implementation of the platform layer
 */

#include "platform.hh"

#ifdef Q_PLATFORM_LINUX

#include "stdafx.hh"

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/X.h>
#include <cstdio>

#endif // Q_PLATFORM_LINUX
    
// WINDOWING
Platform::Platform(std::string name, uint32_t width, uint32_t height) 
    : name{name}, width{width}, height{height}
{
    this->display = nullptr;
    this->wm_delete_window = 0;
    this->should_quit = false;
}

// Create an instance of a window
void
Platform::create_window() {
    // Open connection to the X server
    this->display = XOpenDisplay(nullptr);

    // Create the window
    unsigned long white = WhitePixel(display, DefaultScreen(display));
    Window win = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, width, height, 0, white, white);

    // Set the event types the window wants to be notified by the X server
    XSelectInput(display, win, KeyPressMask | KeyReleaseMask);

    // Also request to be notified when the window is deleted
    // Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", true);
    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", true);

    XSetWMProtocols(display, win, &wm_delete_window, 1);

    // Set window and icon names
    XSetStandardProperties(
        display,
        win,
        name.c_str(),
        "Icon name",
        None,
        nullptr,
        0,
        nullptr);

    // Set the Size Hints with the minimum window size
    XSizeHints sizehints;
    sizehints.flags = PMinSize;
    sizehints.min_width = 640;
    sizehints.min_height = 360;

    // Tell window manager our hints about the minimum window size
    XSetWMSizeHints(display, win, &sizehints, XA_WM_NORMAL_HINTS);

    // Request to display the window on the screen, and flush teh request buffer
    XMapWindow(display, win);
    XFlush(display);
}

void
Platform::handle_x11_event(XEvent& event) {
    switch (event.type) {
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
                should_quit = true;
            }
            break;
        
        case KeyPress:
            switch (event.xkey.keycode) {
                case 0x9: // Escape
                    should_quit = true;
                    break;
            }
            // TODO: handle keypressa
            break;

        case KeyRelease:
            // TODO: handle key release
            break;
        
        default:
            break;
    }
}

bool
Platform::pump_messages() {
    XEvent event;
    while ((XPending(display) > 0)) {
        XNextEvent(display, &event);
        handle_x11_event(event);
    }

    return should_quit;
}
