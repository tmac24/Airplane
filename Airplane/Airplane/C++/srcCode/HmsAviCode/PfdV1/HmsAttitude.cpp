#include "HmsAttitude.h"
#include "display/HmsDrawNode.h"
#include "base/HmsClippingNode.h"
#include "Calculate/CalculateZs.h"

USING_NS_HMS;

CHmsAttitude::CHmsAttitude()
:m_picthScaleWidth(124.0f)
, m_maxPitch(90.0f)
, m_pitchGap(2.5f)
, m_pitchGap_px(12.0f)
, m_skidBoxWidth(27.0f)
, m_curPitch(0.0f)
, m_curRoll(0.0f)
, m_curSkid(0.0f)
, OnPitchChanged(nullptr)
, OnRollChanged(nullptr)
{
	m_maxSkid = m_skidBoxWidth * 2;
	m_dataBus = CHmsGlobal::GetInstance()->GetDataBus();
}


CHmsAttitude::~CHmsAttitude()
{
}

bool CHmsAttitude::Init()
{
	if (!CHmsGauge::Init())
	{
		return false;
	}

	int pitchScaleCount = m_maxPitch / m_pitchGap;
	Vec2 center(0, 0);

	//横滚刻度表盘
	m_pRollScaleDial = CHmsImageNode::Create("res/Pfd/Attitude/roll_scale.png");
	if (m_pRollScaleDial)
	{
		m_pRollScaleDial->SetAnchorPoint(Vec2(0.5, 1));
		m_pRollScaleDial->SetPosition(0, 186);
	}
	

	//横滚指针
	m_pRollPointer = CHmsDrawNode::Create();
	{
		Vec2 rollPointerPnts[3] = { Vec2(0, 171),
			Vec2(12, 156),
			Vec2(-12, 156) };
		m_pRollPointer->DrawLineLoop(rollPointerPnts, 3, 3, Color4B::WHITE);
	}

	//侧滑指示器
	m_pSkidIndicator = CHmsDrawNode::Create();
	{
		Vec2 skidBox[4] = { Vec2(-m_skidBoxWidth/2.0f, 151),
			Vec2(m_skidBoxWidth / 2.0f, 151),
			Vec2(m_skidBoxWidth / 2.0f, 146),
			Vec2(-m_skidBoxWidth / 2.0f, 146) };
		m_pSkidIndicator->DrawLineLoop(skidBox, 4, 2.6f, Color4B::WHITE);
	}
	m_pRollPointer->AddChild(m_pSkidIndicator);

	//俯仰刻度表盘
	auto picthScaleDial = CHmsNode::Create();
	{
		auto pitchScaleClipNode = CHmsClippingNode::create();
		auto pitchScaleStencilNode = CHmsDrawNode::Create();
		pitchScaleStencilNode->DrawDisk(center, 135);
		pitchScaleClipNode->setStencil(pitchScaleStencilNode);

		auto pitchScaleLines = CHmsImageNode::Create("res/Pfd/Attitude/pitch_scale.png");
		if (pitchScaleLines)
		{
			pitchScaleClipNode->AddChild(pitchScaleLines);
		}

		picthScaleDial->AddChild(pitchScaleClipNode);

		m_pPitchScale = pitchScaleLines;
	}
	m_pPicthDial = picthScaleDial;

	//飞机符号
	auto aircraftSymbol = CHmsImageNode::Create("res/Pfd/Attitude/aircraft.png");

	this->AddChild(m_pRollPointer);
	//this->AddChild(m_pSkidIndicator);
	if (m_pRollScaleDial)
	{
		this->AddChild(m_pRollScaleDial);
	}
	this->AddChild(picthScaleDial);
	if (aircraftSymbol)
	{
		this->AddChild(aircraftSymbol);
	}
	
	int bottomPart = 134;
	this->SetContentSize(Size(172 * 2, 184 + bottomPart));

	auto sizeContext = GetContentSize();
	m_pFailWin = CHmsDrawNode::Create();
	DrawFailWin(m_pFailWin, -sizeContext.width / 2.0, -bottomPart, sizeContext.width / 2.0, sizeContext.height - bottomPart);
	this->AddChild(m_pFailWin);

	return true;
}

#include "DataInterface/HmsDataBus.h"

void CHmsAttitude::Update(float delta)
{
	CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
	auto aftData = dataBus->GetData();

	this->UpdatePitch(aftData->pitch);//设置俯仰角度
	this->UpdateRoll(aftData->roll);//设置滚转角
	this->UpdateSkid(aftData->sideSliAngle);//设置侧滑
}

void CHmsAttitude::UpdatePitch(const float &fPitch)
{
	if (std::fabs(fPitch - m_curPitch) > 0.001f)
	{
		m_curPitch = fPitch;
		if (fPitch > m_maxPitch)
		{
			m_curPitch = m_maxPitch;
		}
		else if (fPitch < -m_maxPitch)
		{
			m_curPitch = -m_maxPitch;
		}

		float unitPx = m_pitchGap_px / m_pitchGap;
		float scrollCount = -unitPx * m_curPitch;

		m_pPitchScale->SetPositionY(scrollCount);

		if (OnPitchChanged)
		{
			OnPitchChanged(scrollCount);
		}
	}
}

void CHmsAttitude::UpdateRoll(const float &fRoll)
{
	if (std::fabs(fRoll - m_curRoll) > 0.001f)
	{
		//m_pRollPointer->SetRotation(fRoll);
		RotateNodeOnCenter(m_pRollScaleDial, -fRoll, Vec2(0, 0), 186);
		m_pPicthDial->SetRotation(-fRoll);
		m_curRoll = fRoll;

		if (OnRollChanged)
		{
			OnRollChanged(fRoll);
		}
	}
}

void CHmsAttitude::UpdateSkid(const float &fSkid)
{
	if (std::fabs(fSkid - m_curSkid) > 0.001f)
	{
		m_curSkid = fSkid * m_skidBoxWidth;
		if (m_curSkid > m_maxSkid)
		{
			m_curSkid = m_maxSkid;
		}
		else if (m_curSkid < -m_maxSkid)
		{
			m_curSkid = -m_maxSkid;
		}
		m_pSkidIndicator->SetPositionX(m_curSkid);
	}
}

void CHmsAttitude::SetGaugeMode(GaugeMode flag)
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

void CHmsAttitude::RotateNodeOnCenter(HmsAviPf::CHmsNode *target, const float &angle, const HmsAviPf::Vec2 &center, const float &radius)
{
	double rad = ToRadian(angle);
	float offsetX = radius * sin(rad);
	float offsetY = radius * cos(rad);
	Vec2 newPos = center + Vec2(offsetX, offsetY);
	target->SetPosition(newPos);
	target->SetRotation(angle);
}
