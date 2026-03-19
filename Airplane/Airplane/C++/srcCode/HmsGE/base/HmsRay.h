/****************************************************************************
 Copyright (c) 2014-2017 Chukong Technologies Inc.

 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HMS_RAY_H_
#define __HMS_RAY_H_

#include "math/HmsMath.h"
#include "base/HmsAABB.h"
#include "base/HmsOBB.h"
#include "base/HmsPlane.h"
#include "base/HmsSphere.h"

NS_HMS_BEGIN

/**
 * @brief Ray is a line with one end. usually use it to check intersects with some object,such as Plane, OBB, AABB, sphere
 **/
class HMS_DLL Ray
{
public:
    /**
     * Constructor.
     */
    Ray();

    /**
     * Constructor.
     */
    Ray(const Ray& ray);
    
    /**
     * Constructs a new ray initialized to the specified values.
     *
     * @param origin The ray's origin.
     * @param direction The ray's direction.
     */
    Ray(const Vec3& origin, const Vec3& direction);

    /**
     * Destructor.
     */
    ~Ray();

    /**
     * Check whether this ray intersects with the specified AABB.
     */
    bool intersects(const AABB& aabb, float* distance = nullptr) const;
    
    /**
     * Check whether this ray intersects with the specified OBB.
     */
    bool intersects(const OBB& obb, float* distance = nullptr) const;

	/**
	* Check whether this ray intersects with the specified Sphere.
	*/
	bool intersects(const Sphere & sphere, Vec3 & pos) const;
	bool intersects(const Sphere & sphere, Vec3 & pos1, Vec3 & pos2) const;


    float dist(const Plane& plane) const;
    Vec3 intersects(const Plane& plane) const;
    
    /**
     * Sets this ray to the specified values.
     *
     * @param origin The ray's origin.
     * @param direction The ray's direction.
     */
    void set(const Vec3& origin, const Vec3& direction);

    /**
     * Transforms this ray by the given transformation matrix.
     *
     * @param matrix The transformation matrix to transform by.
     */
    void transform(const Mat4& matrix);

	Vec3 getPosT(float t) const;

    Vec3 _origin;        // The ray origin position.
    Vec3 _direction;     // The ray direction vector.
};

NS_HMS_END

#endif
