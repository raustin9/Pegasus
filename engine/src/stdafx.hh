#pragma once
#include "defines.hh"

#ifdef Q_PLATFORM_WINDOWS
#include <windows.h>
#include <shellapi.h>
#elif defined(Q_PLATFORM_LINUX)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif

// std
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <array>
#include <string.h>
#include <assert.h>
