#include "GLHeader.h"
#include <stdio.h>
#include "Log.h"

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS
#include "OpenGLES/gltypes.h"
#include "OpenGLES/ES2/gl.h"

#endif

#ifndef M96  
#define CPU_CACHE_SIZE (1024*8) //8K FOR windows
#else
#define CPU_CACHE_SIZE (1024) //1K FOR p2020
#endif

void GLHeader_PrintGlError()
{
    GLenum error = glGetError();
    if (error)
    {
        //const GLubyte *errStr = gluErrorString(error);
        //LogWrite(LOG_LEVEL_FATAL, "gl error: %s,%s,%d\n", errStr, __FILE__, __LINE__);

        //printf("gl error %s \n",errStr);
    }
}


CVec4f GetColorByAltAlpha(float t)
{
    // 	static CVec4f minColor = Vec_CreatVec4f(104 / 255.0, 234 / 255.0, 87 / 255.0, 1.0);
    // 	static CVec4f maxColor = Vec_CreatVec4f (234 / 255.0, 42 / 255.0, 42 / 255.0, 1.0);
    static CVec4f minColor = { 0 / 255.0, 0 / 255.0, 255 / 255.0, 1.0 };
    static CVec4f midColor = { 255 / 255.0, 255 / 255.0, 0 / 255.0, 1.0 };
    static CVec4f maxColor = { 255 / 255.0, 0 / 255.0, 0 / 255.0, 1.0 };

    CVec4f color;

    if (t >= 0.5)
    {
        t = (t - 0.5) / 0.5;
        color._v[0] = midColor._v[0] * (1.0f - t) + maxColor._v[0] * t;
        color._v[1] = midColor._v[1] * (1.0f - t) + maxColor._v[1] * t;
        color._v[2] = midColor._v[2] * (1.0f - t) + maxColor._v[2] * t;
        color._v[3] = midColor._v[3] * (1.0f - t) + maxColor._v[3] * t;
    }
    else
    {
        t = t / 0.5;
        color._v[0] = minColor._v[0] * (1.0f - t) + midColor._v[0] * t;
        color._v[1] = minColor._v[1] * (1.0f - t) + midColor._v[1] * t;
        color._v[2] = minColor._v[2] * (1.0f - t) + midColor._v[2] * t;
        color._v[3] = minColor._v[3] * (1.0f - t) + midColor._v[3] * t;
    }
    color._v[3] = 1.0;

    return color;
}

CVec4f GetColorByAltNoAlphaX(float alt)
{
    static CVec4f  colores[] =
    {
        1 / 255.0f, 0 / 255.0f, 76 / 255.0f, 1.0f,
        207 / 255.0f, 222 / 255.0f, 174 / 255.0f, 1.0f,
        223 / 255.0f, 231 / 255.0f, 196 / 255.0f, 1.0f,
        207 / 255.0f, 222 / 255.0f, 174 / 255.0f, 1.0f,
        255 / 255.0f, 244 / 255.0f, 219 / 255.0f, 1.0f,
        247 / 255.0f, 232 / 255.0f, 184 / 255.0f, 1.0f,
        242 / 255.0f, 213 / 255.0f, 139 / 255.0f, 1.0f,
        228 / 255.0f, 191 / 255.0f, 155 / 255.0f, 1.0f,
        238 / 255.0f, 180 / 255.0f, 127 / 255.0f, 1.0f,
        180 / 255.0f, 142 / 255.0f, 63 / 255.0f, 1.0f
    };
    unsigned int i = 0;
    if (alt <= -1)
    {
        return colores[0];
    }
    else	if (alt <= 0.0)
    {
        return colores[i += 1];
    }
    else if (alt <= 1000)
    {
        return colores[i += 2];
    }
    else if (alt <= 2000)
    {
        return colores[i += 3];
    }
    else if (alt <= 3000)
    {
        return colores[i += 4];
    }
    else if (alt <= 5000)
    {
        return colores[i += 5];
    }
    else if (alt <= 7000)
    {
        return colores[i += 1];
    }
    else if (alt <= 9000)
    {
        return colores[i += 6];
    }
    else if (alt <= 12000)
    {
        return colores[i += 7];
    }
    else
    {
        return colores[i += 8];
    }

}

double getd_demical(double num)
{
    return (double)(num - (int)num);
}

float getf_demical(float num)
{
    return (float)(num - (int)num);
}

//when copy block size larger than cpu cache size, copy it more times
void memcpy_op(void* dst, void* src, int n)
{
    if (0 == dst || 0 == src){
        return;
    }

    if (n < CPU_CACHE_SIZE)
    {
        memcpy(dst, src, n);
    }
    else
    {
        unsigned int i = 0;
        unsigned int cp_n = n / CPU_CACHE_SIZE;
        unsigned int cp_c = CPU_CACHE_SIZE;
        unsigned int offset = 0;
#if 0//jiangyan: 如下代码在windows 下要崩溃
        for(i = 0; i < cp_n; i++) 
            memcpy((void*)((unsigned int)(dst)+cp_c*i), (void*)((unsigned int)(src)+cp_c*i), cp_c);

        memcpy((void*)((unsigned int)(dst)+cp_c*(i)), (void*)((unsigned int)(src)+cp_c*(i)), n-cp_c*(cp_n));
#else//jiangyan: 修改如下， windows运行
        for (i = 0; i < cp_n; i++)
        {
            memcpy((unsigned char*)(dst)+cp_c*i, (unsigned char*)(src)+cp_c*i, cp_c);
        }

        memcpy((unsigned char*)(dst)+cp_c*(i), (unsigned char*)(src)+cp_c*(i), n - cp_c*(cp_n));
#endif
    }
}
