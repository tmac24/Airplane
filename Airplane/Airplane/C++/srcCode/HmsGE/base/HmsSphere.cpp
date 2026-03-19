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

#include "base/HmsSphere.h"

NS_HMS_BEGIN

Sphere::Sphere()
: _origin(0, 0, 0)
{
}

Sphere::Sphere(const Sphere& sphere)
	: _origin(sphere._origin)
	, _radius(sphere._radius)
{
	
}

Sphere::Sphere(const Vec3& origin, float radius)
	:_origin(origin)
	, _radius(radius)
{
    
}

Sphere::~Sphere()
{
}

void Sphere::Set(const Vec3& origin, float radius)
{
    _origin = origin;
	_radius = radius;
}

NS_HMS_END
