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

#include "Quaterniond.h"

NS_HMS_MATH_BEGIN

inline const Quaterniond Quaterniond::operator*(const Quaterniond& q) const
{
    Quaterniond result(*this);
    result.multiply(q);
    return result;
}

inline Quaterniond& Quaterniond::operator*=(const Quaterniond& q)
{
    multiply(q);
    return *this;
}

inline Vec3d Quaterniond::operator*(const Vec3d& v) const
{
    Vec3d uv, uuv;
    Vec3d qvec(x, y, z);
    Vec3d::cross(qvec, v, &uv);
    Vec3d::cross(qvec, uv, &uuv);

    uv *= (2.0f * w);
    uuv *= 2.0f;

    return v + uv + uuv;
}

NS_HMS_MATH_END
