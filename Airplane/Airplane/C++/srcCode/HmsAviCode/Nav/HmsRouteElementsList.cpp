#include "HmsRouteElementsList.h"
#include "HmsFrame2DRoot.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsNavFplLayer.h"

CHmsRouteElementsList* CHmsRouteElementsList::Create(Size windowSize, float bottomToolHeight)
{
    CHmsRouteElementsList *ret = new (std::nothrow) CHmsRouteElementsList();
    if (ret && ret->Init(windowSize, bottomToolHeight))
    {
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

CHmsRouteElementsList::CHmsRouteElementsList()
: HmsUiEventInterface(this)
, m_bEnableEvent(true)
, m_bPressOnScrollbar(false)
, m_containerYPressOn(0)
{

}

CHmsRouteElementsList::~CHmsRouteElementsList()
{

}

bool CHmsRouteElementsList::Init(const HmsAviPf::Size & windowSize, float bottomToolHeight)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!pBgNode)
		return false;
#endif
	pBgNode->SetAnchorPoint(Vec2(0, 0));
	pBgNode->SetPosition(0, 0);
	this->AddChild(pBgNode);
	pBgNode->DrawSolidRect(
		Rect(0, 0, windowSize.width, windowSize.height),
		Color4F(Color3B(0x0e, 0x19, 0x25)));

    m_pScissorNode = CHmsScratchPadClipping::Create();
    m_pScissorNode->SetPosition(Vec2(0, bottomToolHeight));
    m_pScissorNode->SetContentSize(windowSize - Size(60, bottomToolHeight));
    m_pScissorNode->SetEventMode(ClippingEventMode::E_TRANSMIT);
    this->AddChild(m_pScissorNode);

    m_pItemsContainer = CHmsUiPanel::Create(m_pScissorNode->GetContentSize());
    m_pItemsContainer->SetAnchorPoint(0, 0);
    m_pItemsContainer->SetPosition(Vec2(0, m_pScissorNode->GetContentSize().height));
    m_pScissorNode->AddChildWithEvent(m_pItemsContainer, true);
    m_pScissorNode->RegisterAllNodeChild();
    //m_pScissorNode->SetClipRect(Rect(Vec2(0, 0), m_pScissorNode->GetContentSize()));
    {
        auto clipDrawNode = CHmsDrawNode::Create();
        m_pScissorNode->setStencil(clipDrawNode);

        clipDrawNode->DrawSolidRect(Rect(Vec2(0, 0), m_pScissorNode->GetContentSize()));
    }
    
    {
        auto pItem = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png");
        pItem->SetColor(Color3B::GRAY);
        pItem->SetContentSize(Size(50, 100));
        pItem->SetAnchorPoint(Vec2(0, 1));
        pItem->SetPosition(Vec2(windowSize.width - 50, windowSize.height));
        this->AddChild(pItem);

        m_pScrollbar = pItem;
        m_pScrollbar->SetVisible(false);
    }
    auto toolBtnSize = Size(260, 95);
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
            toolBtnSize, Color3B(0x1e, 0x37, 0x4f), Color3B(0x1e, 0x37, 0x4f));
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return false;
#endif
		pItem->AddIcon("res/NavImage/del.png");
		pItem->SetAnchorPoint(Vec2(0, 0));
        pItem->SetPosition(Vec2(windowSize.width - (toolBtnSize.width + 10) * 3, 10));
		pItem->SetLocalZOrder(c_maxLocalZ - 100);
		this->AddChild(pItem);

		m_pGarbageButton = pItem;
	}
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
            toolBtnSize, Color3B(0x1e, 0x37, 0x4f), Color3B(0x1e, 0x37, 0x4f));
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return false;
#endif
		pItem->AddText("Reverse", 32);
		pItem->SetAnchorPoint(Vec2(0, 0));
		pItem->SetLocalZOrder(1001);
        pItem->SetPosition(Vec2(windowSize.width - (toolBtnSize.width + 10) * 2, 10));
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* pBtn){

// 			float start = 0;
// 			float end = m_vecRouteElements.size() - 1;
// 			for (int i = 0; i < m_vecRouteElements.size() / 2; ++i)
// 			{
// 				RouteElementStu temp = m_vecRouteElements[i];
// 				m_vecRouteElements[i] = m_vecRouteElements[end - i];
// 				m_vecRouteElements[end - i] = temp;
// 			}
// 			CaculateNextPosition();
// 			SetRouteElementsToNextPosition(0);
 			m_curDragIndex = -1;
 			m_curDragStu.buttonPtr = nullptr;

            if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
			{
                CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->Reverse();
			}
		});
		this->AddChild(pItem);

		m_pReverseButton = pItem;
	}
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
            toolBtnSize, Color3B(0x1e, 0x37, 0x4f), Color3B(0x1e, 0x37, 0x4f));
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return false;
#endif
		pItem->AddText("Clear", 32);
		pItem->SetLocalZOrder(1002);
		pItem->SetAnchorPoint(Vec2(0, 0));
        pItem->SetPosition(Vec2(windowSize.width - (toolBtnSize.width + 10) * 1, 10));
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract *pBtn){
			bool b = m_pClearAllButton->IsVisible();
			m_pClearAllButton->SetVisible(!b);
		});
		this->AddChild(pItem);

		m_pClearButton = pItem;
	}
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
            Size(toolBtnSize.width * 2 + 10, toolBtnSize.height), Color3B(0x1e, 0x37, 0x4f), Color3B(0x1e, 0x37, 0x4f));
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return false;
#endif
		pItem->AddText("Clear All", 32, Color4B::RED);
		pItem->SetAnchorPoint(Vec2(0, 0));
		pItem->SetLocalZOrder(1003);
        pItem->SetPosition(Vec2(windowSize.width - (toolBtnSize.width + 10) * 2, toolBtnSize.height + 30));
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract *pBtn){
// 			for (auto ele : m_vecRouteElements)
// 			{
// 				if (ele.buttonPtr)
// 				{
// 					ele.buttonPtr->RemoveFromParent();
// 				}
// 			}
// 			m_vecRouteElements.clear();
// 			this->RemoveAllChildInterface();
// 			this->RegisterAllNodeChild();
 			m_curDragIndex = -1;
 			m_curDragStu.buttonPtr = nullptr;

            if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
			{
                CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ClearAll();
			}
			m_pClearAllButton->SetVisible(false);
		});
		this->AddChild(pItem);

		m_pClearAllButton = pItem;
		m_pClearAllButton->SetVisible(false);
		{
			auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
			if (!pBgNode)
				return false;
#endif
			pBgNode->SetContentSize(Size(20, 10));
			pBgNode->SetAnchorPoint(Vec2(0.5, 0));
            pBgNode->SetPosition(m_pClearAllButton->GetContentSize().width - toolBtnSize.width * 0.5, -10);
			std::vector<Vec2> vec;
			vec.push_back(Vec2(0, 10));
			vec.push_back(Vec2(20,10));
			vec.push_back(Vec2(10, 0));
			pBgNode->DrawTriangles(vec, Color4F(Color3B(0x1e, 0x37, 0x4f)));
			m_pClearAllButton->AddChild(pBgNode);
		}
	}

	this->RegisterAllNodeChild();

	return true;
}

void CHmsRouteElementsList::UpdateListItems()
{
    auto windowSize = m_pItemsContainer->GetContentSize();
	float curx = c_margin;
	float cury = 0 - (c_buttonHeight + c_margin);

    for (int i = 0; i < (int)m_vecRouteElements.size(); ++i)
	{
		RouteElementStu &ele = m_vecRouteElements[i];
		auto buttonSize = Size(10, c_buttonHeight);
		auto buttonColor = GetButtonColorByRouteType(ele.nodeStu.wType);

		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", buttonSize, buttonColor, Color3B::GRAY);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(ele.nodeStu.ident.c_str(), 26, Color4B::BLACK);
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetOnClickedFunc(std::bind(&CHmsRouteElementsList::OnRouteNodeClick, this, std::placeholders::_1, i));
			pItem->SetLocalZOrder(c_maxLocalZ - 50);

			buttonSize.width = pItem->GetLabel()->GetContentSize().width + 20;
			pItem->SetContentSize(buttonSize);
			pItem->GetLabel()->SetPositionX(buttonSize.width * 0.5);

			if (curx + buttonSize.width > windowSize.width)
			{
				curx = c_margin;
				cury -= c_buttonHeight + c_margin;
			}
			pItem->SetPosition(Vec2(curx, cury));

            m_pItemsContainer->AddChild(pItem);

			curx += buttonSize.width + c_margin;

			ele.buttonPtr = pItem;
			ele.nextPos = ele.buttonPtr->GetPoistion();
		}
	}

    m_pItemsContainer->SetContentSize(Size(windowSize.width, cury));

    m_pItemsContainer->RemoveAllChildInterface();
    m_pItemsContainer->RegisterAllNodeChild();
}

void CHmsRouteElementsList::OnRouteNodeClick(CHmsUiButtonAbstract *pBtn, int index)
{
    if (index >= 0 && index < (int)m_vecRouteElements.size())
	{
		auto & ele = m_vecRouteElements.at(index);
		if (ele.buttonPtr)
		{
			auto pos = ele.buttonPtr->GetPoistion() + ele.buttonPtr->GetUiBottomCenter();
            pos = m_pItemsContainer->ConvertToWorldSpace(pos);

            if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
			{
                CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowRouteNodeWin(ele.nodeStu, index, pos);
			}
		}
	}
}

void CHmsRouteElementsList::UpdateContainerPosition(float deltaY)
{
    float heightDelta = fabsf(m_pItemsContainer->GetContentSize().height) - m_pScissorNode->GetContentSize().height;
    if (heightDelta > 1)
    {
        float bottomToolHeight = this->GetContentSize().height - m_pScissorNode->GetContentSize().height;
        float ratio = (this->GetContentSize().height - bottomToolHeight - m_pScrollbar->GetContentSize().height) / heightDelta;
        if (ratio > 0.0001)
        {
            float y = m_containerYPressOn;
            y -= deltaY / ratio;
            y = HMS_BETWEEN_VALUE(y, m_pScissorNode->GetContentSize().height, m_pScissorNode->GetContentSize().height + heightDelta);
            m_pItemsContainer->SetPositionY(y);          
        } 
    }
    else
    {
        m_pItemsContainer->SetPosition(Vec2(0, m_pScissorNode->GetContentSize().height));
    }
    UpdateScrollbar();
}

void CHmsRouteElementsList::UpdateScrollbar()
{
    float heightDelta = fabsf(m_pItemsContainer->GetContentSize().height) - m_pScissorNode->GetContentSize().height;
    if (heightDelta > 1)
    {
        m_pScrollbar->SetVisible(true);

        float bottomToolHeight = this->GetContentSize().height - m_pScissorNode->GetContentSize().height;
        float ratio = (this->GetContentSize().height - bottomToolHeight - m_pScrollbar->GetContentSize().height) / heightDelta;
        float pos = this->GetContentSize().height - (m_pItemsContainer->GetPoistion().y - m_pScissorNode->GetContentSize().height) * ratio;
        m_pScrollbar->SetPositionY(pos);       
    }
    else
    {
        m_pScrollbar->SetVisible(false);
        m_pScrollbar->SetPositionY(this->GetContentSize().height);
    }
}

HmsAviPf::Color3B CHmsRouteElementsList::GetButtonColorByRouteType(WptPointType t)
{
	switch (t)
	{
	case WptPointType::WPT_VOR:
		return Color3B(0x9b, 0x9d, 0xdc);
		break;
	case WptPointType::WPT_WPT:
		return Color3B(0x9d, 0xb0, 0xfa);
		break;
	case WptPointType::WPT_USERWPT:
		return Color3B(0x9d, 0xb0, 0xfa);
		break;
	case WptPointType::WPT_NDB:
		return Color3B(0xcf, 0xd1, 0xd3);
		break;
	case WptPointType::WPT_AIRPORT:
		return Color3B(0x8f, 0xc2, 0xfc);
		break;
	case WptPointType::WPT_PROGRAM:
		return Color3B(0x8a, 0xcd, 0xcb);
		break;
	default:
		break;
	}
	return Color3B::WHITE;
}

bool CHmsRouteElementsList::OnPressed(const Vec2 & posOrigin)
{
    CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->HideRouteNodeWin();   

	m_time.RecordCurrentTime();
	m_posPressStart = posOrigin;

	m_curDragIndex = -1;
	m_bIsDragOut = false;

    m_bPressOnScrollbar = false;

    if (m_pScrollbar->IsVisible() && m_pScrollbar->GetRectFromParent().containsPoint(posOrigin))
    {
        m_bPressOnScrollbar = true;
        m_containerYPressOn = m_pItemsContainer->GetPoistionY();
    }
    else if (m_bEnableEvent)
    {
        for (int i = 0; i < (int)m_vecRouteElements.size(); ++i)
        {
            const RouteElementStu &ele = m_vecRouteElements.at(i);
            if (ele.buttonPtr)
            {
                auto rect = ele.buttonPtr->GetRectFromParent();
                auto containerPos = m_pItemsContainer->ConvertToNodeSpace(this->ConvertToWorldSpace(posOrigin));
                if (rect.containsPoint(containerPos) && ele.nodeStu.wType != WptPointType::WPT_PROGRAM)
                {
                    m_curDragIndex = i;
                    m_curDragStu = m_vecRouteElements[i];
                    m_posButtonPressStart = ele.buttonPtr->GetPoistion();
                    break;
                }
            }
        }
    }
	
	return true;
}

void CHmsRouteElementsList::OnMove(const Vec2 & posOrigin)
{
	auto DetectEdge = [](float edge1, float &dst, float edge2)
	{
		if (dst < edge1)
		{
			dst = edge1;
		}
		if (dst > edge2)
		{
			dst = edge2;
		}
	};

	Vec2 movePos = posOrigin;
	auto winSize = this->GetContentSize();
	DetectEdge(0, movePos.x, winSize.width);
	DetectEdge(0, movePos.y, winSize.height);

	auto delta = movePos - m_posPressStart;

    if (m_bPressOnScrollbar)
    {
        UpdateContainerPosition(delta.y);    
    }
    else if (m_bEnableEvent)
    {
        if (m_curDragStu.buttonPtr)
        {
            auto newPos = m_posButtonPressStart + delta;
            m_curDragStu.buttonPtr->SetPosition(newPos);

            {
                auto rect = m_pGarbageButton->GetRectFromParent();
                if (rect.containsPoint(movePos))
                {
                    m_pGarbageButton->SetColor(Color3B::RED);
                }
                else
                {
                    m_pGarbageButton->SetColor(Color3B(0x1e, 0x37, 0x4f));
                }
            }

            if (!m_bIsDragOut)
            {
                if (fabs(newPos.x - m_posButtonPressStart.x) > m_curDragStu.buttonPtr->GetContentSize().width
                    || fabs(newPos.y - m_posButtonPressStart.y) > m_curDragStu.buttonPtr->GetContentSize().height)
                {
                    m_vecRouteElements.erase(m_vecRouteElements.begin() + m_curDragIndex);
                    CaculateNextPosition();
                    SetRouteElementsToNextPosition(m_curDragIndex);

                    m_bIsDragOut = true;

                    m_curDragStu.buttonPtr->SetLocalZOrder(c_maxLocalZ);
                }
            }
        }
    }
}

void CHmsRouteElementsList::OnReleased(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_posPressStart;
	auto deltaTime = m_time.GetElapsed();

    if (m_bPressOnScrollbar)
    {
        m_containerYPressOn = m_pItemsContainer->GetPoistionY();
    }
    else if (m_bEnableEvent && deltaTime < 2 && delta.length() < 10)
	{
		HmsUiEventInterface::OnPressed(posOrigin);
		HmsUiEventInterface::OnReleased(posOrigin);
	}

	LayoutRouteElement(posOrigin);
	m_curDragIndex = -1;
	m_curDragStu.buttonPtr = nullptr;
}

void CHmsRouteElementsList::LayoutRouteElement(const Vec2 & posOrigin)
{
	if (m_curDragStu.buttonPtr)
	{
		//已经拖出来了
		if (m_bIsDragOut)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (!m_pGarbageButton)
				return;
#endif
			m_pGarbageButton->SetColor(Color3B(0x1e, 0x37, 0x4f));
			auto rect = m_pGarbageButton->GetRectFromParent();
			if (rect.containsPoint(posOrigin))
			{
				//是删除
				DeleteRouteElement(m_curDragIndex, m_curDragStu);
			}
			else
			{
				//是插入
				int insertIndex = -1;
                for (int i = 0; i < (int)m_vecRouteElements.size(); ++i)
				{
					const RouteElementStu &ele = m_vecRouteElements.at(i);
					if (ele.buttonPtr)
					{
						auto rect = ele.buttonPtr->GetRectFromParent();
                        auto containerPos = m_pItemsContainer->ConvertToNodeSpace(this->ConvertToWorldSpace(posOrigin));
						if (rect.containsPoint(containerPos))
						{
							insertIndex = i;
							break;
						}
					}
				}
				InsertRouteElement(insertIndex, m_curDragStu);
			}
		}
		else//没拖出来，放回原位
		{
			auto moveto = CHmsActionMoveTo::Create(0.2f, m_curDragStu.nextPos);
			m_curDragStu.buttonPtr->RunAction(moveto);
		}
	}
}

void CHmsRouteElementsList::DeleteRouteElement(int index, RouteElementStu &stu)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!stu.buttonPtr)
		return;
#endif
	stu.buttonPtr->RemoveFromParent();
	stu.buttonPtr = nullptr;

	//更新到地图
    if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
	{
        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->DeleteWptNode(FPLNodeIndex(index, -1));
	}
}
void CHmsRouteElementsList::InsertRouteElement(int index, RouteElementStu &stu)
{
	if (index < 0)
	{
		m_vecRouteElements.push_back(stu);
	}
	else
	{
		m_vecRouteElements.insert(m_vecRouteElements.begin() + index, stu);
	}

	CaculateNextPosition();
    for (int i = 0; i < (int)m_vecRouteElements.size(); ++i)
	{
		RouteElementStu &ele = m_vecRouteElements[i];
		auto moveto = CHmsActionMoveTo::Create(0.2f, ele.nextPos);
#if _NAV_SVS_LOGIC_JUDGE
		if (ele.buttonPtr && moveto)
#endif
		{
			ele.buttonPtr->RunAction(moveto);
			ele.buttonPtr->SetLocalZOrder(c_maxLocalZ - 50);
		}
	}

	//更新到地图
    if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
	{
        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->DeleteWptNode(FPLNodeIndex(m_curDragIndex, -1));
        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertWptNode(
			stu.nodeStu.wType,
			stu.nodeStu.lon,
			stu.nodeStu.lat,
			stu.nodeStu.ident,
			FPLNodeIndex(index, -1));
	}
}

void CHmsRouteElementsList::CaculateNextPosition()
{
	auto windowSize = this->GetContentSize();
	float curx = c_margin;
	float cury = 0 - (c_buttonHeight + c_margin);

    for (int i = 0; i < (int)m_vecRouteElements.size(); ++i)
	{
		RouteElementStu &ele = m_vecRouteElements[i];
#if _NAV_SVS_LOGIC_JUDGE
		if (!ele.buttonPtr)
			continue;
#endif
		auto buttonSize = ele.buttonPtr->GetContentSize();

		if (curx + buttonSize.width > windowSize.width)
		{
			curx = c_margin;
			cury -= c_buttonHeight + c_margin;
		}
		ele.nextPos = Vec2(curx, cury);

		curx += buttonSize.width + c_margin;
	}
}

void CHmsRouteElementsList::SetRouteElementsToNextPosition(int beginIndex)
{
    for (int i = beginIndex; i < (int)m_vecRouteElements.size(); ++i)
	{
		RouteElementStu &ele = m_vecRouteElements[i];
		auto moveto = CHmsActionMoveTo::Create(0.2f, ele.nextPos);
#if _NAV_SVS_LOGIC_JUDGE
		if (ele.buttonPtr && moveto)
#endif
		ele.buttonPtr->RunAction(moveto);
	}
}

void CHmsRouteElementsList::SetWptNodeData(const std::vector<FPL2DNodeInfoStu> &vec)
{
	ClearAll();
    for (int i = 0; i < (int)vec.size(); ++i)
	{
		RouteElementStu stu;
		stu.nodeStu.wType = vec[i].ePriType == FPL_TYPE_PROGRAM ? WptPointType::WPT_PROGRAM : vec[i].vWayPoint[0].brief.wType;
		if (stu.nodeStu.wType == WptPointType::WPT_PROGRAM)
		{
			stu.nodeStu.ident = vec[i].ident;
		}
		else
		{
			stu.nodeStu.ident = vec[i].vWayPoint[0].brief.ident;
		}
		stu.nodeStu.lon = vec[i].vWayPoint[0].brief.lon;
		stu.nodeStu.lat = vec[i].vWayPoint[0].brief.lat;
		stu.buttonPtr = nullptr;

		m_vecRouteElements.push_back(stu);
	}
	UpdateListItems();
    UpdateContainerPosition(0);
}

void CHmsRouteElementsList::SetWptNodeData(const HistoryRouteStu &historyStu)
{
	ClearAll();
    for (int i = 0; i < (int)historyStu.vecPoints.size(); ++i)
	{
		const auto &historyOnePoint = historyStu.vecPoints[i];
		RouteElementStu stu;
		if (historyOnePoint.ePriType == FPL_TYPE_PROGRAM)
		{
			stu.nodeStu.wType = WptPointType::WPT_PROGRAM;
		}
		else
		{
			stu.nodeStu.wType = historyOnePoint.pointAttr.type;
		}
		stu.nodeStu.ident = historyOnePoint.GetIdent();
		stu.buttonPtr = nullptr;

		m_vecRouteElements.push_back(stu);
	}
	UpdateListItems();
    UpdateContainerPosition(0);
}

void CHmsRouteElementsList::ClearAll()
{
	for (auto &ele : m_vecRouteElements)
	{
		if (ele.buttonPtr)
		{
			ele.buttonPtr->RemoveFromParent();
		}
	}
	m_vecRouteElements.clear();
}

void CHmsRouteElementsList::SetEnableEvent(bool b)
{
	m_bEnableEvent = b;
}

void CHmsRouteElementsList::SetToolBtnVisible(bool b)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pGarbageButton || !m_pReverseButton || !m_pClearButton)
		return;
#endif
	m_pGarbageButton->SetVisible(b);
	m_pReverseButton->SetVisible(b);
	m_pClearButton->SetVisible(b);
}
