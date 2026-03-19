#include "HmsAirspeed.h"
#include "DataInterface/HmsDataBus.h"
#include "base/HmsClippingNode.h"
#include "ui/HmsUiImagePanel.h"
USING_NS_HMS;

#define KnotKMH				1.852

CHmsAirspeed::CHmsAirspeed()
: m_scaleBoxHeight(318.0f)
, m_scaleBoxWidth(76.0f)
, m_boxColor(0, 0, 0, 60)
, m_vectorColor(0, 0xFF, 0, 0xFF)
, m_scaleLineWidth(14.0f)
, m_scaleGap_px(26.0f)
, m_maxSpeed(500.0f)
, m_speedGap(10.0f)
, m_markingWidth(8)
, m_selSpeed(-1000.0f)
, m_curSpeed(0.0f)
, HmsUiEventInterface(this)
{
}


CHmsAirspeed::~CHmsAirspeed()
{
}

bool CHmsAirspeed::Init()
{
	if (!CHmsGauge::Init())
	{
		return false;
	}

	this->SetAnchorPoint(0.0f, 1.0);
	

	float scaleCount = m_maxSpeed / m_speedGap;

	m_pSelAirspeedBox = CHmsNumBox::Create(m_scaleBoxWidth, 30, 0.0f, m_maxSpeed, m_speedGap);
	//m_pSelAirspeedBox->SetPosition(Vec2(0, -30));
	m_pSelAirspeedBox->OnValueChanged = HMS_CALLBACK_1(CHmsAirspeed::OnSelectedSpeedChanged, this);

	//{{added by wzh 20170710
	//添加米制选择速度显示
	m_pBoxMetricSelSpeed = CHmsDrawNode::Create();
	m_pBoxMetricSelSpeed->DrawSolidRect(0, 0, m_scaleBoxWidth, 20, Color4F(0.0f, 0.0f, 0.0f, 0.4f));
	auto unit_m = CHmsGlobal::CreateLabel("kn", 10);
	unit_m->SetTextColor(Color4B(0x00, 0xFF, 0xFF, 0xFF));//青色
	unit_m->SetAnchorPoint(Vec2(1, 0.5));
	unit_m->SetPosition(m_scaleBoxWidth - 2, 10);

	Color4B selSpeedTxtColor(0xFF, 0, 0xCC, 0xFF);//洋红色
	m_pLableMetricSelSpeed = CHmsGlobal::CreateLabel("000", 16);
	m_pLableMetricSelSpeed->SetTextColor(selSpeedTxtColor);
	m_pLableMetricSelSpeed->SetAnchorPoint(Vec2(1, 0.5));
	m_pLableMetricSelSpeed->SetPosition(unit_m->GetPoistion() - Vec2(unit_m->GetContentSize().width, 0));

	m_pBoxMetricSelSpeed->AddChild(unit_m);
	m_pBoxMetricSelSpeed->AddChild(m_pLableMetricSelSpeed);
	m_pBoxMetricSelSpeed->SetPosition(Vec2(0, 30));

	m_pSelAirspeedBox->AddChild(m_pBoxMetricSelSpeed);
	//}}

	//刻度尺
	auto scaleBoxNode = CHmsDrawNode::Create();
	auto scaleBoxClipNode = CHmsClippingNode::create();
	scaleBoxNode->SetPosition(0, -m_scaleBoxHeight / 2.0f);
	{
		scaleBoxNode->DrawSolidRect(0, m_scaleBoxHeight / 2.0f, m_scaleBoxWidth, -m_scaleBoxHeight / 2.0f, Color4F(m_boxColor));

		auto stencilNode = CHmsDrawNode::Create();
		stencilNode->DrawSolidRect(0, m_scaleBoxHeight / 2.0f, m_scaleBoxWidth + 40, -m_scaleBoxHeight / 2.0f, Color4F::WHITE);
		scaleBoxClipNode->setStencil(stencilNode);

		//刻度条
		m_pScaleBar = CHmsNode::Create();

		//空速表标识 在SetAirspeedMarking方法中绘制空速表标识
		m_pAirspeedMarking = CHmsDrawNode::Create();
		m_pScaleBar->AddChild(m_pAirspeedMarking);

		m_pScaleLines = CHmsDrawNode::Create();
		//刻度条上刻度线，从最小值位置初始化15条刻度线
		for (int i = 0; i < 15; i++)
		{
			m_pScaleLines->DrawHorizontalLine(m_scaleBoxWidth - m_scaleLineWidth, m_scaleBoxWidth, i * m_scaleGap_px);
		}
		m_pScaleBar->AddChild(m_pScaleLines, -1);

		//初始化10个刻度值
		for (int i = 0; i < 10; i++)
		{
			int value = int(i * 2 * m_speedGap);
			std::string scaleText = CHmsGlobal::FormatString("%d", value);
			auto label = CHmsGlobal::CreateLabel(scaleText.c_str(), 16);
			label->SetAnchorPoint(Vec2(1, 0.5f));
			label->SetTextColor(Color4B::WHITE);
			label->SetPosition(m_scaleBoxWidth - m_scaleLineWidth - 4, i * 2 * m_scaleGap_px);
			m_pScaleBar->AddChild(label);
			m_labelMap[value] = label;
		}

		scaleBoxClipNode->AddChild(m_pScaleBar);
		scaleBoxNode->AddChild(scaleBoxClipNode);
	}

	//选定空速游标
	m_pSelSpeedBug = CHmsImageNode::Create("res/Pfd/Airspeed/bug.png");
	{
		m_pSelSpeedBug->SetAnchorPoint(Vec2(0, 0.5));
		m_pSelSpeedBug->SetPosition(m_scaleBoxWidth - m_scaleLineWidth + 1, -m_scaleBoxHeight / 2.0f);
	}
	scaleBoxClipNode->AddChild(m_pSelSpeedBug);

	//当前空速指针
	auto pointer = CHmsImageNode::Create("res/Pfd/Airspeed/pointer.png");
	pointer->SetPosition(m_scaleBoxWidth - m_scaleLineWidth, -m_scaleBoxHeight / 2.0f);
	pointer->SetScaleX(1.2f);
	auto pointerWidth = pointer->GetContentSize().width;
	{
		auto ptrHeight = 24;// pointer->GetContentSize().height;
		pointer->SetAnchorPoint(Vec2(1, 0.5));

#if 0 /*原代码.*/
		auto pointerClipNode = CHmsClippingNode::create();
		auto pointerStencilNode = CHmsImageNode::Create("res/Pfd/Airspeed/pointerMask.png");
		pointerStencilNode->SetAnchorPoint(Vec2(0, 0));
		pointerStencilNode->SetPosition(2, 2);
		pointerClipNode->setStencil(pointerStencilNode);

		auto numerLap = CHmsImageNode::Create("res/Pfd/Airspeed/NumberLap.png");
		numerLap->SetScale(0.5);
		numerLap->SetAnchorPoint(Vec2(0, 0.445f));
		numerLap->SetPosition(30, 0);

		pointerClipNode->AddChild(numerLap);

		auto label = CHmsGlobal::CreateLabel("00", 22, true);
		label->SetAnchorPoint(Vec2(1, 0.5));
		label->SetTextColor(Color4B::WHITE);
		label->SetPosition(30, ptrHeight);

		pointer->AddChild(pointerClipNode);
		pointer->AddChild(label);

		m_pNumberLap = numerLap;
		m_pNumber2Label = label;
#else
		CHmsRollLabel::BitFontSize fontSize;
		fontSize.push_back(22.0);
		fontSize.push_back(22.0);
		fontSize.push_back(22.0);
		m_pRollLabel = CHmsRollLabel::Create(fontSize, CHmsRollLabel::UC_ONE_02, CHmsRollLabel::RS_ROLL | CHmsRollLabel::RS_BACKGROUND, 0.2);
		m_pRollLabel->SetBackgroundColor(Color4B(0, 0, 0, 0));
		m_pRollLabel->SetAnchorPoint(0.5, 0.5);
		m_pRollLabel->SetPosition(pointer->GetContentSize().width * 0.5 - 5.0, pointer->GetContentSize().height*0.5);
		pointer->AddChild(m_pRollLabel);
#endif

		//{{added by wzh 20190710
		//当前速度米制显示
		m_pBoxMetricCurSpeed = CHmsImageNode::Create("res/Pfd/Airspeed/rectangle.png");
		m_pBoxMetricCurSpeed->SetAnchorPoint(Vec2(1, 0));
		m_pBoxMetricCurSpeed->SetPosition(pointerWidth - 10, pointer->GetContentSize().height - 2);
		auto unit_kmh = CHmsGlobal::CreateLabel("kn", 10);
		unit_kmh->SetTextColor(Color4B(0x00, 0xFF, 0xFF, 0xFF));//青色
		unit_kmh->SetAnchorPoint(Vec2(1, 0.5));
		unit_kmh->SetPosition(m_pBoxMetricCurSpeed->GetContentSize().width - 4, m_pBoxMetricCurSpeed->GetContentSize().height / 2.0f);
		m_pLableMetricCurSpeed = CHmsGlobal::CreateLabel("---", 16);
		m_pLableMetricCurSpeed->SetAnchorPoint(Vec2(1, 0.5));
		m_pLableMetricCurSpeed->SetPosition(unit_kmh->GetPoistion() - Vec2(unit_kmh->GetContentSize().width, 0));
		m_pBoxMetricCurSpeed->AddChild(unit_kmh);
		m_pBoxMetricCurSpeed->AddChild(m_pLableMetricCurSpeed);
		pointer->AddChild(m_pBoxMetricCurSpeed);
		//}}
	}

	//速度趋势
	m_pSpeedVector = CHmsDrawNode::Create();
	{
		m_pSpeedVector->SetPosition(pointerWidth, -m_scaleBoxHeight / 2.0f);
	}

	m_pFailWin = CHmsDrawNode::Create();
	DrawFailWin(m_pFailWin, 0, 0, m_scaleBoxWidth, -m_scaleBoxHeight);
	
	//root->AddChild(selSpeedBox);

	m_rootNode = CHmsNode::Create();
	
	float contentWidth = m_scaleBoxWidth + m_pSelSpeedBug->GetContentSize().width - m_scaleLineWidth + 1;
	float contentHeight = m_scaleBoxHeight + 36;
	this->SetContentSize(Size(contentWidth, contentHeight));
	m_rootNode->SetContentSize(Size(contentWidth, m_scaleBoxHeight));
	

	m_rootNode->AddChild(scaleBoxNode);
	m_rootNode->AddChild(pointer);
	m_rootNode->AddChild(m_pSpeedVector);
	m_rootNode->AddChild(m_pFailWin);
	this->AddChild(m_pSelAirspeedBox);
	this->AddChild(m_rootNode);

	//set the position
	m_pSelAirspeedBox->SetPosition(Vec2(0, contentHeight - 30));
	m_rootNode->SetPosition(0, m_scaleBoxHeight);

	MetricSwitch(false);

	UpdateIAS(0.0);
	UpdateSpeedTrend(0.0);

	this->RegisterAllNodeChild();
	return true;
}

void CHmsAirspeed::Update(float delta)
{
 	CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
	auto aftData = dataBus->GetData();

	UpdateIAS(aftData->groundSpeed*KnotKMH);
#if 0
	AircraftInstrumentData *aircraft = dataBus->GetAircraftInstrumentData();
	this->UpdateIAS(aircraft->indicatedAirspeed);
	this->UpdateTasAndMach(aircraft->tureAirspeed, aircraft->mach);
	this->UpdateGroundSpeed(aircraft->groundSpeed);
	if (aircraft->airspeedLimitations.isChanged)
	{
		RefreshLimitSpeedPos(&aircraft->airspeedLimitations);
		aircraft->airspeedLimitations.isChanged = false;

		CHmsGlobal::SetLabelFormat(m_pVFELabel, "VFE\n% d", int(aircraft->airspeedLimitations.VFE));
	}
	ShowOrHideVFE(aircraft->indicatedAirspeed, aircraft->airspeedLimitations.VFE);
#endif
}

void CHmsAirspeed::UpdateIAS(float fSpdIas)
{
#if 0 /*原代码.*/
	if (fabs(fSpdIas - m_curSpeed) < 0.0001)
		return;

	bool showValue = true;
	if ((fSpdIas) < 5.0)
	{
		showValue = false;
	}

	if (showValue)
	{
		auto lastBit = abs(fmod(fSpdIas, 10));
		if (lastBit > 5)
		{
			lastBit -= 10;
		}
		auto move = 760 / 20.0f * lastBit * 0.5;

		CHmsGlobal::SetLabelFormat(m_pNumber2Label, "%d", std::lround(fSpdIas) / 10);
		if (m_pNumberLap)
		{
			m_pNumberLap->SetPositionY(-move);
		}

		CHmsGlobal::SetLabelFormat(m_pLableMetricCurSpeed, "%d", std::lround((fSpdIas / KnotKMH)));
	}
	else
	{
		CHmsGlobal::SetLabelFormat(m_pLableMetricCurSpeed, "---");
		CHmsGlobal::SetLabelFormat(m_pNumber2Label, "--");
		m_pNumberLap->SetPositionY(-m_pNumberLap->GetContentSize().height * 0.5 / 2.0f + 1);
	}
#else
	m_pRollLabel->SetValue(fSpdIas);

	bool showValue = true;
	if ((fSpdIas) < 5.0)
	{
		showValue = false;
	}
	if (showValue)
	{
		CHmsGlobal::SetLabelFormat(m_pLableMetricCurSpeed, "%d", std::lround((fSpdIas / KnotKMH)));
	}
	else
	{
		CHmsGlobal::SetLabelFormat(m_pLableMetricCurSpeed, "---");
	}
#endif

	ScrollScaleBar(fSpdIas);
	MoveSelectedSpeedBug();

	m_curSpeed = fSpdIas;
}

void CHmsAirspeed::InitAirspeedMarking(std::vector<AirspeedMarking> list)
{
	m_pAirspeedMarking->clear();

	float preMaxY = 0;
	for (auto i = 0u; i < list.size(); i++)
	{
		AirspeedMarking am = list[i];
		float minPosY = am.vMin / m_speedGap * m_scaleGap_px;
		float maxPosY = am.vMax / m_speedGap * m_scaleGap_px;
		if (i == 0)
		{
			preMaxY = maxPosY;
		}
		maxPosY = am.vMax > m_maxSpeed ? m_maxSpeed / m_speedGap * m_scaleGap_px : maxPosY;//标识最大速度大于仪表最大速度时，使用仪表最大速度
		if (preMaxY - maxPosY < 0)
		{
			m_pAirspeedMarking->DrawSolidRect(m_scaleBoxWidth - m_markingWidth / 2.0, minPosY, m_scaleBoxWidth, preMaxY, Color4F(am.color));
			m_pAirspeedMarking->DrawSolidRect(m_scaleBoxWidth - m_markingWidth, preMaxY, m_scaleBoxWidth, maxPosY, Color4F(am.color));
		}
		else
		{
			m_pAirspeedMarking->DrawSolidRect(m_scaleBoxWidth - m_markingWidth, minPosY, m_scaleBoxWidth, maxPosY, Color4F(am.color));
		}
		preMaxY = maxPosY;
	}
}

void CHmsAirspeed::SetSelectedSpeed(float fSpd)
{
	fSpd = std::fmax(0, std::fmin(fSpd, m_maxSpeed));
	CHmsGlobal::SetLabelFormat(m_pLableMetricSelSpeed, "%03d", (int)(fSpd /KnotKMH));
	m_selSpeed = fSpd;

	MoveSelectedSpeedBug();
}

void CHmsAirspeed::SetGaugeMode(GaugeMode flag)
{
	if (flag == m_curGaugeMode) return;

	m_curGaugeMode = flag;
	if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_OK)
	{
		m_rootNode->RemoveChild(m_pFailWin);
		this->SetScheduleUpdate();
	}
	else if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_ERROR)
	{
		m_rootNode->AddChild(m_pFailWin);
		this->SetUnSchedule();
	}
}

void CHmsAirspeed::UpdateSpeedTrend(const float &fSpeedTrend)
{
	m_pSpeedVector->clear();

	float speedDiff = fSpeedTrend - m_curSpeed;
	if (speedDiff != 0.0)
	{
		float unit = m_scaleGap_px / m_speedGap;
		float vectorLength = speedDiff * unit;
		float triHeight = 5;
		std::vector<Vec2> triPnts;
		if (vectorLength < 0.0)
		{
			triHeight = -triHeight;
		}

		triPnts.push_back(Vec2(-4, vectorLength - triHeight));
		triPnts.push_back(Vec2(4, vectorLength - triHeight));
		triPnts.push_back(Vec2(0, vectorLength));
		m_pSpeedVector->DrawVerticalLine(0, 0, vectorLength - triHeight, 2.0f, Color4F(m_vectorColor));
		m_pSpeedVector->DrawTriangles(triPnts, Color4F(m_vectorColor));
	}
}

void CHmsAirspeed::ScrollScaleBar(const float &fSpd)
{
	float scrollPx = -SpeedToPosY(fSpd);
	m_pScaleBar->SetPositionY(scrollPx);

	bool downToUp = fSpd - m_curSpeed < 0;
	RefreshScaleLinePos(downToUp);
	RefreshLabelPos(downToUp, fSpd);
}

void CHmsAirspeed::RefreshScaleLinePos(bool downToUp)
{
	float barY = m_pScaleBar->GetPoistionY();
	float lineY = m_pScaleLines->GetPoistionY();

	float newPosY = 0;
	if (downToUp)
	{
		if (lineY > 0 && barY + lineY > -m_scaleGap_px * 8.0f)
		{
			newPosY = lineY - m_scaleGap_px * 1;
		}
		else
		{
			return;
		}
	}
	else
	{
		if (barY + lineY < -m_scaleGap_px * 8)
		{
			newPosY = lineY + m_scaleGap_px * 1;
		}
		else
		{
			return;
		}
	}

	m_pScaleLines->SetPositionY(newPosY);
	RefreshScaleLinePos(downToUp);
}

void CHmsAirspeed::RefreshLabelPos(bool downToUp, const float &fCurSpeed)
{
	std::map<int, HmsAviPf::CHmsLabel*>::iterator beginItem = m_labelMap.begin();
	std::map<int, HmsAviPf::CHmsLabel*>::iterator endItem = m_labelMap.end();
	endItem--;

	int beginValue = beginItem->first;
	HmsAviPf::CHmsLabel* beginLabel = beginItem->second;

	int endValue = endItem->first;
	HmsAviPf::CHmsLabel* endLabel = endItem->second;

	int removeKey = 0;
	int newKey = 0;
	std::string newScaleStr = "";
	HmsAviPf::CHmsLabel* targetLabel = nullptr;
	float newPosY = 0;
	if (downToUp)
	{
		if (beginValue != 0 && fCurSpeed - endValue < -70)
		{
			removeKey = endValue;
			newKey = removeKey - m_speedGap * 2 * 10;
			newScaleStr = CHmsGlobal::FormatString("%d", newKey);
			newPosY = endLabel->GetPoistionY() - m_scaleGap_px * 2 * 10;
			targetLabel = endLabel;
		}
		else
		{
			return;
		}
	}
	else
	{
		if (endValue != (int)m_maxSpeed && fCurSpeed - beginValue > 70)
		{
			removeKey = beginValue;
			newKey = removeKey + m_speedGap * 2 * 10;
			newScaleStr = CHmsGlobal::FormatString("%d", newKey);
			newPosY = beginLabel->GetPoistionY() + m_scaleGap_px * 2 * 10;
			targetLabel = beginLabel;
		}
		else
		{
			return;
		}
	}

	m_labelMap.erase(removeKey);
	m_labelMap[newKey] = targetLabel;
	targetLabel->SetString(newScaleStr);
	targetLabel->SetPositionY(newPosY);
	RefreshLabelPos(downToUp, fCurSpeed);
}

void CHmsAirspeed::MoveSelectedSpeedBug()
{
	float speedDiff = m_selSpeed - m_curSpeed;
	float unit = m_scaleGap_px / m_speedGap;
	float movePx = speedDiff * unit;

	if (speedDiff < 0 && movePx < -m_scaleBoxHeight / 2.0)
	{
		movePx = -m_scaleBoxHeight / 2.0;
	}
	else if (speedDiff > 0 && movePx > m_scaleBoxHeight / 2.0)
	{
		movePx = m_scaleBoxHeight / 2.0;
	}
	m_pSelSpeedBug->SetPositionY(movePx);
}

float CHmsAirspeed::SpeedToPosY(const float &fSpeed)
{
	float unit = m_scaleGap_px / m_speedGap;
	return fSpeed * unit;
}

void CHmsAirspeed::OnSelectedSpeedChanged(int speed)
{
	if (fabs(m_selSpeed - speed) < 0.1) return;
	SetSelectedSpeed(speed);
}

void CHmsAirspeed::MetricSwitch(bool bOn)
{
	m_pBoxMetricSelSpeed->SetVisible(bOn);
	m_pBoxMetricCurSpeed->SetVisible(bOn);

	this->ForceUpdate();
}
