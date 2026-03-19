#include "HmsVSlideBar.h"
#include "HmsVSlideBar.h"
#include "HmsGlobal.h"
#include "math/HmsMath.h"

CHmsVSlideBar::CHmsVSlideBar()
: HmsUiEventInterface(this)
{
   
}

CHmsVSlideBar::~CHmsVSlideBar()
{

}

bool CHmsVSlideBar::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(size);

    auto pDrawNode = CHmsDrawNode::Create();
    pDrawNode->SetContentSize(Size(50, size.height - 100));
    pDrawNode->DrawSolidRect(Rect(0, -50, 50, size.height), Color4F(0.0,0.0,0.0,0.25));
    pDrawNode->SetAnchorPoint(Vec2(0, 0.5));
    pDrawNode->SetPosition(0, size.height * 0.5);

	pDrawNode->DrawSolidRect(Rect(50/2-8, 0-2, 4, size.height - 100 + 2*2), Color4F(1.0f, 1.0f, 1.0f, 0.25f));
	auto fBgHeight = size.height - 100;
	auto nBgHeightInc = int(fBgHeight / 10);
	for (int i = 0; i <= 10; i++)
	{
		pDrawNode->DrawSolidRect(Rect(50 / 2 + 2, i* nBgHeightInc - 2, 18, 4), Color4F(1.0f, 1.0f, 1.0f, 0.75f));
	}
    this->AddChild(pDrawNode);

    m_pImageNode = CHmsImageNode::Create("res/display/slidebar.png");
    m_pImageNode->SetAnchorPoint(Vec2(0, 0.5));
    m_pImageNode->SetPosition(Vec2(25, 50));
    this->AddChild(m_pImageNode);

	m_pLabelCurData = CHmsGlobal::CreateLabel("000", 28);
	m_pLabelCurData->SetAnchorPoint(0.5f, 0.5f);
	auto imageSize = m_pImageNode->GetContentSize();
	m_pLabelCurData->SetPosition(imageSize.width*(0.25 + 0.75*0.5), imageSize.height*0.5);
	m_pImageNode->AddChild(m_pLabelCurData);

    m_slideYMin = 50;
    m_slideYMax = size.height - 50;

    m_minValue = 0;
    m_maxValue = 0;
    m_curValue = 0;

    return true;
}

bool CHmsVSlideBar::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
    auto rect = m_pImageNode->GetRectFromParent();
    rect.origin.x -= 30;
    rect.origin.y -= 30;
    rect.size.width += 60;
    rect.size.height += 60;
    if (rect.containsPoint(posOrigin))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CHmsVSlideBar::OnMove(const HmsAviPf::Vec2 & posOrigin)
{
    auto pos = posOrigin;
    pos.y = HMS_BETWEEN_VALUE(pos.y, m_slideYMin, m_slideYMax);

    m_pImageNode->SetPositionY(pos.y);

    UpdateValueFromUI();
}

void CHmsVSlideBar::OnReleased(const HmsAviPf::Vec2 & posOrigin)
{

}

void CHmsVSlideBar::SetValueRange(float min, float max)
{
    m_minValue = min;
    m_maxValue = max;
}

float CHmsVSlideBar::GetCurValue()
{
    return m_curValue;
}

void CHmsVSlideBar::SetCurValue(float value)
{
    m_curValue = value;

    m_curValue = HMS_BETWEEN_VALUE(m_curValue, m_minValue, m_maxValue);

	CHmsGlobal::SetLabelFormat(m_pLabelCurData, "%.0f", value);

    UpdateValueToUI();
}

void CHmsVSlideBar::UpdateValueToUI()
{
    if (m_maxValue - m_minValue > 1)
    {
        float posY = (m_slideYMax - m_slideYMin) / (m_maxValue - m_minValue) * m_curValue;
        posY += m_slideYMin;

        m_pImageNode->SetPositionY(posY);
    }
}

void CHmsVSlideBar::UpdateValueFromUI()
{
    if (m_slideYMax - m_slideYMin > 1)
    {
        float slideY = m_pImageNode->GetPoistionY();
        slideY = slideY - m_slideYMin;

        m_curValue = (m_maxValue - m_minValue) / (m_slideYMax - m_slideYMin) * slideY;

        SetCurValue(m_curValue);
    }
}
