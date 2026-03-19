#pragma once

#include "display/HmsDrawNode.h"
#include "display/HmsLabel.h"
#include "base/RefPtr.h"
#include "HmsGlobal.h"
#include "math/HmsMathBase.h"
#include "Calculate/CalculateZs.h"
#include "Calculate/PositionZs.h"

using namespace HmsAviPf;

class CHmsMapMeasure : public HmsAviPf::CHmsNode
{
public:
	CHmsMapMeasure();
	~CHmsMapMeasure();

	CREATE_FUNC(CHmsMapMeasure);

	virtual bool Init() override;

	virtual void UpdateLayer();

	void MeasurePos(const Vec2 &touchBegin, const Vec2 &touchEnd, const Vec2 &earthBegin, const Vec2 &earthEnd);

	void CloseMeasure();

private:
	HmsAviPf::RefPtr<CHmsDrawNode>		m_drawNode;
	HmsAviPf::RefPtr<CHmsLabel>			m_labelUp;
	HmsAviPf::RefPtr<CHmsLabel>			m_labelDown;
};

