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

#include "Mat4d.h"

NS_HMS_MATH_BEGIN

inline const Mat4d Mat4d::operator+(const Mat4d& mat) const
{
    Mat4d result(*this);
    result.add(mat);
    return result;
}

inline Mat4d& Mat4d::operator+=(const Mat4d& mat)
{
    add(mat);
    return *this;
}

inline const Mat4d Mat4d::operator-(const Mat4d& mat) const
{
    Mat4d result(*this);
    result.subtract(mat);
    return result;
}

inline Mat4d& Mat4d::operator-=(const Mat4d& mat)
{
    subtract(mat);
    return *this;
}

inline const Mat4d Mat4d::operator-() const
{
    Mat4d mat(*this);
    mat.negate();
    return mat;
}

inline const Mat4d Mat4d::operator*(const Mat4d& mat) const
{
    Mat4d result(*this);
    result.multiply(mat);
    return result;
}

inline Mat4d& Mat4d::operator*=(const Mat4d& mat)
{
    multiply(mat);
    return *this;
}

inline Vec3d& operator*=(Vec3d& v, const Mat4d& m)
{
    m.transformVector(&v);
    return v;
}

inline const Vec3d operator*(const Mat4d& m, const Vec3d& v)
{
    Vec3d x;
    m.transformVector(v, &x);
    return x;
}

inline Vec4& operator*=(Vec4& v, const Mat4d& m)
{
    m.transformVector(&v);
    return v;
}

inline const Vec4 operator*(const Mat4d& m, const Vec4& v)
{
    Vec4 x;
    m.transformVector(v, &x);
    return x;
}

NS_HMS_MATH_END
