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

#ifndef MATH_Vec3d_H
#define MATH_Vec3d_H

#include "math/HmsMathBase.h"

/**
 * @addtogroup base
 * @{
 */

NS_HMS_MATH_BEGIN

class Mat4d;
class Quaterniond;

/**
 * Defines a 3-element doubleing point vector.
 *
 * When using a vector to represent a surface normal,
 * the vector should typically be normalized.
 * Other uses of directional vectors may wish to leave
 * the magnitude of the vector intact. When used as a point,
 * the elements of the vector represent a position in 3D space.
 */
class HMS_DLL Vec3d
{
public:

    /**
     * The x-coordinate.
     */
    double x;

    /**
     * The y-coordinate.
     */
    double y;

    /**
     * The z-coordinate.
     */
    double z;

    /**
     * Constructs a new vector initialized to all zeros.
     */
    Vec3d();

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param xx The x coordinate.
     * @param yy The y coordinate.
     * @param zz The z coordinate.
     */
    Vec3d(double xx, double yy, double zz);

    /**
     * Constructs a new vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z.
     */
    Vec3d(const double* array);

    /**
     * Constructs a vector that describes the direction between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    Vec3d(const Vec3d& p1, const Vec3d& p2);

    /**
     * Constructs a new vector that is a copy of the specified vector.
     *
     * @param copy The vector to copy.
     */
    Vec3d(const Vec3d& copy);

    /**
     * Creates a new vector from an integer interpreted as an RGB value.
     * E.g. 0xff0000 represents red or the vector (1, 0, 0).
     *
     * @param color The integer to interpret as an RGB value.
     *
     * @return A vector corresponding to the interpreted RGB color.
     */
    static Vec3d fromColor(unsigned int color);

    /**
     * Destructor.
     */
    ~Vec3d();

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
    static double angle(const Vec3d& v1, const Vec3d& v2);


    /**
     * Adds the elements of the specified vector to this one.
     *
     * @param v The vector to add.
     */
    inline void add(const Vec3d& v);


    /**
    * Adds the elements of this vector to the specified values.
    *
    * @param xx The add x coordinate.
    * @param yy The add y coordinate.
    * @param zz The add z coordinate.
    */
    inline void add(double xx, double yy, double zz);

    /**
     * Adds the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void add(const Vec3d& v1, const Vec3d& v2, Vec3d* dst);

    /**
     * Clamps this vector within the specified range.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     */
    void clamp(const Vec3d& min, const Vec3d& max);

    /**
     * Clamps the specified vector within the specified range and returns it in dst.
     *
     * @param v The vector to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @param dst A vector to store the result in.
     */
    static void clamp(const Vec3d& v, const Vec3d& min, const Vec3d& max, Vec3d* dst);

    /**
     * Sets this vector to the cross product between itself and the specified vector.
     *
     * @param v The vector to compute the cross product with.
     */
    void cross(const Vec3d& v);

    /**
     * Computes the cross product of the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void cross(const Vec3d& v1, const Vec3d& v2, Vec3d* dst);

    /**
     * Returns the distance between this vector and v.
     *
     * @param v The other vector.
     * 
     * @return The distance between this vector and v.
     * 
     * @see distanceSquared
     */
    double distance(const Vec3d& v) const;

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
    double distanceSquared(const Vec3d& v) const;

    /**
     * Returns the dot product of this vector and the specified vector.
     *
     * @param v The vector to compute the dot product with.
     * 
     * @return The dot product.
     */
    double dot(const Vec3d& v) const;

    /**
     * Returns the dot product between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * 
     * @return The dot product between the vectors.
     */
    static double dot(const Vec3d& v1, const Vec3d& v2);

    /**
     * Computes the length of this vector.
     *
     * @return The length of the vector.
     * 
     * @see lengthSquared
     */
    inline double length() const;

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
     * This method normalizes this Vect3 so that it is of
     * unit length (in other words, the length of the vector
     * after calling this method will be 1.0f). If the vector
     * already has unit length or if the length of the vector
     * is zero, this method does nothing.
     * 
     * @return This vector, after the normalization occurs.
     */
    void normalize();

    /**
     * Get the normalized vector.
     */
    Vec3d getNormalized() const;

    /**
     * Scales all elements of this vector by the specified value.
     *
     * @param scalar The scalar value.
     */
    inline void scale(double scalar);

    /**
     * Sets the elements of this vector to the specified values.
     *
     * @param xx The new x coordinate.
     * @param yy The new y coordinate.
     * @param zz The new z coordinate.
     */
    inline void set(double xx, double yy, double zz);

    /**
     * Sets the elements of this vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z.
     */
    inline void set(const double* array);

    /**
     * Sets the elements of this vector to those in the specified vector.
     *
     * @param v The vector to copy.
     */
    inline void set(const Vec3d& v);

    /**
     * Sets this vector to the directional vector between the specified points.
     */
    inline void set(const Vec3d& p1, const Vec3d& p2);

    /**
    * Sets the elements of this vector to zero.
    */
    inline void setZero();

    /**
     * Subtracts this vector and the specified vector as (this - v)
     * and stores the result in this vector.
     *
     * @param v The vector to subtract.
     */
    inline void subtract(const Vec3d& v);

    /**
     * Subtracts the specified vectors and stores the result in dst.
     * The resulting vector is computed as (v1 - v2).
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst The destination vector.
     */
    static void subtract(const Vec3d& v1, const Vec3d& v2, Vec3d* dst);

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
    void smooth(const Vec3d& target, double elapsedTime, double responseTime);

    /**
     * Linear interpolation between two vectors A and B by alpha which
     * is in the range [0,1]
     */
    inline Vec3d lerp(const Vec3d& target, double alpha) const;

    /**
     * Calculates the sum of this vector with the given vector.
     * 
     * Note: this does not modify this vector.
     * 
     * @param v The vector to add.
     * @return The vector sum.
     */
    inline const Vec3d operator+(const Vec3d& v) const;

    /**
     * Adds the given vector to this vector.
     * 
     * @param v The vector to add.
     * @return This vector, after the addition occurs.
     */
    inline Vec3d& operator+=(const Vec3d& v);

    /**
     * Calculates the difference of this vector with the given vector.
     * 
     * Note: this does not modify this vector.
     * 
     * @param v The vector to subtract.
     * @return The vector difference.
     */
    inline const Vec3d operator-(const Vec3d& v) const;

    /**
     * Subtracts the given vector from this vector.
     * 
     * @param v The vector to subtract.
     * @return This vector, after the subtraction occurs.
     */
    inline Vec3d& operator-=(const Vec3d& v);

    /**
     * Calculates the negation of this vector.
     * 
     * Note: this does not modify this vector.
     * 
     * @return The negation of this vector.
     */
    inline const Vec3d operator-() const;

    /**
     * Calculates the scalar product of this vector with the given value.
     * 
     * Note: this does not modify this vector.
     * 
     * @param s The value to scale by.
     * @return The scaled vector.
     */
    inline const Vec3d operator*(double s) const;

    /**
     * Scales this vector by the given value.
     * 
     * @param s The value to scale by.
     * @return This vector, after the scale occurs.
     */
    inline Vec3d& operator*=(double s);
    
    /**
     * Returns the components of this vector divided by the given constant
     *
     * Note: this does not modify this vector.
     *
     * @param s the constant to divide this vector with
     * @return a smaller vector
     */
    inline const Vec3d operator/(double s) const;

    /** Returns true if the vector's scalar components are all greater
     that the ones of the vector it is compared against.
     */
    inline bool operator < (const Vec3d& rhs) const
    {
        if (x < rhs.x && y < rhs.y && z < rhs.z)
            return true;
        return false;
    }

    /** Returns true if the vector's scalar components are all smaller
     that the ones of the vector it is compared against.
     */
    inline bool operator >(const Vec3d& rhs) const
    {
        if (x > rhs.x && y > rhs.y && z > rhs.z)
            return true;
        return false;
    }

    /**
     * Determines if this vector is equal to the given vector.
     * 
     * @param v The vector to compare against.
     * 
     * @return True if this vector is equal to the given vector, false otherwise.
     */
    inline bool operator==(const Vec3d& v) const;

    /**
     * Determines if this vector is not equal to the given vector.
     * 
     * @param v The vector to compare against.
     * 
     * @return True if this vector is not equal to the given vector, false otherwise.
     */
    inline bool operator!=(const Vec3d& v) const;
    
    /** equals to Vec3d(0,0,0) */
    static const Vec3d ZERO;
    /** equals to Vec3d(1,1,1) */
    static const Vec3d ONE;
    /** equals to Vec3d(1,0,0) */
    static const Vec3d UNIT_X;
    /** equals to Vec3d(0,1,0) */
    static const Vec3d UNIT_Y;
    /** equals to Vec3d(0,0,1) */
    static const Vec3d UNIT_Z;
};

/**
 * Calculates the scalar product of the given vector with the given value.
 * 
 * @param x The value to scale by.
 * @param v The vector to scale.
 * @return The scaled vector.
 */
inline const Vec3d operator*(double x, const Vec3d& v);

//typedef Vec3d Point3;

NS_HMS_MATH_END
/**
 end of base group
 @}
 */
#include "Vec3d.inl"

#endif // MATH_Vec3d_H
