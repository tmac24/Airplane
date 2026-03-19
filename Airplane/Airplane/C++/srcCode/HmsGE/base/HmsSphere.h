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

#ifndef __HMS_SPHERE_H_
#define __HMS_SPHERE_H_

#include "math/HmsMath.h"

NS_HMS_BEGIN

/**
 * @brief Ray is a line with one end. usually use it to check intersects with some object,such as Plane, OBB, AABB, sphere
 **/
class HMS_DLL Sphere
{
public:
    /**
     * Constructor.
     */
    Sphere();

    /**
     * Constructor.
     */
    Sphere(const Sphere& sphere);
    
    /**
     * Constructs a new ray initialized to the specified values.
     *
     * @param origin The ray's origin.
     * @param direction The ray's direction.
     */
    Sphere(const Vec3& origin, float radius);

    /**
     * Destructor.
     */
    ~Sphere();
    
    /**
     * Sets this ray to the specified values.
     *
     * @param origin The ray's origin.
     * @param direction The ray's direction.
     */
	void Set(const Vec3& origin, float radius);

	float GetRadius() const{ return _radius; }
	float GetRadiusSquare() const{ return _radius*_radius; }
	Vec3 GetCenter() const{ return _origin; }

    Vec3 _origin;        // The sphere origin position.
    float _radius;     // The sphere radius length.
};

NS_HMS_END

#endif
