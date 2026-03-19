#include "FileManager.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "GLHeader.h"


/*这个在VxWorks有高低字节问题 还得先留着*/
int loadBinaryFile(const char* file, unsigned char**data, unsigned int* len)
{
    int  nRes = 0;
    FILE* pFile = NULL;

#ifdef WIN32
    fopen_s(&pFile, file, "rb");
#else
    pFile = fopen(file, "rb");
#endif 
    if (pFile)
    {
        unsigned int w, h;
        unsigned int size = 0;
        unsigned char* orgData = NULL;
        fread(&w, 4, 1, pFile);
        fread(&h, 4, 1, pFile);
        *len = size = w * 2 * h + 8;
        orgData = (unsigned char*)malloc(size);
        if (orgData)
        {
            memcpy_op(orgData, &w, 4);
            memcpy_op(orgData + 4, &h, 4);
            fread(orgData + 8, size - 8, 1, pFile);
            *data = orgData;
            nRes = 1;
        }
        else
        {
            *data = 0;
            *len = 0;
            nRes = 0;
        }

        fclose(pFile);
    }

    return nRes;
}

int FileManager_LoadBinaryFile(const char* file, unsigned char**data)
{
    FILE* pFile = NULL;

    long len = 0;

#ifdef WIN32
    fopen_s(&pFile, file, "rb");
#else
    pFile = fopen(file, "rb");
#endif 
    if (pFile)
    {
        unsigned char* orgData = NULL;
        int res = 0;
        fpos_t pos;
        fseek(pFile, 0, SEEK_END);
        len = ftell(pFile);
        fgetpos(pFile, &pos);

#if defined(ANDROID)
        printf("FileManager_LoadBinaryFile %s  :%ld,pos:%d\n", file, len, (long)pos);
#elif defined(__linux)
        printf("FileManager_LoadBinaryFile %s  :%ld,pos:%d\n", file, len, pos.__pos);
#elif defined(__vxworks)
        printf("FileManager_LoadBinaryFile %s  :%ld,pos:%d\n", file, len, pos._Off);
#elif defined(_WIN32)
        printf("FileManager_LoadBinaryFile %s  :%ld,pos:%d\n", file, len, (int)pos);
#else

#endif

        orgData = (unsigned char*)malloc(len);
        if (orgData)
        {
            fseek(pFile, 0, SEEK_SET);
            res = fread(orgData, 1, len, pFile);
            //	res = fwrite(orgData, len, 1, pFile);
            *data = orgData;
            len = res;
        }
        else
        {
            *data = 0;
            len = 0;
        }

        fclose(pFile);
    }

    return len;
}
