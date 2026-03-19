#include "HmsTabWidget.h"
#include "base/HmsEventListenerTouch.h"
#include "../HmsAviDisplayUnit.h"

NS_HMS_BEGIN


CHmsTabWidget::CHmsTabWidget()
:m_tabPosition(West)
, m_containerMargin(6,6,6,6)
{
}

CHmsTabWidget::~CHmsTabWidget()
{
}

bool CHmsTabWidget::Init()
{
	if (!CHmsWidget::Init())
	{
		return false;
	}

	m_pTabbar = CHmsTabBar::Create();

	m_pContainer = CHmsDrawNode::Create();
	this->AddChild(m_pContainer);
	this->AddChild(m_pTabbar);
	
	return true;
}

void CHmsTabWidget::SetSize(const Size &size)
{
	CHmsWidget::SetSize(size);
	ContentLayout();
}

void CHmsTabWidget::SetTabPosition(TabPosistion tabpos)
{
	m_tabPosition = tabpos;
	ContentLayout();
}

Size CHmsTabWidget::GetContainerSize()
{
	return m_pContainer->GetContentSize();
}

void CHmsTabWidget::SetContainerMargin(Margin margin)
{
	m_containerMargin = margin;
	AjustContainerWidgets();
}

int CHmsTabWidget::AddTab(CHmsWidget *widget, const std::string &label)
{
	CHmsTabButton *button = m_pTabbar->AddTabButton(label);
	button->SetRelatedWidget(widget);

	widget->SetVisible(false);
	AjustWidget(widget);
	this->m_pContainer->AddChild(widget);
	return 0;
}

bool CHmsTabWidget::RemoveTab(int index)
{
	return m_pTabbar->RemoveTabButton(index);
}

void CHmsTabWidget::ContentLayout()
{
	Size size = this->GetContentSize();

	float tabbarWidth = 0.0;
	float tabbarHeight = 0.0;
	float containerWidth = 0.0;
	float containerHeight = 0.0;
	if (m_tabPosition == West || m_tabPosition == East)
	{
		tabbarWidth = 60;
		tabbarHeight = size.height;
		containerWidth = size.width - tabbarWidth;
		containerHeight = size.height;
		m_pTabbar->SetOrientation(CHmsTabBar::Vertical);
	}

	if (m_tabPosition == North || m_tabPosition == South)
	{
		tabbarWidth = size.width;
		tabbarHeight = 60;
		containerWidth = size.width;
		containerHeight = size.height - tabbarHeight;
		m_pTabbar->SetOrientation(CHmsTabBar::Horizontal);
	}

	m_pTabbar->SetSize(Size(tabbarWidth, tabbarHeight));
	m_pContainer->SetContentSize(Size(containerWidth, containerHeight));

	m_pContainer->clear();
	m_pContainer->DrawSolidRect(0, 0, containerWidth, containerHeight, Color4F::BLACK);

	switch (m_tabPosition)
	{
	case HmsAviPf::CHmsTabWidget::North:
		m_pTabbar->SetPosition(0, containerHeight - tabbarHeight);
		m_pContainer->SetPosition(0, 0);
		break;
	case HmsAviPf::CHmsTabWidget::South:
		m_pTabbar->SetPosition(0, -tabbarHeight);
		m_pContainer->SetPosition(0, tabbarHeight);
		break;
	case HmsAviPf::CHmsTabWidget::West:
		m_pTabbar->SetPosition(0, 0);
		m_pContainer->SetPosition(tabbarWidth, 0);
		break;
	case HmsAviPf::CHmsTabWidget::East:
		m_pTabbar->SetPosition(containerWidth, 0);
		m_pContainer->SetPosition(0, 0);
		break;
	default:
		break;
	}
}

void CHmsTabWidget::AjustContainerWidgets()
{
	for (CHmsNode *node : m_pContainer->GetChildren())
	{
		CHmsWidget *widget = dynamic_cast<CHmsWidget*>(node);
		AjustWidget(widget);
	}
}

void CHmsTabWidget::AjustWidget(CHmsWidget *widget)
{
	float width = this->GetContainerSize().width - m_containerMargin.left - m_containerMargin.right;
	float height = this->GetContainerSize().height - m_containerMargin.top - m_containerMargin.bottom;
	widget->SetSize(Size(width, height));
	widget->SetPosition(m_containerMargin.left, m_containerMargin.bottom);
}

int CHmsTabWidget::CurrentIndex()
{
	return m_pTabbar->CurrentIndex();
}

void CHmsTabWidget::SetCurrentIndex(int index)
{
	m_pTabbar->SetCurrentIndex(index);
}


NS_HMS_END

