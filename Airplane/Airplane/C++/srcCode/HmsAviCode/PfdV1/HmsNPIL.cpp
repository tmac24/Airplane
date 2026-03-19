#include "HmsNPIL.h"
USING_NS_HMS;

CHmsNPIL::CHmsNPIL()
: m_boxWidth(168)
, m_boxHeight(24)
, m_scaleGap_px(66)
, m_boxColor(0x00, 0x00, 0x00, 0x3C)
, m_scaleColor(Color4B::WHITE)
, m_pointerColor(0xFF, 0x00, 0xCB, 0xFF)
, m_minRange(-0.4f)
, m_maxRange(0.4f)
, m_unitDeviationPx(0.0f)
, m_curDeviation(0.0f)
{
	m_unitDeviationPx = (m_scaleGap_px * 2) / (m_maxRange - m_minRange);
}


CHmsNPIL::~CHmsNPIL()
{
}

bool CHmsNPIL::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_pNodeBox = CHmsDrawNode::Create();
	m_pNodeBox->SetPosition(0, 0);
	m_pNodeBox->DrawSolidRect(-m_boxWidth / 2.0, m_boxHeight / 2.0, m_boxWidth / 2.0, -m_boxHeight / 2.0, Color4F(m_boxColor));

	m_pNodeScale = CHmsDrawNode::Create();
	m_pNodeScale->DrawVerticalLine(0, m_boxHeight / 2.0 - 2, 0, 2.0, Color4F(m_scaleColor));//中间
	m_pNodeScale->DrawVerticalLine(-m_scaleGap_px, m_boxHeight / 2.0 - 2, 0, 2.0, Color4F(m_scaleColor));
	m_pNodeScale->DrawVerticalLine(m_scaleGap_px, m_boxHeight / 2.0 - 2, 0, 2.0, Color4F(m_scaleColor));
	m_pNodeScale->DrawSolidRect(-m_scaleGap_px - 8, m_boxHeight / 2.0 - 2, -m_scaleGap_px + 18, 6, Color4F(m_scaleColor));
	m_pNodeScale->DrawSolidRect(m_scaleGap_px + 8, m_boxHeight / 2.0 - 2, m_scaleGap_px - 18, 6, Color4F(m_scaleColor));

	m_pNodePointer = CHmsDrawNode::Create();
	std::vector<HmsAviPf::Vec2> triPnts;
	triPnts.push_back(Vec2(0, m_boxHeight / 2.0 - 5));
	triPnts.push_back(Vec2(-7, -m_boxHeight / 2.0 + 5));
	triPnts.push_back(Vec2(7, -m_boxHeight / 2.0 + 5));
	m_pNodePointer->DrawTriangles(triPnts, Color4F(m_pointerColor));

	m_pNodeBox->AddChild(m_pNodeScale);
	m_pNodeBox->AddChild(m_pNodePointer);

	this->AddChild(m_pNodeBox);

	return true;
}

void CHmsNPIL::SetRange(float fMin, float fMax)
{
	m_minRange = fMin;
	m_maxRange = fMax;

	m_unitDeviationPx = (m_scaleGap_px * 2) / (m_maxRange - m_minRange);
}

void CHmsNPIL::SetDeviation(float fDev)
{
	float temp = m_curDeviation;
	m_curDeviation = fDev;

	if (std::fabs(temp - fDev) > 0.001)
	{
		UpdatePointer();
	}
}

void CHmsNPIL::UpdatePointer()
{
	float moveX = m_unitDeviationPx * m_curDeviation;
	m_pNodePointer->SetPositionX(moveX);
}

