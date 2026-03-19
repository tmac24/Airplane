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

#include "Vec3d.h"
#include "Mat4d.h"

NS_HMS_MATH_BEGIN


inline bool Vec3d::isZero() const
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

inline bool Vec3d::isOne() const
{
    return x == 1.0f && y == 1.0f && z == 1.0f;
}

inline void Vec3d::add(const Vec3d& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}

inline void Vec3d::add(double xx, double yy, double zz)
{
    x += xx;
    y += yy;
    z += zz;
}

inline double Vec3d::length() const
{
    return sqrt(x * x + y * y + z * z);
}

inline double Vec3d::lengthSquared() const
{
    return (x * x + y * y + z * z);
}

inline void Vec3d::negate()
{
    x = -x;
    y = -y;
    z = -z;
}

inline void Vec3d::scale(double scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
}

inline Vec3d Vec3d::lerp(const Vec3d &target, double alpha) const
{
    return *this * (1.f - alpha) + target * alpha;
}

inline void Vec3d::set(double xx, double yy, double zz)
{
    this->x = xx;
    this->y = yy;
    this->z = zz;
}

inline void Vec3d::set(const double* array)
{
    GP_ASSERT(array);

    x = array[0];
    y = array[1];
    z = array[2];
}

inline void Vec3d::set(const Vec3d& v)
{
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

inline void Vec3d::set(const Vec3d& p1, const Vec3d& p2)
{
    x = p2.x - p1.x;
    y = p2.y - p1.y;
    z = p2.z - p1.z;
}

inline void Vec3d::setZero()
{
    x = y = z = 0.0f;
}

inline void Vec3d::subtract(const Vec3d& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

inline const Vec3d Vec3d::operator+(const Vec3d& v) const
{
    Vec3d result(*this);
    result.add(v);
    return result;
}

inline Vec3d& Vec3d::operator+=(const Vec3d& v)
{
    add(v);
    return *this;
}

inline const Vec3d Vec3d::operator-(const Vec3d& v) const
{
    Vec3d result(*this);
    result.subtract(v);
    return result;
}

inline Vec3d& Vec3d::operator-=(const Vec3d& v)
{
    subtract(v);
    return *this;
}

inline const Vec3d Vec3d::operator-() const
{
    Vec3d result(*this);
    result.negate();
    return result;
}

inline const Vec3d Vec3d::operator*(double s) const
{
    Vec3d result(*this);
    result.scale(s);
    return result;
}

inline Vec3d& Vec3d::operator*=(double s)
{
    scale(s);
    return *this;
}

inline const Vec3d Vec3d::operator/(const double s) const
{
    return Vec3d(this->x / s, this->y / s, this->z / s);
}

inline bool Vec3d::operator==(const Vec3d& v) const
{
    return x==v.x && y==v.y && z==v.z;
}

inline bool Vec3d::operator!=(const Vec3d& v) const
{
    return x!=v.x || y!=v.y || z!=v.z;
}

inline const Vec3d operator*(double x, const Vec3d& v)
{
    Vec3d result(v);
    result.scale(x);
    return result;
}

NS_HMS_MATH_END
