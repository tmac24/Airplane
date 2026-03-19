#include "HmsHSIBox.h"
USING_NS_HMS;

CHmsHSIBox::CHmsHSIBox()
:HmsUiEventInterface(this)
, m_boxWidth(70)
, m_boxHeight(26)
, m_boxColor(0, 0, 0, 60)
, m_defaultColor(Color4B::WHITE)
, m_slectedColor(0x00, 0xFF, 0xFF, 0xFF)
, m_value(0)
, m_strTitle("---")
, m_selected(false)
, OnValueChanged(nullptr)
{
}


CHmsHSIBox::~CHmsHSIBox()
{
}

bool CHmsHSIBox::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

    m_pBoxNode = CHmsDrawNode::Create();
	m_pBoxNode->SetPosition(0, 0);

	auto title = CHmsGlobal::CreateLabel(m_strTitle.c_str(), 12, true);
	if (title)
	{
		title->SetAnchorPoint(Vec2(0, 0.5));
		title->SetPosition(-m_boxWidth / 2.0 + 2, m_boxHeight / 2.0);		
		m_pBoxNode->AddChild(title);
	}
	m_pTitle = title;

	auto value = CHmsGlobal::CreateLabel("000", 14);
	if (value)
	{
		value->SetAnchorPoint(Vec2(1, 0.5));
		value->SetPosition(m_boxWidth / 2.0 - 10, m_boxHeight / 2.0);
		m_pBoxNode->AddChild(value);
	}
	m_pValue = value;

	auto unit = CHmsGlobal::CreateLabel("o", 10);
	if (unit)
	{
		unit->SetAnchorPoint(Vec2(1, 1));
		unit->SetPosition(m_boxWidth / 2.0 - 3.0, m_boxHeight);
		m_pBoxNode->AddChild(unit);
	}

	

	auto minusBox = CHmsDrawNode::Create();
	minusBox->SetPosition(-m_boxWidth / 2.0 - 6, 0);
	minusBox->DrawInnerRect(-m_boxHeight, m_boxHeight, 0, 0, 1.0f, Color4F(m_slectedColor));
	minusBox->DrawSolidRect(-m_boxHeight, m_boxHeight, 0, 0, Color4F::BLACK);
    minusBox->DrawHorizontalLine( - m_boxHeight / 4.0, - m_boxHeight *3 / 4.0, m_boxHeight / 2.0);

	auto plusBox = CHmsDrawNode::Create();
	plusBox->SetPosition(m_boxWidth / 2.0 + 6, 0);
	plusBox->DrawInnerRect(0, m_boxHeight, m_boxHeight, 0, 1.0f, Color4F(m_slectedColor));
	plusBox->DrawSolidRect(0, m_boxHeight, m_boxHeight, 0, Color4F::BLACK);
	plusBox->DrawHorizontalLine(m_boxHeight / 4.0, m_boxHeight * 3 / 4.0, m_boxHeight / 2.0);
	plusBox->DrawVerticalLine(m_boxHeight / 2.0, m_boxHeight / 4.0, m_boxHeight * 3 / 4.0);

	m_pMinusNode = minusBox;
	m_pPlusNode = plusBox;

	m_pBoxNode->AddChild(minusBox);
	m_pBoxNode->AddChild(plusBox);

	this->AddChild(m_pBoxNode);

	this->SetContentSize(Size(m_boxWidth + (6 + m_boxHeight) * 2, m_boxHeight));
	SelectedBox(false);

	return true;
}

void CHmsHSIBox::SetValue(const int &iValue)
{
	m_value = iValue;
	if (iValue > 360)
	{
		m_value = iValue - 360;
	}
	else if (iValue < 0)
	{
		m_value = iValue + 360;
	}

	if (m_pValue)
	{
		std::string strVal = CHmsGlobal::FormatString("%03d", m_value);
		m_pValue->SetString(strVal);
	}
}

int CHmsHSIBox::GetValue()
{
	return m_value;
}

void CHmsHSIBox::SetTitle(const std::string &title)
{
	m_strTitle = title;

	if (m_pTitle)
	{
		m_pTitle->SetString(m_strTitle);
	}
}

void CHmsHSIBox::SelectedBox(bool sel)
{
	if (sel)
	{
		m_pBoxNode->clear();
		m_pBoxNode->DrawInnerRect(-m_boxWidth / 2.0, m_boxHeight, m_boxWidth / 2.0, 0, 1.0f, Color4F(m_slectedColor));
		//m_pBoxNode->DrawSolidRect(-m_boxWidth / 2.0, m_boxHeight, m_boxWidth / 2.0, 0, Color4F::BLACK);
		//m_pBoxNode->DrawInnerRoundRect(-m_boxWidth / 2.0, m_boxHeight, m_boxWidth / 2.0, 0, 1.0f, 1.0, 10, Color4F(m_slectedColor));
		//m_pBoxNode->DrawRoundRect(-m_boxWidth / 2.0, m_boxHeight, m_boxWidth / 2.0, 0, 6.0f, 10, Color4F::BLACK);		
	}
	else
	{
		m_pBoxNode->clear();
		m_pBoxNode->DrawInnerRect(-m_boxWidth / 2.0, m_boxHeight, m_boxWidth / 2.0, 0, 1.0f, Color4F(m_defaultColor));
		//m_pBoxNode->DrawSolidRect(-m_boxWidth / 2.0, m_boxHeight, m_boxWidth / 2.0, 0, Color4F(m_boxColor));
		//m_pBoxNode->DrawInnerRoundRect(-m_boxWidth / 2.0, m_boxHeight, m_boxWidth / 2.0, 0, 1.0f, 1.0f, 10, Color4F(m_defaultColor));
		//m_pBoxNode->DrawRoundRect(-m_boxWidth / 2.0, m_boxHeight, m_boxWidth / 2.0, 0, 6.0f, 10, Color4F(m_boxColor));
	}
	m_pMinusNode->SetVisible(sel);
	m_pPlusNode->SetVisible(sel);
	m_selected = sel;
}

bool CHmsHSIBox::OnPressed(const Vec2 & posOrigin)
{
	return true;
}

void CHmsHSIBox::OnReleased(const Vec2 & posOrigin)
{
	Rect boxRect(-m_boxWidth*0.5f, 0, m_boxWidth, m_boxHeight);
	Rect minusRect(-m_sizeContent.width*0.5f, 0, m_boxHeight, m_boxHeight);
	Rect plusRect(m_sizeContent.width*0.5f - m_boxHeight, 0, m_boxHeight, m_boxHeight);
	if (boxRect.containsPoint(posOrigin))//选中box
	{
		SelectedBox(!m_selected);
	}

	if (m_selected)
	{
		if (minusRect.containsPoint(posOrigin))
		{
			SetValue(m_value - 1);
			if (OnValueChanged)
			{
				OnValueChanged(m_value);
			}
		}
		else if (plusRect.containsPoint(posOrigin))
		{
			SetValue(m_value + 1);
			if (OnValueChanged)
			{
				OnValueChanged(m_value);
			}
		}
	}	
}

HmsAviPf::Rect CHmsHSIBox::GetRectByOrigin()
{
	float scaleWidth = m_sizeContent.width*m_scaleX;
	float scaleHeight = m_sizeContent.height*m_scaleY;
	Rect out(-scaleWidth*0.5f, 0, scaleWidth, scaleHeight);
	return out;
}

