#include "HmsBezier.h"

NS_HMS_BEGIN

CHmsBezier CHmsBezier::fromPoints(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4)
{
	CHmsBezier b;
	b.x1 = p1.x;
	b.y1 = p1.y;
	b.x2 = p2.x;
	b.y2 = p2.y;
	b.x3 = p3.x;
	b.y3 = p3.y;
	b.x4 = p4.x;
	b.y4 = p4.y;
	return b;
}

Vec2 CHmsBezier::pointAt(float t) const
{
	float x, y;

	float m_t = 1. - t;
	{
		float a = x1*m_t + x2*t;
		float b = x2*m_t + x3*t;
		float c = x3*m_t + x4*t;
		a = a*m_t + b*t;
		b = b*m_t + c*t;
		x = a*m_t + b*t;
	}
	{
		float a = y1*m_t + y2*t;
		float b = y2*m_t + y3*t;
		float c = y3*m_t + y4*t;
		a = a*m_t + b*t;
		b = b*m_t + c*t;
		y = a*m_t + b*t;
	}
	return Vec2(x, y);
}

Rect CHmsBezier::bounds() const
{
	float xmin = x1;
	float xmax = x1;
	if (x2 < xmin)
		xmin = x2;
	else if (x2 > xmax)
		xmax = x2;
	if (x3 < xmin)
		xmin = x3;
	else if (x3 > xmax)
		xmax = x3;
	if (x4 < xmin)
		xmin = x4;
	else if (x4 > xmax)
		xmax = x4;

	float ymin = y1;
	float ymax = y1;
	if (y2 < ymin)
		ymin = y2;
	else if (y2 > ymax)
		ymax = y2;
	if (y3 < ymin)
		ymin = y3;
	else if (y3 > ymax)
		ymax = y3;
	if (y4 < ymin)
		ymin = y4;
	else if (y4 > ymax)
		ymax = y4;
	return Rect(xmin, ymin, xmax - xmin, ymax - ymin);
}

// float CHmsBezier::length(float error /*= 0.01*/) const
// {
// 
// }
// 
// float CHmsBezier::tAtLength(float len) const
// {
// 
// }
NS_HMS_END