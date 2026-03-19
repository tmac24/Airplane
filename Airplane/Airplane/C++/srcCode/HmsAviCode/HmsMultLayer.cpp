#include "HmsMultLayer.h"
USING_NS_HMS;

CHmsMultLayer::CHmsMultLayer()
{
}


CHmsMultLayer::~CHmsMultLayer()
{
}

CHmsMultLayer * CHmsMultLayer::create(Hms_Screen_Layout_Type eType)
{
	auto layer = new CHmsMultLayer();
	if (layer && layer->init(eType))
	{ 
		layer->autorelease(); 
		return layer;
	}
	else 
	{ 
		delete layer; 
		layer = nullptr; 	
	}
	return layer;
}

bool CHmsMultLayer::init(Hms_Screen_Layout_Type eType)
{
	InitLayoutType(eType);

	if (!CHmsScreenLayout::Init())
	{
		return false;
	}

	SetRecvTouchEvent();

	return true;
}

void CHmsMultLayer::AddPage(CHmsScreenLayout * pLayout)
{
	if (pLayout)
	{
		if (pLayout->GetParent())
		{
			pLayout->RemoveFromParent();
		}
		m_vPageInfo.pushBack(pLayout);

		auto tag = CreateTag(pLayout);
		if (tag)
		{
			m_vPageTag.pushBack(tag);

			tag->SetUserData((void*)(m_vPageInfo.size() - 1));
		}
	}
}

HmsAviPf::CHmsNode * CHmsMultLayer::CreateTag(CHmsScreenLayout * pLayout)
{
	auto sp = CHmsImageNode::Create("res/pageTag.png");
	if (sp)
	{
		sp->SetAnchorPoint(Vec2(0.5, 0));
		sp->SetLocalZOrder(101);

		auto label = pLayout->GetPageLabel();
		if (label)
		{
			//auto str = label->GetString();
			std::string str = "";
			auto curLabel = CHmsGlobal::CreateLabel(str.c_str(), 30);
			auto size = sp->GetContentSize();
			curLabel->SetPosition(size / 2.0f);
			sp->AddChild(curLabel);
		}
	}
	return sp;
}

void CHmsMultLayer::SortPageAndShowIndex(int nIndex /*= 0*/)
{
	if (nIndex < 0)
	{
		nIndex = 0;
	}
	else if (nIndex >= m_vPageTag.size())
	{
		nIndex = 0;
	}

	this->RemoveAllChildren();

	auto bl = GetBottomLeft();
	auto br = GetBottomRight();

	auto width = (br.x - bl.x)/(m_vPageTag.size()+1);

	for (auto i = 0; i < m_vPageTag.size(); i++)
	{
		auto tag = m_vPageTag.at(i);
		if (tag)
		{
			tag->SetPosition(bl + Vec2(width*(i+1), 0));
			this->AddChild(tag);
		}
	}

	if (m_vPageTag.size())
	{
		auto tag = m_vPageTag.at(nIndex);
		if (tag)
		{
			tag->SetVisible(false);
			long nPageIndex = reinterpret_cast<long>(tag->GetUserData());
			if (nPageIndex >= 0 && nPageIndex <m_vPageInfo.size())
			{
				auto page = m_vPageInfo.at(nPageIndex);
				if (page)
				{
					m_curUsedPageInfo = page;
					page->SetPosition(0, 0);
					this->AddChild(page);
					page->ResetViewer();
				}
			}
		}
	}
}

void CHmsMultLayer::SetRecvTouchEvent()
{
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = [=](HmsAviPf::Touch* touch, HmsAviPf::CHmsEvent* event)
	{
		for (auto c:m_vPageTag)
		{
#if 0
			if (CHmsGlobal::IsTouchSprite(touch, dynamic_cast<CHmsImageNode*>(c)))
			{
				m_curPushTag = dynamic_cast<CHmsImageNode*>(c);
				return true;
			}
#endif
		}

		m_curPushTag = nullptr;
		return false;
	};

// 	listener->onTouchMoved = [=](HmsAviPf::Touch* touch, HmsAviPf::CHmsEvent* event)
// 	{
// 		
// 	};

	listener->onTouchEnded = [=](HmsAviPf::Touch* touch, HmsAviPf::CHmsEvent* event)
	{
		if (m_curPushTag)
		{
#if 0
			if (CHmsGlobal::IsTouchSprite(touch, m_curPushTag))
			{
				OnSelectedTagNode(m_curPushTag);
			}
#endif
		}
		m_curPushTag = nullptr;
	};

	//_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CHmsMultLayer::OnSelectedTagNode(HmsAviPf::CHmsNode * tag)
{
	for (auto c:m_vPageTag)
	{
		if (c==tag)
		{
			c->SetVisible(false);
		}
		else
		{
			c->SetVisible(true);
		}
	}

	this->RemoveChild(m_curUsedPageInfo);
	m_curUsedPageInfo = nullptr;

	
	if (tag)
	{
		tag->SetVisible(false);
		long nPageIndex = reinterpret_cast<long>(tag->GetUserData());
		if (nPageIndex >= 0 && nPageIndex < m_vPageInfo.size())
		{
			auto page = m_vPageInfo.at(nPageIndex);
			if (page)
			{
				m_curUsedPageInfo = page;
				this->AddChild(page);
				page->ResetViewer();
			}
		}
	}
}
