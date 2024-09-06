#pragma once

/*
 *  This file contains type definitions, macros, and other useful things
 *  that will be used across the application
 */

// TODO: Platform detection

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

// FOR C  -- boolean types //
//typedef char b8;
//typedef int b32;
//#define TRUE 1
//#define FALSE 0

// TODO: add static assertions

// Platform Detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define Q_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64 bit is required for windows"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
#define Q_PLATFORM_LINUX 1
#elif defined(__unix__)
#define Q_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
#define Q_PLATFORM_POSIX 1
#elif __APPLE__
#include <TargetConditionals.h>
#define Q_PLATFORM_APPLE 1
#endif

#ifdef QEXPORT
#ifdef _MSC_VER
#define QAPI __declspec(dllexport)
#else
#define QAPI __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define QAPI __declspec(dllimport)
#else
#define QAPI
#endif
#endif

// Inlining
#ifdef _MSC_VER
#define PINLINE __forceinline
#define PNOINLINE __declspec(noinline)
#else
#define PINLINE static inline
#define PNOINLINE 
#endif