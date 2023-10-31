#pragma once
#include "defines.hh"

#ifdef Q_PLATFORM_WINDOWS
#include <windows.h>
#include <shellapi.h>
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(Q_PLATFORM_LINUX)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_NO_PLATFORM_XCB_KHR
#include <time.h>
#endif

// std
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <array>
#include <string.h>
#include <assert.h>
#include <vulkan/vulkan.h>
