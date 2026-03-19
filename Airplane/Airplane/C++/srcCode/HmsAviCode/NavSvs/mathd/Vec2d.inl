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

#include "Vec2d.h"

NS_HMS_MATH_BEGIN

inline Vec2d::Vec2d()
: x(0.0f), y(0.0f)
{
}

inline Vec2d::Vec2d(double xx, double yy)
: x(xx), y(yy)
{
}

inline Vec2d::Vec2d(const double* array)
{
    set(array);
}

inline Vec2d::Vec2d(const Vec2d& p1, const Vec2d& p2)
{
    set(p1, p2);
}

inline Vec2d::Vec2d(const Vec2d& copy)
{
    set(copy);
}

inline Vec2d::~Vec2d()
{
}

inline bool Vec2d::isZero() const
{
    return x == 0.0f && y == 0.0f;
}

bool Vec2d::isOne() const
{
    return x == 1.0f && y == 1.0f;
}

inline void Vec2d::add(const Vec2d& v)
{
    x += v.x;
    y += v.y;
}

inline double Vec2d::distanceSquared(const Vec2d& v) const
{
    double dx = v.x - x;
    double dy = v.y - y;
    return (dx * dx + dy * dy);
}

inline double Vec2d::dot(const Vec2d& v) const
{
    return (x * v.x + y * v.y);
}

inline double Vec2d::lengthSquared() const
{
    return (x * x + y * y);
}

inline void Vec2d::negate()
{
    x = -x;
    y = -y;
}

inline void Vec2d::scale(double scalar)
{
    x *= scalar;
    y *= scalar;
}

inline void Vec2d::scale(const Vec2d& scale)
{
    x *= scale.x;
    y *= scale.y;
}

inline void Vec2d::set(double xx, double yy)
{
    this->x = xx;
    this->y = yy;
}

inline void Vec2d::set(const Vec2d& v)
{
    this->x = v.x;
    this->y = v.y;
}

inline void Vec2d::set(const Vec2d& p1, const Vec2d& p2)
{
    x = p2.x - p1.x;
    y = p2.y - p1.y;
}

void Vec2d::setZero()
{
    x = y = 0.0f;
}

inline void Vec2d::subtract(const Vec2d& v)
{
    x -= v.x;
    y -= v.y;
}

inline void Vec2d::smooth(const Vec2d& target, double elapsedTime, double responseTime)
{
    if (elapsedTime > 0)
    {
        *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
    }
}

inline const Vec2d Vec2d::operator+(const Vec2d& v) const
{
    Vec2d result(*this);
    result.add(v);
    return result;
}

inline Vec2d& Vec2d::operator+=(const Vec2d& v)
{
    add(v);
    return *this;
}

inline const Vec2d Vec2d::operator-(const Vec2d& v) const
{
    Vec2d result(*this);
    result.subtract(v);
    return result;
}

inline Vec2d& Vec2d::operator-=(const Vec2d& v)
{
    subtract(v);
    return *this;
}

inline const Vec2d Vec2d::operator-() const
{
    Vec2d result(*this);
    result.negate();
    return result;
}

inline const Vec2d Vec2d::operator*(double s) const
{
    Vec2d result(*this);
    result.scale(s);
    return result;
}

inline Vec2d& Vec2d::operator*=(double s)
{
    scale(s);
    return *this;
}

inline const Vec2d Vec2d::operator/(const double s) const
{
    return Vec2d(this->x / s, this->y / s);
}

inline bool Vec2d::operator<(const Vec2d& v) const
{
    if (x == v.x)
    {
        return y < v.y;
    }
    return x < v.x;
}

inline bool Vec2d::operator>(const Vec2d& v) const
{
    if (x == v.x)
    {
        return y > v.y;
    }
    return x > v.x;
}

inline bool Vec2d::operator==(const Vec2d& v) const
{
    return x==v.x && y==v.y;
}

inline bool Vec2d::operator!=(const Vec2d& v) const
{
    return x!=v.x || y!=v.y;
}

inline const Vec2d operator*(double x, const Vec2d& v)
{
    Vec2d result(v);
    result.scale(x);
    return result;
}

void Vec2d::setPoint(double xx, double yy)
{
    this->x = xx;
    this->y = yy;
}

NS_HMS_MATH_END
