#include "HmsGlideslope.h"
USING_NS_HMS;

CHmsGlideslope::CHmsGlideslope()
: m_boxWidth(20)
, m_boxHeight(168)
, m_scaleGap_px(33)
, m_boxColor(0x00, 0x00, 0x00, 0x3C)
, m_scaleColor(Color4B::WHITE)
, m_pointerColor(0xFF, 0x00, 0xCB, 0xFF)
, m_minRange(-0.7f)
, m_maxRange(0.7f)
, m_unitDeviationPx(0.0f)
, m_curDeviation(0.0f)
{
	m_unitDeviationPx = (m_scaleGap_px * 4) / (m_maxRange - m_minRange);
}


CHmsGlideslope::~CHmsGlideslope()
{
}

bool CHmsGlideslope::Init()
{
	if (!CHmsDrawNode::Init())
	{
		return false;
	}

	this->DrawSolidRect(-m_boxWidth / 2.0, m_boxHeight / 2.0, m_boxWidth / 2.0, -m_boxHeight / 2.0, Color4F(m_boxColor));
	//绘制刻度
	this->DrawRing(Vec2(0, m_scaleGap_px), 4, 1.4f);//上1圆圈
	this->DrawRing(Vec2(0, m_scaleGap_px * 2), 4, 1.4f);//上2圆圈
	this->DrawHorizontalLine(-m_boxWidth / 2.0 + 2, m_boxWidth / 2.0 - 2, 0, 2.0, Color4F(m_scaleColor));//中间
	this->DrawRing(Vec2(0, -m_scaleGap_px), 4, 1.4f);//下1圆圈
	this->DrawRing(Vec2(0, -m_scaleGap_px * 2), 4, 1.4f);//下2圆圈

	m_pNodePointer = CHmsImageNode::Create("res/Pfd/ILS/diamond.png");
	if (m_pNodePointer)
	{
		m_pNodePointer->SetColor(Color3B::GREEN);

		this->AddChild(m_pNodePointer);
	}
	
	this->SetScheduleUpdate();
	return true;
}

void CHmsGlideslope::Update(float delta)
{
	CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
#if 0
	AircraftInstrumentData *aircraft = dataBus->GetAircraftInstrumentData();

	m_pNodePointer->SetVisible(aircraft->gsiVailable);
	if (aircraft->gsiVailable)
	{
		SetDeviation(aircraft->gsiNeedle);
	}
#endif
}

void CHmsGlideslope::SetRange(float fMin, float fMax)
{
	m_minRange = fMin;
	m_maxRange = fMax;

	m_unitDeviationPx = (m_scaleGap_px * 4) / (m_maxRange - m_minRange);
}

void CHmsGlideslope::SetDeviation(float fDev)
{
	if (std::fabs(m_curDeviation - fDev) < 0.001)
	{
		return;
	}

	m_curDeviation = fDev;
	MovePointer();
}

void CHmsGlideslope::MovePointer()
{
	float normalDev = 0.0f;
	normalDev = m_curDeviation > m_maxRange ? m_maxRange : m_curDeviation;
	normalDev = m_curDeviation < m_minRange ? m_minRange : m_curDeviation;

	float moveY = m_unitDeviationPx * normalDev;
	if (m_pNodePointer)
	{
		m_pNodePointer->SetPositionY(moveY);
	}
}

void CHmsGlideslope::SetNoData()
{
	if (m_pNodePointer)
	{
		m_pNodePointer->SetVisible(false);
	}
}
