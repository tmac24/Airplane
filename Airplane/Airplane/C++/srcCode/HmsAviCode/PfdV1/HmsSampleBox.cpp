#include "HmsSampleBox.h"
USING_NS_HMS;

#define ADD_WIDTH 8
#define ADD_HEIGHT 20

CHmsSampleBox::CHmsSampleBox()
: HmsUiEventInterface(this)
, m_unitBoxWidth(44 + ADD_WIDTH)
, m_unitBoxHeight(26 + ADD_HEIGHT)
, m_defBorderColor(Color4B::WHITE)
, m_selBorderColor(Color4B::GREEN)
, m_isSelected(false)
, m_isToggle(false)
{
}


CHmsSampleBox::~CHmsSampleBox()
{
}

void CHmsSampleBox::SetText(const std::string &text)
{
	m_pNodeLabel->SetString(text);
}

void CHmsSampleBox::SetToggle(const bool &bToggle)
{
	m_isToggle = bToggle;
}

bool CHmsSampleBox::Init()
{
	if (!CHmsDrawNode::Init())
	{
		return false;
	}

	UpdateBackground();
	this->SetContentSize(Size(m_unitBoxWidth, m_unitBoxHeight));

	m_pNodeLabel = CHmsGlobal::CreateLabel("", 12, true);
	if (m_pNodeLabel)
	{
		m_pNodeLabel->SetPosition(m_sizeContent.width / 2.0f, m_sizeContent.height / 2.0f);

		this->AddChild(m_pNodeLabel);
	}

	return true;
}

bool CHmsSampleBox::OnPressed(const Vec2 & posOrigin)
{
	if (!m_pNodeLabel)
	{
		return false;
	}
	if (m_isToggle)
	{
		m_isSelected = !m_isSelected;
		Color4B color = m_isSelected ? m_selBorderColor : m_defBorderColor;
		m_pNodeLabel->SetTextColor(color);
	}
	else
	{
		m_pNodeLabel->SetTextColor(m_selBorderColor); 
	}
	return true;
}

void CHmsSampleBox::OnReleased(const Vec2 & posOrigin)
{
	if (!m_pNodeLabel)
	{
		return ;
	}
	//UpdateBackground();
	if (!m_isToggle)
	{
		m_pNodeLabel->SetTextColor(m_defBorderColor);
	}
	if (OnSelectedChanged)
	{
		OnSelectedChanged(m_isSelected);
	}
}

void CHmsSampleBox::UpdateBackground()
{
	Color4B color = m_isSelected ? m_selBorderColor : m_defBorderColor;
	this->clear();
#if 0
	this->DrawSolidRect(0, 0, m_unitBoxWidth, m_unitBoxHeight, Color4F::BLACK);
	this->DrawInnerRect(0, 0, m_unitBoxWidth, m_unitBoxHeight, 2.0f, Color4F(color));
#else
	this->DrawSolidRect(ADD_WIDTH / 2.0f, ADD_HEIGHT / 2.0f, m_unitBoxWidth - ADD_WIDTH / 2.0f, m_unitBoxHeight-ADD_HEIGHT / 2.0f, Color4F::BLACK);
	this->DrawInnerRect(ADD_WIDTH / 2.0f, ADD_HEIGHT / 2.0f, m_unitBoxWidth - ADD_WIDTH / 2.0f, m_unitBoxHeight-ADD_HEIGHT / 2.0f, 2.0f, Color4F(color));
#endif
}
