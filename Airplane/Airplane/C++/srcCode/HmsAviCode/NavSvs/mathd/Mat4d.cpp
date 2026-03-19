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
#include "Quaterniond.h"
#include "MathUtild.h"
#include <math.h>


NS_HMS_MATH_BEGIN

Mat4d::Mat4d()
{
    *this = IDENTITY;
}

Mat4d::Mat4d(double m11, double m12, double m13, double m14, double m21, double m22, double m23, double m24,
               double m31, double m32, double m33, double m34, double m41, double m42, double m43, double m44)
{
    set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
}

Mat4d::Mat4d(const double* mat)
{
    set(mat);
}

Mat4d::Mat4d(const Mat4d& copy)
{
    memcpy(m, copy.m, MATRIX_SIZE);
}

Mat4d::~Mat4d()
{
}

void Mat4d::createLookAt(const Vec3d& eyePosition, const Vec3d& tarGetPoistion, const Vec3d& up, Mat4d* dst)
{
    createLookAt(eyePosition.x, eyePosition.y, eyePosition.z, tarGetPoistion.x, tarGetPoistion.y, tarGetPoistion.z,
                 up.x, up.y, up.z, dst);
}

void Mat4d::createLookAt(double eyePositionX, double eyePositionY, double eyePositionZ,
                          double tarGetPoistionX, double tarGetPoistionY, double tarGetPoistionZ,
                          double upX, double upY, double upZ, Mat4d* dst)
{
    GP_ASSERT(dst);

    Vec3d eye(eyePositionX, eyePositionY, eyePositionZ);
    Vec3d target(tarGetPoistionX, tarGetPoistionY, tarGetPoistionZ);
    Vec3d up(upX, upY, upZ);
    up.normalize();

    Vec3d zaxis;
    Vec3d::subtract(eye, target, &zaxis);
    zaxis.normalize();

    Vec3d xaxis;
    Vec3d::cross(up, zaxis, &xaxis);
    xaxis.normalize();

    Vec3d yaxis;
    Vec3d::cross(zaxis, xaxis, &yaxis);
    yaxis.normalize();

    dst->m[0] = xaxis.x;
    dst->m[1] = yaxis.x;
    dst->m[2] = zaxis.x;
    dst->m[3] = 0.0f;

    dst->m[4] = xaxis.y;
    dst->m[5] = yaxis.y;
    dst->m[6] = zaxis.y;
    dst->m[7] = 0.0f;

    dst->m[8] = xaxis.z;
    dst->m[9] = yaxis.z;
    dst->m[10] = zaxis.z;
    dst->m[11] = 0.0f;

    dst->m[12] = -Vec3d::dot(xaxis, eye);
    dst->m[13] = -Vec3d::dot(yaxis, eye);
    dst->m[14] = -Vec3d::dot(zaxis, eye);
    dst->m[15] = 1.0f;
}

void Mat4d::createPerspective(double fieldOfView, double aspectRatio,
                                     double zNearPlane, double zFarPlane, Mat4d* dst)
{
    GP_ASSERT(dst);
    GP_ASSERT(zFarPlane != zNearPlane);

    double f_n = 1.0f / (zFarPlane - zNearPlane);
    double theta = MATH_DEG_TO_RAD(fieldOfView) * 0.5f;
    if (fabs(fmod(theta, MATH_PIOVER2)) < MATH_EPSILON)
    {
        CCLOGERROR("Invalid field of view value (%f) causes attempted calculation tan(%f), which is undefined.", fieldOfView, theta);
        return;
    }
    double divisor = tan(theta);
    GP_ASSERT(divisor);
    double factor = 1.0f / divisor;

    memset(dst, 0, MATRIX_SIZE);

    GP_ASSERT(aspectRatio);
    dst->m[0] = (1.0f / aspectRatio) * factor;
    dst->m[5] = factor;
    dst->m[10] = (-(zFarPlane + zNearPlane)) * f_n;
    dst->m[11] = -1.0f;
    dst->m[14] = -2.0f * zFarPlane * zNearPlane * f_n;
}

void Mat4d::createOrthographic(double width, double height, double zNearPlane, double zFarPlane, Mat4d* dst)
{
    double halfWidth = width / 2.0f;
    double halfHeight = height / 2.0f;
    createOrthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, zNearPlane, zFarPlane, dst);
}

void Mat4d::createOrthographicOffCenter(double left, double right, double bottom, double top,
                                         double zNearPlane, double zFarPlane, Mat4d* dst)
{
    GP_ASSERT(dst);
    GP_ASSERT(right != left);
    GP_ASSERT(top != bottom);
    GP_ASSERT(zFarPlane != zNearPlane);

    memset(dst, 0, MATRIX_SIZE);
    dst->m[0] = 2 / (right - left);
    dst->m[5] = 2 / (top - bottom);
    dst->m[10] = 2 / (zNearPlane - zFarPlane);

    dst->m[12] = (left + right) / (left - right);
    dst->m[13] = (top + bottom) / (bottom - top);
    dst->m[14] = (zNearPlane + zFarPlane) / (zNearPlane - zFarPlane);
    dst->m[15] = 1;
}
    
void Mat4d::createBillboard(const Vec3d& objectPosition, const Vec3d& cameraPosition,
                             const Vec3d& cameraUpVector, Mat4d* dst)
{
    createBillboardHelper(objectPosition, cameraPosition, cameraUpVector, nullptr, dst);
}

void Mat4d::createBillboard(const Vec3d& objectPosition, const Vec3d& cameraPosition,
                             const Vec3d& cameraUpVector, const Vec3d& cameraForwardVector,
                             Mat4d* dst)
{
    createBillboardHelper(objectPosition, cameraPosition, cameraUpVector, &cameraForwardVector, dst);
}

void Mat4d::createBillboardHelper(const Vec3d& objectPosition, const Vec3d& cameraPosition,
                                   const Vec3d& cameraUpVector, const Vec3d* cameraForwardVector,
                                   Mat4d* dst)
{
    Vec3d delta(objectPosition, cameraPosition);
    bool isSufficientDelta = delta.lengthSquared() > MATH_EPSILON;

    dst->setIdentity();
    dst->m[3] = objectPosition.x;
    dst->m[7] = objectPosition.y;
    dst->m[11] = objectPosition.z;

    // As per the contracts for the 2 variants of createBillboard, we need
    // either a safe default or a sufficient distance between object and camera.
    if (cameraForwardVector || isSufficientDelta)
    {
        Vec3d target = isSufficientDelta ? cameraPosition : (objectPosition - *cameraForwardVector);

        // A billboard is the inverse of a lookAt rotation
        Mat4d lookAt;
        createLookAt(objectPosition, target, cameraUpVector, &lookAt);
        dst->m[0] = lookAt.m[0];
        dst->m[1] = lookAt.m[4];
        dst->m[2] = lookAt.m[8];
        dst->m[4] = lookAt.m[1];
        dst->m[5] = lookAt.m[5];
        dst->m[6] = lookAt.m[9];
        dst->m[8] = lookAt.m[2];
        dst->m[9] = lookAt.m[6];
        dst->m[10] = lookAt.m[10];
    }
}
    
// void Mat4d::createReflection(const Plane& plane, Mat4d* dst)
// {
//     Vec3d normal(plane.getNormal());
//     double k = -2.0f * plane.getDistance();

//     dst->setIdentity();

//     dst->m[0] -= 2.0f * normal.x * normal.x;
//     dst->m[5] -= 2.0f * normal.y * normal.y;
//     dst->m[10] -= 2.0f * normal.z * normal.z;
//     dst->m[1] = dst->m[4] = -2.0f * normal.x * normal.y;
//     dst->m[2] = dst->m[8] = -2.0f * normal.x * normal.z;
//     dst->m[6] = dst->m[9] = -2.0f * normal.y * normal.z;
    
//     dst->m[3] = k * normal.x;
//     dst->m[7] = k * normal.y;
//     dst->m[11] = k * normal.z;
// }

void Mat4d::createScale(const Vec3d& scale, Mat4d* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, &IDENTITY, MATRIX_SIZE);

    dst->m[0] = scale.x;
    dst->m[5] = scale.y;
    dst->m[10] = scale.z;
}

void Mat4d::createScale(double xScale, double yScale, double zScale, Mat4d* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, &IDENTITY, MATRIX_SIZE);

    dst->m[0] = xScale;
    dst->m[5] = yScale;
    dst->m[10] = zScale;
}


void Mat4d::createRotation(const Quaterniond& q, Mat4d* dst)
{
    GP_ASSERT(dst);

    double x2 = q.x + q.x;
    double y2 = q.y + q.y;
    double z2 = q.z + q.z;

    double xx2 = q.x * x2;
    double yy2 = q.y * y2;
    double zz2 = q.z * z2;
    double xy2 = q.x * y2;
    double xz2 = q.x * z2;
    double yz2 = q.y * z2;
    double wx2 = q.w * x2;
    double wy2 = q.w * y2;
    double wz2 = q.w * z2;

    dst->m[0] = 1.0f - yy2 - zz2;
    dst->m[1] = xy2 + wz2;
    dst->m[2] = xz2 - wy2;
    dst->m[3] = 0.0f;

    dst->m[4] = xy2 - wz2;
    dst->m[5] = 1.0f - xx2 - zz2;
    dst->m[6] = yz2 + wx2;
    dst->m[7] = 0.0f;

    dst->m[8] = xz2 + wy2;
    dst->m[9] = yz2 - wx2;
    dst->m[10] = 1.0f - xx2 - yy2;
    dst->m[11] = 0.0f;

    dst->m[12] = 0.0f;
    dst->m[13] = 0.0f;
    dst->m[14] = 0.0f;
    dst->m[15] = 1.0f;
}

void Mat4d::createRotation(const Vec3d& axis, double angle, Mat4d* dst)
{
    GP_ASSERT(dst);

    double x = axis.x;
    double y = axis.y;
    double z = axis.z;

    // Make sure the input axis is normalized.
    double n = x*x + y*y + z*z;
    if (n != 1.0f)
    {
        // Not normalized.
        n = sqrt(n);
        // Prevent divide too close to zero.
        if (n > 0.000001f)
        {
            n = 1.0f / n;
            x *= n;
            y *= n;
            z *= n;
        }
    }

    double c = cos(angle);
    double s = sin(angle);

    double t = 1.0f - c;
    double tx = t * x;
    double ty = t * y;
    double tz = t * z;
    double txy = tx * y;
    double txz = tx * z;
    double tyz = ty * z;
    double sx = s * x;
    double sy = s * y;
    double sz = s * z;

    dst->m[0] = c + tx*x;
    dst->m[1] = txy + sz;
    dst->m[2] = txz - sy;
    dst->m[3] = 0.0f;

    dst->m[4] = txy - sz;
    dst->m[5] = c + ty*y;
    dst->m[6] = tyz + sx;
    dst->m[7] = 0.0f;

    dst->m[8] = txz + sy;
    dst->m[9] = tyz - sx;
    dst->m[10] = c + tz*z;
    dst->m[11] = 0.0f;

    dst->m[12] = 0.0f;
    dst->m[13] = 0.0f;
    dst->m[14] = 0.0f;
    dst->m[15] = 1.0f;
}

void Mat4d::createRotationX(double angle, Mat4d* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, &IDENTITY, MATRIX_SIZE);

    double c = cos(angle);
    double s = sin(angle);

    dst->m[5]  = c;
    dst->m[6]  = s;
    dst->m[9]  = -s;
    dst->m[10] = c;
}

void Mat4d::createRotationY(double angle, Mat4d* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, &IDENTITY, MATRIX_SIZE);

    double c = cos(angle);
    double s = sin(angle);

    dst->m[0]  = c;
    dst->m[2]  = -s;
    dst->m[8]  = s;
    dst->m[10] = c;
}

void Mat4d::createRotationZ(double angle, Mat4d* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, &IDENTITY, MATRIX_SIZE);

    double c = cos(angle);
    double s = sin(angle);

    dst->m[0] = c;
    dst->m[1] = s;
    dst->m[4] = -s;
    dst->m[5] = c;
}

void Mat4d::createTranslation(const Vec3d& translation, Mat4d* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, &IDENTITY, MATRIX_SIZE);

    dst->m[12] = translation.x;
    dst->m[13] = translation.y;
    dst->m[14] = translation.z;
}

void Mat4d::createTranslation(double xTranslation, double yTranslation, double zTranslation, Mat4d* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, &IDENTITY, MATRIX_SIZE);

    dst->m[12] = xTranslation;
    dst->m[13] = yTranslation;
    dst->m[14] = zTranslation;
}

void Mat4d::add(double scalar)
{
    add(scalar, this);
}

void Mat4d::add(double scalar, Mat4d* dst)
{
    GP_ASSERT(dst);
#ifdef __SSE__
    MathUtild::addMatrix(col, scalar, dst->col);
#else
    MathUtild::addMatrix(m, scalar, dst->m);
#endif
}

void Mat4d::add(const Mat4d& mat)
{
    add(*this, mat, this);
}

void Mat4d::add(const Mat4d& m1, const Mat4d& m2, Mat4d* dst)
{
    GP_ASSERT(dst);
#ifdef __SSE__
    MathUtild::addMatrix(m1.col, m2.col, dst->col);
#else
    MathUtild::addMatrix(m1.m, m2.m, dst->m);
#endif
}

bool Mat4d::decompose(Vec3d* scale, Quaterniond* rotation, Vec3d* translation) const
{
    if (translation)
    {
        // Extract the translation.
        translation->x = m[12];
        translation->y = m[13];
        translation->z = m[14];
    }

    // Nothing left to do.
    if (scale == nullptr && rotation == nullptr)
        return true;

    // Extract the scale.
    // This is simply the length of each axis (row/column) in the matrix.
    Vec3d xaxis(m[0], m[1], m[2]);
    double scaleX = xaxis.length();

    Vec3d yaxis(m[4], m[5], m[6]);
    double scaleY = yaxis.length();

    Vec3d zaxis(m[8], m[9], m[10]);
    double scaleZ = zaxis.length();

    // Determine if we have a negative scale (true if determinant is less than zero).
    // In this case, we simply negate a single axis of the scale.
    double det = determinant();
    if (det < 0)
        scaleZ = -scaleZ;

    if (scale)
    {
        scale->x = scaleX;
        scale->y = scaleY;
        scale->z = scaleZ;
    }

    // Nothing left to do.
    if (rotation == nullptr)
        return true;

    // Scale too close to zero, can't decompose rotation.
    if (scaleX < MATH_TOLERANCE || scaleY < MATH_TOLERANCE || fabs(scaleZ) < MATH_TOLERANCE)
        return false;

    double rn;

    // Factor the scale out of the matrix axes.
    rn = 1.0f / scaleX;
    xaxis.x *= rn;
    xaxis.y *= rn;
    xaxis.z *= rn;

    rn = 1.0f / scaleY;
    yaxis.x *= rn;
    yaxis.y *= rn;
    yaxis.z *= rn;

    rn = 1.0f / scaleZ;
    zaxis.x *= rn;
    zaxis.y *= rn;
    zaxis.z *= rn;

    // Now calculate the rotation from the resulting matrix (axes).
    double trace = xaxis.x + yaxis.y + zaxis.z + 1.0f;

    if (trace > MATH_EPSILON)
    {
        double s = 0.5f / sqrt(trace);
        rotation->w = 0.25f / s;
        rotation->x = (yaxis.z - zaxis.y) * s;
        rotation->y = (zaxis.x - xaxis.z) * s;
        rotation->z = (xaxis.y - yaxis.x) * s;
    }
    else
    {
        // Note: since xaxis, yaxis, and zaxis are normalized, 
        // we will never divide by zero in the code below.
        if (xaxis.x > yaxis.y && xaxis.x > zaxis.z)
        {
            double s = 0.5f / sqrt(1.0f + xaxis.x - yaxis.y - zaxis.z);
            rotation->w = (yaxis.z - zaxis.y) * s;
            rotation->x = 0.25f / s;
            rotation->y = (yaxis.x + xaxis.y) * s;
            rotation->z = (zaxis.x + xaxis.z) * s;
        }
        else if (yaxis.y > zaxis.z)
        {
            double s = 0.5f / sqrt(1.0f + yaxis.y - xaxis.x - zaxis.z);
            rotation->w = (zaxis.x - xaxis.z) * s;
            rotation->x = (yaxis.x + xaxis.y) * s;
            rotation->y = 0.25f / s;
            rotation->z = (zaxis.y + yaxis.z) * s;
        }
        else
        {
            double s = 0.5f / sqrt(1.0f + zaxis.z - xaxis.x - yaxis.y );
            rotation->w = (xaxis.y - yaxis.x ) * s;
            rotation->x = (zaxis.x + xaxis.z ) * s;
            rotation->y = (zaxis.y + yaxis.z ) * s;
            rotation->z = 0.25f / s;
        }
    }

    return true;
}

double Mat4d::determinant() const
{
    double a0 = m[0] * m[5] - m[1] * m[4];
    double a1 = m[0] * m[6] - m[2] * m[4];
    double a2 = m[0] * m[7] - m[3] * m[4];
    double a3 = m[1] * m[6] - m[2] * m[5];
    double a4 = m[1] * m[7] - m[3] * m[5];
    double a5 = m[2] * m[7] - m[3] * m[6];
    double b0 = m[8] * m[13] - m[9] * m[12];
    double b1 = m[8] * m[14] - m[10] * m[12];
    double b2 = m[8] * m[15] - m[11] * m[12];
    double b3 = m[9] * m[14] - m[10] * m[13];
    double b4 = m[9] * m[15] - m[11] * m[13];
    double b5 = m[10] * m[15] - m[11] * m[14];

    // Calculate the determinant.
    return (a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0);
}

void Mat4d::getScale(Vec3d* scale) const
{
    decompose(scale, nullptr, nullptr);
}

bool Mat4d::getRotation(Quaterniond* rotation) const
{
    return decompose(nullptr, rotation, nullptr);
}

void Mat4d::getTranslation(Vec3d* translation) const
{
    decompose(nullptr, nullptr, translation);
}

void Mat4d::getUpVector(Vec3d* dst) const
{
    GP_ASSERT(dst);

    dst->x = m[4];
    dst->y = m[5];
    dst->z = m[6];
}

void Mat4d::getDownVector(Vec3d* dst) const
{
    GP_ASSERT(dst);
    
    dst->x = -m[4];
    dst->y = -m[5];
    dst->z = -m[6];
}

void Mat4d::getLeftVector(Vec3d* dst) const
{
    GP_ASSERT(dst);

    dst->x = -m[0];
    dst->y = -m[1];
    dst->z = -m[2];
}

void Mat4d::getRightVector(Vec3d* dst) const
{
    GP_ASSERT(dst);

    dst->x = m[0];
    dst->y = m[1];
    dst->z = m[2];
}

void Mat4d::getForwardVector(Vec3d* dst) const
{
    GP_ASSERT(dst);

    dst->x = -m[8];
    dst->y = -m[9];
    dst->z = -m[10];
}

void Mat4d::getBackVector(Vec3d* dst) const
{
    GP_ASSERT(dst);

    dst->x = m[8];
    dst->y = m[9];
    dst->z = m[10];
}

Mat4d Mat4d::getInversed() const
{
    Mat4d mat(*this);
    mat.inverse();
    return mat;
}

bool Mat4d::inverse()
{
    double a0 = m[0] * m[5] - m[1] * m[4];
    double a1 = m[0] * m[6] - m[2] * m[4];
    double a2 = m[0] * m[7] - m[3] * m[4];
    double a3 = m[1] * m[6] - m[2] * m[5];
    double a4 = m[1] * m[7] - m[3] * m[5];
    double a5 = m[2] * m[7] - m[3] * m[6];
    double b0 = m[8] * m[13] - m[9] * m[12];
    double b1 = m[8] * m[14] - m[10] * m[12];
    double b2 = m[8] * m[15] - m[11] * m[12];
    double b3 = m[9] * m[14] - m[10] * m[13];
    double b4 = m[9] * m[15] - m[11] * m[13];
    double b5 = m[10] * m[15] - m[11] * m[14];

    // Calculate the determinant.
    double det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

    // Close to zero, can't invert.
    if (fabs(det) <= MATH_TOLERANCE)
        return false;

    // Support the case where m == dst.
    Mat4d inverse;
    inverse.m[0]  = m[5] * b5 - m[6] * b4 + m[7] * b3;
    inverse.m[1]  = -m[1] * b5 + m[2] * b4 - m[3] * b3;
    inverse.m[2]  = m[13] * a5 - m[14] * a4 + m[15] * a3;
    inverse.m[3]  = -m[9] * a5 + m[10] * a4 - m[11] * a3;

    inverse.m[4]  = -m[4] * b5 + m[6] * b2 - m[7] * b1;
    inverse.m[5]  = m[0] * b5 - m[2] * b2 + m[3] * b1;
    inverse.m[6]  = -m[12] * a5 + m[14] * a2 - m[15] * a1;
    inverse.m[7]  = m[8] * a5 - m[10] * a2 + m[11] * a1;

    inverse.m[8]  = m[4] * b4 - m[5] * b2 + m[7] * b0;
    inverse.m[9]  = -m[0] * b4 + m[1] * b2 - m[3] * b0;
    inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
    inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

    inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
    inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
    inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
    inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

    multiply(inverse, 1.0f / det, this);

    return true;
}

bool Mat4d::isIdentity() const
{
    return (memcmp(m, &IDENTITY, MATRIX_SIZE) == 0);
}

void Mat4d::multiply(double scalar)
{
    multiply(scalar, this);
}

void Mat4d::multiply(double scalar, Mat4d* dst) const
{
    multiply(*this, scalar, dst);
}

void Mat4d::multiply(const Mat4d& m, double scalar, Mat4d* dst)
{
    GP_ASSERT(dst);
#ifdef __SSE__
    MathUtild::multiplyMatrix(m.col, scalar, dst->col);
#else
    MathUtild::multiplyMatrix(m.m, scalar, dst->m);
#endif
}

void Mat4d::multiply(const Mat4d& mat)
{
    multiply(*this, mat, this);
}

void Mat4d::multiply(const Mat4d& m1, const Mat4d& m2, Mat4d* dst)
{
    GP_ASSERT(dst);
#ifdef __SSE__
    MathUtild::multiplyMatrix(m1.col, m2.col, dst->col);
#else
    MathUtild::multiplyMatrix(m1.m, m2.m, dst->m);
#endif
}

void Mat4d::negate()
{
#ifdef __SSE__
    MathUtild::negateMatrix(col, col);
#else
    MathUtild::negateMatrix(m, m);
#endif
}

Mat4d Mat4d::getNegated() const
{
    Mat4d mat(*this);
    mat.negate();
    return mat;
}

void Mat4d::rotate(const Quaterniond& q)
{
    rotate(q, this);
}

void Mat4d::rotate(const Quaterniond& q, Mat4d* dst) const
{
    Mat4d r;
    createRotation(q, &r);
    multiply(*this, r, dst);
}

void Mat4d::rotate(const Vec3d& axis, double angle)
{
    rotate(axis, angle, this);
}

void Mat4d::rotate(const Vec3d& axis, double angle, Mat4d* dst) const
{
    Mat4d r;
    createRotation(axis, angle, &r);
    multiply(*this, r, dst);
}

void Mat4d::rotateX(double angle)
{
    rotateX(angle, this);
}

void Mat4d::rotateX(double angle, Mat4d* dst) const
{
    Mat4d r;
    createRotationX(angle, &r);
    multiply(*this, r, dst);
}

void Mat4d::rotateY(double angle)
{
    rotateY(angle, this);
}

void Mat4d::rotateY(double angle, Mat4d* dst) const
{
    Mat4d r;
    createRotationY(angle, &r);
    multiply(*this, r, dst);
}

void Mat4d::rotateZ(double angle)
{
    rotateZ(angle, this);
}

void Mat4d::rotateZ(double angle, Mat4d* dst) const
{
    Mat4d r;
    createRotationZ(angle, &r);
    multiply(*this, r, dst);
}

void Mat4d::scale(double value)
{
    scale(value, this);
}

void Mat4d::scale(double value, Mat4d* dst) const
{
    scale(value, value, value, dst);
}

void Mat4d::scale(double xScale, double yScale, double zScale)
{
    scale(xScale, yScale, zScale, this);
}

void Mat4d::scale(double xScale, double yScale, double zScale, Mat4d* dst) const
{
    Mat4d s;
    createScale(xScale, yScale, zScale, &s);
    multiply(*this, s, dst);
}

void Mat4d::scale(const Vec3d& s)
{
    scale(s.x, s.y, s.z, this);
}

void Mat4d::scale(const Vec3d& s, Mat4d* dst) const
{
    scale(s.x, s.y, s.z, dst);
}

void Mat4d::set(double m11, double m12, double m13, double m14, double m21, double m22, double m23, double m24,
                 double m31, double m32, double m33, double m34, double m41, double m42, double m43, double m44)
{
    m[0]  = m11;
    m[1]  = m21;
    m[2]  = m31;
    m[3]  = m41;
    m[4]  = m12;
    m[5]  = m22;
    m[6]  = m32;
    m[7]  = m42;
    m[8]  = m13;
    m[9]  = m23;
    m[10] = m33;
    m[11] = m43;
    m[12] = m14;
    m[13] = m24;
    m[14] = m34;
    m[15] = m44;
}

void Mat4d::set(const double* mat)
{
    GP_ASSERT(mat);
    memcpy(this->m, mat, MATRIX_SIZE);
}

void Mat4d::set(const Mat4d& mat)
{
    memcpy(this->m, mat.m, MATRIX_SIZE);
}

void Mat4d::setIdentity()
{
    memcpy(m, &IDENTITY, MATRIX_SIZE);
}

void Mat4d::setZero()
{
    memset(m, 0, MATRIX_SIZE);
}

void Mat4d::subtract(const Mat4d& mat)
{
    subtract(*this, mat, this);
}

void Mat4d::subtract(const Mat4d& m1, const Mat4d& m2, Mat4d* dst)
{
    GP_ASSERT(dst);
#ifdef __SSE__
    MathUtild::subtractMatrix(m1.col, m2.col, dst->col);
#else
    MathUtild::subtractMatrix(m1.m, m2.m, dst->m);
#endif
}

void Mat4d::transformVector(Vec3d* vector) const
{
    GP_ASSERT(vector);
    transformVector(vector->x, vector->y, vector->z, 0.0f, vector);
}

void Mat4d::transformVector(const Vec3d& vector, Vec3d* dst) const
{
    transformVector(vector.x, vector.y, vector.z, 0.0f, dst);
}

void Mat4d::transformVector(double x, double y, double z, double w, Vec3d* dst) const
{
    GP_ASSERT(dst);

    MathUtild::transformVec4(m, x, y, z, w, (double*)dst);
}

void Mat4d::transformVector(Vec4* vector) const
{
    GP_ASSERT(vector);
    transformVector(*vector, vector);
}

void Mat4d::transformVector(const Vec4& vector, Vec4* dst) const
{
    GP_ASSERT(dst);
#ifdef __SSE__
    MathUtild::transformVec4(col, vector.v, dst->v);
#else
    MathUtild::transformVec4(m, (const double*) &vector, (double*)dst);
#endif
}

void Mat4d::translate(double x, double y, double z)
{
    translate(x, y, z, this);
}

void Mat4d::translate(double x, double y, double z, Mat4d* dst) const
{
    Mat4d t;
    createTranslation(x, y, z, &t);
    multiply(*this, t, dst);
}

void Mat4d::translate(const Vec3d& t)
{
    translate(t.x, t.y, t.z, this);
}

void Mat4d::translate(const Vec3d& t, Mat4d* dst) const
{
    translate(t.x, t.y, t.z, dst);
}

void Mat4d::transpose()
{
#ifdef __SSE__
    MathUtild::transposeMatrix(col, col);
#else
    MathUtild::transposeMatrix(m, m);
#endif
}

Mat4d Mat4d::getTransposed() const
{
    Mat4d mat(*this);
    mat.transpose();
    return mat;
}

const Mat4d Mat4d::IDENTITY = Mat4d(
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);

const Mat4d Mat4d::ZERO = Mat4d(
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0 );

NS_HMS_MATH_END
