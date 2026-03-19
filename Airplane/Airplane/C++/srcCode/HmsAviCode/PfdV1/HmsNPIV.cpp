#include "HmsNPIV.h"
USING_NS_HMS;

CHmsNPIV::CHmsNPIV()
: m_boxWidth(24)
, m_boxHeight(168)
, m_scaleGap_px(66)
, m_boxColor(0x00, 0x00, 0x00, 0x3C)
, m_scaleColor(Color4B::WHITE)
, m_pointerColor(0xFF, 0x00, 0xCB, 0xFF)
, m_minRange(-0.8f)
, m_maxRange(0.8f)
, m_unitDeviationPx(0.0f)
, m_curDeviation(0.0f)
{
    m_unitDeviationPx = (m_scaleGap_px * 2) / (m_maxRange - m_minRange);
}


CHmsNPIV::~CHmsNPIV()
{
}

bool CHmsNPIV::Init()
{
    if (!CHmsNode::Init())
    {
        return false;
    }

    m_pNodeBox = CHmsDrawNode::Create();
    m_pNodeBox->SetPosition(0, 0);
    m_pNodeBox->DrawSolidRect(-m_boxWidth / 2.0, m_boxHeight / 2.0, m_boxWidth / 2.0, -m_boxHeight / 2.0, Color4F(m_boxColor));

    m_pNodeScale = CHmsDrawNode::Create();
    m_pNodeScale->DrawHorizontalLine(-m_boxWidth / 2.0 + 2, 0, 0, 2.0, Color4F(m_scaleColor));//中间
    m_pNodeScale->DrawHorizontalLine(-m_boxWidth / 2.0 + 2, 0, m_scaleGap_px, 2.0, Color4F(m_scaleColor));//上
    m_pNodeScale->DrawHorizontalLine(-m_boxWidth / 2.0 + 2, 0, -m_scaleGap_px, 2.0, Color4F(m_scaleColor));//下
    m_pNodeScale->DrawSolidRect(-m_boxWidth / 2.0 + 2, m_scaleGap_px + 8, -6, m_scaleGap_px - 18, Color4F(m_scaleColor));//上矩形
    m_pNodeScale->DrawSolidRect(-m_boxWidth / 2.0 + 2, -m_scaleGap_px - 8, -6, -m_scaleGap_px + 18, Color4F(m_scaleColor));//下矩形

    m_pNodePointer = CHmsDrawNode::Create();
    std::vector<HmsAviPf::Vec2> triPnts;
    triPnts.push_back(Vec2(-m_boxWidth / 2.0 + 5, 0));
    triPnts.push_back(Vec2(m_boxWidth / 2.0 - 5, -7));
    triPnts.push_back(Vec2(m_boxWidth / 2.0 - 5, 7));
    m_pNodePointer->DrawTriangles(triPnts, Color4F(m_pointerColor));

    m_pNodeBox->AddChild(m_pNodeScale);
    m_pNodeBox->AddChild(m_pNodePointer);

    this->AddChild(m_pNodeBox);

    return true;
}

void CHmsNPIV::SetRange(float fMin, float fMax)
{
    m_minRange = fMin;
    m_maxRange = fMax;

    m_unitDeviationPx = (m_scaleGap_px * 2) / (m_maxRange - m_minRange);
}

void CHmsNPIV::SetDeviation(float fDev)
{
    float temp = m_curDeviation;
    m_curDeviation = fDev;

    if (std::fabs(temp - fDev) > 0.001)
    {
        UpdatePointer();
    }
}

void CHmsNPIV::UpdatePointer()
{
    float moveY = m_unitDeviationPx * m_curDeviation;
    m_pNodePointer->SetPositionY(moveY);
}