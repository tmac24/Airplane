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
#include "MathUtild.h"


NS_HMS_MATH_BEGIN

// returns true if segment A-B intersects with segment C-D. S->E is the ovderlap part
bool isOneDimensionSegmentOverlap(double A, double B, double C, double D, double *S, double * E)
{
    double ABmin = std::min(A, B);
    double ABmax = std::max(A, B);
    double CDmin = std::min(C, D);
    double CDmax = std::max(C, D);
    
    if (ABmax < CDmin || CDmax < ABmin)
    {
        // ABmin->ABmax->CDmin->CDmax or CDmin->CDmax->ABmin->ABmax
        return false;
    }
    else
    {
        if (ABmin >= CDmin && ABmin <= CDmax)
        {
            // CDmin->ABmin->CDmax->ABmax or CDmin->ABmin->ABmax->CDmax
            if (S != nullptr) *S = ABmin;
            if (E != nullptr) *E = CDmax < ABmax ? CDmax : ABmax;
        }
        else if (ABmax >= CDmin && ABmax <= CDmax)
        {
            // ABmin->CDmin->ABmax->CDmax
            if (S != nullptr) *S = CDmin;
            if (E != nullptr) *E = ABmax;
        }
        else
        {
            // ABmin->CDmin->CDmax->ABmax
            if (S != nullptr) *S = CDmin;
            if (E != nullptr) *E = CDmax;
        }
        return true;
    }
}

// cross product of 2 vector. A->B X C->D
double crossProduct2Vector(const Vec2d& A, const Vec2d& B, const Vec2d& C, const Vec2d& D)
{
    return (D.y - C.y) * (B.x - A.x) - (D.x - C.x) * (B.y - A.y);
}

double Vec2d::angle(const Vec2d& v1, const Vec2d& v2)
{
    double dz = v1.x * v2.y - v1.y * v2.x;
    return atan2f(fabsf(dz) + MATH_FLOAT_SMALL, dot(v1, v2));
}

void Vec2d::add(const Vec2d& v1, const Vec2d& v2, Vec2d* dst)
{
    GP_ASSERT(dst);

    dst->x = v1.x + v2.x;
    dst->y = v1.y + v2.y;
}

void Vec2d::clamp(const Vec2d& min, const Vec2d& max)
{
    GP_ASSERT(!(min.x > max.x || min.y > max.y ));

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
}

void Vec2d::clamp(const Vec2d& v, const Vec2d& min, const Vec2d& max, Vec2d* dst)
{
    GP_ASSERT(dst);
    GP_ASSERT(!(min.x > max.x || min.y > max.y ));

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
}

double Vec2d::distance(const Vec2d& v) const
{
    double dx = v.x - x;
    double dy = v.y - y;

    return sqrt(dx * dx + dy * dy);
}

double Vec2d::dot(const Vec2d& v1, const Vec2d& v2)
{
    return (v1.x * v2.x + v1.y * v2.y);
}

double Vec2d::crossproduct(const Vec2d& v) const
{
	return crossproduct(*this, v);
}

double Vec2d::crossproduct(const Vec2d& v1, const Vec2d& v2)
{
	return v1.x * v2.y - v2.x * v1.y;
}

double Vec2d::length() const
{
    return sqrt(x * x + y * y);
}

void Vec2d::normalize()
{
    double n = x * x + y * y;
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
}

Vec2d Vec2d::getNormalized() const
{
    Vec2d v(*this);
    v.normalize();
    return v;
}

void Vec2d::rotate(const Vec2d& point, double angle)
{
    double sinAngle = sin(angle);
    double cosAngle = cos(angle);

    if (point.isZero())
    {
        double tempX = double(x * cosAngle - y * sinAngle);
        y = double(y * cosAngle + x * sinAngle);
        x = tempX;
    }
    else
    {
        double tempX = x - point.x;
        double tempY = y - point.y;

        x = double(tempX * cosAngle - tempY * sinAngle + point.x);
        y = double(tempY * cosAngle + tempX * sinAngle + point.y);
    }
}

void Vec2d::set(const double* array)
{
    GP_ASSERT(array);

    x = array[0];
    y = array[1];
}

void Vec2d::subtract(const Vec2d& v1, const Vec2d& v2, Vec2d* dst)
{
    GP_ASSERT(dst);

    dst->x = v1.x - v2.x;
    dst->y = v1.y - v2.y;
}

bool Vec2d::equals(const Vec2d& target) const
{
    return (fabs(this->x - target.x) < FLT_EPSILON)
        && (fabs(this->y - target.y) < FLT_EPSILON);
}

bool Vec2d::fuzzyEquals(const Vec2d& b, double var) const
{
    if(x - var <= b.x && b.x <= x + var)
        if(y - var <= b.y && b.y <= y + var)
            return true;
    return false;
}

double Vec2d::getAngle(const Vec2d& other) const
{
    Vec2d a2 = getNormalized();
    Vec2d b2 = other.getNormalized();
    double angle = atan2f(a2.cross(b2), a2.dot(b2));
    if( fabs(angle) < FLT_EPSILON ) return 0.f;
    return angle;
}

Vec2d Vec2d::rotateByAngle(const Vec2d& pivot, double angle) const
{
    return pivot + (*this - pivot).rotate(Vec2d::forAngle(angle));
}

bool Vec2d::isLineIntersect(const Vec2d& A, const Vec2d& B,
                            const Vec2d& C, const Vec2d& D,
                            double *S, double *T)
{
    // FAIL: Line undefined
    if ( (A.x==B.x && A.y==B.y) || (C.x==D.x && C.y==D.y) )
    {
        return false;
    }
    
    const double denom = crossProduct2Vector(A, B, C, D);
    
    if (denom == 0)
    {
        // Lines parallel or overlap
        return false;
    }
    
    if (S != nullptr) *S = crossProduct2Vector(C, D, C, A) / denom;
    if (T != nullptr) *T = crossProduct2Vector(A, B, C, A) / denom;
    
    return true;
}

bool Vec2d::isLineParallel(const Vec2d& A, const Vec2d& B,
                           const Vec2d& C, const Vec2d& D)
{
    // FAIL: Line undefined
    if ( (A.x==B.x && A.y==B.y) || (C.x==D.x && C.y==D.y) )
    {
        return false;
    }
    
    if (crossProduct2Vector(A, B, C, D) == 0)
    {
        // line overlap
        if (crossProduct2Vector(C, D, C, A) == 0 || crossProduct2Vector(A, B, C, A) == 0)
        {
            return false;
        }
        
        return true;
    }
    
    return false;
}

bool Vec2d::isLineOverlap(const Vec2d& A, const Vec2d& B,
                            const Vec2d& C, const Vec2d& D)
{
    // FAIL: Line undefined
    if ( (A.x==B.x && A.y==B.y) || (C.x==D.x && C.y==D.y) )
    {
        return false;
    }
    
    if (crossProduct2Vector(A, B, C, D) == 0 &&
        (crossProduct2Vector(C, D, C, A) == 0 || crossProduct2Vector(A, B, C, A) == 0))
    {
        return true;
    }
    
    return false;
}

bool Vec2d::isSegmentOverlap(const Vec2d& A, const Vec2d& B, const Vec2d& C, const Vec2d& D, Vec2d* S, Vec2d* E)
{
    
    if (isLineOverlap(A, B, C, D))
    {
        return isOneDimensionSegmentOverlap(A.x, B.x, C.x, D.x, &S->x, &E->x) &&
        isOneDimensionSegmentOverlap(A.y, B.y, C.y, D.y, &S->y, &E->y);
    }  
    
    return false;
}

bool Vec2d::isSegmentIntersect(const Vec2d& A, const Vec2d& B, const Vec2d& C, const Vec2d& D)
{
    double S, T;
    
    if (isLineIntersect(A, B, C, D, &S, &T )&&
        (S >= 0.0f && S <= 1.0f && T >= 0.0f && T <= 1.0f))
    {
        return true;
    }
    
    return false;
}

Vec2d Vec2d::getIntersectPoint(const Vec2d& A, const Vec2d& B, const Vec2d& C, const Vec2d& D)
{
    double S, T;
    
    if (isLineIntersect(A, B, C, D, &S, &T))
    {
        // Vec2d of intersection
        Vec2d P;
        P.x = A.x + S * (B.x - A.x);
        P.y = A.y + S * (B.y - A.y);
        return P;
    }
    
    return Vec2d::ZERO;
}


const Vec2d Vec2d::ZERO(0.0f, 0.0f);
const Vec2d Vec2d::ONE(1.0f, 1.0f);
const Vec2d Vec2d::UNIT_X(1.0f, 0.0f);
const Vec2d Vec2d::UNIT_Y(0.0f, 1.0f);
const Vec2d Vec2d::ANCHOR_MIDDLE(0.5f, 0.5f);
const Vec2d Vec2d::ANCHOR_BOTTOM_LEFT(0.0f, 0.0f);
const Vec2d Vec2d::ANCHOR_TOP_LEFT(0.0f, 1.0f);
const Vec2d Vec2d::ANCHOR_BOTTOM_RIGHT(1.0f, 0.0f);
const Vec2d Vec2d::ANCHOR_TOP_RIGHT(1.0f, 1.0f);
const Vec2d Vec2d::ANCHOR_MIDDLE_RIGHT(1.0f, 0.5f);
const Vec2d Vec2d::ANCHOR_MIDDLE_LEFT(0.0f, 0.5f);
const Vec2d Vec2d::ANCHOR_MIDDLE_TOP(0.5f, 1.0f);
const Vec2d Vec2d::ANCHOR_MIDDLE_BOTTOM(0.5f, 0.0f);

NS_HMS_MATH_END
