#include "HmsScreenLayer.h"
#include "base/HmsActionTimeRef.h"
USING_NS_HMS;

CHmsScreenLayer::CHmsScreenLayer()
	: m_bCurrentStride(false)
{
	m_timeChangeMenu.RecordCurrentTime();
	m_nScreenChangeBack = 0;
}


CHmsScreenLayer::~CHmsScreenLayer()
{
}

bool CHmsScreenLayer::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_nodeRoot = CHmsNode::Create();
	this->AddChild(m_nodeRoot.get());

	m_nodeContent = CHmsNode::Create();
	m_nodeRoot->AddChild(m_nodeContent.get());

	return true;
}

void CHmsScreenLayer::SetRightScreenMenu(CHmsScreenLayout * node)
{
	node->SetLayoutType(Hms_Screen_Layout_Type::Layout_Menu);
	node->SetPosition(CHmsGlobal::GetDesignContentWidthNormal(), CHmsGlobal::GetDesignContentHeight());
	node->SetLocalZOrder(40);

	AddScreenLayoutToMenu(node);
}

void CHmsScreenLayer::SetLeftScreenMenu(CHmsScreenLayout * node)
{
	node->SetLayoutType(Hms_Screen_Layout_Type::Layout_Menu);
	node->SetPosition(0, CHmsGlobal::GetDesignContentHeight());
	node->SetLocalZOrder(40);

	AddScreenLayoutToMenu(node);
}

void CHmsScreenLayer::SetFullScreenMenu(CHmsScreenLayout * node)
{
	node->SetLayoutType(Hms_Screen_Layout_Type::Layout_Menu_Full);
	node->SetPosition(0, CHmsGlobal::GetDesignContentHeight());
	node->SetLocalZOrder(40);

	AddScreenLayoutToMenu(node);
}

void CHmsScreenLayer::SetScreenSmall(CHmsScreenLayout * node, bool bLeft, int nIndex)
{
	node->SetLayoutType(Hms_Screen_Layout_Type::Layout_Small);
	auto size = node->GetSize();
	if (bLeft)
	{
		node->SetPosition(size.width * nIndex, 0);
	}
	else
	{
		node->SetPosition(CHmsGlobal::GetDesignContentWidthNormal() + size.width * nIndex, 0);
	}
	node->SetLocalZOrder(30);

	ScreenChangeOver();

	AddScreenLayoutToScene(node);
}

void CHmsScreenLayer::SetScreenMiddle(CHmsScreenLayout * node, bool bLeft, int nIndex)
{
	node->SetLayoutType(Hms_Screen_Layout_Type::Layout_Middle);
	auto size = node->GetSize();
	if (bLeft)
	{
		node->SetPosition(size.width * nIndex, 0);
	}
	else
	{
		node->SetPosition(CHmsGlobal::GetDesignContentWidthNormal() + size.width * nIndex, 0);
	}
	node->SetLocalZOrder(20);

	ScreenChangeOver();

	AddScreenLayoutToScene(node);
}

void CHmsScreenLayer::SetScreenNormal(CHmsScreenLayout * node, bool bLeft)
{
	node->SetLayoutType(Hms_Screen_Layout_Type::Layout_Normal);
	auto size = node->GetSize();
	if (bLeft)
	{
		node->SetPosition(0, 0);
	}
	else
	{
		node->SetPosition(CHmsGlobal::GetDesignContentWidthNormal(), 0);
	}
	node->SetLocalZOrder(10);

	ScreenChangeOver();

	AddScreenLayoutToScene(node);
}

void CHmsScreenLayer::SetScreenFull(CHmsScreenLayout * node)
{
    node->SetLayoutType(Hms_Screen_Layout_Type::Layout_Full);
    node->SetPosition(0, 0);
    node->SetLocalZOrder(10);

    ScreenChangeOver();

    AddScreenLayoutToScene(node);
}

void CHmsScreenLayer::SetScreenNormalMid(CHmsScreenLayout * node, bool bLeft)
{
	node->SetLayoutType(Hms_Screen_Layout_Type::Layout_Normal);
	auto size = node->GetSize();

	node->SetPosition(CHmsGlobal::GetDesignContentWidthHalf(), 0);
	node->SetLocalZOrder(10);

	ScreenChangeOver();

	AddScreenLayoutToScene(node);
}

void CHmsScreenLayer::SetScreenStride(CHmsScreenLayout * node, int nIndex)
{
	node->SetLayoutType(Hms_Screen_Layout_Type::Layout_Large);
	auto size = node->GetScreenLayoutSize(Hms_Screen_Layout_Type::Layout_Middle);
	node->SetPosition(size.width * nIndex, 0);
	node->SetLocalZOrder(0);

	AddScreenLayoutToScene(node);

	ScreenChangeOver();

	m_bCurrentStride = true;
}

void CHmsScreenLayer::AddScreenLayoutToScene(CHmsScreenLayout * node)
{
	m_nodeContent->AddChild(node);
	if (node)
	{
		node->ResetViewer();
	}
}

void CHmsScreenLayer::AddScreenLayoutToMenu(CHmsScreenLayout * node)
{
	m_nodeRoot->AddChild(node);
}

CHmsMultLayer * CHmsScreenLayer::CreateMultScreen(Hms_Screen_Layout_Type eType)
{
	auto layer = CHmsMultLayer::create(eType);
	return layer;
}
#include <base/HmsActionImmediatelyRef.h>
void CHmsScreenLayer::ScreenLeftRightChange()
{
	const float c_timeActionDuration = 1.0;

	if (m_timeChangeMenu.GetElapsed() < c_timeActionDuration*1.25)
	{
		return;
	}

// 	if (m_nScreenChangeBack > 0)
// 	{
// 		return;
// 	}

	m_timeChangeMenu.RecordCurrentTime();

	auto child = m_nodeContent->GetChildren();

	if (m_bCurrentStride)
	{
		for (auto & c:child)
		{
			auto pos = c->GetPoistion();
			auto layout = dynamic_cast<CHmsScreenLayout *>(c);
			auto bLarge = true;

			if (layout)
			{
				auto eType = layout->GetLayoutType();
				if (eType == Hms_Screen_Layout_Type::Layout_Large)
				{
					if (0 == pos.x)
					{
						pos.x = CHmsGlobal::GetDesignContentWidthHalf();
					}
					else
					{
						pos.x = 0;
					}
				}
				else
				{
					bLarge = false;
					if (0 == pos.x)
					{
						pos.x = CHmsGlobal::GetDesignContentWidthHalf() * 3;
					}
					else
					{
						pos.x = 0;
					}
				}
			}
#if 0
// 			auto jumpTo = JumpTo::create(c_timeActionDuration, pos, (bLarge) ? 10 : -10, 2);
// 			c->runAction(jumpTo);
			c->SetPosition(pos);
#else
			auto jumpTo = CHmsActionMoveTo::Create(c_timeActionDuration, pos);
			auto callback = CHmsActionCallback::Create([this, c, pos](){
				c->SetPosition(pos);
				c->ForceUpdateAllChild();
				ScreenChangeEnd();
			});
			auto seq = CHmsActionQueue::Create(jumpTo, callback, nullptr);
			ScreenChangeStart();
			c->RunAction(seq);
#endif
		}
	}
	else
	{
		bool bHasSecondNormal = false;
		for(auto & c : child)
		{
			auto pos = c->GetPoistion();
			auto tempPage = dynamic_cast<CHmsScreenLayout*>(c);
			if (tempPage)
			{
				if (tempPage->GetLayoutType() == Hms_Screen_Layout_Type::Layout_Normal && abs(pos.x - CHmsGlobal::GetDesignContentWidthHalf()) < 20)
				{
					bHasSecondNormal = true;
					break;
				}
			}
		}

		if (bHasSecondNormal)
		{
			for(auto & c : child)
			{
				auto pos = c->GetPoistion();

				auto tempPage = dynamic_cast<CHmsScreenLayout*>(c);
				if (tempPage)
				{
					if (tempPage->GetLayoutType() == Hms_Screen_Layout_Type::Layout_Normal && abs(pos.x - CHmsGlobal::GetDesignContentWidthHalf()) < 20)
					{
						continue;
					}
				}

				if (abs(pos.x - 0) < 20)
				{
					pos.x = CHmsGlobal::GetDesignContentWidthHalf() * 3.0f;
				}
				else
				{
					pos.x = 0;
				}
#if 0
// 				auto jumpTo = JumpTo::create(c_timeActionDuration, pos, 10, 2);
// 				c->runAction(jumpTo);
				c->SetPosition(pos);
#else
				auto jumpTo = CHmsActionMoveTo::Create(c_timeActionDuration, pos);
				auto callback = CHmsActionCallback::Create([this,c, pos](){
					c->SetPosition(pos);
					c->ForceUpdateAllChild();
					ScreenChangeEnd();
				});
				auto seq = CHmsActionQueue::Create(jumpTo, callback, nullptr);
				ScreenChangeStart();
				c->RunAction(seq);
#endif
			}
		}
		else
		{
			for(auto & c:child)
			{
				auto pos = c->GetPoistion();

				pos.x += CHmsGlobal::GetDesignContentWidthNormal();
				//pos.x = (pos.x > CHmsGlobal::g_sizeScreenShow.width)?(pos.x - CHmsGlobal::g_sizeScreenShow.width):pos.x;
				pos.x = fmodf(pos.x, CHmsGlobal::GetDesignWidth());

#if 0
				// 				auto jumpTo = JumpTo::create(c_timeActionDuration, pos, 10, 2);
				// 				c->runAction(jumpTo);
				c->SetPosition(pos);
#else
				auto jumpTo = CHmsActionMoveTo::Create(c_timeActionDuration, pos);
				auto callback = CHmsActionCallback::Create([this,c, pos](){
					c->SetPosition(pos);
					c->ForceUpdateAllChild();
					ScreenChangeEnd();
				});
				auto seq = CHmsActionQueue::Create(jumpTo, callback, nullptr);
				ScreenChangeStart();
				c->RunAction(seq);
#endif
			}
		}
	}
}

void CHmsScreenLayer::CleanOldScreenContent()
{
	m_bCurrentStride = false;
	if (m_nodeContent)
	{
		m_nodeContent->RemoveAllChildren();
	}
}

void CHmsScreenLayer::ScreenChangeStart()
{
	m_nScreenChangeBack++;
}

void CHmsScreenLayer::ScreenChangeEnd()
{
	m_nScreenChangeBack--;
}

void CHmsScreenLayer::ScreenChangeOver()
{
	m_nScreenChangeBack = 0;
}

void CHmsScreenLayer::OnSoftKeysChange(int nBtnID, int nBtnState)
{
	auto child = m_nodeContent->GetChildren();
	CHmsScreenLayout * pLayout = nullptr;
	for (auto & c:child)
	{
		pLayout = dynamic_cast<CHmsScreenLayout*>(c);
		if (pLayout && pLayout->DispatchScreenButtonChange(nBtnID, nBtnState))
		{
			return;
		}
	}
}
