#ifndef __MATH_CCAFFINETRANSFORM_H__
#define __MATH_CCAFFINETRANSFORM_H__

#include "HmsAviMacros.h"

#include "math/HmsGeometry.h"
#include "math/HmsMath.h"

NS_HMS_BEGIN

/**@{
 Affine transform
 a   b    0
 c   d    0
 tx  ty   1
 
 Identity
 1   0    0
 0   1    0
 0   0    1
 */
struct HMS_DLL AffineTransform {
    float a, b, c, d;
    float tx, ty;

    static const AffineTransform IDENTITY;
};

/**@}*/

/**Make affine transform.*/
HMS_DLL AffineTransform __CCAffineTransformMake(float a, float b, float c, float d, float tx, float ty);
#define AffineTransformMake __CCAffineTransformMake

/**Multiply point (x,y,1) by a  affine transform.*/
HMS_DLL Vec2 __CCPointApplyAffineTransform(const Vec2& point, const AffineTransform& t);
#define PointApplyAffineTransform __CCPointApplyAffineTransform

/**Multiply size (width,height,0) by a  affine transform.*/
HMS_DLL Size __CCSizeApplyAffineTransform(const Size& size, const AffineTransform& t);
#define SizeApplyAffineTransform __CCSizeApplyAffineTransform
/**Make identity affine transform.*/
HMS_DLL AffineTransform AffineTransformMakeIdentity();
/**Transform Rect, which will transform the four vertices of the point.*/
HMS_DLL Rect RectApplyAffineTransform(const Rect& rect, const AffineTransform& anAffineTransform);
/**@{
 Transform vec2 and Rect by Mat4.
 */
HMS_DLL Rect RectApplyTransform(const Rect& rect, const Mat4& transform);
HMS_DLL Vec2 PointApplyTransform(const Vec2& point, const Mat4& transform);
/**@}*/
/**
 Translation, equals
 1  0  1
 0  1  0   * affine transform
 tx ty 1
 */
HMS_DLL AffineTransform AffineTransformTranslate(const AffineTransform& t, float tx, float ty);
/**
 Rotation, equals
 cos(angle)   sin(angle)   0
 -sin(angle)  cos(angle)   0  * AffineTransform
 0            0            1
 */
HMS_DLL AffineTransform AffineTransformRotate(const AffineTransform& aTransform, float anAngle);
/**
 Scale, equals
 sx   0   0
 0    sy  0  * affineTransform
 0    0   1
 */
HMS_DLL AffineTransform AffineTransformScale(const AffineTransform& t, float sx, float sy);
/**Concat two affine transform, t1 * t2*/
HMS_DLL AffineTransform AffineTransformConcat(const AffineTransform& t1, const AffineTransform& t2);
/**Compare affine transform.*/
HMS_DLL bool AffineTransformEqualToTransform(const AffineTransform& t1, const AffineTransform& t2);
/**Get the inverse of affine transform.*/
HMS_DLL AffineTransform AffineTransformInvert(const AffineTransform& t);
/**Concat Mat4, return t1 * t2.*/
HMS_DLL Mat4 TransformConcat(const Mat4& t1, const Mat4& t2);

extern HMS_DLL const AffineTransform AffineTransformIdentity;

NS_HMS_END

#endif // __MATH_CCAFFINETRANSFORM_H__
