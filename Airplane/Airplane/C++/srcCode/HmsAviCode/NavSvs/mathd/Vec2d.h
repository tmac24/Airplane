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

#ifndef MATH_Vec2d_H
#define MATH_Vec2d_H

#include <algorithm>
#include <functional>
#include <math.h>
#include "math/HmsMathBase.h"
#include <float.h>
#include <iostream>
/**
 * @addtogroup base
 * @{
 */

NS_HMS_MATH_BEGIN

/** Clamp a value between from and to.
 */

inline double clampf(double value, double min_inclusive, double max_inclusive)
{
    if (min_inclusive > max_inclusive) {
        std::swap(min_inclusive, max_inclusive);
    }
    return value < min_inclusive ? min_inclusive : value < max_inclusive? value : max_inclusive;
}

class Mat4d;

/**
 * Defines a 2-element doubleing point vector.
 */
class HMS_DLL Vec2d
{
public:

    /**
     * The x coordinate.
     */
    double x;

    /**
     * The y coordinate.
     */
    double y;

    /**
     * Constructs a new vector initialized to all zeros.
     */
    Vec2d();

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param xx The x coordinate.
     * @param yy The y coordinate.
     */
    Vec2d(double xx, double yy);

    /**
     * Constructs a new vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y.
     */
    Vec2d(const double* array);

    /**
     * Constructs a vector that describes the direction between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    Vec2d(const Vec2d& p1, const Vec2d& p2);

    /**
     * Constructs a new vector that is a copy of the specified vector.
     *
     * @param copy The vector to copy.
     */
    Vec2d(const Vec2d& copy);

    /**
     * Destructor.
     */
    ~Vec2d();

    /**
     * Indicates whether this vector contains all zeros.
     *
     * @return true if this vector contains all zeros, false otherwise.
     */
    inline bool isZero() const;

    /**
     * Indicates whether this vector contains all ones.
     *
     * @return true if this vector contains all ones, false otherwise.
     */
    inline bool isOne() const;

    /**
     * Returns the angle (in radians) between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * 
     * @return The angle between the two vectors (in radians).
     */
    static double angle(const Vec2d& v1, const Vec2d& v2);

    /**
     * Adds the elements of the specified vector to this one.
     *
     * @param v The vector to add.
     */
    inline void add(const Vec2d& v);

    /**
     * Adds the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void add(const Vec2d& v1, const Vec2d& v2, Vec2d* dst);

    /**
     * Clamps this vector within the specified range.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     */
    void clamp(const Vec2d& min, const Vec2d& max);

    /**
     * Clamps the specified vector within the specified range and returns it in dst.
     *
     * @param v The vector to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @param dst A vector to store the result in.
     */
    static void clamp(const Vec2d& v, const Vec2d& min, const Vec2d& max, Vec2d* dst);

    /**
     * Returns the distance between this vector and v.
     *
     * @param v The other vector.
     * 
     * @return The distance between this vector and v.
     * 
     * @see distanceSquared
     */
    double distance(const Vec2d& v) const;

    /**
     * Returns the squared distance between this vector and v.
     *
     * When it is not necessary to get the exact distance between
     * two vectors (for example, when simply comparing the
     * distance between different vectors), it is advised to use
     * this method instead of distance.
     *
     * @param v The other vector.
     * 
     * @return The squared distance between this vector and v.
     * 
     * @see distance
     */
    inline double distanceSquared(const Vec2d& v) const;

    /**
     * Returns the dot product of this vector and the specified vector.
     *
     * @param v The vector to compute the dot product with.
     * 
     * @return The dot product.
     */
    inline double dot(const Vec2d& v) const;

    /**
     * Returns the dot product between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * 
     * @return The dot product between the vectors.
     */
    static double dot(const Vec2d& v1, const Vec2d& v2);

	/**
	* Returns the cross product of this vector and the specified vector.
	*
	* @param v The vector to compute the dot product with.
	*
	* @return The cross product.
	*/
	double crossproduct(const Vec2d& v) const;

	/**
	* Returns the cross product between the specified vectors.
	*
	* @param v1 The first vector.
	* @param v2 The second vector.
	*
	* @return The cross product between the vectors.
	*/
	static double crossproduct(const Vec2d& v1, const Vec2d& v2);

    /**
     * Computes the length of this vector.
     *
     * @return The length of the vector.
     * 
     * @see lengthSquared
     */
    double length() const;

    /**
     * Returns the squared length of this vector.
     *
     * When it is not necessary to get the exact length of a
     * vector (for example, when simply comparing the lengths of
     * different vectors), it is advised to use this method
     * instead of length.
     *
     * @return The squared length of the vector.
     * 
     * @see length
     */
    inline double lengthSquared() const;

    /**
     * Negates this vector.
     */
    inline void negate();

    /**
     * Normalizes this vector.
     *
     * This method normalizes this Vec2d so that it is of
     * unit length (in other words, the length of the vector
     * after calling this method will be 1.0f). If the vector
     * already has unit length or if the length of the vector
     * is zero, this method does nothing.
     * 
     * @return This vector, after the normalization occurs.
     */
    void normalize();

    /**
     Get the normalized vector.
     */
    Vec2d getNormalized() const;

    /**
     * Scales all elements of this vector by the specified value.
     *
     * @param scalar The scalar value.
     */
    inline void scale(double scalar);

    /**
     * Scales each element of this vector by the matching component of scale.
     *
     * @param scale The vector to scale by.
     */
    inline void scale(const Vec2d& scale);

    /**
     * Rotates this vector by angle (specified in radians) around the given point.
     *
     * @param point The point to rotate around.
     * @param angle The angle to rotate by (in radians).
     */
    void rotate(const Vec2d& point, double angle);

    /**
     * Sets the elements of this vector to the specified values.
     *
     * @param xx The new x coordinate.
     * @param yy The new y coordinate.
     */
    inline void set(double xx, double yy);

    /**
     * Sets the elements of this vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y.
     */
    void set(const double* array);

    /**
     * Sets the elements of this vector to those in the specified vector.
     *
     * @param v The vector to copy.
     */
    inline void set(const Vec2d& v);

    /**
     * Sets this vector to the directional vector between the specified points.
     * 
     * @param p1 The first point.
     * @param p2 The second point.
     */
    inline void set(const Vec2d& p1, const Vec2d& p2);

    /**
    * Sets the elements of this vector to zero.
    */
    inline void setZero();
	friend std::ostream & operator<<(std::ostream &out, const Vec2d& v);
    /**
     * Subtracts this vector and the specified vector as (this - v)
     * and stores the result in this vector.
     *
     * @param v The vector to subtract.
     */
    inline void subtract(const Vec2d& v);

    /**
     * Subtracts the specified vectors and stores the result in dst.
     * The resulting vector is computed as (v1 - v2).
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst The destination vector.
     */
    static void subtract(const Vec2d& v1, const Vec2d& v2, Vec2d* dst);

    /**
     * Updates this vector towards the given target using a smoothing function.
     * The given response time determines the amount of smoothing (lag). A longer
     * response time yields a smoother result and more lag. To force this vector to
     * follow the target closely, provide a response time that is very small relative
     * to the given elapsed time.
     *
     * @param target target value.
     * @param elapsedTime elapsed time between calls.
     * @param responseTime response time (in the same units as elapsedTime).
     */
    inline void smooth(const Vec2d& target, double elapsedTime, double responseTime);

    /**
     * Calculates the sum of this vector with the given vector.
     * 
     * Note: this does not modify this vector.
     * 
     * @param v The vector to add.
     * @return The vector sum.
     */
    inline const Vec2d operator+(const Vec2d& v) const;

    /**
     * Adds the given vector to this vector.
     * 
     * @param v The vector to add.
     * @return This vector, after the addition occurs.
     */
    inline Vec2d& operator+=(const Vec2d& v);

    /**
     * Calculates the sum of this vector with the given vector.
     * 
     * Note: this does not modify this vector.
     * 
     * @param v The vector to add.
     * @return The vector sum.
     */
    inline const Vec2d operator-(const Vec2d& v) const;

    /**
     * Subtracts the given vector from this vector.
     * 
     * @param v The vector to subtract.
     * @return This vector, after the subtraction occurs.
     */
    inline Vec2d& operator-=(const Vec2d& v);

    /**
     * Calculates the negation of this vector.
     * 
     * Note: this does not modify this vector.
     * 
     * @return The negation of this vector.
     */
    inline const Vec2d operator-() const;

    /**
     * Calculates the scalar product of this vector with the given value.
     * 
     * Note: this does not modify this vector.
     * 
     * @param s The value to scale by.
     * @return The scaled vector.
     */
    inline const Vec2d operator*(double s) const;

    /**
     * Scales this vector by the given value.
     * 
     * @param s The value to scale by.
     * @return This vector, after the scale occurs.
     */
    inline Vec2d& operator*=(double s);
    
    /**
     * Returns the components of this vector divided by the given constant
     *
     * Note: this does not modify this vector.
     *
     * @param s the constant to divide this vector with
     * @return a smaller vector
     */
    inline const Vec2d operator/(double s) const;

    /**
     * Determines if this vector is less than the given vector.
     * 
     * @param v The vector to compare against.
     * 
     * @return True if this vector is less than the given vector, false otherwise.
     */
    inline bool operator<(const Vec2d& v) const;
    
    /**
     * Determines if this vector is greater than the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is greater than the given vector, false otherwise.
     */
    inline bool operator>(const Vec2d& v) const;

    /**
     * Determines if this vector is equal to the given vector.
     * 
     * @param v The vector to compare against.
     * 
     * @return True if this vector is equal to the given vector, false otherwise.
     */
    inline bool operator==(const Vec2d& v) const;

    /**
     * Determines if this vector is not equal to the given vector.
     * 
     * @param v The vector to compare against.
     * 
     * @return True if this vector is not equal to the given vector, false otherwise.
     */
    inline bool operator!=(const Vec2d& v) const;

    //code added compatible for Point
public:
      /**
     * @js NA
     * @lua NA
     */
    inline void setPoint(double xx, double yy);
    /**
     * @js NA
     */
    bool equals(const Vec2d& target) const;
    
    /** @returns if points have fuzzy equality which means equal with some degree of variance.
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    bool fuzzyEquals(const Vec2d& target, double variance) const;

    /** Calculates distance between point an origin
     @return double
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline double getLength() const {
        return sqrtf(x*x + y*y);
    };

    /** Calculates the square length of a Vec2d (not calling sqrt() )
     @return double
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline double getLengthSq() const {
        return dot(*this); //x*x + y*y;
    };

    /** Calculates the square distance between two points (not calling sqrt() )
     @return double
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline double getDistanceSq(const Vec2d& other) const {
        return (*this - other).getLengthSq();
    };

    /** Calculates the distance between two points
     @return double
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline double getDistance(const Vec2d& other) const {
        return (*this - other).getLength();
    };

    /** @returns the angle in radians between this vector and the x axis
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline double getAngle() const {
        return atan2f(y, x);
    };

    /** @returns the angle in radians between two vector directions
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    double getAngle(const Vec2d& other) const;

    /** Calculates cross product of two points.
     @return double
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline double cross(const Vec2d& other) const {
        return x*other.y - y*other.x;
    };

    /** Calculates perpendicular of v, rotated 90 degrees counter-clockwise -- cross(v, perp(v)) >= 0
     @return Vec2d
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2d getPerp() const {
        return Vec2d(-y, x);
    };
    
    /** Calculates midpoint between two points.
     @return Vec2d
     @since v3.0
     * @js NA
     * @lua NA
     */
    inline Vec2d getMidpoint(const Vec2d& other) const
    {
        return Vec2d((x + other.x) / 2.0f, (y + other.y) / 2.0f);
    }
    
    /** Clamp a point between from and to.
     @since v3.0
     * @js NA
     * @lua NA
     */
    inline Vec2d getClampPoint(const Vec2d& min_inclusive, const Vec2d& max_inclusive) const
    {
        return Vec2d(clampf(x,min_inclusive.x,max_inclusive.x), clampf(y, min_inclusive.y, max_inclusive.y));
    }
    
    /** Run a math operation function on each point component
     * absf, fllorf, ceilf, roundf
     * any function that has the signature: double func(double);
     * For example: let's try to take the floor of x,y
     * p.compOp(floorf);
     @since v3.0
     * @js NA
     * @lua NA
     */
    inline Vec2d compOp(std::function<double(double)> function) const
    {
        return Vec2d(function(x), function(y));
    }

    /** Calculates perpendicular of v, rotated 90 degrees clockwise -- cross(v, rperp(v)) <= 0
     @return Vec2d
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2d getRPerp() const {
        return Vec2d(y, -x);
    };

    /** Calculates the projection of this over other.
     @return Vec2d
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2d project(const Vec2d& other) const {
        return other * (dot(other)/other.dot(other));
    };

    /** Complex multiplication of two points ("rotates" two points).
     @return Vec2d vector with an angle of this.getAngle() + other.getAngle(),
     and a length of this.getLength() * other.getLength().
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2d rotate(const Vec2d& other) const {
        return Vec2d(x*other.x - y*other.y, x*other.y + y*other.x);
    };

    /** Unrotates two points.
     @return Vec2d vector with an angle of this.getAngle() - other.getAngle(),
     and a length of this.getLength() * other.getLength().
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2d unrotate(const Vec2d& other) const {
        return Vec2d(x*other.x + y*other.y, y*other.x - x*other.y);
    };

    /** Linear Interpolation between two points a and b
     @returns
        alpha == 0 ? a
        alpha == 1 ? b
        otherwise a value between a..b
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2d lerp(const Vec2d& other, double alpha) const {
        return *this * (1.f - alpha) + other * alpha;
    };

    /** Rotates a point counter clockwise by the angle around a pivot
     @param pivot is the pivot, naturally
     @param angle is the angle of rotation ccw in radians
     @returns the rotated point
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    Vec2d rotateByAngle(const Vec2d& pivot, double angle) const;

    /**
     * @js NA
     * @lua NA
     */
    static inline Vec2d forAngle(const double a)
    {
        return Vec2d(cosf(a), sinf(a));
    }
    
    /** A general line-line intersection test
     @param A   the startpoint for the first line L1 = (A - B)
     @param B   the endpoint for the first line L1 = (A - B)
     @param C   the startpoint for the second line L2 = (C - D)
     @param D   the endpoint for the second line L2 = (C - D)
     @param S   the range for a hitpoint in L1 (p = A + S*(B - A))
     @param T   the range for a hitpoint in L2 (p = C + T*(D - C))
     @returns   whether these two lines interects.

     Note that to truly test intersection for segments we have to make
     sure that S & T lie within [0..1] and for rays, make sure S & T > 0
     the hit point is        C + T * (D - C);
     the hit point also is   A + S * (B - A);
     @since 3.0
     * @js NA
     * @lua NA
     */
    static bool isLineIntersect(const Vec2d& A, const Vec2d& B,
                                 const Vec2d& C, const Vec2d& D,
                                 double *S = nullptr, double *T = nullptr);
    
    /**
     returns true if Line A-B overlap with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isLineOverlap(const Vec2d& A, const Vec2d& B,
                                const Vec2d& C, const Vec2d& D);
    
    /**
     returns true if Line A-B parallel with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isLineParallel(const Vec2d& A, const Vec2d& B,
                   const Vec2d& C, const Vec2d& D);
    
    /**
     returns true if Segment A-B overlap with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isSegmentOverlap(const Vec2d& A, const Vec2d& B,
                                 const Vec2d& C, const Vec2d& D,
                                 Vec2d* S = nullptr, Vec2d* E = nullptr);
    
    /**
     returns true if Segment A-B intersects with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isSegmentIntersect(const Vec2d& A, const Vec2d& B, const Vec2d& C, const Vec2d& D);
    
    /**
     returns the intersection point of line A-B, C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static Vec2d getIntersectPoint(const Vec2d& A, const Vec2d& B, const Vec2d& C, const Vec2d& D);
    
    /** equals to Vec2d(0,0) */
    static const Vec2d ZERO;
    /** equals to Vec2d(1,1) */
    static const Vec2d ONE;
    /** equals to Vec2d(1,0) */
    static const Vec2d UNIT_X;
    /** equals to Vec2d(0,1) */
    static const Vec2d UNIT_Y;
    /** equals to Vec2d(0.5, 0.5) */
    static const Vec2d ANCHOR_MIDDLE;
    /** equals to Vec2d(0, 0) */
    static const Vec2d ANCHOR_BOTTOM_LEFT;
    /** equals to Vec2d(0, 1) */
    static const Vec2d ANCHOR_TOP_LEFT;
    /** equals to Vec2d(1, 0) */
    static const Vec2d ANCHOR_BOTTOM_RIGHT;
    /** equals to Vec2d(1, 1) */
    static const Vec2d ANCHOR_TOP_RIGHT;
    /** equals to Vec2d(1, 0.5) */
    static const Vec2d ANCHOR_MIDDLE_RIGHT;
    /** equals to Vec2d(0, 0.5) */
    static const Vec2d ANCHOR_MIDDLE_LEFT;
    /** equals to Vec2d(0.5, 1) */
    static const Vec2d ANCHOR_MIDDLE_TOP;
    /** equals to Vec2d(0.5, 0) */
    static const Vec2d ANCHOR_MIDDLE_BOTTOM;
};

inline std::ostream & operator<<(std::ostream &out, const Vec2d& v)
{
	out << "(" << v.x << "," << v.y << ")";
	return out;
}

/**
 * Calculates the scalar product of the given vector with the given value.
 * 
 * @param x The value to scale by.
 * @param v The vector to scale.
 * @return The scaled vector.
 */
inline const Vec2d operator*(double x, const Vec2d& v);

//typedef Vec2d Point;

NS_HMS_MATH_END

/**
 end of base group
 @}
 */

#include "Vec2d.inl"

#endif // MATH_Vec2d_H
