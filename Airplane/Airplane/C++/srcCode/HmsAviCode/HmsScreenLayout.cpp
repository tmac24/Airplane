#include "HmsScreenLayout.h"
#include "display/HmsStretchImageNode.h"
//USING_NS_HMS_EXT;
USING_NS_HMS;

#define HMS_NODE_TAG_SCREEN_LAYOUT_BG	0xFF00001

CHmsScreenLayout::CHmsScreenLayout()
	: m_eScreenLayoutType(Hms_Screen_Layout_Type::Layout_Middle)
	, m_pNodeCentre(nullptr)
	, m_nScreenPosIndex(0)
{
}


CHmsScreenLayout::~CHmsScreenLayout()
{
	if (m_pNodeCentre)
	{
		m_pNodeCentre->autorelease();
	}
}

bool CHmsScreenLayout::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_sizeLayout = GetScreenLayoutSize();
	SetContentSize(m_sizeLayout);
	
	m_pNodeCentre = new HmsAviPf::CHmsNode();
	m_pNodeCentre->retain();
	m_pNodeCentre->SetPosition(GetCenter());
	this->AddChild(m_pNodeCentre);

#if 0
	auto bg = CHmsDrawNode::Create();
	bg->DrawInnerRect(0, 0, m_sizeLayout.width, -m_sizeLayout.height, 2.0f);
	bg->SetTag(HMS_NODE_TAG_SCREEN_LAYOUT_BG);
	this->AddChild(bg);
#endif
	
	auto bg = CHmsStretchImageNode::Create("res/BgFrame1.png");
	if (bg)
	{
		bg->SetTag(HMS_NODE_TAG_SCREEN_LAYOUT_BG);
		bg->SetLocalZOrder(-2);
		//bg->setPreferredSize(m_sizeLayout);
		bg->SetContentSize(m_sizeLayout);
		bg->SetAnchorPoint(Vec2(0, 0.0));
		this->AddChild(bg);
	}

	ResetViewer();
	return true;
}

void CHmsScreenLayout::InitLayoutType(Hms_Screen_Layout_Type eType)
{
	m_eScreenLayoutType = eType;
	m_sizeLayout = GetScreenLayoutSize();

	if (m_pNodeCentre)
	{
		m_pNodeCentre->SetPosition(GetCenter());
	}

	auto bg = dynamic_cast<CHmsStretchImageNode*>(this->GetChildByTag(HMS_NODE_TAG_SCREEN_LAYOUT_BG));
	if (bg)
	{
		bg->SetContentSize(m_sizeLayout);
		//bg->setPreferredSize(m_sizeLayout);
// 		bg->clear();
// 		bg->DrawInnerRect(0, 0, m_sizeLayout.width, -m_sizeLayout.height, 2.0f);
	}

	SetContentSize(m_sizeLayout);
}

void CHmsScreenLayout::SetLayoutType(Hms_Screen_Layout_Type eType)
{
	if (m_eScreenLayoutType != eType)
	{
		Hms_Screen_Layout_Type eOld = m_eScreenLayoutType;
		InitLayoutType(eType);
		ChangePageLabelByLayoutType(eType);
		OnScreenLayoutChanged(eOld, m_eScreenLayoutType);
	}
}

HmsAviPf::Size CHmsScreenLayout::GetScreenLayoutSize()
{
	return GetScreenLayoutSize(m_eScreenLayoutType);
}

HmsAviPf::Size CHmsScreenLayout::GetScreenLayoutSize(Hms_Screen_Layout_Type eType)
{
	Size sizeOut(0, 0);
	switch (eType)
	{
	case Hms_Screen_Layout_Type::Layout_Small:
		sizeOut.setSize(CHmsGlobal::GetDesignContentWidthSmall(), CHmsGlobal::GetDesignContentHeightSmall());
		break;
	case Hms_Screen_Layout_Type::Layout_Middle:
		sizeOut.setSize(CHmsGlobal::GetDesignContentWidthHalf(), CHmsGlobal::GetDesignContentHeight());
		break;
	case Hms_Screen_Layout_Type::Layout_Normal:
		sizeOut.setSize(CHmsGlobal::GetDesignContentWidthNormal(), CHmsGlobal::GetDesignContentHeight());
		break;
	case Hms_Screen_Layout_Type::Layout_Large:
		sizeOut.setSize(CHmsGlobal::GetDesignContentWidthStride(), CHmsGlobal::GetDesignContentHeight());
		break;
	case Hms_Screen_Layout_Type::Layout_Menu:
		sizeOut.setSize(CHmsGlobal::GetDesignContentWidthNormal(), CHmsGlobal::GetDesignMenuBarHeight());
		break;
	case Hms_Screen_Layout_Type::Layout_Menu_Full:
		sizeOut.setSize(CHmsGlobal::GetDesignWidth(), CHmsGlobal::GetDesignMenuBarHeight());
        break;
    case Hms_Screen_Layout_Type::Layout_Full:
        sizeOut.setSize(CHmsGlobal::GetDesignWidth(), CHmsGlobal::GetDesignHeight());
        break;
	default:
		break;
	}

	return sizeOut;
}

HmsAviPf::Size CHmsScreenLayout::GetSize()
{
	return m_sizeLayout;
}

HmsAviPf::Vec2 CHmsScreenLayout::GetTopLeft()
{
	return Vec2(0, m_sizeLayout.height);
}

HmsAviPf::Vec2 CHmsScreenLayout::GetTopRight()
{
	return Vec2(m_sizeLayout.width, m_sizeLayout.height);
}

HmsAviPf::Vec2 CHmsScreenLayout::GetBottomLeft()
{
	return Vec2(0, 0);
}

HmsAviPf::Vec2 CHmsScreenLayout::GetBottomRight()
{
	return Vec2(m_sizeLayout.width, 0);
}

HmsAviPf::Vec2 CHmsScreenLayout::GetCenter()
{
	return Vec2(m_sizeLayout.width/2, m_sizeLayout.height/2);
}

HmsAviPf::Vec2 CHmsScreenLayout::GetCenterLeft()
{
	return Vec2(0, m_sizeLayout.height / 2);
}

HmsAviPf::Vec2 CHmsScreenLayout::GetCenterRight()
{
	return Vec2(m_sizeLayout.width, m_sizeLayout.height / 2);
}

HmsAviPf::Vec2 CHmsScreenLayout::GetTopCenter()
{
	return Vec2(m_sizeLayout.width / 2, m_sizeLayout.height);
}

HmsAviPf::Vec2 CHmsScreenLayout::GetBottomCenter()
{
	return Vec2(m_sizeLayout.width / 2, 0);
}

bool CHmsScreenLayout::TouchInLayout(HmsAviPf::Touch * touch)
{
	bool bRet = false;

	auto rect = Rect(0, 0, m_sizeLayout.width, m_sizeLayout.height);
	if (rect.containsPoint(this->ConvertTouchToNodeSpaceAR(touch)))
	{
		bRet = true;
	}
	return bRet;
}

// bool CHmsScreenLayout::MouseInLayout(HmsAviPf::CHmsEventMouse * mouse)
// {
// 	bool bRet = false;
// 
// 	auto rect = Rect(0, -m_sizeLayout.height, m_sizeLayout.width, m_sizeLayout.height);
// 	auto locav = mouse->getLocationInView();
// 	auto pos = this->convertToNodeSpaceAR(locav);
// 	if (rect.containsPoint(pos))
// 	{
// 		bRet = true;
// 	}
// 	return bRet;
// }

HmsAviPf::Rect CHmsScreenLayout::GetRect()
{
	return Rect(0, 0, m_sizeLayout.width, -m_sizeLayout.height);
}

bool CHmsScreenLayout::onTouchBegan(HmsAviPf::Touch* touch, HmsAviPf::CHmsEvent* event)
{
	if (TouchInLayout(touch))
	{
		return true;
	}
	return false;
}

void CHmsScreenLayout::SetRecvTouchEvent()
{
// {
// 	auto listener = EventListenerTouchOneByOne::create();
// 	listener->setSwallowTouches(true);
// 
// 	listener->onTouchBegan = HMS_CALLBACK_2(CHmsScreenLayout::onTouchBegan, this);
//  	listener->onTouchMoved = HMS_CALLBACK_2(CHmsScreenLayout::onTouchMoved, this);
//  	listener->onTouchEnded = HMS_CALLBACK_2(CHmsScreenLayout::onTouchEnded, this);
// 
// 	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

const int c_nScreenLayoutPageLabel = 7472;

HmsAviPf::CHmsLabel *  CHmsScreenLayout::CreateOrSetPageLabel(const char * strTitle)
{
	HmsAviPf::CHmsLabel * labelTitle = GetPageLabel();
	if (labelTitle)
	{
		CHmsGlobal::SetLabelText(labelTitle, strTitle);
	}
	else
	{
		labelTitle = CHmsGlobal::CreateLabel(strTitle, 22);
		labelTitle->SetTag(c_nScreenLayoutPageLabel);
		//labelTitle->SetLocalZOrder(100);
		this->AddChild(labelTitle);
	}
	
	ChangePageLabelByLayoutType(m_eScreenLayoutType);

	return labelTitle;
} 

void CHmsScreenLayout::ChangePageLabelByLayoutType(Hms_Screen_Layout_Type tType)
{
	HmsAviPf::CHmsLabel * labelTitle = GetPageLabel();
	if (labelTitle)
	{
		labelTitle->SetAnchorPoint(Vec2(0, 1.0));
		labelTitle->SetPosition(10, -10);
	}
}

HmsAviPf::CHmsLabel * CHmsScreenLayout::GetPageLabel()
{
	auto label = dynamic_cast<HmsAviPf::CHmsLabel*>(this->GetChildByTag(c_nScreenLayoutPageLabel));
	return label;
}

void CHmsScreenLayout::SetPosition(float x, float y)
{
	CHmsNode::SetPosition(x, y);
	UpdateScreenPosIndex();
}

void CHmsScreenLayout::SetPosition(const Vec2 & pos)
{
	CHmsNode::SetPosition(pos.x, pos.y);
	UpdateScreenPosIndex();
}

void CHmsScreenLayout::SetPositionX(float x)
{
	CHmsNode::SetPositionX(x);
	UpdateScreenPosIndex();
}

void CHmsScreenLayout::UpdateScreenPosIndex()
{
	auto posX = GetPoistionX() + 5;		//防止误差
	m_nScreenPosIndex = posX / CHmsGlobal::GetDesignContentWidthHalf();
	int a = 0;
}

HmsAviPf::CHmsNode * CHmsScreenLayout::GetLayoutBg()
{
	return this->GetChildByTag(HMS_NODE_TAG_SCREEN_LAYOUT_BG);
}

bool CHmsScreenLayout::DispatchScreenButtonChange(int nBtnIndex, int nBtnState)
{
	bool bRet = false;
	int nIndexStart = m_nScreenPosIndex * 4;
	int nIndexEnd = nIndexStart + 4 * ((int)m_eScreenLayoutType - (int)Hms_Screen_Layout_Type::Layout_Middle + 1);

	if ((nBtnIndex >= nIndexStart) && (nBtnIndex < nIndexEnd))
	{
		OnLayoutBtnChange(nBtnIndex - nIndexStart, nBtnState);
		bRet = true;
	}

	return bRet;
}




