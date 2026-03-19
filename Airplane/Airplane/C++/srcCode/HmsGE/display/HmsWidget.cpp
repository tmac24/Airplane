#include "HmsWidget.h"
#include "base/HmsEventListenerTouch.h"
#include "../HmsAviDisplayUnit.h"
#include "HmsDrawNode.h"

NS_HMS_BEGIN

CHmsWidget::CHmsWidget()
: onClicked(nullptr)
, m_focus(false)
, m_enabled(true)
, m_borderWidth(2.0)
, m_borderColor(0x00, 0x00, 0x00, 0x00)
, m_backgroundColor(0x00, 0x00, 0x00, 0xff)
{

}

CHmsWidget::~CHmsWidget()
{
}

bool CHmsWidget::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_pBackgroundNode = CHmsDrawNode::Create();
	//this->AddChild(m_pBackgroundNode);

	return true;
}

void CHmsWidget::RegisterTouchEvent()
{
	auto listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = [=](HmsAviPf::Touch * touch, HmsAviPf::CHmsEvent * event)
	{
		//查询当前节点的父节点是否为不可显示状态，如果有一个父节点不可显示，则返回
		//防止在父节点被隐藏的情况下，子节点还接收事件
		CHmsNode *parent = this->GetParent();
		while (parent != nullptr)
		{
			if (!parent->IsVisible())
			{
				return false;
			}

			parent = parent->GetParent();
		}

		auto pos1 = this->ConvertTouchToNodeSpace(touch);
		auto pos = this->ConvertTouchToNodeSpaceAR(touch);
		auto rect = this->GetRectByAnchorPoint();
		if (rect.containsPoint(pos))
		{
			auto & child = this->GetChildren();
			bool ignore = false;
			for (auto c : child)
			{
				TouchInterface * ti = dynamic_cast<TouchInterface*>(c);
				if (ti && c->IsVisible() && c->ContainParentPosition(pos1))
				{
					ti->m_bPressed = true;
					ti->OnPressed(pos);
					ignore = true;
					break;
				}
			}

			if (!ignore && this->IsVisible())
			{
				this->m_bPressed = true;
				this->OnPressed(pos);
			}

			return true;
		}
		return false;
	};

	listener->onTouchMoved = [=](HmsAviPf::Touch * touch, HmsAviPf::CHmsEvent * event)
	{
		auto pos = this->ConvertTouchToNodeSpace(touch);

		auto & child = this->GetChildren();
		for (auto c : child)
		{
			TouchInterface * ti = dynamic_cast<TouchInterface*>(c);
			if (ti && ti->m_bPressed)
			{
				ti->OnMove(pos);
				break;
			}
		}

		if (this->m_bPressed)
		{
			this->OnMove(pos);
		}
	};

	listener->onTouchEnded = [=](HmsAviPf::Touch * touch, HmsAviPf::CHmsEvent * event)
	{
		auto pos = this->ConvertTouchToNodeSpace(touch);

		auto & child = this->GetChildren();
		for (auto c : child)
		{
			TouchInterface * ti = dynamic_cast<TouchInterface*>(c);
			if (ti && ti->m_bPressed)
			{
				ti->m_bPressed = false;
				ti->OnReleased(pos);
				break;
			}
		}

		if (this->m_bPressed)
		{
			this->OnReleased(pos);
		}

	};

	auto m_pDU = HmsAviPf::CHmsAviDisplayUnit::GetInstance();
	auto _eventDispatcher = m_pDU->GetEventDispatcher();

	_eventDispatcher->AddEventListener(listener, this);
}

void CHmsWidget::Focus()
{
	bool oldFocus = m_focus;
	m_focus = true;

	if (oldFocus == false)
	{
		FocusChanged();
	}
}

void CHmsWidget::Blur()
{
	bool oldFocus = m_focus;
	m_focus = false;

	if (oldFocus == true)
	{
		FocusChanged();
	}
	
}

bool CHmsWidget::HasFocus()
{
	return m_focus;
}

void CHmsWidget::SetEnabel(bool enable)
{
	bool oldEnable = m_enabled;
	m_enabled = enable;

	if (oldEnable != enable)
	{
		EnableChanged();
	}
}

void CHmsWidget::SetSize(const Size &size)
{
	this->SetContentSize(size);
}

void CHmsWidget::SetClickedCallback(onClickCallback callback)
{
	onClicked = callback;
}

void CHmsWidget::SetContentSize(const Size & size)
{
	CHmsNode::SetContentSize(size);

	UpdateBackground();
}

void CHmsWidget::UpdateBackground()
{
	m_pBackgroundNode->clear();
	auto width = this->GetContentSize().width;
	auto height = this->GetContentSize().height;
	m_pBackgroundNode->DrawSolidRect(0, 0, width, height, Color4F(m_backgroundColor));
	if (m_borderWidth > 0)
	{
		m_pBackgroundNode->DrawOutterRect(0, 0, width, height, m_borderWidth, Color4F(m_borderColor));
	}
}

void CHmsWidget::SetBackgroundColor(const Color4B &color)
{
	m_backgroundColor = color;
	UpdateBackground();
}

void CHmsWidget::SetBorderWidth(const float &width)
{
	m_borderWidth = width;
	UpdateBackground();
}

void CHmsWidget::SetBorderColor(const Color4B &color)
{
	m_borderColor = color;
	UpdateBackground();
}

NS_HMS_END

