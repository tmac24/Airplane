#include "HmsUiVerticalScrollView.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "base/HmsActionTimeRef.h"

NS_HMS_BEGIN

#define HMS_UI_BTN_ICON_ID				0x86
#define HMS_UI_BTN_STATE_LABEL_ID		0x87
#define HMS_UI_WINDOW_ITEM				0xF000

CHmsUiVerticalScrollView* CHmsUiVerticalScrollView::Create(const Size & sizeShow)
{
	CHmsUiVerticalScrollView *ret = new CHmsUiVerticalScrollView();
	if (ret && ret->Init(sizeShow))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsUiVerticalScrollView::CHmsUiVerticalScrollView()
	: HmsUiEventInterface(this)
	, m_fPanelContentHeight(0)
	, m_fMinimumShowHeight(-100)
	, m_fItemSpace(20.0f)
{

}

CHmsUiVerticalScrollView::~CHmsUiVerticalScrollView()
{
}

bool CHmsUiVerticalScrollView::Init(const Size & sizeShow)
{
	if (!CHmsClippingNode::Init())
	{
		return false;
	}
	SetContentSize(sizeShow);

	m_nodeMask = CHmsDrawNode::Create();
	m_nodeMask->DrawSolidRect(Rect(Vec2(), sizeShow));
	this->setStencil(m_nodeMask);
	//this->setInverted(true);

	m_nodePanel = CHmsUiPanel::Create(sizeShow);
	this->AddChild(m_nodePanel);

	m_nodePanel->SetPosition(0, sizeShow.height);

	RegisterAllNodeChild();
	return true;
}

void CHmsUiVerticalScrollView::SetWindowShowRect(const Rect & rect)
{
	if (m_nodeMask)
	{
		m_nodeMask->clear();
		m_nodeMask->DrawSolidRect(rect);
	}
}

void CHmsUiVerticalScrollView::SetWindowShowRectWithBoder(const Rect & rect, float fBoderLeft, float fBoderRight, float fBoderUp, float fBoderDown)
{
	if (m_nodeMask)
	{
		m_nodeMask->clear();
		auto tempRect = rect;
		tempRect.origin.x -= fBoderLeft;
		tempRect.origin.y -= fBoderDown;
		tempRect.size.width += fBoderLeft + fBoderRight;
		tempRect.size.height += fBoderUp + fBoderDown;
		m_nodeMask->DrawSolidRect(tempRect);
	}
}

CHmsUiPanel * CHmsUiVerticalScrollView::GetUiPanel()
{
	return m_nodePanel;
}

void CHmsUiVerticalScrollView::AddWindowItem(CHmsNode * node)
{
	if (m_nodePanel && node)
	{
		node->SetTag(HMS_UI_WINDOW_ITEM);
		node->SetPosition(0, m_fPanelContentHeight);
		m_nodePanel->AddChild(node);

		auto uiInterface = dynamic_cast<HmsUiEventInterface *>(node);
		if (uiInterface)
		{
			m_nodePanel->AddChildInterface(uiInterface);
		}

		m_fPanelContentHeight -= node->GetContentSize().height;
		m_fPanelContentHeight -= m_fItemSpace;

		auto size = m_nodePanel->GetContentSize();
		size.height = m_fPanelContentHeight;
		m_nodePanel->SetContentSize(size);
	}
}

void  CHmsUiVerticalScrollView::RemoveAllItem()
{
	if (m_nodePanel)
	{
		m_fPanelContentHeight = 0;
		m_nodePanel->RemoveAllChildren();
		m_nodePanel->RemoveAllChildInterface();
	}
}

void CHmsUiVerticalScrollView::RegisterAllChildUiEvent()
{
	if (m_nodePanel)
	{
		m_nodePanel->RemoveAllChildInterface();
		m_nodePanel->RegisterAllNodeChild();
	}
// 	RemoveAllChildInterface();
// 	RegisterAllNodeChild();
}

bool CHmsUiVerticalScrollView::OnPressed(const Vec2 & posOrigin)
{
	m_time.RecordCurrentTime();
	m_posPressStart = posOrigin;
	m_posPanelPressStart = m_nodePanel->GetPoistion();
	return true;
	//return HmsUiEventInterface::OnPressed(posOrigin);
}

void CHmsUiVerticalScrollView::OnMove(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_posPressStart;
	auto newPos = m_posPanelPressStart + Vec2(0, delta.y);
	m_nodePanel->SetPosition(newPos);

	//HmsUiEventInterface::OnMove(posOrigin);
}

void CHmsUiVerticalScrollView::OnReleased(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_posPressStart;
	auto deltaTime = m_time.GetElapsed();
	if (deltaTime < 2 && fabsf(delta.y) < 10)
	{
		HmsUiEventInterface::OnPressed(posOrigin);
		HmsUiEventInterface::OnReleased(posOrigin);
	}
	else
	{
		bool bSetPos = false;
		auto pos = m_nodePanel->GetPoistion();
		auto fHeightWindow = GetContentSize().height;
		if (pos.y < fHeightWindow)
		{
			pos.y = fHeightWindow;
			bSetPos = true;
		}
		else
		{
			auto fShowHeight = pos.y - fHeightWindow + m_fPanelContentHeight;		//显示的高度
			if (fShowHeight > m_fMinimumShowHeight)
			{
				pos.y = m_fMinimumShowHeight + fHeightWindow - m_fPanelContentHeight;
				bSetPos = true;
			}
		}
		
		if (bSetPos)
		{
#if 1
			auto moveto = CHmsActionMoveTo::Create(0.5f, pos);
			m_nodePanel->RunAction(moveto);
#else
			m_nodePanel->SetPosition(pos);
#endif
		}
	}
}

void CHmsUiVerticalScrollView::SetMinimumShowHeight(float fMinimunShowHeight)
{
	m_fMinimumShowHeight = -fMinimunShowHeight;
}

HmsAviPf::Size CHmsUiVerticalScrollView::GetPanelSize()
{
	auto size = GetContentSize();
	size.height = -m_fPanelContentHeight;
	return size;
}

void CHmsUiVerticalScrollView::CheckAndSetIndexWasShow(int nIndex)
{
	auto child = m_nodePanel->GetChildren();
	if (nIndex >= 0 && nIndex < child.size())
	{
		auto c = child.at(nIndex);
		CheckAndSetItemWasShow(c);
	}
}

void CHmsUiVerticalScrollView::CheckAndSetItemWasShow(CHmsNode * nodeItem)
{
	if (nodeItem)
	{
		bool bSetPos = false;
		auto posPanel = m_nodePanel->GetPoistion();
		auto posItem = nodeItem->GetPoistion();
		auto fItemHeight = nodeItem->GetContentSize().height;
		auto fWindowHeight = GetContentSize().height;

		auto fItemShowTop = -(posPanel.y - fWindowHeight + posItem.y);
		auto fItemShowBottom = -(posPanel.y - fWindowHeight + posItem.y - fItemHeight);


		if (fItemShowTop < 0)
		{
			bSetPos = true;
			posPanel.y = fWindowHeight - posItem.y;
		}
		else if (fItemShowBottom > fWindowHeight*0.75)
		{
			bSetPos = true;
			posPanel.y = -posItem.y + fItemHeight + 10 + fWindowHeight*0.25;
		}

		if (bSetPos)
		{
			auto moveto = CHmsActionMoveTo::Create(0.3f, posPanel);
			m_nodePanel->RunAction(moveto);
		}
	}
	
}

void CHmsUiVerticalScrollView::ScrollToTop()
{
	auto pos = GetUiTopLeft();
	m_nodePanel->SetPosition(pos);
}

NS_HMS_END


