/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2014-2015 Chukong Technologies

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

#ifndef MathUtild_H_
#define MathUtild_H_

#ifdef __SSE__
#include <xmmintrin.h>
#endif

#include "math/HmsMathBase.h"


NS_HMS_MATH_BEGIN

/**
 * Defines a math utility class.
 *
 * This is primarily used for optimized internal math operations.
 */
class HMS_DLL MathUtild
{
    friend class Mat4d;
    friend class Vec3d;

public:

    /**
     * Updates the given scalar towards the given target using a smoothing function.
     * The given response time determines the amount of smoothing (lag). A longer
     * response time yields a smoother result and more lag. To force the scalar to
     * follow the target closely, provide a response time that is very small relative
     * to the given elapsed time.
     *
     * @param x the scalar to update.
     * @param target target value.
     * @param elapsedTime elapsed time between calls.
     * @param responseTime response time (in the same units as elapsedTime).
     */
    static void smooth(double* x, double target, double elapsedTime, double responseTime);

    /**
     * Updates the given scalar towards the given target using a smoothing function.
     * The given rise and fall times determine the amount of smoothing (lag). Longer
     * rise and fall times yield a smoother result and more lag. To force the scalar to
     * follow the target closely, provide rise and fall times that are very small relative
     * to the given elapsed time.
     *
     * @param x the scalar to update.
     * @param target target value.
     * @param elapsedTime elapsed time between calls.
     * @param riseTime response time for rising slope (in the same units as elapsedTime).
     * @param fallTime response time for falling slope (in the same units as elapsedTime).
     */
    static void smooth(double* x, double target, double elapsedTime, double riseTime, double fallTime);
    
    /**
     * Linearly interpolates between from value to to value by alpha which is in
     * the range [0,1]
     *
     * @param from the from value.
     * @param to the to value.
     * @param alpha the alpha value between [0,1]
     *
     * @return interpolated double value
     */
    static double lerp(double from, double to, double alpha);
private:
    //Indicates that if neon is enabled
    static bool isNeon32Enabled();
    static bool isNeon64Enabled();
private:
#ifdef __SSE__
    static void addMatrix(const __m128 m[4], double scalar, __m128 dst[4]);
    
    static void addMatrix(const __m128 m1[4], const __m128 m2[4], __m128 dst[4]);
    
    static void subtractMatrix(const __m128 m1[4], const __m128 m2[4], __m128 dst[4]);
    
    static void multiplyMatrix(const __m128 m[4], double scalar, __m128 dst[4]);
    
    static void multiplyMatrix(const __m128 m1[4], const __m128 m2[4], __m128 dst[4]);
    
    static void negateMatrix(const __m128 m[4], __m128 dst[4]);
    
    static void transposeMatrix(const __m128 m[4], __m128 dst[4]);
        
    static void transformVec4(const __m128 m[4], const __m128& v, __m128& dst);
#endif
    static void addMatrix(const double* m, double scalar, double* dst);

    static void addMatrix(const double* m1, const double* m2, double* dst);

    static void subtractMatrix(const double* m1, const double* m2, double* dst);

    static void multiplyMatrix(const double* m, double scalar, double* dst);

    static void multiplyMatrix(const double* m1, const double* m2, double* dst);

    static void negateMatrix(const double* m, double* dst);

    static void transposeMatrix(const double* m, double* dst);

    static void transformVec4(const double* m, double x, double y, double z, double w, double* dst);

    static void transformVec4(const double* m, const double* v, double* dst);

    static void crossVec3d(const double* v1, const double* v2, double* dst);

};

NS_HMS_MATH_END

#define MATRIX_SIZE ( sizeof(double) * 16)

#endif
