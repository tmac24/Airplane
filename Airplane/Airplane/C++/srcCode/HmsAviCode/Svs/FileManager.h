#ifndef __FINE_MANAGER__
#define __FINE_MANAGER__

#include "Common.h"
 
int loadBinaryFile(const char* file, unsigned char**data, unsigned int* len);

int FileManager_LoadBinaryFile(const char* file, unsigned char**data);

#endif // !__FILE_MANAGER__