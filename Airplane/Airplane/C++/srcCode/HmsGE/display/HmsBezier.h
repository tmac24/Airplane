#pragma once
#include "math/HmsMath.h"
#include "math/HmsGeometry.h"

NS_HMS_BEGIN

class CHmsBezier
{
public:
	static CHmsBezier fromPoints(const Vec2 &p1, const Vec2 &p2,
		const Vec2 &p3, const Vec2 &p4);

	Vec2 pointAt(float t) const;

	Rect bounds() const;
// 	float length(float error = 0.01) const;
// 	
// 	float tAtLength(float len) const;


	Vec2 pt1() const { return Vec2(x1, y1); }
	Vec2 pt2() const { return Vec2(x2, y2); }
	Vec2 pt3() const { return Vec2(x3, y3); }
	Vec2 pt4() const { return Vec2(x4, y4); }

	float x1, y1, x2, y2, x3, y3, x4, y4;
};

NS_HMS_END