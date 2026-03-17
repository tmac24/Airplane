/*
  Configuration defines for installed libtiff.
  This file maintained for backward compatibility. Do not use definitions
  from this file in your programs.
*/

#ifdef __vxworks
	#include "tiffconf_vxworks.h"
#elif defined(__APPLE__)
    #include "tiffconf_apple.h"
#else
	#include "tiffconf_win32.h"
#endif
