#include "HmsProfileChart.h"
#include "../Calculate/CalculateZs.h"
#include "HmsChartAxis.h"
#include "HmsProfileGraph.h"
#include "HmsGE/HmsConfigXMLReader.h"


USING_NS_HMS;

#define WARNING_COLOR_STYLE_INDEX 0

CHmsProfileChart::CHmsProfileChart()
{
    m_borderColor = Color4F::BLACK;
    m_bgColor = Color4F::WHITE;
    m_bLeftBorder = false;
    m_bRightBorder = false;
    m_bTopBorder = false;
    m_bBottomBorder = false;
    m_nSubdivisionCnt = 300;
    m_fMaxHeight = 6000.0f;
}

CHmsProfileChart* CHmsProfileChart::Create(float width, float height)
{
    CHmsProfileChart *pRet = new CHmsProfileChart();
    if (pRet && pRet->Init(width, height))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

const float c_xAxisHeight = 50;
const float c_yAxisWidth = 80;

bool CHmsProfileChart::Init(float width, float height)
{
    if (!CHmsNode::Init())
    {
        return false;
    }

    this->SetContentSize(Size(width, height));

    m_sizeContent.setSize(width - c_yAxisWidth, height - c_xAxisHeight);

    xAxis = CHmsChartAxis::Create(width - c_yAxisWidth, c_xAxisHeight, CHmsChartAxis::AtBottom);
    xAxis->SetRange(-2.5, 22.5);
    xAxis->SetTickStep(5);
    xAxis->SetPosition(c_yAxisWidth, 0);
    yAxis = CHmsChartAxis::Create(c_yAxisWidth, height - c_xAxisHeight, CHmsChartAxis::AtLeft);
    yAxis->SetNumberPrecision(0);
    yAxis->SetRange(0, 6000);
    yAxis->SetTickStep(1000);
    yAxis->SetPosition(0, c_xAxisHeight);

    this->AddChild(xAxis);
    this->AddChild(yAxis);

    graph = CHmsProfileGraph::Create();
    graph->SetPosition(c_yAxisWidth, c_xAxisHeight);
    graph->SetShowSize(Size(width - c_yAxisWidth, height - c_xAxisHeight));
    this->AddChild(graph);

    m_pGraphAftLevel = CHmsDrawNode::Create();
    {
        m_pGraphAftLevel->SetPosition(c_yAxisWidth + m_sizeContent.width / 10, c_xAxisHeight);
        Vec2 posStart(0, 0);
        auto tempAftTriangle = CHmsImageNode::Create("res/Terrain/aftTriangle.png");
        if (tempAftTriangle)
        {
            tempAftTriangle->SetAnchorPoint(Vec2(1.0f, 0.0f));
        }
        m_pGraphAftLevel->DrawDashLine(posStart, 90, m_sizeContent.width * 9 / 10, 10, 10, 2.0f, Color4F(237 / 255.0f, 237 / 255.0f, 237 / 255.0f, 1.0f));
		if (tempAftTriangle)
		{
			m_pGraphAftLevel->AddChild(tempAftTriangle);
		}
    }
    this->AddChild(m_pGraphAftLevel);

    m_pGraphBorder = CHmsDrawNode::Create();
    graph->AddChild(m_pGraphBorder);
    UpdateBorder();

    m_nodeAxisXUnit = CHmsGlobal::CreateLabel("KM", 26);
	if (m_nodeAxisXUnit)
	{
		m_nodeAxisXUnit->SetAnchorPoint(Vec2(1.0, 1.0));
		m_nodeAxisXUnit->SetPosition(Vec2(width, c_xAxisHeight - 8));
		this->AddChild(m_nodeAxisXUnit);
	}
    m_nodeAxisYUnit = CHmsGlobal::CreateLabel("M", 26);
	if (m_nodeAxisYUnit)
	{
		m_nodeAxisYUnit->SetAnchorPoint(Vec2(1.0, 1.0));
		m_nodeAxisYUnit->SetPosition(Vec2(c_yAxisWidth - 10, height + 6));
		this->AddChild(m_nodeAxisYUnit);
	}

    //this->SetScheduleUpdate();
    auto config = CHmsConfigXMLReader::GetSingleConfig();

    if (!config->GetTawsColorStyleInfo("VerticalProfile_WarningColorConfig/Style", m_warningColorStylePtrs))
    {
        //read failed and set the warning color
        //warning color style init
        //style 0
        std::vector<float> alts0{ -30, -300 };
        std::vector<HmsAviPf::Vec4> colors0{
            HmsAviPf::Vec4(230.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f),
            HmsAviPf::Vec4(230.0f / 255.0f, 230.0f / 255.0f, 0.0f / 255.0f, 1.0f),
            HmsAviPf::Vec4(0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f)
        };
        auto pst0 = warningColorStyle::create(alts0, colors0);//style 0
        if (pst0)
        {
            m_warningColorStylePtrs.push_back(pst0);
        }
    }

    return true;
}

void CHmsProfileChart::Update(float delta)
{

}

void CHmsProfileChart::Resize(float width, float height)
{
    this->SetContentSize(Size(width, height));
    UpdateBorder();
}

void CHmsProfileChart::SetRange(float fRangeShow)
{
    if (xAxis)
    {
        float fRangeTemp = fRangeShow*1.25;
        xAxis->SetRange(-fRangeTemp*0.1, fRangeTemp*0.9f);
        xAxis->SetTickStep(fRangeShow / 2.0f);
    }
}

void CHmsProfileChart::SetBorderColor(const HmsAviPf::Color4B &color)
{
    m_borderColor = Color4F(color);
    UpdateBorder();
}

void CHmsProfileChart::SetBorder(bool left, bool bottom, bool right, bool top)
{
    m_bLeftBorder = left;
    m_bRightBorder = right;
    m_bTopBorder = top;
    m_bBottomBorder = bottom;
    UpdateBorder();
}

void CHmsProfileChart::SetData(std::vector<HmsAviPf::Vec2> & data, float fHightestTerrain)
{
    if (data.size() == 0) return;

    auto fMaxHeight = fmax(fHightestTerrain, m_fCurHeight) * 1.3f;
    if (abs(fMaxHeight - m_fMaxHeight) > 10)
    {
        m_fMaxHeight = ceilf(fMaxHeight / 1000.0f) * 1000;
        if (m_fMaxHeight < 1000)
        {
            m_fMaxHeight = 1000;
        }
        if (yAxis)
        {
            yAxis->SetRange(0, m_fMaxHeight);

            auto fSegment = m_fMaxHeight / 5;
            if (fSegment > 1000)
                fSegment = ceilf(fSegment / 1000.0f) * 1000;
            else if (fSegment > 100)
                fSegment = ceilf(fSegment / 100.0f) * 100;
            else if (fSegment > 10)
                fSegment = ceilf(fSegment / 10.0f) * 10;
            else
                fSegment = 1000;

            yAxis->SetTickStep(fSegment);
        }
    }

    std::vector<Vec2> vec;
	for (std::vector<HmsAviPf::Vec2>::size_type i = 0; i < data.size(); i++)
    {
        Vec2 temp;
        temp.x = data[i].x * xAxis->GetContentSize().width / m_nSubdivisionCnt;
        temp.y = data[i].y * yAxis->GetContentSize().height / m_fMaxHeight;

        vec.push_back(temp);
        vec.push_back(Vec2(temp.x, 0));
    }
    graph->clear();
    graph->DrawTriangleStrip(vec);
}

void CHmsProfileChart::SetDataClear()
{
    graph->clear();
}

float CHmsProfileChart::LogicYToWorldPixel(float logicY)
{
    Vec2 tempVec = graph->ConvertToWorldSpace(Vec2(0, logicY));
    return tempVec.y;
}

void CHmsProfileChart::SetAftAltMeter(float fAltMeter)
{
    m_fCurHeight = fAltMeter;

    auto showHeight = yAxis->GetContentSize().height;
    if (graph)
    {
        if (m_warningColorStylePtrs.size() > 0)
        {
            auto patcnt = m_warningColorStylePtrs[WARNING_COLOR_STYLE_INDEX]->ThresholdCnt();
            auto patts = m_warningColorStylePtrs[WARNING_COLOR_STYLE_INDEX]->AltThresholds();
            auto pclrs = m_warningColorStylePtrs[WARNING_COLOR_STYLE_INDEX]->Colors();

            if (patts && pclrs)
            {
                m_altThresholdLevelsPixel.clear();
                m_warningColors.clear();

                for (auto& altThreshold : *patts)
                {
                    auto hPixel = LogicYToWorldPixel((fAltMeter + altThreshold) * showHeight / m_fMaxHeight);
                    m_altThresholdLevelsPixel.push_back(hPixel);
                }
                m_warningColors = *pclrs;

                graph->SetLowHighColorAltitude(m_altThresholdLevelsPixel, m_warningColors);
            }
        }
    }

    if (m_pGraphAftLevel)
    {
        m_pGraphAftLevel->SetPositionY(fAltMeter * showHeight / m_fMaxHeight + c_xAxisHeight);
    }
}

void CHmsProfileChart::UpdateBorder()
{
    m_pGraphBorder->clear();
    Size size = m_sizeContent;
    if (m_bLeftBorder)
    {
        m_pGraphBorder->DrawVerticalLine(0, 0, size.height, 2.0f, m_borderColor);
    }
    if (m_bRightBorder)
    {
        m_pGraphBorder->DrawVerticalLine(size.width, 0, size.height, 2.0f, m_borderColor);
    }
    if (m_bTopBorder)
    {
        m_pGraphBorder->DrawHorizontalLine(-1, size.width + 1, size.height, 2.0f, m_borderColor);
    }
    if (m_bBottomBorder)
    {
        m_pGraphBorder->DrawHorizontalLine(-1, size.width + 1, 0, 2.0f, m_borderColor);
    }
}