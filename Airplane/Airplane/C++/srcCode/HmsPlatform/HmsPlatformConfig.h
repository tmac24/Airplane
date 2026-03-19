//
// Created by Administrator on 2019/8/13.
//

#ifndef PROJ_ANDROID_STUDIO_HMSPLATFORMCONFIG_H
#define PROJ_ANDROID_STUDIO_HMSPLATFORMCONFIG_H

#define HMS_PLATFORM_UNKNOWN            0
#define HMS_PLATFORM_IOS                1
#define HMS_PLATFORM_ANDROID            2
#define HMS_PLATFORM_WIN32              3
#define HMS_PLATFORM_WINRT              4
#define HMS_PLATFORM_LINUX              5
#define HMS_PLATFORM_MAC                6
#define HMS_PLATFORM_VXWORKS            7

// Determine target platform by compile environment macro.
#define HMS_TARGET_PLATFORM             HMS_PLATFORM_UNKNOWN

// Apple: Mac and iOS
#if defined(__APPLE__) && !defined(ANDROID) // exclude android for binding generator.
#include <TargetConditionals.h>
    #if TARGET_OS_IPHONE // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
        #undef  HMS_TARGET_PLATFORM
        #define HMS_TARGET_PLATFORM         HMS_PLATFORM_IOS
    #elif TARGET_OS_MAC
        #undef  HMS_TARGET_PLATFORM
        #define HMS_TARGET_PLATFORM         HMS_PLATFORM_MAC
    #endif
#endif

// android
#if defined(ANDROID)
    #undef  HMS_TARGET_PLATFORM
    #define HMS_TARGET_PLATFORM         HMS_PLATFORM_ANDROID
#endif

// win32
#if defined(_WIN32)
    #undef  HMS_TARGET_PLATFORM
    #define HMS_TARGET_PLATFORM         HMS_PLATFORM_WIN32
#endif

// WinRT (Windows 8.1 Store/Phone App)
#if defined(WINRT)
    #undef  HMS_TARGET_PLATFORM
    #define HMS_TARGET_PLATFORM          HMS_PLATFORM_WINRT
#endif

// linux
#if defined(__linux) && !defined(__APPLE__) && !defined(ANDROID)
    #undef  HMS_TARGET_PLATFORM
    #define HMS_TARGET_PLATFORM         HMS_PLATFORM_LINUX
#endif

// vxworks
#if defined(__vxworks)
    #undef  HMS_TARGET_PLATFORM
    #define HMS_TARGET_PLATFORM         HMS_PLATFORM_VXWORKS
#endif

#endif //PROJ_ANDROID_STUDIO_HMSPLATFORMCONFIG_H
