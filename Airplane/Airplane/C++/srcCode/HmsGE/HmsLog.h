//
// Created by Administrator on 2019/8/22.
//

#ifndef PROJ_TAWS_ANDROID_STUDIO_HMSLOG_H
#define PROJ_TAWS_ANDROID_STUDIO_HMSLOG_H

#include "HmsPlatformConfig.h"
#include "HmsLogFile.h"

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
#include <android/log.h>
#define HMSLOG(...) __android_log_print(ANDROID_LOG_INFO   , "HmsLog", __VA_ARGS__)
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX)
#define HMSLOG(...)  printf(__VA_ARGS__);
#else
//#define HMSLOG(...)  printf(##__VA_ARGS__);

#define HMS_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define HMSLOG(fmt, ...) \
    do { \
        printf("[CCLOG][%s:%d] " fmt "\n", HMS_FILENAME, __LINE__, ##__VA_ARGS__); \
        fflush(stdout); \
    } while (0)

#endif


#if 1
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
#define HMSLOGFILE(...)  CHmsLogFile::GetInstance()->LogFormat(__VA_ARGS__)
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX)
#define HMSLOGFILE(...)  CHmsLogFile::GetInstance()->LogFormat(__VA_ARGS__)
#else
#define HMSLOGFILE(...)  CHmsLogFile::GetInstance()->LogFormat(##__VA_ARGS__)
#endif
#else
#define HMSLOGFILE(...)
#endif


#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
#define HMSLOGERROR(...)  CHmsLogFile::GetInstance()->LogFormat("\nHmsError: time:%s %s %s %d;", __DATE__, __TIME__, __FILE__, __LINE__); \
    CHmsLogFile::GetInstance()->LogFormat(__VA_ARGS__)
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX)
#define HMSLOGERROR(...)  CHmsLogFile::GetInstance()->LogFormat("\nHmsError: time:%s %s %s %d;", __DATE__, __TIME__, __FILE__, __LINE__); \
    CHmsLogFile::GetInstance()->LogFormat(__VA_ARGS__)
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
#define HMSLOGERROR(...) CHmsLogFile::GetInstance()->LogFormat("\nHmsError: time:%s %s %s %d;", __DATE__, __TIME__, __FILE__, __LINE__); \
    CHmsLogFile::GetInstance()->LogFormat(__VA_ARGS__)
#else
#define HMSLOGERROR(...)  CHmsLogFile::GetInstance()->LogFormat("\nHmsError: time:%s %s %s %d;", __DATE__, __TIME__, __FILE__, __LINE__); \
    CHmsLogFile::GetInstance()->LogFormat(##__VA_ARGS__)
#endif

#endif //PROJ_TAWS_ANDROID_STUDIO_HMSLOG_H
