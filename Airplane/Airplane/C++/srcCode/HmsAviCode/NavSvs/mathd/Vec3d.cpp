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
#include "MathUtild.h"


NS_HMS_MATH_BEGIN

Vec3d::Vec3d()
    : x(0.0f), y(0.0f), z(0.0f)
{
}

Vec3d::Vec3d(double xx, double yy, double zz)
    : x(xx), y(yy), z(zz)
{
}

Vec3d::Vec3d(const double* array)
{
    set(array);
}

Vec3d::Vec3d(const Vec3d& p1, const Vec3d& p2)
{
    set(p1, p2);
}

Vec3d::Vec3d(const Vec3d& copy)
{
    set(copy);
}

Vec3d Vec3d::fromColor(unsigned int color)
{
    double components[3];
    int componentIndex = 0;
    for (int i = 2; i >= 0; --i)
    {
        int component = (color >> i*8) & 0x0000ff;

        components[componentIndex++] = static_cast<double>(component) / 255.0f;
    }

    Vec3d value(components);
    return value;
}

Vec3d::~Vec3d()
{
}

double Vec3d::angle(const Vec3d& v1, const Vec3d& v2)
{
    double dx = v1.y * v2.z - v1.z * v2.y;
    double dy = v1.z * v2.x - v1.x * v2.z;
    double dz = v1.x * v2.y - v1.y * v2.x;

    return atan2f(sqrt(dx * dx + dy * dy + dz * dz) + MATH_FLOAT_SMALL, dot(v1, v2));
}

void Vec3d::add(const Vec3d& v1, const Vec3d& v2, Vec3d* dst)
{
    GP_ASSERT(dst);

    dst->x = v1.x + v2.x;
    dst->y = v1.y + v2.y;
    dst->z = v1.z + v2.z;
}

void Vec3d::clamp(const Vec3d& min, const Vec3d& max)
{
    GP_ASSERT(!(min.x > max.x || min.y > max.y || min.z > max.z));

    // Clamp the x value.
    if (x < min.x)
        x = min.x;
    if (x > max.x)
        x = max.x;

    // Clamp the y value.
    if (y < min.y)
        y = min.y;
    if (y > max.y)
        y = max.y;

    // Clamp the z value.
    if (z < min.z)
        z = min.z;
    if (z > max.z)
        z = max.z;
}

void Vec3d::clamp(const Vec3d& v, const Vec3d& min, const Vec3d& max, Vec3d* dst)
{
    GP_ASSERT(dst);
    GP_ASSERT(!(min.x > max.x || min.y > max.y || min.z > max.z));

    // Clamp the x value.
    dst->x = v.x;
    if (dst->x < min.x)
        dst->x = min.x;
    if (dst->x > max.x)
        dst->x = max.x;

    // Clamp the y value.
    dst->y = v.y;
    if (dst->y < min.y)
        dst->y = min.y;
    if (dst->y > max.y)
        dst->y = max.y;

    // Clamp the z value.
    dst->z = v.z;
    if (dst->z < min.z)
        dst->z = min.z;
    if (dst->z > max.z)
        dst->z = max.z;
}

void Vec3d::cross(const Vec3d& v)
{
    cross(*this, v, this);
}

void Vec3d::cross(const Vec3d& v1, const Vec3d& v2, Vec3d* dst)
{
    GP_ASSERT(dst);

    // NOTE: This code assumes Vec3d struct members are contiguous doubles in memory.
    // We might want to revisit this (and other areas of code that make this assumption)
    // later to guarantee 100% safety/compatibility.
    MathUtild::crossVec3d(&v1.x, &v2.x, &dst->x);
}

double Vec3d::distance(const Vec3d& v) const
{
    double dx = v.x - x;
    double dy = v.y - y;
    double dz = v.z - z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}

double Vec3d::distanceSquared(const Vec3d& v) const
{
    double dx = v.x - x;
    double dy = v.y - y;
    double dz = v.z - z;

    return (dx * dx + dy * dy + dz * dz);
}

double Vec3d::dot(const Vec3d& v) const
{
    return (x * v.x + y * v.y + z * v.z);
}

double Vec3d::dot(const Vec3d& v1, const Vec3d& v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

void Vec3d::normalize()
{
    double n = x * x + y * y + z * z;
    // Already normalized.
    if (n == 1.0f)
        return;
    
    n = sqrt(n);
    // Too close to zero.
    if (n < MATH_TOLERANCE)
        return;
    
    n = 1.0f / n;
    x *= n;
    y *= n;
    z *= n;
}

Vec3d Vec3d::getNormalized() const
{
    Vec3d v(*this);
    v.normalize();
    return v;
}

void Vec3d::subtract(const Vec3d& v1, const Vec3d& v2, Vec3d* dst)
{
    GP_ASSERT(dst);

    dst->x = v1.x - v2.x;
    dst->y = v1.y - v2.y;
    dst->z = v1.z - v2.z;
}

void Vec3d::smooth(const Vec3d& target, double elapsedTime, double responseTime)
{
    if (elapsedTime > 0)
    {
        *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
    }
}

const Vec3d Vec3d::ZERO(0.0f, 0.0f, 0.0f);
const Vec3d Vec3d::ONE(1.0f, 1.0f, 1.0f);
const Vec3d Vec3d::UNIT_X(1.0f, 0.0f, 0.0f);
const Vec3d Vec3d::UNIT_Y(0.0f, 1.0f, 0.0f);
const Vec3d Vec3d::UNIT_Z(0.0f, 0.0f, 1.0f);

NS_HMS_MATH_END
