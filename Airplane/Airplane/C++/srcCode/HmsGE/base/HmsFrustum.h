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

#ifndef __HMS_FRUSTUM_H_
#define __HMS_FRUSTUM_H_
#include <stdio.h>
#include "base/HmsAABB.h"
#include "base/HmsOBB.h"
#include "base/HmsPlane.h"

NS_HMS_BEGIN

class Camera;

/**
 * the frustum is a six-side geometry, usually use the frustum to do fast-culling:
 * check a entity whether is a potential visible entity
 * @js NA
 * @lua NA
 */
class HMS_DLL Frustum
{
    friend class Camera;
public:
    /**
     * Constructor & Destructor.
     */
    Frustum(): _clipZ(true), _initialized(false) {}
    ~Frustum(){}

    /**
     * init frustum from camera.
     */
    bool initFrustum(const Camera* camera);

	bool initFrustum(const Camera* camera, float curFarPct);

    bool initFrustum(const Mat4* pMat);

    /**
     * is aabb out of frustum.
     */
    bool isOutOfFrustum(const AABB& aabb) const;

	bool isOutOfFrustum(const Vec3 & pos) const;
    /**
     * is obb out of frustum
     */
    bool isOutOfFrustum(const OBB& obb) const;

    /**
     * get & set z clip. if bclipZ == true use near and far plane
     */
    void setClipZ(bool clipZ) { _clipZ = clipZ; }
    bool isClipZ() { return _clipZ; }
    
protected:
    /**
     * create clip plane
     */
    void createPlane(const Camera* camera);
	void createPlane(const Camera* camera, float curFarPct);
    void createPlane(const Mat4* pMat);

    Plane _plane[6];             // clip plane, left, right, top, bottom, near, far
    bool _clipZ;                // use near and far clip plane
    bool _initialized;
};

NS_HMS_END

#endif//__HMS_FRUSTUM_H_
