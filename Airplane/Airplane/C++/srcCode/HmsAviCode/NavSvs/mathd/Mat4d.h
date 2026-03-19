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

#ifndef MATH_Mat4d_H
#define MATH_Mat4d_H



#include "Vec3d.h"
#include "math/Vec4.h"

#ifdef __SSE__
#include <xmmintrin.h>
#endif

/**
 * @addtogroup base
 * @{
 */

NS_HMS_MATH_BEGIN

//class Plane;

/**
 * Defines a 4 x 4 doubleing point matrix representing a 3D transformation.
 *
 * Vectors are treated as columns, resulting in a matrix that is represented as follows,
 * where x, y and z are the translation components of the matrix:
 *
 *     1  0  0  x
 *     0  1  0  y
 *     0  0  1  z
 *     0  0  0  1
 *
 * This matrix class is directly compatible with OpenGL since its elements are
 * laid out in memory exactly as they are expected by OpenGL.
 * The matrix uses column-major format such that array indices increase down column first.
 * Since matrix multiplication is not commutative, multiplication must be done in the
 * correct order when combining transformations. Suppose we have a translation
 * matrix T and a rotation matrix R. To first rotate an object around the origin
 * and then translate it, you would multiply the two matrices as TR.
 *
 * Likewise, to first translate the object and then rotate it, you would do RT.
 * So generally, matrices must be multiplied in the reverse order in which you
 * want the transformations to take place (this also applies to
 * the scale, rotate, and translate methods below; these methods are convenience
 * methods for post-multiplying by a matrix representing a scale, rotation, or translation).
 *
 * In the case of repeated local transformations (i.e. rotate around the Z-axis by 0.76 radians,
 * then translate by 2.1 along the X-axis, then ...), it is better to use the Transform class
 * (which is optimized for that kind of usage).
 *
 * @see Transform
 */
class HMS_DLL Mat4d
{
public:
    // //temp add conversion
    // operator kmMat4d() const
    // {
    //     kmMat4d result;
    //     kmMat4dFill(&result, m);
    //     return result;
    // }
    
    // Mat4d(const kmMat4d& mat)
    // {
    //     set(mat.mat);
    // }
    /**
     * Stores the columns of this 4x4 matrix.
     * */
#ifdef __SSE__
    union {
        __m128 col[4];
        double m[16];
    };
#else
    double m[16];
#endif

    /** 
     * Default constructor.
     * Constructs a matrix initialized to the identity matrix:
     *
     *     1  0  0  0
     *     0  1  0  0
     *     0  0  1  0
     *     0  0  0  1
     */
    Mat4d();

    /**
     * Constructs a matrix initialized to the specified value.
     *
     * @param m11 The first element of the first row.
     * @param m12 The second element of the first row.
     * @param m13 The third element of the first row.
     * @param m14 The fourth element of the first row.
     * @param m21 The first element of the second row.
     * @param m22 The second element of the second row.
     * @param m23 The third element of the second row.
     * @param m24 The fourth element of the second row.
     * @param m31 The first element of the third row.
     * @param m32 The second element of the third row.
     * @param m33 The third element of the third row.
     * @param m34 The fourth element of the third row.
     * @param m41 The first element of the fourth row.
     * @param m42 The second element of the fourth row.
     * @param m43 The third element of the fourth row.
     * @param m44 The fourth element of the fourth row.
     */
    Mat4d(double m11, double m12, double m13, double m14, double m21, double m22, double m23, double m24,
           double m31, double m32, double m33, double m34, double m41, double m42, double m43, double m44);

    /**
     * Creates a matrix initialized to the specified column-major array.
     *
     * The passed-in array is in column-major order, so the memory layout of the array is as follows:
     *
     *     0   4   8   12
     *     1   5   9   13
     *     2   6   10  14
     *     3   7   11  15
     *
     * @param mat An array containing 16 elements in column-major order.
     */
    Mat4d(const double* mat);

    /**
     * Constructs a new matrix by copying the values from the specified matrix.
     *
     * @param copy The matrix to copy.
     */
    Mat4d(const Mat4d& copy);

    /**
     * Destructor.
     */
    ~Mat4d();

    /**
     * Creates a view matrix based on the specified input parameters.
     *
     * @param eyePosition The eye position.
     * @param tarGetPoistion The target's center position.
     * @param up The up vector.
     * @param dst A matrix to store the result in.
     */
    static void createLookAt(const Vec3d& eyePosition, const Vec3d& tarGetPoistion, const Vec3d& up, Mat4d* dst);

    /**
     * Creates a view matrix based on the specified input parameters.
     *
     * @param eyePositionX The eye x-coordinate position.
     * @param eyePositionY The eye y-coordinate position.
     * @param eyePositionZ The eye z-coordinate position.
     * @param targetCenterX The target's center x-coordinate position.
     * @param targetCenterY The target's center y-coordinate position.
     * @param targetCenterZ The target's center z-coordinate position.
     * @param upX The up vector x-coordinate value.
     * @param upY The up vector y-coordinate value.
     * @param upZ The up vector z-coordinate value.
     * @param dst A matrix to store the result in.
     */
    static void createLookAt(double eyePositionX, double eyePositionY, double eyePositionZ,
                             double targetCenterX, double targetCenterY, double targetCenterZ,
                             double upX, double upY, double upZ, Mat4d* dst);

    /**
     * Builds a perspective projection matrix based on a field of view and returns by value.
     *
     * Projection space refers to the space after applying projection transformation from view space.
     * After the projection transformation, visible content has x- and y-coordinates ranging from -1 to 1,
     * and a z-coordinate ranging from 0 to 1. To obtain the viewable area (in world space) of a scene,
     * create a BoundingFrustum and pass the combined view and projection matrix to the constructor.
     *
     * @param fieldOfView The field of view in the y direction (in degrees).
     * @param aspectRatio The aspect ratio, defined as view space width divided by height.
     * @param zNearPlane The distance to the near view plane.
     * @param zFarPlane The distance to the far view plane.
     * @param dst A matrix to store the result in.
     */
    static void createPerspective(double fieldOfView, double aspectRatio, double zNearPlane, double zFarPlane, Mat4d* dst);

    /**
     * Creates an orthographic projection matrix.
     *
     * @param width The width of the view.
     * @param height The height of the view.
     * @param zNearPlane The minimum z-value of the view volume.
     * @param zFarPlane The maximum z-value of the view volume.
     * @param dst A matrix to store the result in.
     */
    static void createOrthographic(double width, double height, double zNearPlane, double zFarPlane, Mat4d* dst);

    /**
     * Creates an orthographic projection matrix.
     *
     * Projection space refers to the space after applying
     * projection transformation from view space. After the
     * projection transformation, visible content has
     * x and y coordinates ranging from -1 to 1, and z coordinates
     * ranging from 0 to 1.
     *
     * Unlike perspective projection, in orthographic projection
     * there is no perspective foreshortening.
     *
     * The viewable area of this orthographic projection extends
     * from left to right on the x-axis, bottom to top on the y-axis,
     * and zNearPlane to zFarPlane on the z-axis. These values are
     * relative to the position and x, y, and z-axes of the view.
     * To obtain the viewable area (in world space) of a scene,
     * create a BoundingFrustum and pass the combined view and
     * projection matrix to the constructor.
     *
     * @param left The minimum x-value of the view volume.
     * @param right The maximum x-value of the view volume.
     * @param bottom The minimum y-value of the view volume.
     * @param top The maximum y-value of the view volume.
     * @param zNearPlane The minimum z-value of the view volume.
     * @param zFarPlane The maximum z-value of the view volume.
     * @param dst A matrix to store the result in.
     */
    static void createOrthographicOffCenter(double left, double right, double bottom, double top,
                                            double zNearPlane, double zFarPlane, Mat4d* dst);

    /**
     * Creates a spherical billboard that rotates around a specified object position.
     *
     * This method computes the facing direction of the billboard from the object position
     * and camera position. When the object and camera positions are too close, the matrix
     * will not be accurate. To avoid this problem, this method defaults to the identity
     * rotation if the positions are too close. (See the other overload of createBillboard
     * for an alternative approach).
     *
     * @param objectPosition The position of the object the billboard will rotate around.
     * @param cameraPosition The position of the camera.
     * @param cameraUpVector The up vector of the camera.
     * @param dst A matrix to store the result in.
     */
    static void createBillboard(const Vec3d& objectPosition, const Vec3d& cameraPosition,
                                const Vec3d& cameraUpVector, Mat4d* dst);

    /**
     * Creates a spherical billboard that rotates around a specified object position with
     * provision for a safe default orientation.
     *
     * This method computes the facing direction of the billboard from the object position
     * and camera position. When the object and camera positions are too close, the matrix
     * will not be accurate. To avoid this problem, this method uses the specified camera
     * forward vector if the positions are too close. (See the other overload of createBillboard
     * for an alternative approach).
     *
     * @param objectPosition The position of the object the billboard will rotate around.
     * @param cameraPosition The position of the camera.
     * @param cameraUpVector The up vector of the camera.
     * @param cameraForwardVector The forward vector of the camera, used if the positions are too close.
     * @param dst A matrix to store the result in.
     */
    static void createBillboard(const Vec3d& objectPosition, const Vec3d& cameraPosition,
                                const Vec3d& cameraUpVector, const Vec3d& cameraForwardVector,
                                Mat4d* dst);

    //Fills in an existing Mat4d so that it reflects the coordinate system about a specified Plane.
    //plane The Plane about which to create a reflection.
    //dst A matrix to store the result in.
    //static void createReflection(const Plane& plane, Mat4d* dst);

    /**
     * Creates a scale matrix.
     *
     * @param scale The amount to scale.
     * @param dst A matrix to store the result in.
     */
    static void createScale(const Vec3d& scale, Mat4d* dst);

    /**
     * Creates a scale matrix.
     *
     * @param xScale The amount to scale along the x-axis.
     * @param yScale The amount to scale along the y-axis.
     * @param zScale The amount to scale along the z-axis.
     * @param dst A matrix to store the result in.
     */
    static void createScale(double xScale, double yScale, double zScale, Mat4d* dst);

    /**
     * Creates a rotation matrix from the specified Quaterniond.
     *
     * @param quat A Quaterniond describing a 3D orientation.
     * @param dst A matrix to store the result in.
     */
    static void createRotation(const Quaterniond& quat, Mat4d* dst);

    /**
     * Creates a rotation matrix from the specified axis and angle.
     *
     * @param axis A vector describing the axis to rotate about.
     * @param angle The angle (in radians).
     * @param dst A matrix to store the result in.
     */
    static void createRotation(const Vec3d& axis, double angle, Mat4d* dst);

    /**
     * Creates a matrix describing a rotation around the x-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    static void createRotationX(double angle, Mat4d* dst);

    /**
     * Creates a matrix describing a rotation around the y-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    static void createRotationY(double angle, Mat4d* dst);

    /**
     * Creates a matrix describing a rotation around the z-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    static void createRotationZ(double angle, Mat4d* dst);

    /**
     * Creates a translation matrix.
     *
     * @param translation The translation.
     * @param dst A matrix to store the result in.
     */
    static void createTranslation(const Vec3d& translation, Mat4d* dst);

    /**
     * Creates a translation matrix.
     *
     * @param xTranslation The translation on the x-axis.
     * @param yTranslation The translation on the y-axis.
     * @param zTranslation The translation on the z-axis.
     * @param dst A matrix to store the result in.
     */
    static void createTranslation(double xTranslation, double yTranslation, double zTranslation, Mat4d* dst);

    /**
     * Adds a scalar value to each component of this matrix.
     *
     * @param scalar The scalar to add.
     */
    void add(double scalar);

    /**
     * Adds a scalar value to each component of this matrix and stores the result in dst.
     *
     * @param scalar The scalar value to add.
     * @param dst A matrix to store the result in.
     */
    void add(double scalar, Mat4d* dst);

    /**
     * Adds the specified matrix to this matrix.
     *
     * @param mat The matrix to add.
     */
    void add(const Mat4d& mat);

    /**
     * Adds the specified matrices and stores the result in dst.
     *
     * @param m1 The first matrix.
     * @param m2 The second matrix.
     * @param dst The destination matrix to add to.
     */
    static void add(const Mat4d& m1, const Mat4d& m2, Mat4d* dst);

    /**
     * Decomposes the scale, rotation and translation components of this matrix.
     *
     * @param scale The scale.
     * @param rotation The rotation.
     * @param translation The translation.
     */
    bool decompose(Vec3d* scale, Quaterniond* rotation, Vec3d* translation) const;

    /**
     * Computes the determinant of this matrix.
     *
     * @return The determinant.
     */
    double determinant() const;

    /**
     * Gets the scalar component of this matrix in the specified vector.
     *
     * If the scalar component of this matrix has negative parts,
     * it is not possible to always extract the exact scalar component;
     * instead, a scale vector that is mathematically equivalent to the
     * original scale vector is extracted and returned.
     *
     * @param scale A vector to receive the scale.
     */
    void getScale(Vec3d* scale) const;

    /**
     * Gets the rotational component of this matrix in the specified Quaterniond.
     *
     * @param rotation A Quaterniond to receive the rotation.
     * 
     * @return true if the rotation is successfully extracted, false otherwise.
     */
    bool getRotation(Quaterniond* rotation) const;

    /**
     * Gets the translational component of this matrix in the specified vector.
     *
     * @param translation A vector to receive the translation.
     */
    void getTranslation(Vec3d* translation) const;

    /**
     * Gets the up vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getUpVector(Vec3d* dst) const;

    /**
     * Gets the down vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getDownVector(Vec3d* dst) const;

    /**
     * Gets the left vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getLeftVector(Vec3d* dst) const;

    /**
     * Gets the right vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getRightVector(Vec3d* dst) const;

    /**
     * Gets the forward vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getForwardVector(Vec3d* dst) const;

    /**
     * Gets the backward vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getBackVector(Vec3d* dst) const;

    /**
     * Inverts this matrix.
     *
     * @return true if the matrix can be inverted, false otherwise.
     */
    bool inverse();

    /**
     * Get the inversed matrix.
     */
    Mat4d getInversed() const;

    /**
     * Determines if this matrix is equal to the identity matrix.
     *
     * @return true if the matrix is an identity matrix, false otherwise.
     */
    bool isIdentity() const;

    /**
     * Multiplies the components of this matrix by the specified scalar.
     *
     * @param scalar The scalar value.
     */
    void multiply(double scalar);

    /**
     * Multiplies the components of this matrix by a scalar and stores the result in dst.
     *
     * @param scalar The scalar value.
     * @param dst A matrix to store the result in.
     */
    void multiply(double scalar, Mat4d* dst) const;

    /**
     * Multiplies the components of the specified matrix by a scalar and stores the result in dst.
     *
     * @param mat The matrix.
     * @param scalar The scalar value.
     * @param dst A matrix to store the result in.
     */
    static void multiply(const Mat4d& mat, double scalar, Mat4d* dst);

    /**
     * Multiplies this matrix by the specified one.
     *
     * @param mat The matrix to multiply.
     */
    void multiply(const Mat4d& mat);

    /**
     * Multiplies m1 by m2 and stores the result in dst.
     *
     * @param m1 The first matrix to multiply.
     * @param m2 The second matrix to multiply.
     * @param dst A matrix to store the result in.
     */
    static void multiply(const Mat4d& m1, const Mat4d& m2, Mat4d* dst);

    /**
     * Negates this matrix.
     */
    void negate();

    /**
     Get the Negated matrix.
     */
    Mat4d getNegated() const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified Quaterniond rotation.
     *
     * @param q The Quaterniond to rotate by.
     */
    void rotate(const Quaterniond& q);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified Quaterniond rotation and stores the result in dst.
     *
     * @param q The Quaterniond to rotate by.
     * @param dst A matrix to store the result in.
     */
    void rotate(const Quaterniond& q, Mat4d* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified rotation about the specified axis.
     *
     * @param axis The axis to rotate about.
     * @param angle The angle (in radians).
     */
    void rotate(const Vec3d& axis, double angle);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the specified
     * rotation about the specified axis and stores the result in dst.
     *
     * @param axis The axis to rotate about.
     * @param angle The angle (in radians).
     * @param dst A matrix to store the result in.
     */
    void rotate(const Vec3d& axis, double angle, Mat4d* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified rotation around the x-axis.
     *
     * @param angle The angle (in radians).
     */
    void rotateX(double angle);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified rotation around the x-axis and stores the result in dst.
     *
     * @param angle The angle (in radians).
     * @param dst A matrix to store the result in.
     */
    void rotateX(double angle, Mat4d* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified rotation around the y-axis.
     *
     * @param angle The angle (in radians).
     */
    void rotateY(double angle);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified rotation around the y-axis and stores the result in dst.
     *
     * @param angle The angle (in radians).
     * @param dst A matrix to store the result in.
     */
    void rotateY(double angle, Mat4d* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified rotation around the z-axis.
     *
     * @param angle The angle (in radians).
     */
    void rotateZ(double angle);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified rotation around the z-axis and stores the result in dst.
     *
     * @param angle The angle (in radians).
     * @param dst A matrix to store the result in.
     */
    void rotateZ(double angle, Mat4d* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation.
     *
     * @param value The amount to scale along all axes.
     */
    void scale(double value);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation and stores the result in dst.
     *
     * @param value The amount to scale along all axes.
     * @param dst A matrix to store the result in.
     */
    void scale(double value, Mat4d* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation.
     *
     * @param xScale The amount to scale along the x-axis.
     * @param yScale The amount to scale along the y-axis.
     * @param zScale The amount to scale along the z-axis.
     */
    void scale(double xScale, double yScale, double zScale);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation and stores the result in dst.
     *
     * @param xScale The amount to scale along the x-axis.
     * @param yScale The amount to scale along the y-axis.
     * @param zScale The amount to scale along the z-axis.
     * @param dst A matrix to store the result in.
     */
    void scale(double xScale, double yScale, double zScale, Mat4d* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation.
     *
     * @param s The scale values along the x, y and z axes.
     */
    void scale(const Vec3d& s);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation and stores the result in dst.
     *
     * @param s The scale values along the x, y and z axes.
     * @param dst A matrix to store the result in.
     */
    void scale(const Vec3d& s, Mat4d* dst) const;

    /**
     * Sets the values of this matrix.
     *
     * @param m11 The first element of the first row.
     * @param m12 The second element of the first row.
     * @param m13 The third element of the first row.
     * @param m14 The fourth element of the first row.
     * @param m21 The first element of the second row.
     * @param m22 The second element of the second row.
     * @param m23 The third element of the second row.
     * @param m24 The fourth element of the second row.
     * @param m31 The first element of the third row.
     * @param m32 The second element of the third row.
     * @param m33 The third element of the third row.
     * @param m34 The fourth element of the third row.
     * @param m41 The first element of the fourth row.
     * @param m42 The second element of the fourth row.
     * @param m43 The third element of the fourth row.
     * @param m44 The fourth element of the fourth row.
     */
    void set(double m11, double m12, double m13, double m14, double m21, double m22, double m23, double m24,
             double m31, double m32, double m33, double m34, double m41, double m42, double m43, double m44);

    /**
     * Sets the values of this matrix to those in the specified column-major array.
     *
     * @param mat An array containing 16 elements in column-major format.
     */
    void set(const double* mat);

    /**
     * Sets the values of this matrix to those of the specified matrix.
     *
     * @param mat The source matrix.
     */
    void set(const Mat4d& mat);

    /**
     * Sets this matrix to the identity matrix.
     */
    void setIdentity();

    /**
     * Sets all elements of the current matrix to zero.
     */
    void setZero();

    /**
     * Subtracts the specified matrix from the current matrix.
     *
     * @param mat The matrix to subtract.
     */
    void subtract(const Mat4d& mat);

    /**
     * Subtracts the specified matrix from the current matrix.
     *
     * @param m1 The first matrix.
     * @param m2 The second matrix.
     * @param dst A matrix to store the result in.
     */
    static void subtract(const Mat4d& m1, const Mat4d& m2, Mat4d* dst);

    /**
     * Transforms the specified point by this matrix.
     *
     * The result of the transformation is stored directly into point.
     *
     * @param point The point to transform and also a vector to hold the result in.
     */
    inline void transformPoint(Vec3d* point) const { GP_ASSERT(point); transformVector(point->x, point->y, point->z, 1.0f, point); }

    /**
     * Transforms the specified point by this matrix, and stores
     * the result in dst.
     *
     * @param point The point to transform.
     * @param dst A vector to store the transformed point in.
     */
    inline void transformPoint(const Vec3d& point, Vec3d* dst) const { GP_ASSERT(dst); transformVector(point.x, point.y, point.z, 1.0f, dst); }

    /**
     * Transforms the specified vector by this matrix by
     * treating the fourth (w) coordinate as zero.
     *
     * The result of the transformation is stored directly into vector.
     *
     * @param vector The vector to transform and also a vector to hold the result in.
     */
    void transformVector(Vec3d* vector) const;

    /**
     * Transforms the specified vector by this matrix by
     * treating the fourth (w) coordinate as zero, and stores the
     * result in dst.
     *
     * @param vector The vector to transform.
     * @param dst A vector to store the transformed vector in.
     */
    void transformVector(const Vec3d& vector, Vec3d* dst) const;

    /**
     * Transforms the specified vector by this matrix.
     *
     * @param x The vector x-coordinate to transform by.
     * @param y The vector y-coordinate to transform by.
     * @param z The vector z-coordinate to transform by.
     * @param w The vector w-coordinate to transform by.
     * @param dst A vector to store the transformed point in.
     */
    void transformVector(double x, double y, double z, double w, Vec3d* dst) const;

    /**
     * Transforms the specified vector by this matrix.
     *
     * The result of the transformation is stored directly into vector.
     *
     * @param vector The vector to transform.
     */
    void transformVector(Vec4* vector) const;

    /**
     * Transforms the specified vector by this matrix.
     *
     * @param vector The vector to transform.
     * @param dst A vector to store the transformed point in.
     */
    void transformVector(const Vec4& vector, Vec4* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified translation.
     *
     * @param x The amount to translate along the x-axis.
     * @param y The amount to translate along the y-axis.
     * @param z The amount to translate along the z-axis.
     */
    void translate(double x, double y, double z);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified translation and stores the result in dst.
     *
     * @param x The amount to translate along the x-axis.
     * @param y The amount to translate along the y-axis.
     * @param z The amount to translate along the z-axis.
     * @param dst A matrix to store the result in.
     */
    void translate(double x, double y, double z, Mat4d* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified translation.
     *
     * @param t The translation values along the x, y and z axes.
     */
    void translate(const Vec3d& t);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified translation and stores the result in dst.
     *
     * @param t The translation values along the x, y and z axes.
     * @param dst A matrix to store the result in.
     */
    void translate(const Vec3d& t, Mat4d* dst) const;

    /**
     * Transposes this matrix.
     */
    void transpose();

    /**
     * Get the Transposed matrix.
     */
    Mat4d getTransposed() const;

    /**
     * Calculates the sum of this matrix with the given matrix.
     * 
     * Note: this does not modify this matrix.
     * 
     * @param mat The matrix to add.
     * @return The matrix sum.
     */
    inline const Mat4d operator+(const Mat4d& mat) const;
    
    /**
     * Adds the given matrix to this matrix.
     * 
     * @param mat The matrix to add.
     * @return This matrix, after the addition occurs.
     */
    inline Mat4d& operator+=(const Mat4d& mat);

    /**
     * Calculates the difference of this matrix with the given matrix.
     * 
     * Note: this does not modify this matrix.
     * 
     * @param mat The matrix to subtract.
     * @return The matrix difference.
     */
    inline const Mat4d operator-(const Mat4d& mat) const;

    /**
     * Subtracts the given matrix from this matrix.
     * 
     * @param mat The matrix to subtract.
     * @return This matrix, after the subtraction occurs.
     */
    inline Mat4d& operator-=(const Mat4d& mat);

    /**
     * Calculates the negation of this matrix.
     * 
     * Note: this does not modify this matrix.
     * 
     * @return The negation of this matrix.
     */
    inline const Mat4d operator-() const;

    /**
     * Calculates the matrix product of this matrix with the given matrix.
     * 
     * Note: this does not modify this matrix.
     * 
     * @param mat The matrix to multiply by.
     * @return The matrix product.
     */
    inline const Mat4d operator*(const Mat4d& mat) const;

    /**
     * Right-multiplies this matrix by the given matrix.
     * 
     * @param mat The matrix to multiply by.
     * @return This matrix, after the multiplication occurs.
     */
    inline Mat4d& operator*=(const Mat4d& mat);

    /** equals to a matrix full of zeros */
    static const Mat4d ZERO;
    /** equals to the identity matrix */
    static const Mat4d IDENTITY;

private:

    static void createBillboardHelper(const Vec3d& objectPosition, const Vec3d& cameraPosition,
                                      const Vec3d& cameraUpVector, const Vec3d* cameraForwardVector,
                                      Mat4d* dst);
};

/**
 * Transforms the given vector by the given matrix.
 * 
 * Note: this treats the given vector as a vector and not as a point.
 * 
 * @param v The vector to transform.
 * @param m The matrix to transform by.
 * @return This vector, after the transformation occurs.
 */
inline Vec3d& operator*=(Vec3d& v, const Mat4d& m);

/**
 * Transforms the given vector by the given matrix.
 * 
 * Note: this treats the given vector as a vector and not as a point.
 * 
 * @param m The matrix to transform by.
 * @param v The vector to transform.
 * @return The resulting transformed vector.
 */
inline const Vec3d operator*(const Mat4d& m, const Vec3d& v);

/**
 * Transforms the given vector by the given matrix.
 * 
 * Note: this treats the given vector as a vector and not as a point.
 * 
 * @param v The vector to transform.
 * @param m The matrix to transform by.
 * @return This vector, after the transformation occurs.
 */
inline Vec4& operator*=(Vec4& v, const Mat4d& m);

/**
 * Transforms the given vector by the given matrix.
 * 
 * Note: this treats the given vector as a vector and not as a point.
 * 
 * @param m The matrix to transform by.
 * @param v The vector to transform.
 * @return The resulting transformed vector.
 */
inline const Vec4 operator*(const Mat4d& m, const Vec4& v);

NS_HMS_MATH_END
/**
 end of base group
 @}
 */
#include "Mat4d.inl"

#endif // MATH_Mat4d_H
