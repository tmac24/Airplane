#include "HmsGaugeOAT.h"
USING_NS_HMS;

CHmsGaugeOAT::CHmsGaugeOAT()
: m_boxWidth(100)
, m_boxHeight(24)
, m_boxColor(Color4B::BLACK)
, m_oatValue(0.0)
{
}


CHmsGaugeOAT::~CHmsGaugeOAT()
{
}

bool CHmsGaugeOAT::Init()
{
	if (!CHmsGauge::Init())
	{
		return false;
	}

	auto label = CHmsGlobal::CreateLabel("OAT", 16);
	if (label)
	{
		label->SetAnchorPoint(Vec2(0, 0.5));
		label->SetPosition(0, -m_boxHeight / 2.0);
		this->AddChild(label);
	}

	m_pValue = CHmsGlobal::CreateLabel("--- \342\204\203", 16, true);
	if (m_pValue)
	{
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

void CHmsGaugeOAT::Update(float delta)
{
	if (m_pValue)
	{
		std::string tsStr = CHmsGlobal::FormatString("%.1f \342\204\203", m_oatValue);
		m_pValue->SetString(tsStr);
	}
}

void CHmsGaugeOAT::SetValue(const float &fValue)
{
	m_oatValue = fValue;
}

void CHmsGaugeOAT::SetGaugeMode(GaugeMode flag)
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
