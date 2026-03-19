#include "HmsRadioAlt.h"
USING_NS_HMS;

CHmsRadioAlt::CHmsRadioAlt()
: m_boxWidth(60)
, m_boxHeight(22)
, m_boxColor(Color4B::BLACK)
, m_radioAlt(0)
{
}


CHmsRadioAlt::~CHmsRadioAlt()
{
}

bool CHmsRadioAlt::Init()
{
	if (!CHmsGauge::Init())
	{
		return false;
	}

    m_pBoxNode = CHmsDrawNode::Create();
	m_pBoxNode->DrawSolidRect(-m_boxWidth / 2.0, m_boxHeight / 2.0, m_boxWidth / 2.0, -m_boxHeight / 2.0, Color4F(m_boxColor));

	m_pValue = CHmsGlobal::CreateLabel("0000", 16, true);

	if (m_pValue)
	{
		m_pBoxNode->AddChild(m_pValue);
	}
	this->AddChild(m_pBoxNode);

	this->SetContentSize(Size(m_boxWidth, m_boxHeight));

	m_curGaugeMode = CHmsGauge::GaugeMode::GAUGE_MODE_ERROR;
	m_pFailWin = CHmsDrawNode::Create();
	DrawFailWin(m_pFailWin, -m_boxWidth / 2.0, m_boxHeight / 2.0, m_boxWidth / 2.0, -m_boxHeight / 2.0);
	this->AddChild(m_pFailWin);
	return true;
}

void CHmsRadioAlt::Update(float delta)
{
	if (m_pValue)
	{
		std::string strVal = CHmsGlobal::FormatString("%04d", m_radioAlt);
		m_pValue->SetString(strVal);
	}
}

void CHmsRadioAlt::SetValue(const int &iValue)
{
	m_radioAlt = iValue;
}

void CHmsRadioAlt::SetGaugeMode(GaugeMode flag)
{
	if (flag == m_curGaugeMode) return;

	m_curGaugeMode = flag;
	this->SetVisible(true);
	if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_OK)
	{
		this->RemoveChild(m_pFailWin);
		//m_pFailWin->RemoveFromParent();
		this->SetScheduleUpdate();
	}
	else if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_ERROR)
	{
		this->AddChild(m_pFailWin);
		this->SetUnSchedule();
	}
	else if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_NODATA)
	{
		this->SetVisible(false);
	}
}
