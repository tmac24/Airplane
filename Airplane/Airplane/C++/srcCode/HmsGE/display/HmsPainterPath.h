#pragma once
#include "math/HmsMath.h"
#include "math/HmsGeometry.h"
#include <string>
#include <vector>
#include <math.h>

NS_HMS_BEGIN

class CHmsPainterPath
{
public:
	enum ElementType {
		MoveToElement,
		LineToElement,
		CurveToElement,
		CurveToDataElement
	};

	class Element {
	public:
		double x;
		double y;
		ElementType type;

		bool isMoveTo() const { return type == MoveToElement; }
		bool isLineTo() const { return type == LineToElement; }
		bool isCurveTo() const { return type == CurveToElement; }

		operator Vec2() const { return Vec2(float(x), float(y)); }

		bool operator==(const Element &e) const {
			return FuzzyCompare(x, e.x)
				&& FuzzyCompare(y, e.y) && type == e.type;
		}
		inline bool operator!=(const Element &e) const { return !operator==(e); }
	};
public:
	CHmsPainterPath();
	~CHmsPainterPath();

	const std::vector<CHmsPainterPath::Element> * GetPathElements() const { return &m_vElements; }

	void CopyByMove(CHmsPainterPath & path, const Vec2 & pos);

	void CopyByScale(CHmsPainterPath & path, float fScale);

	void CopyByFlapY(CHmsPainterPath & path);

public:
	void closeSubpath();

	void moveTo(const Vec2 &p);
	void moveTo(double x, double y);

	void lineTo(const Vec2 &p);
	void lineTo(double x, double y);

	void arcMoveTo(const Rect &rect, double angle);
	void arcMoveTo(double x, double y, double w, double h, double angle);

	void arcTo(const Rect &rect, double startAngle, double arcLength);
	void arcTo(double x, double y, double w, double h, double startAngle, double arcLength);

	void cubicTo(const Vec2 &ctrlPt1, const Vec2 &ctrlPt2, const Vec2 &endPt);
	void cubicTo(double ctrlPt1x, double ctrlPt1y, double ctrlPt2x, double ctrlPt2y,
		double endPtx, double endPty);
	void quadTo(const Vec2 &ctrlPt, const Vec2 &endPt);
	void quadTo(double ctrlPtx, double ctrlPty, double endPtx, double endPty);

private:
	std::vector<CHmsPainterPath::Element>	m_vElements;
};

NS_HMS_END