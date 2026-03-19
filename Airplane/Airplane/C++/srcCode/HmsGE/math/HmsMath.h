#ifndef __HMS_MATH_H__
#define __HMS_MATH_H__

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"
#include "Quaternion.h"

#ifndef HMS_MAX
#define HMS_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef HMS_MIN
#define HMS_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef HMS_BETWEEN_VALUE
#define HMS_BETWEEN_VALUE(_value_, _min_, _max_) ((_value_) < (_min_) ? (_min_) : (_value_) < (_max_) ? (_value_) : (_max_))
#endif

//线程安全的浮点数模糊比较, 处理了浮点精度误差的问题
static inline bool FuzzyCompare(double p1, double p2)
{
	return (fabs(p1 - p2) <= 0.000000000001 * HMS_MIN(fabs(p1), fabs(p2)));
}
static inline bool FuzzyCompare(float p1, float p2)
{
	return (fabsf(p1 - p2) <= 0.00001f * HMS_MIN(fabsf(p1), fabsf(p2)));
}

static inline bool FuzzyIsNull(double d)
{
	return fabs(d) <= 0.000000000001;
}

static inline bool FuzzyIsNull(float f)
{
	return fabsf(f) <= 0.00001f;
}

static inline double HmsMath_acos(double v)
{
    v = HMS_BETWEEN_VALUE(v, -1, 1);
    return acos(v);
}

static inline float HmsMath_acos(float v)
{
    v = HMS_BETWEEN_VALUE(v, -1, 1);
    return acos(v);
}

static inline double HmsMath_asin(double v)
{
    v = HMS_BETWEEN_VALUE(v, -1, 1);
    return asin(v);
}

static inline float HmsMath_asin(float v)
{
    v = HMS_BETWEEN_VALUE(v, -1, 1);
    return asin(v);
}

#endif
