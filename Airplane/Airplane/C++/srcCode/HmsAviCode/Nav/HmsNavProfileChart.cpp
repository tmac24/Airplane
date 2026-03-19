#include "HmsNavProfileChart.h"

#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "base/HmsActionTimeRef.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsFrame2DRoot.h"

NS_HMS_BEGIN

CHmsNavProfileChart* CHmsNavProfileChart::Create(const Size & sizeShow)
{
	CHmsNavProfileChart *ret = new CHmsNavProfileChart();
	if (ret && ret->Init(sizeShow))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsNavProfileChart::CHmsNavProfileChart()
: HmsUiEventInterface(this)
, m_bIsMoveRefLineBtn(false)
, m_yRatio(0)
, m_lowColorDisToRefLine(1000)
, m_highColorDisToRefLine(100)
, m_xStepLength(0.5)
, m_firstStrikeIndex(-1)
, m_onRefLineChangeCallback(nullptr)
, m_fixWindowX(0)
, m_resetCallback(nullptr)
, m_bNeedUpdateChart(false)
, m_bNeedGetAltDataByAsyn(false)
, m_bDetectRangeY(false)
{

}

CHmsNavProfileChart::~CHmsNavProfileChart()
{
}

bool CHmsNavProfileChart::Init(const Size & sizeShow)
{
	if (!CHmsClippingNode::Init())
	{
		return false;
	}
	SetContentSize(sizeShow);

	m_fixWindowX = sizeShow.width / 2;

	m_nodeMask = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_nodeMask)
		return false;
#endif
	m_nodeMask->DrawSolidRect(Rect(Vec2(), sizeShow));
	this->setStencil(m_nodeMask);

	InitChartLayer(sizeShow);

	InitRefLine(sizeShow);

	SetRefLineAltitude(3000);


	//===========testdata
    auto toolButtomSize = Size(100, 70);
    auto CreateButton = [this, toolButtomSize, sizeShow](int index, const char* str,
        std::function<void(CHmsUiButtonAbstract*, HMS_UI_BUTTON_STATE)> func)
    {
        auto pToolBtn = CHmsFixSizeUiImageButton::CreateWithImage(str, toolButtomSize, func);
#if _NAV_SVS_LOGIC_JUDGE
		if (pToolBtn)
#endif
		{
			pToolBtn->SetAnchorPoint(Vec2(1, 1));
			pToolBtn->SetPosition(Vec2(sizeShow.width - 10, sizeShow.height - (toolButtomSize.height + 10) * index - 30));
			this->AddChild(pToolBtn);
		}
        return pToolBtn;
    };
    int index = 0;
    CHmsFixSizeUiImageButton *pButton = nullptr;
    CreateButton(index++, "res/AirportInfo/zoomIn.png", [=](CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE eState){

        if (eState == HMS_UI_BUTTON_STATE::btn_selected)
        {
            ZoomIn();
        }
    });
    CreateButton(index++, "res/AirportInfo/zoomOut.png", [=](CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE eState){

        if (eState == HMS_UI_BUTTON_STATE::btn_selected)
        {
            ZoomOut();
        }  
    });
	//===========

	SetScheduleUpdate();

	RegisterAllNodeChild();
	return true;
}

void CHmsNavProfileChart::InitChartLayer(const Size & sizeShow)
{
	m_pNodeChartBg = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartBg)
		return;
#endif
	m_pNodeChartBg->SetAnchorPoint(Vec2(0, 0));
	m_pNodeChartBg->SetPosition(0, c_chartBottomLabelHeight);
	m_pNodeChartBg->SetContentSize(Size(sizeShow.width, sizeShow.height - c_chartBottomLabelHeight));

	m_pNodeChart = CHmsDrawNodeChart::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChart)
		return;
#endif
	m_pNodeChart->SetAnchorPoint(Vec2(0, 0));
	m_pNodeChart->SetPosition(0, c_chartBottomLabelHeight);
	m_pNodeChart->SetContentSize(Size(sizeShow.width, sizeShow.height - c_chartBottomLabelHeight));

	m_pNodeChartScaleplate = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartScaleplate)
		return;
#endif
	m_pNodeChartScaleplate->SetAnchorPoint(Vec2(0, 0));
	m_pNodeChartScaleplate->SetPosition(0, 0);
	m_pNodeChartScaleplate->SetContentSize(Size(sizeShow.width, sizeShow.height));

	m_pChartContainer = CHmsNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pChartContainer)
		return;
#endif
	m_pChartContainer->SetAnchorPoint(Vec2(0, 0));
	m_pChartContainer->SetPosition(0, 0);
	m_pChartContainer->SetContentSize(Size(sizeShow.width, sizeShow.height));

	m_pChartContainer->AddChild(m_pNodeChartBg);
	m_pChartContainer->AddChild(m_pNodeChart);
	m_pChartContainer->AddChild(m_pNodeChartScaleplate);
	this->AddChild(m_pChartContainer);
}

void CHmsNavProfileChart::InitRefLine(const Size & sizeShow)
{
	m_pNodeChartRefLine = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartRefLine)
		return;
#endif
	m_pNodeChartRefLine->SetAnchorPoint(Vec2(0, 0.5));
	m_pNodeChartRefLine->SetPosition(0, sizeShow.height / 2);
	m_pNodeChartRefLine->SetContentSize(Size(sizeShow.width, c_refLineBtnHeight));
	m_pNodeChartRefLine->DrawSolidRect(Rect(c_refLineBtnLeftSpace, 0, c_refLineBtnWidth, c_refLineBtnHeight), Color4F(0.0f, 0.0f, 0.0f, 0.7f));
	{
		std::vector<Vec2> vec;
		vec.push_back({ c_refLineBtnLeftSpace, 0 });
		vec.push_back({ c_refLineBtnLeftSpace, c_refLineBtnHeight });
		vec.push_back({ c_refLineBtnLeftSpace + c_refLineBtnWidth, c_refLineBtnHeight });
		vec.push_back({ c_refLineBtnLeftSpace + c_refLineBtnWidth, 0 });
		m_pNodeChartRefLine->DrawLineLoop(vec, 1);
	}
	this->AddChild(m_pNodeChartRefLine);
	{
		std::vector<Vec2> vec;
		for (int i = -100; i < c_refLineBtnLeftSpace / 10; ++i)
		{
			Vec2 temp;
			temp.x = i * 10;
			temp.y = c_refLineBtnHeight / 2;

			Vec2 temp2;
			temp2.x = i * 10 + 7;
			temp2.y = temp.y;

			vec.push_back(temp);
			vec.push_back(temp2);
		}
		for (int i = (c_refLineBtnLeftSpace + c_refLineBtnWidth) / 10; i < sizeShow.width / 10; ++i)
		{
			Vec2 temp;
			temp.x = i * 10;
			temp.y = c_refLineBtnHeight / 2;

			Vec2 temp2;
			temp2.x = i * 10 + 7;
			temp2.y = temp.y;

			vec.push_back(temp);
			vec.push_back(temp2);
		}
		m_pNodeChartRefLine->DrawLines(vec, 1);
		auto label = CHmsGlobal::CreateLabel("None", 26);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return;
#endif
		label->SetAnchorPoint(Vec2(0.5, 0.5));
		label->SetPosition(c_refLineBtnLeftSpace + c_refLineBtnWidth / 2, c_refLineBtnHeight / 2);
		m_pNodeChartRefLine->AddChild(label, 1, REFLINE_TEXT_TAG);
	}
}

bool CHmsNavProfileChart::OnPressed(const Vec2 & posOrigin)
{
	m_time.RecordCurrentTime();
	m_posPressStart = posOrigin;
	
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartRefLine)
		return false;
#endif
	auto rect = m_pNodeChartRefLine->GetRectFromParent();
	rect.origin.x = c_refLineBtnLeftSpace;
	rect.size.width = c_refLineBtnWidth;
	if (rect.containsPoint(posOrigin))
	{
		m_bIsMoveRefLineBtn = true;
		m_posRefLineBtnPressStart = m_pNodeChartRefLine->GetPoistion();

		m_refLineTime.RecordCurrentTime();
		m_fRefLineChangePeriod = 0;
		m_nRefLineChangeStep = 0;

		m_rangeYChangeTime.RecordCurrentTime();
		m_bDetectRangeY = true;
	}
	else
	{
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pChartContainer)
			return false;
#endif
		m_posPanelPressStart = m_pChartContainer->GetPoistion();
	}

	return true;
}

void CHmsNavProfileChart::OnMove(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_posPressStart;
	m_cursorLastMovePos = posOrigin;

	if (m_bIsMoveRefLineBtn)
	{
#if 0
		auto newPos = Vec2(c_refLineBtnWidth, m_posRefLineBtnPressStart.y + delta.y);
		if (newPos.y > (m_pChartContainer->GetContentSize().height - c_refLineBtnHeight / 2))
		{
			newPos.y = m_pChartContainer->GetContentSize().height - c_refLineBtnHeight / 2;
			SetXYLogicRange(m_xLogicRange, Vec2(m_yLogicRange.x, m_yLogicRange.y*1.02));
			UpdateChart();
			ZoomReset();
		}
		else if (newPos.y < c_chartBottomLabelHeight)
		{
			newPos.y = c_chartBottomLabelHeight;
			SetXYLogicRange(m_xLogicRange, Vec2(m_yLogicRange.x, m_yLogicRange.y*0.98));
			UpdateChart();
			ZoomReset();
		}
		m_pNodeChartRefLine->SetPosition(newPos);
		RefLineBtnPosChanged();
#endif
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pNodeChartRefLine)
			return;
#endif
		m_pNodeChartRefLine->SetPositionX(c_refLineBtnWidth);
		//RefLineBtnPosChanged();
		
		UpdateRefLineMoveStep(m_cursorLastMovePos);
	}
	else
	{
		auto newPos = Vec2(m_posPanelPressStart.x + delta.x, m_posPanelPressStart.y);
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pChartContainer)
			return;
#endif
		m_pChartContainer->SetPosition(newPos);
	}
}

void CHmsNavProfileChart::OnReleased(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_posPressStart;
	auto deltaTime = m_time.GetElapsed();
	if (deltaTime < 0.3 && fabsf(delta.x) < 10)
	{
		HmsUiEventInterface::OnPressed(posOrigin);
		HmsUiEventInterface::OnReleased(posOrigin);
	}
	else
	{
		if (m_bIsMoveRefLineBtn)
		{
			m_bIsMoveRefLineBtn = false;

			m_fRefLineChangePeriod = 0;
			m_nRefLineChangeStep = 0;

			m_bDetectRangeY = false;

#if _NAV_SVS_LOGIC_JUDGE
			if (!m_pNodeChartRefLine)
				return;
#endif
			m_pNodeChartRefLine->SetPositionX(0);
		}
		else
		{
			EdgeDetection();
		}	
	}
}

void CHmsNavProfileChart::EdgeDetection()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pChartContainer)
		return;
#endif
	auto itemContentSize = m_pChartContainer->GetContentSize();
	auto itemScale = m_pChartContainer->GetScale();
	float itemHeight = itemContentSize.height * itemScale;
	float itemWidth = itemContentSize.width * itemScale;

	bool bSetPos = false;
	auto pos = m_pChartContainer->GetPoistion();
	auto windowHeight = GetContentSize().height;
	auto windowWidth = GetContentSize().width;
	//X
	auto leftX = pos.x;
	auto rightX = pos.x + itemWidth;
	if (!(leftX > 0 && rightX < windowWidth))
	{
		if (leftX > 0 || rightX < windowWidth)
		{
			if (fabs(rightX - windowWidth) < fabs(leftX))
			{
				pos.x = windowWidth - itemWidth;
				bSetPos = true;
			}
			else
			{
				pos.x = 0;
				bSetPos = true;
			}
		}
	}
	if (bSetPos)
	{
		auto moveto = CHmsActionMoveTo::Create(0.3f, pos);
		m_pChartContainer->RunAction(moveto);
	}
}

void CHmsNavProfileChart::SetAltitudeData(std::vector<AltitudeDataStu> vecAltData, std::vector<NavNodeStu> vecNavNodeName)
{
	if (vecAltData.empty())
	{
		return;
	}
	m_vecAltData = vecAltData;
	m_vecNavNodeName = vecNavNodeName;

	std::sort(vecAltData.begin(), vecAltData.end(), 
		[](const AltitudeDataStu &stuA, const AltitudeDataStu &stuB){
		return stuA.distance < stuB.distance;
	});
	Vec2 xRange = Vec2(vecAltData[0].distance, vecAltData[vecAltData.size() - 1].distance);

	std::sort(vecAltData.begin(), vecAltData.end(),
		[](const AltitudeDataStu &stuA, const AltitudeDataStu &stuB){
		return stuA.altitude < stuB.altitude;
	});
	Vec2 yRange = Vec2(0, vecAltData[vecAltData.size() - 1].altitude);
	m_maxYAltitude = vecAltData[vecAltData.size() - 1].altitude;

	SetXYLogicRange(xRange, yRange);

	UpdateChart();

	RefLineBtnPosChanged();

	ZoomReset();
}

void CHmsNavProfileChart::SetXYLogicRange(Vec2 x, Vec2 y)
{
	m_xLogicRange = x;
	m_yLogicRange = y;
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChart)
		return;
#endif
	m_yRatio = (m_yLogicRange.y - m_yLogicRange.x) / (m_pNodeChart->GetContentSize().height - c_chartTopSpaceHeight);
}

void CHmsNavProfileChart::UpdateChart()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartBg || !m_pNodeChart || !m_pChartContainer)
		return;
#endif
	Size temp = Size(m_vecAltData.size(), this->GetContentSize().height);
	m_pNodeChartBg->SetContentSize(temp - Size(0, c_chartBottomLabelHeight));
	m_pNodeChart->SetContentSize(temp - Size(0, c_chartBottomLabelHeight));
	{
		std::vector<Vec2> vec;
		m_pNodeChart->clear();
		for (int i = 0; i < (int)m_vecAltData.size(); ++i)
		{
			Vec2 temp;
			temp.x = i;
			temp.y = LogicYToRelativePixel(m_vecAltData.at(i).altitude);

			vec.push_back(temp);
			vec.push_back(Vec2(temp.x, 0));
		}
		m_pNodeChart->DrawTriangleStrip(vec);
	}
	{
		std::vector<Vec2> vec;

		for (auto temp : m_vecNavNodeVLines)
		{
            if (temp.pVLineDrawNode)
            {
                temp.pVLineDrawNode->RemoveFromParent();
            }	
		}
		m_vecNavNodeVLines.clear();

		for (auto temp:m_vecNavNodeLabel)
		{
            if (temp.pLabel)
            {
                temp.pLabel->RemoveFromParent();
            }	
		}
		m_vecNavNodeLabel.clear();

        for (int i = 0; i < (int)m_vecNavNodeName.size(); ++i)
		{
			Vec2 temp;
			temp.x = m_vecNavNodeName[i].index;
			temp.y = c_chartBottomLabelHeight;
			
			std::vector<Vec2> vec;
			vec.push_back({ 0, 0});
			vec.push_back({ 0, m_pChartContainer->GetContentSize().height - c_chartBottomLabelHeight });

			auto gridNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
			if (!gridNode)
				return;
#endif
			gridNode->SetAnchorPoint(Vec2(0.5, 0));
			gridNode->SetPosition(temp.x, c_chartBottomLabelHeight);
			gridNode->SetContentSize(Size(1, m_pChartContainer->GetContentSize().height - c_chartBottomLabelHeight));
			gridNode->DrawLines(vec, 1);
			m_pChartContainer->AddChild(gridNode);
            m_vecNavNodeVLines.push_back(NavNodeVLinesStu(gridNode->GetPoistion(), gridNode));

			auto label = CHmsGlobal::CreateLabel(m_vecNavNodeName[i].name.c_str(), 20);
#if _NAV_SVS_LOGIC_JUDGE
			if (!label)
				return;
#endif
			label->SetAnchorPoint(Vec2(0.5, 0.5));
			label->SetPosition(Vec2(temp.x, temp.y/2));
            label->SetMaxLineWidth(115);
            label->SetMaxLineCount(2);
			m_pChartContainer->AddChild(label);
            m_vecNavNodeLabel.push_back(NavNodeLabelStu(label->GetPoistion(), label));
		}
	}
}

void CHmsNavProfileChart::SetLowHighColorAltitude(float lowAlt, float highAlt)
{
	float low = LogicYToWorldPixel(lowAlt);
	float high = LogicYToWorldPixel(highAlt);
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChart)
		return;
#endif
	m_pNodeChart->SetLowHighColorAltitude(low, high);
}

float CHmsNavProfileChart::LogicYToWorldPixel(float logicY)
{
	if (m_yRatio == 0)
	{
		return 0;
	}
	float temp = (logicY - m_yLogicRange.x) / m_yRatio;
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChart)
		return 0.0;
#endif
	Vec2 tempVec = m_pNodeChart->ConvertToWorldSpace(Vec2(0, temp));
	return tempVec.y;
}

float CHmsNavProfileChart::LogicYToRelativePixel(float logicY)
{
	if (m_yRatio == 0)
	{
		return 0;
	}
	return (logicY - m_yLogicRange.x) / m_yRatio;
}

float CHmsNavProfileChart::RelativePixelToLogicY(float pixel)
{
	return pixel * m_yRatio + m_yLogicRange.x;
}

void CHmsNavProfileChart::SetRefLineAltitude(float altitude)
{
	if (altitude > m_yLogicRange.y)
	{
		altitude = m_yLogicRange.y;
	}
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartRefLine)
		return;
#endif
	auto pos = m_pNodeChartRefLine->GetPoistion();
	pos.y = LogicYToRelativePixel(altitude) + c_chartBottomLabelHeight;
	m_pNodeChartRefLine->SetPosition(pos);

	RefLineBtnPosChanged();
}

void CHmsNavProfileChart::RefLineBtnPosChanged()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartRefLine || !m_pChartContainer)
		return;
#endif
	auto temp = m_pNodeChartRefLine->GetChildByTag(REFLINE_TEXT_TAG);
	auto text = dynamic_cast<CHmsLabel*>(temp);
	if (text)
	{
		auto pos = m_pNodeChartRefLine->GetPoistion();
		float logicY = RelativePixelToLogicY(pos.y - c_chartBottomLabelHeight);      
        if (CHmsNavComm::GetInstance()->IsMetric())
        {     
            logicY = FeetToMeter(logicY);
            logicY = (int)(logicY + 0.5) / 100 * 100;
            text->SetString(CHmsGlobal::FormatString("%0.0fm", logicY));
            logicY = MeterToFeet(logicY);
        }
        else
        {
            logicY = (int)(logicY + 0.5) / 100 * 100;
            text->SetString(CHmsGlobal::FormatString("%0.0fft", logicY));
        }

		auto pixelY = LogicYToRelativePixel(logicY) + c_chartBottomLabelHeight;
		pixelY = HMS_MIN(pixelY, m_pChartContainer->GetContentSize().height - c_refLineBtnHeight / 2);
		pixelY = HMS_MAX(pixelY, c_chartBottomLabelHeight);
		pos.y = pixelY;
		m_pNodeChartRefLine->SetPosition(pos);		

		SetLowHighColorAltitude(logicY - m_lowColorDisToRefLine, logicY - m_highColorDisToRefLine);

		FindFirstStrikeIndex(logicY);

		if (m_onRefLineChangeCallback)
		{
			m_onRefLineChangeCallback(m_maxYAltitude, logicY - m_maxYAltitude, m_firstStrikeIndex * m_xStepLength);
		}
	}
}

void CHmsNavProfileChart::FindFirstStrikeIndex(float refLinePos)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartBg)
		return;
#endif
	float highY = refLinePos - m_highColorDisToRefLine;
	bool needClear = true;
	auto s = m_pNodeChartBg->GetContentSize();
	m_firstStrikeIndex = -1;
	for (int i = 0; i < (int)m_vecAltData.size(); ++i)
	{
		const AltitudeDataStu &stu = m_vecAltData.at(i);
		if (stu.altitude > highY)
		{

			m_pNodeChartBg->clear();
			m_pNodeChartBg->DrawSolidRect(Rect(0, 0, i, s.height), Color4F(Color3B(0x1e, 0x3e, 0xad)));
            m_pNodeChartBg->DrawSolidRect(Rect(i, 0, (int)m_vecAltData.size() - 1 - i, s.height), Color4F(Color3B(0x58, 0, 0)));
			needClear = false;
			m_firstStrikeIndex = i;
			break;
		}
	}
	if (needClear)
	{
		m_pNodeChartBg->clear();
		m_pNodeChartBg->DrawSolidRect(Rect(0, 0, s.width, s.height), Color4F(Color3B(0x1e, 0x3e, 0xad)));
	}
}

void CHmsNavProfileChart::ZoomIn(float step /*= 0.2*/)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChart)
		return;
#endif
	float temp = m_pNodeChart->GetScaleX() * (1 + step);
	if (temp > 10)
	{
		return;
	}
	ZoomByScale(temp);
}

void CHmsNavProfileChart::ZoomOut(float step /*= 0.2*/)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChart)
		return;
#endif
	float temp = m_pNodeChart->GetScaleX() * (1 - step);
	float scale = this->GetContentSize().width / (m_vecAltData.size() - 1);
	temp = temp < scale ? scale : temp;
	
	ZoomByScale(temp);
}

void CHmsNavProfileChart::ZoomReset()
{
	float scale = this->GetContentSize().width / (m_vecAltData.size() - 1);

	ZoomByScale(scale);
}

void CHmsNavProfileChart::ZoomByScale(float scale)
{
	if (m_vecAltData.size() < 2)
	{
		return;
	}
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChart || !m_pNodeChartBg || !m_pChartContainer || !m_pNodeChartScaleplate)
		return;
#endif
	float lastScale = m_pNodeChart->GetScaleX();
	float step = scale - lastScale;

	m_pNodeChartBg->SetScaleX(scale);
	m_pNodeChart->SetScaleX(scale);

	Size s;
	s.width = (m_vecAltData.size() - 1) * scale;
	s.height = this->GetContentSize().height;
	m_pChartContainer->SetContentSize(s);
	m_pNodeChartScaleplate->SetContentSize(s);

	for (auto node : m_vecNavNodeVLines)
	{
		auto pos = node.pos;
        if (!node.pVLineDrawNode)
        {
            continue;
        }
		pos.x = pos.x * scale;
		node.pVLineDrawNode->SetPosition(pos);
	}

    Vec2 lastLabelPos;
    Size lastLabelSize;
    for (int i = 0; i < (int)m_vecNavNodeLabel.size(); ++i)
	{
        auto &node = m_vecNavNodeLabel[i];
        if (!node.pLabel)
        {
            continue;
        }
        auto pos = node.pos;
        pos.x = pos.x * scale;
        node.pLabel->SetPosition(pos);
        if (i == 0)
        {
            node.pLabel->SetVisible(true);
            lastLabelPos = pos;
            lastLabelSize = node.pLabel->GetContentSize();
        }
        else
        {
            float dis = lastLabelSize.width * 0.5 + node.pLabel->GetContentSize().width * 0.5 + 30;
            if (fabsf(pos.x - lastLabelPos.x) < dis)
            {
                node.pLabel->SetVisible(false);
            }
            else
            {
                node.pLabel->SetVisible(true);
                lastLabelPos = pos;
                lastLabelSize = node.pLabel->GetContentSize();
            }
        }
	}
	DrawScaleplate();

	//
	auto lastPos = m_pChartContainer->GetPoistion();
	auto newPos = Vec2(lastPos.x - (m_fixWindowX - lastPos.x) / lastScale * step, lastPos.y);
	m_pChartContainer->SetPosition(newPos);

	EdgeDetection();
}

void CHmsNavProfileChart::DrawScaleplate()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartScaleplate || !m_pNodeChart)
		return;
#endif
	m_pNodeChartScaleplate->clear();
	m_pNodeChartScaleplate->RemoveAllChildren();

	float scale = m_pNodeChart->GetScaleX();
	float distance = 0;
	float count = 0;
	while (distance < 50)
	{
		count += 1;
		distance = count * scale;
	}
	auto plateSize = m_pNodeChartScaleplate->GetContentSize();
	Size rectSize = Size(distance, 10);

	std::vector<Vec2> vec;
	for (int i = 0; i < plateSize.width / rectSize.width; i += 2)
	{
		Rect r = Rect(i * rectSize.width, plateSize.height - rectSize.height, rectSize.width, rectSize.height);

		m_pNodeChartScaleplate->DrawSolidRect(r, Color4F::GRAY);

        std::string str;
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            //这里不用单位换算，米制时是0.5km插值，英制时是0.5nm插值
            str = CHmsGlobal::FormatString("%0.0f km", i * count * m_xStepLength);
        }
        else
        {
            str = CHmsGlobal::FormatString("%0.0f nm", i * count * m_xStepLength);
        }
		auto label = CHmsGlobal::CreateLabel(str.c_str(), 20);
#if _NAV_SVS_LOGIC_JUDGE
		if (label)
#endif
		{
			label->SetAnchorPoint(Vec2(0.5, 1));
			label->SetPosition(Vec2(r.origin.x, r.origin.y - 5));
			m_pNodeChartScaleplate->AddChild(label);
		}

	}
	{
		std::vector<Vec2> vec;
		vec.push_back({ 0, plateSize.height });
		vec.push_back({ plateSize.width, plateSize.height });
		m_pNodeChartScaleplate->DrawLines(vec, 1, Color4B::GRAY);
	}
	{
		std::vector<Vec2> vec;
		vec.push_back({ 0, plateSize.height - rectSize.height });
		vec.push_back({ plateSize.width, plateSize.height - rectSize.height });
		m_pNodeChartScaleplate->DrawLines(vec, 1, Color4B::GRAY);
	}
}

void CHmsNavProfileChart::SetCorridorWidth(float width)
{
    //width 是没有单位的，米制时是0.5km，英制时是0.5nm
	m_xStepLength = width;

    int step = m_xStepLength / 0.5;;

    if (step < 1)
    {
        step = 1;
    }

	m_vecAltData.clear();
	{
		int i = 0;
		while (i < (int)m_vecAltDataSource.size())
		{
			m_vecAltData.push_back(m_vecAltDataSource[i]);
			i += step;
		}
	}
	m_vecNavNodeName.clear();
	for (int i = 0; i < (int)m_vecNavNodeNameSource.size(); ++i)
	{
		NavNodeStu stu = m_vecNavNodeNameSource[i];
		stu.index = stu.index / step;
		if (stu.index >= ((int)m_vecAltData.size() - 1)) stu.index = (int)m_vecAltData.size() - 1;
		m_vecNavNodeName.push_back(stu);
	}
	SetAltitudeData(m_vecAltData, m_vecNavNodeName);
}

void CHmsNavProfileChart::SetHazardAltitudes(float high, float low)
{
	m_highColorDisToRefLine = high;
	m_lowColorDisToRefLine = low;
	RefLineBtnPosChanged();
}

void CHmsNavProfileChart::Update(float delta)
{
	if (m_bNeedUpdateChart)
	{
		m_bNeedUpdateChart = false;

		SetAltitudeData(m_vecAltDataSource, m_vecNavNodeNameSource);
		SetRefLineAltitude(3000);

		if (m_resetCallback)
		{
			m_resetCallback();
		}
	}
	if (m_bNeedGetAltDataByAsyn)
	{
		m_bNeedGetAltDataByAsyn = false;
		
		auto server = CHmsGlobal::GetInstance()->GetTerrainServer();
		if (server && m_vecAltDataSource.size() > 0)
		{
			std::vector<AltitudeDataStu> *vAltData = new std::vector<AltitudeDataStu>;
			if (vAltData)
			{
				vAltData->assign(m_vecAltDataSource.begin(), m_vecAltDataSource.end());

				server->GetAltDataByAsyn(vAltData, [=](bool bRet, std::vector<AltitudeDataStu> * vAltData)
				{
					for (int i = 0; i < (int)vAltData->size(); ++i)
					{
						vAltData->at(i).altitude *= 3.2808399f;
					}
					m_vecAltDataSource.assign(vAltData->begin(), vAltData->end());
					delete vAltData;
					m_bNeedUpdateChart = true;
				});
			}
		}
	}

	if (m_bDetectRangeY && m_rangeYChangeTime.GetElapsed() > 0.05)
	{
		m_rangeYChangeTime.RecordCurrentTime();

		if (m_cursorLastMovePos.y > (m_pChartContainer->GetContentSize().height - c_refLineBtnHeight / 2) + 50)
		{
			SetXYLogicRange(m_xLogicRange, Vec2(m_yLogicRange.x, m_yLogicRange.y*1.05));
			UpdateChart();
			ZoomReset();
		}
		else if (m_cursorLastMovePos.y < c_chartBottomLabelHeight - 50)
		{
			SetXYLogicRange(m_xLogicRange, Vec2(m_yLogicRange.x, m_yLogicRange.y*0.95));
			UpdateChart();
			ZoomReset();
		}
	}

	if (m_nRefLineChangeStep != 0)
	{
		auto period = m_refLineTime.GetElapsed();
		if (period > m_fRefLineChangePeriod)
		{
			m_refLineTime.RecordCurrentTime();

			auto pos = m_pNodeChartRefLine->GetPoistion();
			double logicY = RelativePixelToLogicY(pos.y - c_chartBottomLabelHeight);
            if (CHmsNavComm::GetInstance()->IsMetric())
            {
                logicY = FeetToMeter(logicY);
                logicY += m_nRefLineChangeStep;
                logicY = MeterToFeet(logicY);
            }
            else
            {
                logicY += m_nRefLineChangeStep;
            }
			
			SetRefLineAltitude(logicY);

            m_fRefLineChangePeriod = 0;
            m_nRefLineChangeStep = 0;
		}
	}
}

void CHmsNavProfileChart::SetWptNodeData(const std::vector<WptNodeStu2D> &vec)
{
	m_vecAltDataSource.clear();
	m_vecNavNodeNameSource.clear();
	float dis = 0;
	int count = 0;
	for (int i = 1; i < (int)vec.size(); ++i)
	{
		const WptNodeStu2D &lastStu = vec.at(i - 1);
		const WptNodeStu2D &curStu = vec.at(i);
		//0.5nm插值，用于获取高度数据
		std::vector<Vec2> vecLonLat;
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            //0.5km
            CHmsFrame2DRoot::InsertLonLat(Vec2(lastStu.brief.lon, lastStu.brief.lat), Vec2(curStu.brief.lon, curStu.brief.lat), vecLonLat, 0.5);
        }
        else
        {
            //0.5nm
            CHmsFrame2DRoot::InsertLonLat(Vec2(lastStu.brief.lon, lastStu.brief.lat), Vec2(curStu.brief.lon, curStu.brief.lat), vecLonLat, KnotToKMH(0.5));
        }

		int k = 0;
		if (1 == i)
		{
			k = 0;
			m_vecNavNodeNameSource.push_back({ 0, lastStu.brief.ident });
            count += (int)vecLonLat.size() - 1;
			m_vecNavNodeNameSource.push_back({ count, curStu.brief.ident });
		}
		else
		{
			k = 1;
			//第二段开始，起点和上一段终点重复，去掉起点
            count += (int)vecLonLat.size() - 1;
			m_vecNavNodeNameSource.push_back({ count, curStu.brief.ident });
		}
        for (; k < (int)vecLonLat.size(); ++k)
		{
			AltitudeDataStu ads;
			ads.lon = vecLonLat.at(k).x;
			ads.lat = vecLonLat.at(k).y;
			ads.distance = dis;
			dis += 0.5;
			m_vecAltDataSource.push_back(ads);
		}
	}
	m_bNeedGetAltDataByAsyn = true;
}

void CHmsNavProfileChart::UpdateRefLineMoveStep(Vec2 cursorPos)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNodeChartRefLine)
		return;
#endif
	auto delta = cursorPos - m_pNodeChartRefLine->GetPoistion();
	auto moveLen = fabsf(delta.y);
	if (moveLen < 3)
	{
		m_fRefLineChangePeriod = 0;
		m_nRefLineChangeStep = 0;
	}
	else if (moveLen >= 3 && moveLen < 30)
	{
		m_fRefLineChangePeriod = 0.3f;
		m_nRefLineChangeStep = 100;
	}
	else if (moveLen >= 30 && moveLen < 60)
	{
		m_fRefLineChangePeriod = 0.1f;
		m_nRefLineChangeStep = 100;
	}
	else
	{
		m_fRefLineChangePeriod = 0.03f;
		m_nRefLineChangeStep = 200;
	}

	if (delta.y < 0)
	{
		m_nRefLineChangeStep = 0 - m_nRefLineChangeStep;
	}
}


NS_HMS_END


