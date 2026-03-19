#include "HmsGaugeAOA.h"
USING_NS_HMS;

CHmsGaugeAOA::CHmsGaugeAOA()
: m_boxWidth(100)
, m_boxHeight(24)
, m_boxColor(Color4B::BLACK)
, m_aoaValue(0.0)
{
}


CHmsGaugeAOA::~CHmsGaugeAOA()
{
}

bool CHmsGaugeAOA::Init()
{
	if (!CHmsGauge::Init())
	{
		return false;
	}

	auto label = CHmsGlobal::CreateLabel("AOA", 16);
	if (label)
	{
		label->SetAnchorPoint(Vec2(0, 0.5));
		label->SetPosition(0, -m_boxHeight / 2.0);
		this->AddChild(label);
	}

	m_pValue = CHmsGlobal::CreateLabel("--- °", 16, true);
	if (m_pValue)
	{
		//m_pValue = CHmsGlobal::CreateLabel("我", 16, true);
		m_pValue->SetAnchorPoint(Vec2(1, 0.5));
		m_pValue->SetPosition(m_boxWidth, -m_boxHeight / 2.0);

		this->AddChild(m_pValue);
	}
	

	this->SetContentSize(Size(m_boxWidth, m_boxHeight));

	m_curGaugeMode = CHmsGauge::GaugeMode::GAUGE_MODE_ERROR;
	m_pFailWin = CHmsDrawNode::Create();
	DrawFailWin(m_pFailWin, 0, 0, m_boxWidth, -m_boxHeight);
	this->AddChild(m_pFailWin);
	return true;
}

void CHmsGaugeAOA::Update(float delta)
{
	if (m_pValue)
	{
		std::string tsStr = CHmsGlobal::FormatString("%.2f \302\260", m_aoaValue);
		m_pValue->SetString(tsStr);
	}
}

void CHmsGaugeAOA::SetValue(const float &fValue)
{
	m_aoaValue = fValue;
}

void CHmsGaugeAOA::SetGaugeMode(GaugeMode flag)
{
	if (flag == m_curGaugeMode) return;

	m_curGaugeMode = flag;
	if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_OK)
	{
		this->RemoveChild(m_pFailWin);
		this->SetScheduleUpdate();
	}
	else if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_ERROR)
	{
		this->AddChild(m_pFailWin);
		this->SetUnSchedule();
	}
}
