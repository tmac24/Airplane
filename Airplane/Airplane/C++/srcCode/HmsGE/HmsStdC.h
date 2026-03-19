#pragma once

#include <cmath>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif  // MAX

#if _MSC_VER >= 1600 || defined(__MINGW32__)
#include <stdint.h>
#else
#include "stdint.h"
#endif

#ifdef _WIN32
#ifndef __SSIZE_T
#define __SSIZE_T
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif // __SSIZE_T
#endif