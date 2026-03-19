/**
Copyright 2013 BlackBerry Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 
Original file from GamePlay3D: http://gameplay3d.org

This file was modified to fit the HmsAviPf-x project
*/

#include "MathUtild.h"

//#define USE_NEON32        : neon 32 code will be used
//#define USE_NEON64        : neon 64 code will be used
//#define INCLUDE_NEON32    : neon 32 code included
//#define INCLUDE_NEON64    : neon 64 code included
//#define USE_SSE           : SSE code used
//#define INCLUDE_SSE       : SSE code included

#if 0

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
    #if defined (__arm64__)
    #define USE_NEON64
    #define INCLUDE_NEON64
    #elif defined (__ARM_NEON__)
    #define USE_NEON32
    #define INCLUDE_NEON32
    #else
    #endif
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
    #if defined (__arm64__) || defined (__aarch64__)
    #define USE_NEON64
    #define INCLUDE_NEON64
    #elif defined (__ARM_NEON__)
    #define INCLUDE_NEON32
    #else
    #endif
#else

#endif

#if defined (__SSE__)
#define USE_SSE
#define INCLUDE_SSE
#endif

#ifdef INCLUDE_NEON32
#include "MathUtildNeon.inl"
#endif

#ifdef INCLUDE_NEON64
#include "MathUtildNeon64.inl"
#endif

#ifdef INCLUDE_SSE
#include "MathUtildSSE.inl"
#endif
#endif

#include "MathUtild.inl"

NS_HMS_MATH_BEGIN

void MathUtild::smooth(double* x, double target, double elapsedTime, double responseTime)
{
    GP_ASSERT(x);
    
    if (elapsedTime > 0)
    {
        *x += (target - *x) * elapsedTime / (elapsedTime + responseTime);
    }
}

void MathUtild::smooth(double* x, double target, double elapsedTime, double riseTime, double fallTime)
{
    GP_ASSERT(x);
    
    if (elapsedTime > 0)
    {
        double delta = target - *x;
        *x += delta * elapsedTime / (elapsedTime + (delta > 0 ? riseTime : fallTime));
    }
}

double MathUtild::lerp(double from, double to, double alpha)
{
    return from * (1.0f - alpha) + to * alpha;
}

bool MathUtild::isNeon32Enabled()
{
#ifdef USE_NEON32
    return true;
#elif (defined (INCLUDE_NEON32) && (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID) )
    class AnrdoidNeonChecker
    {
    public:
        AnrdoidNeonChecker()
        {
            if (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM && (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0)
                _isNeonEnabled = true;
            else
                _isNeonEnabled = false;
        }
        bool isNeonEnabled() const { return _isNeonEnabled; }
    private:
        bool _isNeonEnabled;
    };
    static AnrdoidNeonChecker checker;
    return checker.isNeonEnabled();
#else
    return false;
#endif
}

bool MathUtild::isNeon64Enabled()
{
#ifdef USE_NEON64
    return true;
#else
    return false;
#endif
}

void MathUtild::addMatrix(const double* m, double scalar, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::addMatrix(m, scalar, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::addMatrix(m, scalar, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::addMatrix(m, scalar, dst);
    else MathUtildC::addMatrix(m, scalar, dst);
#else
    MathUtildC::addMatrix(m, scalar, dst);
#endif
}

void MathUtild::addMatrix(const double* m1, const double* m2, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::addMatrix(m1, m2, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::addMatrix(m1, m2, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::addMatrix(m1, m2, dst);
    else MathUtildC::addMatrix(m1, m2, dst);
#else
    MathUtildC::addMatrix(m1, m2, dst);
#endif
}

void MathUtild::subtractMatrix(const double* m1, const double* m2, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::subtractMatrix(m1, m2, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::subtractMatrix(m1, m2, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::subtractMatrix(m1, m2, dst);
    else MathUtildC::subtractMatrix(m1, m2, dst);
#else
    MathUtildC::subtractMatrix(m1, m2, dst);
#endif
}

void MathUtild::multiplyMatrix(const double* m, double scalar, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::multiplyMatrix(m, scalar, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::multiplyMatrix(m, scalar, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::multiplyMatrix(m, scalar, dst);
    else MathUtildC::multiplyMatrix(m, scalar, dst);
#else
    MathUtildC::multiplyMatrix(m, scalar, dst);
#endif
}

void MathUtild::multiplyMatrix(const double* m1, const double* m2, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::multiplyMatrix(m1, m2, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::multiplyMatrix(m1, m2, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::multiplyMatrix(m1, m2, dst);
    else MathUtildC::multiplyMatrix(m1, m2, dst);
#else
    MathUtildC::multiplyMatrix(m1, m2, dst);
#endif
}

void MathUtild::negateMatrix(const double* m, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::negateMatrix(m, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::negateMatrix(m, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::negateMatrix(m, dst);
    else MathUtildC::negateMatrix(m, dst);
#else
    MathUtildC::negateMatrix(m, dst);
#endif
}

void MathUtild::transposeMatrix(const double* m, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::transposeMatrix(m, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::transposeMatrix(m, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::transposeMatrix(m, dst);
    else MathUtildC::transposeMatrix(m, dst);
#else
    MathUtildC::transposeMatrix(m, dst);
#endif
}

void MathUtild::transformVec4(const double* m, double x, double y, double z, double w, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::transformVec4(m, x, y, z, w, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::transformVec4(m, x, y, z, w, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::transformVec4(m, x, y, z, w, dst);
    else MathUtildC::transformVec4(m, x, y, z, w, dst);
#else
    MathUtildC::transformVec4(m, x, y, z, w, dst);
#endif
}

void MathUtild::transformVec4(const double* m, const double* v, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::transformVec4(m, v, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::transformVec4(m, v, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::transformVec4(m, v, dst);
    else MathUtildC::transformVec4(m, v, dst);
#else
    MathUtildC::transformVec4(m, v, dst);
#endif
}

void MathUtild::crossVec3d(const double* v1, const double* v2, double* dst)
{
#ifdef USE_NEON32
    MathUtildNeon::crossVec3(v1, v2, dst);
#elif defined (USE_NEON64)
    MathUtildNeon64::crossVec3(v1, v2, dst);
#elif defined (INCLUDE_NEON32)
    if(isNeon32Enabled()) MathUtildNeon::crossVec3(v1, v2, dst);
    else MathUtildC::crossVec3(v1, v2, dst);
#else
    MathUtildC::crossVec3(v1, v2, dst);
#endif
}

NS_HMS_MATH_END
