#include "HmsAltimeter.h"
#include "display/HmsDrawNode.h"
#include "base/HmsClippingNode.h"
#include "../Calculate/CalculateZs.h"

USING_NS_HMS;

CHmsAltimeter::CHmsAltimeter()
:scaleBoxHeight_px(318.0f)
, scaleBoxWidth_px(80.0f)
, m_boxColor(0, 0, 0, 60)
, m_vectorColor(0x00, 0xFF, 0x00, 0xFF)
, m_scaleWidth_px(14.0f)
, m_scaleGap_px(38.0f)
, m_minAlt(-1000.0f)
, m_maxAlt(40000.0f)
, m_altGap(100.0f)
, m_minBaro(15.0f)
, m_maxBaro(31.0f)
, m_selAlt(0.0f)
, m_curAlt(-1000.0f)
, m_curBaroUnit(UNIT_INHG)
, m_curBaroRef(REF_STD)
, m_curBarometric(29.92f)
, HmsUiEventInterface(this)
{
	m_dataBus = CHmsGlobal::GetInstance()->GetDataBus();

	BARO_UNIT_STR[0] = "IN";
	BARO_UNIT_STR[1] = "HPA";

	BARO_REF_STR[0] = "STD";
	BARO_REF_STR[1] = "QFE";
	BARO_REF_STR[2] = "QNH";
	m_pRollLabel = nullptr;
}


CHmsAltimeter::~CHmsAltimeter()
{
}

bool CHmsAltimeter::Init()
{
	if (!CHmsGauge::Init())
	{
		return false;
	}

	this->SetAnchorPoint(0.0f, 1.0);

	auto selAltitudeBox = CHmsNumBox::Create(scaleBoxWidth_px, 30, 0, m_maxAlt, m_altGap);
	if (!selAltitudeBox)
	{
		return false;
	}

	selAltitudeBox->SetValueFormat("%04d");
	selAltitudeBox->OnValueChanged = HMS_CALLBACK_1(CHmsAltimeter::OnSelectedAltitudeChanged, this);
	//selAltitudeBox->SetPosition(0, -30);
	{
		Color4B selSpeedTxtColor(0xFF, 0, 0xCC, 0xFF);//洋红色

		m_pBoxMetricSelAlt = CHmsDrawNode::Create();
		m_pBoxMetricSelAlt->DrawSolidRect(0, 0, scaleBoxWidth_px, 20, Color4F(m_boxColor));
		auto unit_m = CHmsGlobal::CreateLabel("ft", 12);
		if (unit_m)
		{
			unit_m->SetTextColor(Color4B(0x00, 0xFF, 0xFF, 0xFF));//青色
			unit_m->SetAnchorPoint(Vec2(1, 0.5));
			unit_m->SetPosition(scaleBoxWidth_px - 1.5, 10);
		}
		

		m_pLableMetricSelAlt = CHmsGlobal::CreateLabel("0000", 16);
		if (m_pLableMetricSelAlt)
		{
			m_pLableMetricSelAlt->SetTextColor(selSpeedTxtColor);
			m_pLableMetricSelAlt->SetAnchorPoint(Vec2(1, 0.5));
			if (unit_m)
			{
				m_pLableMetricSelAlt->SetPosition(unit_m->GetPoistion() - Vec2(unit_m->GetContentSize().width, 0));
				m_pBoxMetricSelAlt->AddChild(unit_m);
			}
			m_pBoxMetricSelAlt->AddChild(m_pLableMetricSelAlt);
		}		
		m_pBoxMetricSelAlt->SetPosition(Vec2(0, 30));
		
		selAltitudeBox->AddChild(m_pBoxMetricSelAlt);
	}

	//刻度尺
	float scaleCount = (m_maxAlt - m_minAlt) / m_altGap;
	auto scaleBoxNode = CHmsDrawNode::Create();
	auto scaleBoxClipNode = CHmsClippingNode::create();
	scaleBoxNode->SetPosition(0, -scaleBoxHeight_px / 2.0f);
	{
		scaleBoxNode->DrawSolidRect(0, scaleBoxHeight_px / 2.0f, scaleBoxWidth_px, -scaleBoxHeight_px / 2.0f, Color4F(m_boxColor));

		auto stencilNode = CHmsDrawNode::Create();
		stencilNode->DrawSolidRect(-9, scaleBoxHeight_px / 2.0f, scaleBoxWidth_px, -scaleBoxHeight_px / 2.0f, Color4F::WHITE);
		scaleBoxClipNode->setStencil(stencilNode);

		//刻度条
		m_pScaleBar = CHmsNode::Create();
		
		m_pScaleLines = CHmsDrawNode::Create();
		//刻度条上刻度线，从最小值位置初始化11条刻度线
		for (int i = 0; i < 11; i++)
		{
			m_pScaleLines->DrawHorizontalLine(0, m_scaleWidth_px, i * m_scaleGap_px);
		}
		m_pScaleBar->AddChild(m_pScaleLines);
		//在刻度尺上，从最小值位置初始化7个刻度值
		for (int i = 0; i < 7; i++)
		{
			int value = int(m_minAlt + i * 2 * m_altGap);
			std::string scaleText = CHmsGlobal::FormatString("%d", value);
			auto label = CHmsGlobal::CreateLabel(scaleText.c_str(), 16);
			if (label)
			{
				label->SetAnchorPoint(Vec2(0, 0.5f));
				label->SetTextColor(Color4B::WHITE);
				label->SetPosition(m_scaleWidth_px + 2, i * 2 * m_scaleGap_px);
				m_pScaleBar->AddChild(label);
				m_labelMap[value] = label;
			}
		}

		scaleBoxClipNode->AddChild(m_pScaleBar);

		scaleBoxNode->AddChild(scaleBoxClipNode);
	}

	//选定空速游标
	auto bug = CHmsImageNode::Create("res/Pfd/Altimeter/bug.png");
	if (bug)
	{
		float bugWidth = bug->GetContentSize().width;
		{
			bug->SetAnchorPoint(Vec2(0, 0.5));
			bug->SetPosition(-9, 0);//-scaleBoxHeight_px / 2.0f - 37

			m_pSelAltBug = bug;
		}
		scaleBoxClipNode->AddChild(bug);
	}
	

	
	//当前空速指针
	auto pointer = CHmsImageNode::Create("res/Pfd/Altimeter/pointer.png");
	if (!pointer)
	{
		return false;
	}
	pointer->SetPosition(m_scaleWidth_px-6, -scaleBoxHeight_px / 2.0f - 1);
	{
		//万位占位符
		m_pPlaceholder = CHmsImageNode::Create("res/Pfd/Altimeter/placeholder.png");
		if (!m_pPlaceholder)
		{
			return false;
		}

		auto ptrHeight = pointer->GetContentSize().height / 2.0;
		pointer->SetAnchorPoint(Vec2(0, 0.5));
		auto pointerWidth = pointer->GetContentSize().width;
		auto pointerHeight = pointer->GetContentSize().height;
#if 0
		auto pointerClipNode = CHmsClippingNode::create();
		auto pointerStencilNode = CHmsImageNode::Create("res/Pfd/Altimeter/pointerMask.png");
		pointerStencilNode->SetAnchorPoint(Vec2(0, 0));
		pointerStencilNode->SetPosition(3.2f, 1.78f);
		pointerClipNode->setStencil(pointerStencilNode);

		m_pNumberLap = CHmsImageNode::Create("res/Pfd/Altimeter/Number2Lap.png");
		m_pNumberLap->SetScale(0.4f);
		m_pNumberLap->SetAnchorPoint(Vec2(0, 0.382f));
		m_pNumberLap->SetPosition(52, 0);

		pointerClipNode->AddChild(m_pNumberLap);

		m_pPointerHundred = CHmsLabel::createWithTTF("0", "fonts/Amble-Bold.ttf", 16);
		m_pPointerHundred->SetAnchorPoint(Vec2(1, 0.5));
		m_pPointerHundred->SetTextColor(Color4B::WHITE);
		m_pPointerHundred->SetPosition(51, ptrHeight);

		m_pPointerThousand = CHmsLabel::createWithTTF("30", "fonts/Amble-Bold.ttf", 24);
		m_pPointerThousand->SetAnchorPoint(Vec2(1, 0.5));
		m_pPointerThousand->SetTextColor(Color4B::WHITE);
		m_pPointerThousand->SetPosition(m_pPointerHundred->GetPoistion() + Vec2(-m_pPointerHundred->GetContentSize().width, 0));

		m_pPlaceholder->SetAnchorPoint(Vec2(0, 0.5f));
		m_pPlaceholder->SetPosition(14, ptrHeight);

		pointer->AddChild(pointerClipNode);
		pointer->AddChild(m_pPointerHundred);
		pointer->AddChild(m_pPointerThousand);
		pointer->AddChild(m_pPlaceholder);

#else
		CHmsRollLabel::BitFontSize fontSize;
		fontSize.push_back(22.0);
		fontSize.push_back(22.0);
		fontSize.push_back(22.0);
		fontSize.push_back(22.0);
		m_pRollLabel = CHmsRollLabel::Create(fontSize, CHmsRollLabel::UC_TWO_20, CHmsRollLabel::RS_ROLL | CHmsRollLabel::RS_SYMBOL_PLACE | CHmsRollLabel::RS_BACKGROUND, 0.5);
		m_pRollLabel->SetBackgroundColor(Color4B(0, 0, 0, 0));
		m_pRollLabel->SetAnchorPoint(1.0, 0.5);
		m_pRollLabel->SetPosition(pointer->GetContentSize().width-2.0, pointer->GetContentSize().height*0.5);
		pointer->AddChild(m_pRollLabel);
#endif

		m_pBoxMetricCurAlt = CHmsImageNode::Create("res/Pfd/Altimeter/rectangle.png");
		if (m_pBoxMetricCurAlt)
		{
			m_pBoxMetricCurAlt->SetAnchorPoint(Vec2(1, 0));
			m_pBoxMetricCurAlt->SetPosition(pointerWidth, pointerHeight);
			auto unit_m = CHmsGlobal::CreateLabel("ft", 12);
			unit_m->SetTextColor(Color4B(0x00, 0xFF, 0xFF, 0xFF));
			unit_m->SetAnchorPoint(Vec2(1, 0.5));
			unit_m->SetPosition(m_pBoxMetricCurAlt->GetContentSize().width - 7, m_pBoxMetricCurAlt->GetContentSize().height / 2.0f);
			m_pLableMetricCurAlt = CHmsGlobal::CreateLabel("0000", 16);
			m_pLableMetricCurAlt->SetAnchorPoint(Vec2(1, 0.5));
			m_pLableMetricCurAlt->SetPosition(unit_m->GetPoistion() - Vec2(unit_m->GetContentSize().width, 0));
			m_pBoxMetricCurAlt->AddChild(unit_m);
			m_pBoxMetricCurAlt->AddChild(m_pLableMetricCurAlt);
			pointer->AddChild(m_pBoxMetricCurAlt);
		}		
	}

	//高度趋势矢量线
	m_pAltVector = CHmsDrawNode::Create();
	m_pAltVector->SetPosition(m_scaleWidth_px - 6, -scaleBoxHeight_px / 2.0f - 1);

	m_pFailWin = CHmsDrawNode::Create();
	DrawFailWin(m_pFailWin, 0, 0, scaleBoxWidth_px, -scaleBoxHeight_px);

	m_nodeRoot = CHmsNode::Create();

	float contentWidth = 9 + pointer->GetPoistionX() + pointer->GetContentSize().width;
	float contentHeight = scaleBoxHeight_px + 36;
	this->SetContentSize(Size(contentWidth, contentHeight));
	m_nodeRoot->SetContentSize(Size(contentWidth, scaleBoxHeight_px));

	m_nodeRoot->AddChild(scaleBoxNode);
	m_nodeRoot->AddChild(pointer);
	m_nodeRoot->AddChild(m_pAltVector);
	m_nodeRoot->AddChild(m_pFailWin);
	this->AddChild(m_nodeRoot);
	this->AddChild(selAltitudeBox);

	//set the position
	selAltitudeBox->SetPosition(Vec2(0, contentHeight - 30));
	m_nodeRoot->SetPosition(0, scaleBoxHeight_px);

	
	//SetSelectedAlt(0);
	UpdateAltitude(0.0);
	MetricSwitch(false);

	this->RegisterAllNodeChild();
	return true;
}

#include "DataInterface/HmsDataBus.h"
void CHmsAltimeter::Update(float delta)
{
	CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
	auto aftData = dataBus->GetData();
	UpdateAltitude(dataBus->GetMslAltitudeMeter());

#if 0
	AircraftInstrumentData *aircraft = m_dataBus->GetAircraftInstrumentData();
	UpdateAltitude(aircraft->indicatedAltitude);
	UpdateAltTrend(aircraft->altitudeTrend);
	SetBarometric(aircraft->baroSet);
	SetBaroRef((BARO_REF)aircraft->baroStandard);
#endif
}

void CHmsAltimeter::ScrollScaleBar(const float &fFeetAlt)
{
	float oldAlt = m_curAlt;
	float scrollAltCount = fFeetAlt - oldAlt;
	if (scrollAltCount == 0) return;

	float unit = m_scaleGap_px / m_altGap;
	float scrollPx = -scrollAltCount * unit;
	float posY = m_pScaleBar->GetPoistionY() + scrollPx;
	m_pScaleBar->SetPositionY(posY);

	RefreshScaleLinePos(fFeetAlt - oldAlt < 0);
	RefreshLabelPos(fFeetAlt - oldAlt < 0, fFeetAlt);
	m_curAlt = fFeetAlt;
	MoveSelectedAltBug();
}

void CHmsAltimeter::RefreshScaleLinePos(bool downToUp)
{
	float barY = m_pScaleBar->GetPoistionY();
	float lineY = m_pScaleLines->GetPoistionY();
	
	float newPosY = 0;
	if (downToUp)
	{
		if (lineY > 0 && barY + lineY > -m_scaleGap_px * 5.0f)
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
		if (barY + lineY < -m_scaleGap_px * 5)
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

void CHmsAltimeter::RefreshLabelPos(bool downToUp, const float &fCurAlt)
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
		if (beginValue != (int)m_minAlt && fCurAlt - endValue < -700)
		{
			removeKey = endValue;
			newKey = removeKey - m_altGap * 2 * 7;
			newScaleStr = CHmsGlobal::FormatString("%d", newKey);
			newPosY = endLabel->GetPoistionY() - m_scaleGap_px * 2 * 7;
			targetLabel = endLabel;
		}
		else
		{
			return;
		}
	}
	else
	{
		if (fCurAlt - beginValue > 700)
		{
			removeKey = beginValue;
			newKey = removeKey + m_altGap * 2 * 7;
			newScaleStr = CHmsGlobal::FormatString("%d", newKey);
			newPosY = beginLabel->GetPoistionY() + m_scaleGap_px * 2 * 7;
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
	RefreshLabelPos(downToUp, fCurAlt);
}

void CHmsAltimeter::UpdateAltitude(const float &fMetricAlt)
{
#if 0
	m_pPointerThousand->SetVisible(fMetricAlt >= 0);
	m_pPlaceholder->SetVisible(fMetricAlt < 10000.0);

	auto pos = fmod(fMetricAlt, 100);
	auto offset = (pos > 50) ? pos - 100 : pos;

	offset = offset * 380 / 200 * 0.4;
	if (m_pNumberLap)
	{
		m_pNumberLap->SetPositionY(-offset);
	}

	auto husand = (int)fmod(fMetricAlt / 100, 10);
	auto thousend = (int)(fMetricAlt / 1000);
	
	CHmsGlobal::SetLabelFormat(m_pPointerHundred, "%d", husand);
	CHmsGlobal::SetLabelFormat(m_pPointerThousand, "%d", thousend);
#else
	m_pRollLabel->SetValue(fMetricAlt);
#endif

	ScrollScaleBar(fMetricAlt);

	if (m_pBoxMetricCurAlt->IsVisible())
	{
		CHmsGlobal::SetLabelFormat(m_pLableMetricCurAlt, "%04d", (int)MeterToFeet(fMetricAlt));
	}
}

void CHmsAltimeter::MetricSwitch(bool bOn)
{
	m_pBoxMetricSelAlt->SetVisible(bOn);
	m_pBoxMetricCurAlt->SetVisible(bOn);

	this->ForceUpdate();
}

void CHmsAltimeter::SetSelectedAlt(const float &fAlt)
{

	CHmsGlobal::SetLabelFormat(m_pLableMetricSelAlt, "%04d", int(MeterToFeet(fAlt)));
	m_selAlt = fAlt;

	MoveSelectedAltBug();
}

float CHmsAltimeter::GetSelectedAlt()
{
	return m_selAlt;
}

void CHmsAltimeter::MoveSelectedAltBug()
{
	float altDiff = m_selAlt - m_curAlt;
	float unit = m_scaleGap_px / m_altGap;
	float movePx = altDiff * unit;

	if (altDiff < 0 && movePx < -scaleBoxHeight_px / 2.0)
	{
		movePx = -scaleBoxHeight_px / 2.0;
	}
	else if (altDiff > 0 && movePx > scaleBoxHeight_px / 2.0)
	{
		movePx = scaleBoxHeight_px / 2.0;
	}
	m_pSelAltBug->SetPositionY(movePx);
}

void CHmsAltimeter::UpdateAltTrend(const float &fAltTrend)
{
	m_pAltVector->clear();

	float speedDiff = fAltTrend - m_curAlt;
	if (speedDiff != 0.0)
	{
		float unit = m_scaleGap_px / m_altGap;
		float vectorLength = speedDiff * unit;//速度趋势线长度
		float triHeight = 5;//箭头长度
		if (vectorLength < 0.0 && (vectorLength - triHeight) < -scaleBoxHeight_px / 2.0f)
		{
			vectorLength = -scaleBoxHeight_px / 2.0f + triHeight;
		}
		else if (vectorLength > 0.0 && (vectorLength + triHeight) > scaleBoxHeight_px / 2.0f)
		{
			vectorLength = scaleBoxHeight_px / 2.0f - triHeight;
		}

		if (vectorLength < 0.0)
		{
			triHeight = -triHeight;
		}

		std::vector<Vec2> triPnts;
		triPnts.push_back(Vec2(-4, vectorLength - triHeight));
		triPnts.push_back(Vec2(4, vectorLength - triHeight));
		triPnts.push_back(Vec2(0, vectorLength));
		m_pAltVector->DrawVerticalLine(0, 0, vectorLength - triHeight, 2.0f, Color4F(m_vectorColor));
		m_pAltVector->DrawTriangles(triPnts, Color4F(m_vectorColor));
	}
}

float CHmsAltimeter::GetBarometric()
{
	return m_curBarometric;
}

float CHmsAltimeter::InHgToHpa(const float &finHg)
{
	return finHg * 3386 / 100.0f;
}

void CHmsAltimeter::SetGaugeMode(GaugeMode flag)
{
	if (flag == m_curGaugeMode) return;

	m_curGaugeMode = flag;
	if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_OK)
	{
		m_nodeRoot->RemoveChild(m_pFailWin);
		this->SetScheduleUpdate();
	}
	else if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_ERROR)
	{
		m_nodeRoot->AddChild(m_pFailWin);
		this->SetUnSchedule();
	}
}

void CHmsAltimeter::OnSelectedAltitudeChanged(int altitude)
{
	if (fabs(m_selAlt - altitude) < 0.1) return;
	SetSelectedAlt(altitude);
}
