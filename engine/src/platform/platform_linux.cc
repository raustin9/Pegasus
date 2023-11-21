/*
 * platform_linux.cc
 *
 * This holds the linux implementation of the platform layer
 */

#include "platform.hh"
#include <chrono>

#ifdef Q_PLATFORM_LINUX

#include "core/input.hh"
#include "renderer/vkcommon.hh"
#include <vulkan/vulkan_core.h>
#include "stdafx.hh"

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/X.h>
#include <cstdio>

    
// WINDOWING
Platform::Platform(std::string name, uint32_t width, uint32_t height) 
    : name{name}, width{width}, height{height},
{
    this->display = nullptr;
    this->wm_delete_window = 0;
    this->should_quit = false;
}

std::chrono::time_point<std::chrono::high_resolution_clock>
Platform::get_current_time() {
    return std::chrono::high_resolution_clock::now();
}

// Set the title of the window
void
Platform::set_title(std::string title) {
    XSetStandardProperties(
            display,
            handle,
            title.c_str(),
            title.c_str(),
            None,
            nullptr,
            0,
            nullptr);
}

// Create an instance of a window
void
Platform::create_window() {
    // Open connection to the X server
    this->display = XOpenDisplay(nullptr);

    // Create the window
    unsigned long white = WhitePixel(display, DefaultScreen(display));
    handle = XCreateSimpleWindow(
            display, 
            DefaultRootWindow(display), 
            0, 
            0, 
            width, 
            height, 
            0, 
            white, 
            white);

    // Set the event types the window wants to be notified by the X server
    XSelectInput(display, handle, KeyPressMask | KeyReleaseMask | StructureNotifyMask | ExposureMask);

    // Also request to be notified when the window is deleted
    Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", true);
    (void)wm_protocols;;
    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", true);

    XSetWMProtocols(display, handle, &wm_delete_window, 1);

    // Set window and icon names
    XSetStandardProperties(
        display,
        handle,
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
    XSetWMSizeHints(display, handle, &sizehints, XA_WM_NORMAL_HINTS);

    // Request to display the window on the screen, and flush teh request buffer
    XMapWindow(display, handle);
    XFlush(display);
}

void
Platform::destroy_window() {
    XDestroyWindow(display, handle);
    XCloseDisplay(display);
}

void
Platform::handle_x11_event(XEvent& event) {
    Keys key;
    xcb_keycode_t code = 0;
    KeySym keysym;
    switch (event.type) {
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
                should_quit = true;
            }
            break;
        case ConfigureNotify:
            if (static_cast<uint32_t>(event.xconfigure.width) != width
                || static_cast<uint32_t>(event.xconfigure.height) != height) {
                width = static_cast<uint32_t>(event.xconfigure.width);
                height = static_cast<uint32_t>(event.xconfigure.height);
                m_inputHandler.ProcessResize(
                    static_cast<uint32_t>(event.xconfigure.width), 
                    static_cast<uint32_t>(event.xconfigure.height));
            }

            break;
        
        case KeyPress:
            code = event.xkey.keycode;
            keysym = XkbKeycodeToKeysym(display, code, 0, code & ShiftMask ? 1: 0);
            key = _translateKey(keysym);
            m_inputHandler.ProcessKey(key, false);
            break;

        case KeyRelease:
            code = event.xkey.keycode;
            keysym = XkbKeycodeToKeysym(display, code, 0, code & ShiftMask ? 1: 0);
            key = _translateKey(keysym);
            m_inputHandler.ProcessKey(key, true);
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
    XFlush(display);

    return should_quit;
}

// Linux specific vulkan surface creation
bool
Platform::create_vulkan_surface(VKCommonParameters &params) {
    VkResult err = VK_SUCCESS;

    VkXlibSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.dpy = display;
    createInfo.window = handle;
    err = vkCreateXlibSurfaceKHR(params.Instance, &createInfo, params.Allocator, &params.PresentationSurface);

    return (err == VK_SUCCESS) ? true : false;
}


Keys
Platform::_translateKey(uint32_t code) {
    switch (code) {
        case XK_BackSpace:
            return KEY_BACKSPACE;
        case XK_Return:
            return KEY_ENTER;
        case XK_Tab:
            return KEY_TAB;
            //case XK_Shift: return KEY_SHIFT;
            //case XK_Control: return KEY_CONTROL;

        case XK_Pause:
            return KEY_PAUSE;
        case XK_Caps_Lock:
            return KEY_CAPITAL;

        case XK_Escape:
            return KEY_ESCAPE;

            // Not supported
            // case : return KEY_CONVERT;
            // case : return KEY_NONCONVERT;
            // case : return KEY_ACCEPT;

        case XK_Mode_switch:
            return KEY_MODECHANGE;

        case XK_space:
            return KEY_SPACE;
        case XK_Prior:
            return KEY_PRIOR;
        case XK_Next:
            return KEY_NEXT;
        case XK_End:
            return KEY_END;
        case XK_Home:
            return KEY_HOME;
        case XK_Left:
            return KEY_LEFT;
        case XK_Up:
            return KEY_UP;
        case XK_Right:
            return KEY_RIGHT;
        case XK_Down:
            return KEY_DOWN;
        case XK_Select:
            return KEY_SELECT;
        case XK_Print:
            return KEY_PRINT;
        case XK_Execute:
            return KEY_EXECUTE;
        // case XK_snapshot: return KEY_SNAPSHOT; // not supported
        case XK_Insert:
            return KEY_INSERT;
        case XK_Delete:
            return KEY_DELETE;
        case XK_Help:
            return KEY_HELP;

        case XK_Meta_L:
            return KEY_LWIN;  // TODO: not sure this is right
        case XK_Meta_R:
            return KEY_RWIN;
            // case XK_apps: return KEY_APPS; // not supported

            // case XK_sleep: return KEY_SLEEP; //not supported

        case XK_KP_0:
            return KEY_NUMPAD0;
        case XK_KP_1:
            return KEY_NUMPAD1;
        case XK_KP_2:
            return KEY_NUMPAD2;
        case XK_KP_3:
            return KEY_NUMPAD3;
        case XK_KP_4:
            return KEY_NUMPAD4;
        case XK_KP_5:
            return KEY_NUMPAD5;
        case XK_KP_6:
            return KEY_NUMPAD6;
        case XK_KP_7:
            return KEY_NUMPAD7;
        case XK_KP_8:
            return KEY_NUMPAD8;
        case XK_KP_9:
            return KEY_NUMPAD9;
        case XK_multiply:
            return KEY_MULTIPLY;
        case XK_KP_Add:
            return KEY_ADD;
        case XK_KP_Separator:
            return KEY_SEPARATOR;
        case XK_KP_Subtract:
            return KEY_SUBTRACT;
        case XK_KP_Decimal:
            return KEY_DECIMAL;
        case XK_KP_Divide:
            return KEY_DIVIDE;
        case XK_F1:
            return KEY_F1;
        case XK_F2:
            return KEY_F2;
        case XK_F3:
            return KEY_F3;
        case XK_F4:
            return KEY_F4;
        case XK_F5:
            return KEY_F5;
        case XK_F6:
            return KEY_F6;
        case XK_F7:
            return KEY_F7;
        case XK_F8:
            return KEY_F8;
        case XK_F9:
            return KEY_F9;
        case XK_F10:
            return KEY_F10;
        case XK_F11:
            return KEY_F11;
        case XK_F12:
            return KEY_F12;
        case XK_F13:
            return KEY_F13;
        case XK_F14:
            return KEY_F14;
        case XK_F15:
            return KEY_F15;
        case XK_F16:
            return KEY_F16;
        case XK_F17:
            return KEY_F17;
        case XK_F18:
            return KEY_F18;
        case XK_F19:
            return KEY_F19;
        case XK_F20:
            return KEY_F20;
        case XK_F21:
            return KEY_F21;
        case XK_F22:
            return KEY_F22;
        case XK_F23:
            return KEY_F23;
        case XK_F24:
            return KEY_F24;

        case XK_Num_Lock:
            return KEY_NUMLOCK;
        case XK_Scroll_Lock:
            return KEY_SCROLL;

        case XK_KP_Equal:
            return KEY_NUMPAD_EQUAL;

        case XK_Shift_L:
            return KEY_LSHIFT;
        case XK_Shift_R:
            return KEY_RSHIFT;
        case XK_Control_L:
            return KEY_LCONTROL;
        case XK_Control_R:
            return KEY_RCONTROL;
        // case XK_Menu: return KEY_LMENU;
        case XK_Menu:
            return KEY_RMENU;

        case XK_semicolon:
            return KEY_SEMICOLON;
        case XK_plus:
            return KEY_PLUS;
        case XK_comma:
            return KEY_COMMA;
        case XK_minus:
            return KEY_MINUS;
        case XK_period:
            return KEY_PERIOD;
        case XK_slash:
            return KEY_SLASH;
        case XK_grave:
            return KEY_GRAVE;

        case XK_a:
        case XK_A:
            return KEY_A;
        case XK_b:
        case XK_B:
            return KEY_B;
        case XK_c:
        case XK_C:
            return KEY_C;
        case XK_d:
        case XK_D:
            return KEY_D;
        case XK_e:
        case XK_E:
            return KEY_E;
        case XK_f:
        case XK_F:
            return KEY_F;
        case XK_g:
        case XK_G:
            return KEY_G;
        case XK_h:
        case XK_H:
            return KEY_H;
        case XK_i:
        case XK_I:
            return KEY_I;
        case XK_j:
        case XK_J:
            return KEY_J;
        case XK_k:
        case XK_K:
            return KEY_K;
        case XK_l:
        case XK_L:
            return KEY_L;
        case XK_m:
        case XK_M:
            return KEY_M;
        case XK_n:
        case XK_N:
            return KEY_N;
        case XK_o:
        case XK_O:
            return KEY_O;
        case XK_p:
        case XK_P:
            return KEY_P;
        case XK_q:
        case XK_Q:
            return KEY_Q;
        case XK_r:
        case XK_R:
            return KEY_R;
        case XK_s:
        case XK_S:
            return KEY_S;
        case XK_t:
        case XK_T:
            return KEY_T;
        case XK_u:
        case XK_U:
            return KEY_U;
        case XK_v:
        case XK_V:
            return KEY_V;
        case XK_w:
        case XK_W:
            return KEY_W;
        case XK_x:
        case XK_X:
            return KEY_X;
        case XK_y:
        case XK_Y:
            return KEY_Y;
        case XK_z:
        case XK_Z:
            return KEY_Z;
        default:
            return KEYS_MAX_KEY;
    }
}

#endif // Q_PLATFORM_LINUX