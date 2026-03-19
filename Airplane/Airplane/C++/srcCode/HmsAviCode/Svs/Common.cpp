#include "Common.h"
#include "Global.h"

#include <stdlib.h>
#include "LEBEConvert.h"

#define _USE_WX_CONVERT__  1

extern char *s_taws_data_path;

const char *GetTawsDataPath(void)
{
    if (s_taws_data_path)
    {
        return s_taws_data_path;
    }
    s_taws_data_path = getenv("TAWS_DISPLAY_DATA");
    if (s_taws_data_path == NULL)
    {
#ifdef WIN32
        //s_taws_data_path = ".";
        s_taws_data_path = "./TAWS_DISPLAY_DATA";//jiangyan: mod
#else
        //s_taws_data_path = "/edisk/TAWS_DISPLAY_DATA";
        //s_taws_data_path = "/hostfs/root/E/TAWS_DISPLAY_DATA";
        //s_taws_data_path = "/bd0/TAWS_DISPLAY_DATA";
        s_taws_data_path = "/ata1:1/TAWS_DISPLAY_DATA";
#endif
    }
    return s_taws_data_path;
}

void  SetTawsDataPath(char* path)
{
    if (!s_taws_data_path)
    {
        s_taws_data_path = (char*)malloc(256);
        memset(s_taws_data_path, 0, 256);
        memcpy_op(s_taws_data_path, path, strlen(path));
    }
}
unsigned int LittleEndianToBigEndianUINT(unsigned int dat)
{
#ifdef __LITTLE_ENDIAN__
    return dat;
#endif
    return byte_swap_uint32(dat);
}


unsigned short LittleEndianToBigEndianUSHORT(unsigned short data)
{
#ifdef __LITTLE_ENDIAN__ 
    return data;
#endif 
    return byte_swap_uint16(data);
}

float LittleEndianToBigEndianFloat(float data)
{
#ifdef __LITTLE_ENDIAN__ 
    return data;
#endif 
    return  byte_swap_float32(data);
}


int LittleEndianToBigEndianInt(int data)
{
#ifdef __LITTLE_ENDIAN__ 
    return data;
#endif 
    return byte_swap_int32(data);
}
double LEndianToBEndianDouble(double val)
{
#ifdef __LITTLE_ENDIAN__ 
    return val;
#endif 
    return byte_swap_float64(val);
}
double LittleEndianToBigEndianDouble(double data)
{
#ifdef __LITTLE_ENDIAN__ 
    return data;
#endif 
    return  byte_swap_float64(data);
}
long long LittleEndianToBigEndianLongLong(long long data)
{
#ifdef __LITTLE_ENDIAN__ 
    return data;
#endif 
    return  byte_swap_int64(data);
}
long LittleEndianToBigEndianLong(long data)
{
#ifdef __LITTLE_ENDIAN__ 
    return data;
#endif  
    return  byte_swap_int32(data);
}
