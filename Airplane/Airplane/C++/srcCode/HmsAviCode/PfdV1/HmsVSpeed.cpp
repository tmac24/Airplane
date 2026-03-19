#include "HmsVSpeed.h"
#include "../Calculate/CalculateZs.h"

USING_NS_HMS;

CHmsVSpeed::CHmsVSpeed()
: m_boxWidth(46.0f)
, m_boxHeight(224.0f)
, m_boxColor(0, 0, 0, 60)
, m_hollowWidth(14.0f)
, m_gapPx0_1(48.0f)
, m_gapPx1_2(32.0f)
, m_gapPx2_6(24.0f)
{
	m_pointerCenterPos = Vec2(m_boxWidth / 2.0 + 26, 0);
	m_pointerStartX = m_boxWidth / 2.0 - 2.0;
	m_pointerEndX = -m_boxWidth / 2.0 + m_hollowWidth + 5;
}


CHmsVSpeed::~CHmsVSpeed()
{
}

bool CHmsVSpeed::Init()
{
	if (!CHmsGauge::Init())
	{
		return false;
	}
	//表盘
	auto dailBox = CHmsImageNode::Create("res/Pfd/VSpeed/dial.png");

	//垂直速度指针
	auto vspeedPointer = CHmsDrawNode::Create();
	{
		vspeedPointer->DrawHorizontalLine(m_pointerStartX, m_pointerEndX, 0);
		m_pVSpeedPointer = vspeedPointer;
	}
	
	m_pVSpeedLabel = CHmsGlobal::CreateLabel("0", 16);
	if (m_pVSpeedLabel)
	{
		m_pVSpeedLabel->SetAnchorPoint(Vec2(0, 0.5));
		m_pVSpeedLabel->SetPosition(-m_boxWidth / 2.0, m_boxHeight / 2.0 + 16);
	}

	if (dailBox)
	{
		this->AddChild(dailBox);
	}
	this->AddChild(vspeedPointer);
	if (m_pVSpeedLabel)
	{
		this->AddChild(m_pVSpeedLabel);
	}

	m_pFailWin = CHmsDrawNode::Create();
	DrawFailWin(m_pFailWin, -m_boxWidth / 2.0, m_boxHeight/2.0, m_boxWidth/2.0, -m_boxHeight/2.0);
	this->AddChild(m_pFailWin);

	return true;
}

void CHmsVSpeed::UpdateVSpeed(const float &fVSpeed)
{
	if (!m_pVSpeedLabel)
	{
		return;
	}
	refreshPointer(fVSpeed);

	int vsAbs = fabsf(fVSpeed);
	if (vsAbs < 400)
	{
		m_pVSpeedLabel->SetVisible(false);
		return;
	}

	m_pVSpeedLabel->SetVisible(true);
	int displayValue = int(vsAbs / 10) * 10;//去掉十位以下数字
	std::string strVSpeed = CHmsGlobal::FormatString("%d", displayValue);
	m_pVSpeedLabel->SetString(strVSpeed);

	float poxY = fVSpeed > 0 ? m_boxHeight / 2.0 + 16 : -m_boxHeight / 2.0 - 16;
	m_pVSpeedLabel->SetPositionY(poxY);
}

void CHmsVSpeed::refreshPointer(const float &fVSpeed)
{
	float normVSpeed = fVSpeed / 1000.0f;
	float absVS = fabs(normVSpeed);
	float offsetPx = 0.0;
	if (absVS >= 0 && absVS <= 1.0)
	{
		offsetPx = absVS * m_gapPx0_1;
	}
	else if (absVS > 1.0 && absVS <= 2.0)
	{
		offsetPx = m_gapPx0_1 + (absVS - 1.0) * m_gapPx1_2;
	}
	else if (absVS > 2.0 && absVS <= 6.0)
	{
		offsetPx = m_gapPx0_1 + m_gapPx1_2 + (absVS - 2.0) / 4 * m_gapPx2_6;
	}
	else // >6.0当做6.0处理
	{
		offsetPx = m_gapPx0_1 + m_gapPx1_2 + m_gapPx2_6;
	}

	m_pVSpeedPointer->clear();

	if (fVSpeed == 0.0)
	{
		m_pVSpeedPointer->DrawHorizontalLine(m_pointerStartX, m_pointerEndX, 0);
	}
	else
	{
		float x1 = fabs(m_pointerEndX - m_pointerCenterPos.x);
		float x2 = fabs(m_pointerStartX - m_pointerCenterPos.x);
		float y1 = offsetPx;
		float y2 = y1 * x2 / x1;

		if (fVSpeed < 0.0)
		{
			y1 = -y1;
			y2 = -y2;
		}

		Vec2 pnts[2] = { Vec2(m_pointerStartX, y2),
		 				 Vec2(m_pointerEndX, y1) };
		m_pVSpeedPointer->DrawLineStrip(pnts, 2, 2.0);
	}
}
#include "DataInterface/HmsDataBus.h"
void CHmsVSpeed::Update(float delta)
{
	CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
#if 0
	AircraftInstrumentData *aircraft = dataBus->GetAircraftInstrumentData();
	
	//更新垂直速度
	this->UpdateVSpeed(aircraft->verticalSpeed);
#endif
	
	this->UpdateVSpeed(FeetToMeter(dataBus->GetData()->verticalSpeed));
}

void CHmsVSpeed::SetGaugeMode(GaugeMode flag)
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

