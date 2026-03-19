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

#ifndef Quaterniond_H_
#define Quaterniond_H_

#include "Vec3d.h"
#include "Mat4d.h"
//#include "Plane.h"

/**
 * @addtogroup base
 * @{
 */

NS_HMS_MATH_BEGIN

class Mat4d;

/**
 * Defines a 4-element Quaterniond that represents the orientation of an object in space.
 *
 * Quaternionds are typically used as a replacement for euler angles and rotation matrices as a way to achieve smooth interpolation and avoid gimbal lock.
 *
 * Note that this Quaterniond class does not automatically keep the Quaterniond normalized. Therefore, care must be taken to normalize the Quaterniond when necessary, by calling the normalize method.
 * This class provides three methods for doing Quaterniond interpolation: lerp, slerp, and squad.
 *
 * lerp (linear interpolation): the interpolation curve gives a straight line in Quaterniond space. It is simple and fast to compute. The only problem is that it does not provide constant angular velocity. Note that a constant velocity is not necessarily a requirement for a curve;
 * slerp (spherical linear interpolation): the interpolation curve forms a great arc on the Quaterniond unit sphere. Slerp provides constant angular velocity;
 * squad (spherical spline interpolation): interpolating between a series of rotations using slerp leads to the following problems:
 * - the curve is not smooth at the control points;
 * - the angular velocity is not constant;
 * - the angular velocity is not continuous at the control points.
 *
 * Since squad is continuously differentiable, it remedies the first and third problems mentioned above.
 * The slerp method provided here is intended for interpolation of principal rotations. It treats +q and -q as the same principal rotation and is at liberty to use the negative of either input. The resulting path is always the shorter arc.
 *
 * The lerp method provided here interpolates strictly in Quaterniond space. Note that the resulting path may pass through the origin if interpolating between a Quaterniond and its exact negative.
 *
 * As an example, consider the following Quaternionds:
 *
 * q1 = (0.6, 0.8, 0.0, 0.0),
 * q2 = (0.0, 0.6, 0.8, 0.0),
 * q3 = (0.6, 0.0, 0.8, 0.0), and
 * q4 = (-0.8, 0.0, -0.6, 0.0).
 * For the point p = (1.0, 1.0, 1.0), the following figures show the trajectories of p using lerp, slerp, and squad.
 */
class HMS_DLL Quaterniond
{
    friend class Curve;
    friend class Transform;

public:

    /**
     * The x-value of the Quaterniond's vector component.
     */
    double x;
    /**
     * The y-value of the Quaterniond's vector component.
     */
    double y;
    /**
     * The z-value of the Quaterniond's vector component.
     */
    double z;
    /**
     * The scalar component of the Quaterniond.
     */
    double w;

    /**
     * Constructs a Quaterniond initialized to (0, 0, 0, 1).
     */
    Quaterniond();

    /**
     * Constructs a Quaterniond initialized to (0, 0, 0, 1).
     *
     * @param xx The x component of the Quaterniond.
     * @param yy The y component of the Quaterniond.
     * @param zz The z component of the Quaterniond.
     * @param ww The w component of the Quaterniond.
     */
    Quaterniond(double xx, double yy, double zz, double ww);

    /**
     * Constructs a new Quaterniond from the values in the specified array.
     *
     * @param array The values for the new Quaterniond.
     */
    Quaterniond(double* array);

    /**
     * Constructs a Quaterniond equal to the rotational part of the specified matrix.
     *
     * @param m The matrix.
     */
    Quaterniond(const Mat4d& m);

    /**
     * Constructs a Quaterniond equal to the rotation from the specified axis and angle.
     *
     * @param axis A vector describing the axis of rotation.
     * @param angle The angle of rotation (in radians).
     */
    Quaterniond(const Vec3d& axis, double angle);

    /**
     * Constructs a new Quaterniond that is a copy of the specified one.
     *
     * @param copy The Quaterniond to copy.
     */
    Quaterniond(const Quaterniond& copy);

    /**
     * Destructor.
     */
    ~Quaterniond();

    /**
     * Returns the identity Quaterniond.
     *
     * @return The identity Quaterniond.
     */
    static const Quaterniond& identity();

    /**
     * Returns the Quaterniond with all zeros.
     *
     * @return The Quaterniond.
     */
    static const Quaterniond& zero();

    /**
     * Determines if this Quaterniond is equal to the identity Quaterniond.
     *
     * @return true if it is the identity Quaterniond, false otherwise.
     */
    bool isIdentity() const;

    /**
     * Determines if this Quaterniond is all zeros.
     *
     * @return true if this Quaterniond is all zeros, false otherwise.
     */
    bool isZero() const;

    /**
     * Creates a Quaterniond equal to the rotational part of the specified matrix
     * and stores the result in dst.
     *
     * @param m The matrix.
     * @param dst A Quaterniond to store the conjugate in.
     */
    static void createFromRotationMatrix(const Mat4d& m, Quaterniond* dst);

    /**
     * Creates this Quaterniond equal to the rotation from the specified axis and angle
     * and stores the result in dst.
     *
     * @param axis A vector describing the axis of rotation.
     * @param angle The angle of rotation (in radians).
     * @param dst A Quaterniond to store the conjugate in.
     */
    static void createFromAxisAngle(const Vec3d& axis, double angle, Quaterniond* dst);

    /**
     * Sets this Quaterniond to the conjugate of itself.
     */
    void conjugate();

    /**
     * Gets the conjugate of this Quaterniond.
     *
     */
    Quaterniond getConjugated() const;

    /**
     * Sets this Quaterniond to the inverse of itself.
     *
     * Note that the inverse of a Quaterniond is equal to its conjugate
     * when the Quaterniond is unit-length. For this reason, it is more
     * efficient to use the conjugate method directly when you know your
     * Quaterniond is already unit-length.
     *
     * @return true if the inverse can be computed, false otherwise.
     */
    bool inverse();

    /**
     * Gets the inverse of this Quaterniond.
     *
     * Note that the inverse of a Quaterniond is equal to its conjugate
     * when the Quaterniond is unit-length. For this reason, it is more
     * efficient to use the conjugate method directly when you know your
     * Quaterniond is already unit-length.
     */
    Quaterniond getInversed() const;

    /**
     * Multiplies this Quaterniond by the specified one and stores the result in this Quaterniond.
     *
     * @param q The Quaterniond to multiply.
     */
    void multiply(const Quaterniond& q);

    /**
     * Multiplies the specified Quaternionds and stores the result in dst.
     *
     * @param q1 The first Quaterniond.
     * @param q2 The second Quaterniond.
     * @param dst A Quaterniond to store the result in.
     */
    static void multiply(const Quaterniond& q1, const Quaterniond& q2, Quaterniond* dst);

    /**
     * Normalizes this Quaterniond to have unit length.
     *
     * If the Quaterniond already has unit length or if the length
     * of the Quaterniond is zero, this method does nothing.
     */
    void normalize();

    /**
     * Get the normalized Quaterniond.
     *
     * If the Quaterniond already has unit length or if the length
     * of the Quaterniond is zero, this method simply copies
     * this vector.
     */
    Quaterniond getNormalized() const;

    /**
     * Sets the elements of the Quaterniond to the specified values.
     *
     * @param xx The new x-value.
     * @param yy The new y-value.
     * @param zz The new z-value.
     * @param ww The new w-value.
     */
    void set(double xx, double yy, double zz, double ww);

    /**
     * Sets the elements of the Quaterniond from the values in the specified array.
     *
     * @param array An array containing the elements of the Quaterniond in the order x, y, z, w.
     */
    void set(double* array);

    /**
     * Sets the Quaterniond equal to the rotational part of the specified matrix.
     *
     * @param m The matrix.
     */
    void set(const Mat4d& m);

    /**
     * Sets the Quaterniond equal to the rotation from the specified axis and angle.
     * 
     * @param axis The axis of rotation.
     * @param angle The angle of rotation (in radians).
     */
    void set(const Vec3d& axis, double angle);

    /**
     * Sets the elements of this Quaterniond to a copy of the specified Quaterniond.
     *
     * @param q The Quaterniond to copy.
     */
    void set(const Quaterniond& q);

    /**
     * Sets this Quaterniond to be equal to the identity Quaterniond.
     */
    void setIdentity();

    /**
     * Converts this Quaterniond4f to axis-angle notation. The axis is normalized.
     *
     * @param e The Vec3df which stores the axis.
     * 
     * @return The angle (in radians).
     */
    double toAxisAngle(Vec3d* e) const;

    /**
     * Interpolates between two Quaternionds using linear interpolation.
     *
     * The interpolation curve for linear interpolation between
     * Quaternionds gives a straight line in Quaterniond space.
     *
     * @param q1 The first Quaterniond.
     * @param q2 The second Quaterniond.
     * @param t The interpolation coefficient.
     * @param dst A Quaterniond to store the result in.
     */
    static void lerp(const Quaterniond& q1, const Quaterniond& q2, double t, Quaterniond* dst);
    
    /**
     * Interpolates between two Quaternionds using spherical linear interpolation.
     *
     * Spherical linear interpolation provides smooth transitions between different
     * orientations and is often useful for animating models or cameras in 3D.
     *
     * Note: For accurate interpolation, the input Quaternionds must be at (or close to) unit length.
     * This method does not automatically normalize the input Quaternionds, so it is up to the
     * caller to ensure they call normalize beforehand, if necessary.
     *
     * @param q1 The first Quaterniond.
     * @param q2 The second Quaterniond.
     * @param t The interpolation coefficient.
     * @param dst A Quaterniond to store the result in.
     */
    static void slerp(const Quaterniond& q1, const Quaterniond& q2, double t, Quaterniond* dst);
    
    /**
     * Interpolates over a series of Quaternionds using spherical spline interpolation.
     *
     * Spherical spline interpolation provides smooth transitions between different
     * orientations and is often useful for animating models or cameras in 3D.
     *
     * Note: For accurate interpolation, the input Quaternionds must be unit.
     * This method does not automatically normalize the input Quaternionds,
     * so it is up to the caller to ensure they call normalize beforehand, if necessary.
     *
     * @param q1 The first Quaterniond.
     * @param q2 The second Quaterniond.
     * @param s1 The first control point.
     * @param s2 The second control point.
     * @param t The interpolation coefficient.
     * @param dst A Quaterniond to store the result in.
     */
    static void squad(const Quaterniond& q1, const Quaterniond& q2, const Quaterniond& s1, const Quaterniond& s2, double t, Quaterniond* dst);

    /**
     * Calculates the Quaterniond product of this Quaterniond with the given Quaterniond.
     * 
     * Note: this does not modify this Quaterniond.
     * 
     * @param q The Quaterniond to multiply.
     * @return The Quaterniond product.
     */
    inline const Quaterniond operator*(const Quaterniond& q) const;

    /**
     * Calculates the Quaterniond product of this Quaterniond with the given Vec3d.
     * @param v The Vec3d to multiply.
     * @return The Vec3d product.
     */
    inline Vec3d operator*(const Vec3d& v) const;

    /**
     * Multiplies this Quaterniond with the given Quaterniond.
     * 
     * @param q The Quaterniond to multiply.
     * @return This Quaterniond, after the multiplication occurs.
     */
    inline Quaterniond& operator*=(const Quaterniond& q);
    
    /** equals to Quaterniond(0,0,0, 0) */
    static const Quaterniond ZERO;

private:

    /**
     * Interpolates between two Quaternionds using spherical linear interpolation.
     *
     * Spherical linear interpolation provides smooth transitions between different
     * orientations and is often useful for animating models or cameras in 3D.
     *
     * Note: For accurate interpolation, the input Quaternionds must be at (or close to) unit length.
     * This method does not automatically normalize the input Quaternionds, so it is up to the
     * caller to ensure they call normalize beforehand, if necessary.
     *
     * @param q1x The x component of the first Quaterniond.
     * @param q1y The y component of the first Quaterniond.
     * @param q1z The z component of the first Quaterniond.
     * @param q1w The w component of the first Quaterniond.
     * @param q2x The x component of the second Quaterniond.
     * @param q2y The y component of the second Quaterniond.
     * @param q2z The z component of the second Quaterniond.
     * @param q2w The w component of the second Quaterniond.
     * @param t The interpolation coefficient.
     * @param dstx A pointer to store the x component of the slerp in.
     * @param dsty A pointer to store the y component of the slerp in.
     * @param dstz A pointer to store the z component of the slerp in.
     * @param dstw A pointer to store the w component of the slerp in.
     */
    static void slerp(double q1x, double q1y, double q1z, double q1w, double q2x, double q2y, double q2z, double q2w, double t, double* dstx, double* dsty, double* dstz, double* dstw);

    static void slerpForSquad(const Quaterniond& q1, const Quaterniond& q2, double t, Quaterniond* dst);
};

NS_HMS_MATH_END
/**
 end of base group
 @}
 */
#include "Quaterniond.inl"

#endif
