#pragma once

#ifdef _DEBUG
#include <Windows.h>
#include "Winbase.h"
#include <stdio.h>

#pragma comment(lib, "Kernel32.lib")

#define SIM_TRACE(...)  {\
	char * strBuffer = new char[1024]; \
	strBuffer[0] = 0;\
	sprintf_s(strBuffer, 1024 , ##__VA_ARGS__);\
	OutputDebugStringA(strBuffer);\
	delete[] strBuffer;\
	}
#else
#define SIM_TRACE(...)
#endif