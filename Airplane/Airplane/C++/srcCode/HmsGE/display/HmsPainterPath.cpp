#include "HmsPainterPath.h"
#include <math.h>
#include <algorithm>
using namespace std;

NS_HMS_BEGIN

// static inline bool isnan(double d) { return std::isnan(d); }
// static inline bool isinf(double d) { return std::isinf(d); }
// static inline bool isfinite(double d) { return std::isfinite(d); }
// static inline bool isnan(float f) { return std::isnan(f); }
// static inline bool isinf(float f) { return std::isinf(f); }
// static inline bool isfinite(float f) { return std::isfinite(f); }

CHmsPainterPath::CHmsPainterPath()
{
}


CHmsPainterPath::~CHmsPainterPath()
{
}

void CHmsPainterPath::closeSubpath()
{
	if (m_vElements.size())
	{
		m_vElements.push_back(m_vElements.at(0));
	}
}

void CHmsPainterPath::moveTo(const Vec2 &p)
{
	if (!isfinite(p.x) || !isfinite(p.y))
	{
		return;
	}
	Element elm = { p.x, p.y, MoveToElement };
	m_vElements.push_back(elm);
}

void CHmsPainterPath::moveTo(double x, double y)
{
	if (!isfinite(x) || !isfinite(y))
	{
		return;
	}
	Element elm = { x, y, MoveToElement };
	m_vElements.push_back(elm);
}

void CHmsPainterPath::lineTo(const Vec2 &p)
{
	if (!isfinite(p.x) || !isfinite(p.y))
	{
		return;
	}
	Element elm = { p.x, p.y, LineToElement };
	m_vElements.push_back(elm);
}

void CHmsPainterPath::lineTo(double x, double y)
{
	if (!isfinite(x) || !isfinite(y))
	{
		return;
	}
	Element elm = { x, y, LineToElement };
	m_vElements.push_back(elm);
}
#if 0
void qt_find_ellipse_coords(const QRectF &r, qreal angle, qreal length,
	QPointF* startPoint, QPointF *endPoint)
{
	if (r.isNull()) {
		if (startPoint)
			*startPoint = QPointF();
		if (endPoint)
			*endPoint = QPointF();
		return;
	}

	qreal w2 = r.width() / 2;
	qreal h2 = r.height() / 2;

	qreal angles[2] = { angle, angle + length };
	QPointF *points[2] = { startPoint, endPoint };

	for (int i = 0; i < 2; ++i) {
		if (!points[i])
			continue;

		qreal theta = angles[i] - 360 * qFloor(angles[i] / 360);
		qreal t = theta / 90;
		// truncate
		int quadrant = int(t);
		t -= quadrant;

		t = qt_t_for_arc_angle(90 * t);

		// swap x and y?
		if (quadrant & 1)
			t = 1 - t;

		qreal a, b, c, d;
		QBezier::coefficients(t, a, b, c, d);
		QPointF p(a + b + c*QT_PATH_KAPPA, d + c + b*QT_PATH_KAPPA);

		// left quadrants
		if (quadrant == 1 || quadrant == 2)
			p.rx() = -p.x();

		// top quadrants
		if (quadrant == 0 || quadrant == 1)
			p.ry() = -p.y();

		*points[i] = r.center() + QPointF(w2 * p.x(), h2 * p.y());
	}
}
#endif
void CHmsPainterPath::arcMoveTo(const Rect &rect, double angle)
{
#if 0
	Vec2 pt;
	//qt_find_ellipse_coords(rect, angle, 0, &pt, 0);
	moveTo(pt);
#endif
}

void CHmsPainterPath::arcMoveTo(double x, double y, double w, double h, double angle)
{
	arcMoveTo(Rect(x, y, w, h), angle);
}

void CHmsPainterPath::arcTo(const Rect &rect, double startAngle, double arcLength)
{
#if 0
	int point_count;
	Vec2 pts[15];
	Vec2 curve_start = qt_curves_for_arc(rect, startAngle, arcLength, pts, &point_count);

	lineTo(curve_start);
	for (int i = 0; i < point_count; i += 3) {
		cubicTo(pts[i].x, pts[i].y,
			pts[i + 1].x, pts[i + 1].y,
			pts[i + 2].x, pts[i + 2].y);
	}
#endif
}

void CHmsPainterPath::arcTo(double x, double y, double w, double h, double startAngle, double arcLength)
{
	arcTo(Rect(x, y, w, h), startAngle, arcLength);
}

void CHmsPainterPath::cubicTo(const Vec2 &c1, const Vec2 &c2, const Vec2 &e)
{
	if (!isfinite(c1.x) || !isfinite(c1.y) || !isfinite(c2.x) || !isfinite(c2.y) || !isfinite(e.x) || !isfinite(e.y))
	{
		return;
	}
	Element ce1 = { c1.x, c1.y, CurveToElement };
	Element ce2 = { c2.x, c2.y, CurveToDataElement };
	Element ee = { e.x, e.y, CurveToDataElement };
	m_vElements.push_back(ce1);
	m_vElements.push_back(ce2);
	m_vElements.push_back(ee);
}

void CHmsPainterPath::cubicTo(double ctrlPt1x, double ctrlPt1y, double ctrlPt2x, double ctrlPt2y, double endPtx, double endPty)
{
	cubicTo(Vec2(ctrlPt1x, ctrlPt1y), Vec2(ctrlPt2x, ctrlPt2y), Vec2(endPtx, endPty));
}

void CHmsPainterPath::quadTo(const Vec2 &c, const Vec2 &e)
{
	const Element &elm = m_vElements.at(m_vElements.size() - 1);
	Vec2 prev(elm.x, elm.y);

	Vec2 c1((prev.x + 2 * c.x) / 3, (prev.y + 2 * c.y) / 3);
	Vec2 c2((e.x + 2 * c.x) / 3, (e.y + 2 * c.y) / 3);
	cubicTo(c1, c2, e);
}

void CHmsPainterPath::quadTo(double ctrlPtx, double ctrlPty, double endPtx, double endPty)
{
	quadTo(Vec2(ctrlPtx, ctrlPty), Vec2(endPtx, endPty));
}

void CHmsPainterPath::CopyByMove(CHmsPainterPath & path, const Vec2 & pos)
{
	path.m_vElements.clear();
	path.m_vElements.reserve(m_vElements.size());

	for (auto c:m_vElements)
	{
		c.x += pos.x;
		c.y += pos.y;
		path.m_vElements.push_back(c);
	}
}

void CHmsPainterPath::CopyByScale(CHmsPainterPath & path, float fScale)
{
	path.m_vElements.clear();
	path.m_vElements.reserve(m_vElements.size());

	for (auto c : m_vElements)
	{
		c.x *= fScale;
		c.y *= fScale;
		path.m_vElements.push_back(c);
	}
}

void CHmsPainterPath::CopyByFlapY(CHmsPainterPath & path)
{
	path.m_vElements.clear();
	path.m_vElements.reserve(m_vElements.size());

	for (auto c : m_vElements)
	{
		c.y = -c.y;
		path.m_vElements.push_back(c);
	}
}

NS_HMS_END