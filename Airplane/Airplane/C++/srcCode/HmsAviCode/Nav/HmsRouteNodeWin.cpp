#include "HmsRouteNodeWin.h"
#include "HmsFrame2DRoot.h"
#include "HmsNavEarthFrame.h"
#include "Language/HmsLanguageMananger.h"

CHmsRouteNodeWin::CHmsRouteNodeWin()
: HmsUiEventInterface(this)
{

}

CHmsRouteNodeWin::~CHmsRouteNodeWin()
{

}

bool CHmsRouteNodeWin::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	float triangleHeight = 20;

	Vec2 areaOut_pos = Vec2(triangleHeight, triangleHeight);
	Size areaOut_size = Size(windowSize.width - triangleHeight * 2, windowSize.height - triangleHeight * 2);

	Vec2 useArea_pos = areaOut_pos + Vec2(10,10);
	Size useArea_size = areaOut_size - Size(20,20);

	auto btnSize = Size((useArea_size.width-4) / 3.0, 70);

	auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!pBgNode)
		return false;
#endif
	pBgNode->SetAnchorPoint(Vec2(0, 0));
	pBgNode->SetPosition(0, 0);
	this->AddChild(pBgNode);
	pBgNode->DrawSolidRect(Rect(areaOut_pos, areaOut_size), Color4F(Color3B(0x0e, 0x19, 0x25)));
	pBgNode->DrawSolidRect(Rect(useArea_pos.x, useArea_pos.y, useArea_size.width, useArea_size.height - btnSize.height - 10),
        Color4F(Color3B(0x1e, 0x37, 0x4f)));

	//画边框
	{
		std::vector<Vec2> vec;
		vec.push_back(Vec2(areaOut_pos.x, areaOut_pos.y));
		vec.push_back(Vec2(areaOut_pos.x + areaOut_size.width, areaOut_pos.y));
		vec.push_back(Vec2(areaOut_pos.x + areaOut_size.width, areaOut_pos.y + areaOut_size.height));
		vec.push_back(Vec2(areaOut_pos.x, areaOut_pos.y + areaOut_size.height));
		vec.push_back(Vec2(areaOut_pos.x, areaOut_pos.y));
		pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
	}
	//画三角
	InitTriangleNode(Size(triangleHeight, triangleHeight), areaOut_size);

	auto CreateButton = [=](Size s, Vec2 pos, const char * text)
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(text, 30, Color4B::WHITE, true);
			pItem->SetAnchorPoint(Vec2(0, 1));
			pItem->SetPosition(pos);
			//pItem->SetOnClickedFunc(std::bind(&CHmsRouteNodeWin::ShowPage, this, std::placeholders::_1, WinPageShow::PAGE_ACTION));
			this->AddChild(pItem);
		}
		return pItem;
	};

	CHmsUiStretchButton *pItem = nullptr;
	pItem = CreateButton(btnSize, Vec2(useArea_pos.x + btnSize.width * 0, useArea_pos.y + useArea_size.height), "Show On Map");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::GREEN);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		auto earthFrame = CHmsGlobal::GetInstance()->GetNavEarthFramePointer();
		if (m_showPageNode.bEnableShowOnMap && earthFrame)
		{
			earthFrame->SetPlaneAlwaysCenter(false);
			earthFrame->SetViewCenterByLonLat(m_showPageNode.lon, m_showPageNode.lat);
			this->SetVisible(false);
		}
	});

	pItem = CreateButton(btnSize, Vec2(useArea_pos.x + btnSize.width * 1 + 2, useArea_pos.y + useArea_size.height), "Direct To");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::RED);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
        if (m_showPageNode.bEnableShowOnMap && CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
		{
            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->DirectTo(FPLNodeIndex(m_curNodeIndex, -1));
			this->SetVisible(false);
		}	
	});

	pItem = CreateButton(btnSize, Vec2(useArea_pos.x + btnSize.width * 2 + 4, useArea_pos.y + useArea_size.height), "Delete");
	pItem->SetTextColor(Color4B::RED);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
        if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
		{
            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->DeleteWptNode(FPLNodeIndex(m_curNodeIndex, -1), false);
		}
		this->SetVisible(false);
	});

	InitListBox(Rect(useArea_pos.x, useArea_pos.y, useArea_size.width, useArea_size.height - btnSize.height - 20));

	this->RegisterAllNodeChild();

	return true;
}

void CHmsRouteNodeWin::InitTriangleNode(Size tSize, Size outSize)
{
	m_pTriangleNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pTriangleNode)
		return;
#endif
	m_pTriangleNode->SetContentSize(tSize);
	this->AddChild(m_pTriangleNode);

	{
		std::vector<Vec2> vec;
		vec.push_back(Vec2(0, 0));
		vec.push_back(Vec2(0, tSize.height));
		vec.push_back(Vec2(tSize.width, tSize.height * 0.5));

		m_pTriangleNode->DrawTriangles(vec, Color4F(Color3B(0x0e, 0x19, 0x25)));
	}
	{
		std::vector<Vec2> vec;
		vec.push_back(Vec2(0, tSize.height));
		vec.push_back(Vec2(tSize.width, tSize.height * 0.5));
		m_pTriangleNode->DrawLines(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
		vec.clear();
		vec.push_back(Vec2(tSize.width, tSize.height * 0.5));
		vec.push_back(Vec2(0, 0));
		m_pTriangleNode->DrawLines(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
	}

	m_pTriangleNode->SetPosition(outSize.width / 2 + tSize.height / 2 + tSize.height, outSize.height + tSize.height - 1);
	m_pTriangleNode->SetRotation(-90);
}

void CHmsRouteNodeWin::InitListBox(const HmsAviPf::Rect & windowRect)
{
	auto boxSize = windowRect.size;

	{
		auto pListBox = CHmsListBox::CreateWithImage(
			NULL,
			NULL,
			"res/airport/blue_selbox.png",
			"res/airport/star_normal.png",
			NULL);
#if _NAV_SVS_LOGIC_JUDGE
		if (!pListBox)
			return;
#endif
		this->AddChild(pListBox);
		m_pListBox = pListBox;

        pListBox->SetItemHeight(90);
		pListBox->SetBoxSize(boxSize);
		pListBox->SetScrollbarWidth(4);
		pListBox->SetBacktopButtonVisible(false);
		pListBox->SetSelectWithCheck(true);
		pListBox->SetItemHeight(50);
		pListBox->SetAnchorPoint(Vec2(0, 0));
		pListBox->SetPosition(windowRect.origin);
		pListBox->SetItemNormalTextColor(Color4B::WHITE);
		pListBox->SetItemSelectChangeCallBack(HMS_CALLBACK_3(CHmsRouteNodeWin::OnListBoxItemClick, this));
	}
}

void CHmsRouteNodeWin::ShowPage(WinPageShow page, ShowPageWptNode node, int index)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pListBox)
		return;
#endif
	auto AppendListItem = [=](const char * text)
	{
		m_pListBox->AppendItem(text, false, true, 26, true);
	};

	m_showPageNode = node;
	m_curNodeIndex = index;
	m_curPage = page;

#if 0
	char chBefore[128], chAfter[128];
	memset(chBefore, 0, sizeof(chBefore));
	memset(chAfter, 0, sizeof(chAfter));
	sprintf(chBefore, "%s %s", m_cInsertBefore.c_str(), m_showPageNode.ident.c_str());
	sprintf(chAfter, "%s %s", m_cInsertAfter.c_str(), m_showPageNode.ident.c_str());
#endif

	switch (page)
	{
	case WinPageShow::PAGE_ACTION_NORMAL:
		m_pListBox->Clear();
		//AppendListItem(chBefore);
		//AppendListItem(chAfter);
		break;
	case WinPageShow::PAGE_ACTION_SID:
		m_pListBox->Clear();
		//AppendListItem(chBefore);
		//AppendListItem(chAfter);
		AppendListItem(m_cSelectDeparture.c_str());
		break;
	case WinPageShow::PAGE_ACTION_STARAPP:
		m_pListBox->Clear();
		//AppendListItem(chBefore);
		//AppendListItem(chAfter);
		AppendListItem(m_cSelectArrival.c_str());
		AppendListItem(m_cSelectApproach.c_str());
		break;
	case WinPageShow::PAGE_ACTION_CHGSID:
		m_pListBox->Clear();
		//AppendListItem(chBefore);
		//AppendListItem(chAfter);
		AppendListItem(m_cChangeDeparture.c_str());
		break;
	case WinPageShow::PAGE_ACTION_CHGSTAR:
		m_pListBox->Clear();
		//AppendListItem(chBefore);
		//AppendListItem(chAfter);
		AppendListItem(m_cChangeArrival.c_str());
		break;
	case WinPageShow::PAGE_ACTION_CHGAPP:
		m_pListBox->Clear();
		//AppendListItem(chBefore);
		//AppendListItem(chAfter);
		AppendListItem(m_cChangeApproach.c_str());
		break;
	case WinPageShow::PAGE_LIST_SID:
	case WinPageShow::PAGE_LIST_STAR:
	case WinPageShow::PAGE_LIST_APP:
	{
		m_pListBox->Clear();

		for (auto &c : m_mapProgramInfo)
		{
			std::string strRnw;
			for (auto &r : c.second.mapRunway)
			{
				strRnw += r.first + " ";
			}
			//如果是选择进场程序，并且进近程序已经确定了跑道，就做一个筛选
			if (page == WinPageShow::PAGE_LIST_STAR && m_strAPPRunwayFilter.size() > 0 && strRnw.size() > 0)
			{
				if (strRnw.find(m_strAPPRunwayFilter) == std::string::npos)
				{
					continue;
				}
			}
			if (page == WinPageShow::PAGE_LIST_APP && m_strSTARRunwayFilter.size() > 0 && strRnw.size() > 0)
			{
				if (strRnw.find(m_strSTARRunwayFilter) == std::string::npos)
				{
					continue;
				}
			}
			if (strRnw.size() > 0)
			{
				strRnw = c.second.name + m_cLinkString + strRnw;
			}
			else
			{
				strRnw = c.second.name;
			}
			
			AppendListItem(strRnw.c_str());
		}
	}
		break;
	case WinPageShow::PAGE_LIST_SID_TRANS:
	case WinPageShow::PAGE_LIST_STAR_TRANS:
	case WinPageShow::PAGE_LIST_APP_TRANS:
		m_pListBox->Clear();
		AppendListItem("No Transition");
		for (auto &c : m_selectInfo.program.mapTrans)
		{
			AppendListItem(c.second.transName);
		}
		break;
	case WinPageShow::PAGE_LIST_SID_RNW:
	case WinPageShow::PAGE_LIST_STAR_RNW:
	case WinPageShow::PAGE_LIST_APP_RNW:
		m_pListBox->Clear();
		for (auto &c : m_selectInfo.program.mapRunway)
		{
			AppendListItem(c.first.c_str());
		}
		break;
	default:
		break;
	}

	this->SetVisible(true);
}

void CHmsRouteNodeWin::OnListBoxItemClick(CHmsListBoxItem *pItem, int index, bool selected)
{
	if (!selected || index < 0 || !pItem)
	{
		return;
	}
	if (m_curPage == WinPageShow::PAGE_LIST_SID || 
		m_curPage == WinPageShow::PAGE_LIST_STAR ||
		m_curPage == WinPageShow::PAGE_LIST_APP)
	{
		WinPageShow page_trans, page_runway;
		if (m_curPage == WinPageShow::PAGE_LIST_SID)
		{
			page_trans = WinPageShow::PAGE_LIST_SID_TRANS;
			page_runway = WinPageShow::PAGE_LIST_SID_RNW;	
		}
		if (m_curPage == WinPageShow::PAGE_LIST_STAR)
		{
			page_trans = WinPageShow::PAGE_LIST_STAR_TRANS;
			page_runway = WinPageShow::PAGE_LIST_STAR_RNW;
		}
		if (m_curPage == WinPageShow::PAGE_LIST_APP)
		{
			page_trans = WinPageShow::PAGE_LIST_APP_TRANS;
			page_runway = WinPageShow::PAGE_LIST_APP_RNW;
		}

		auto itemText = pItem->GetText();
		auto pos = itemText.find(m_cLinkString);
		if (pos != std::string::npos)
		{
			itemText = itemText.substr(0, pos);
		}

		auto iter = m_mapProgramInfo.find(itemText);
		if (iter != m_mapProgramInfo.end())
		{
			m_selectInfo.program = iter->second;

			if (m_selectInfo.program.mapTrans.size() > 0)
			{
				ShowPage(page_trans, m_showPageNode, m_curNodeIndex);
			}
			else
			{
				m_selectInfo.programTrans.ClearName();
				m_selectInfo.programTrans.vWayPoint.clear();

				if (m_selectInfo.program.mapRunway.size() > 1)
				{
					ShowPage(page_runway, m_showPageNode, m_curNodeIndex);
				}
				else
				{
					if (m_selectInfo.program.mapRunway.size() > 0)
					{
						m_selectInfo.programRunway = m_selectInfo.program.mapRunway.begin()->second;
					}
					else
					{
						m_selectInfo.programRunway = McduRunwayInfo();
					}
					
					//选择结束
					SelectSIDSTARFinished();
				}
			}		
		}
	}
	else if (m_curPage == WinPageShow::PAGE_LIST_SID_TRANS ||
		m_curPage == WinPageShow::PAGE_LIST_STAR_TRANS ||
		m_curPage == WinPageShow::PAGE_LIST_APP_TRANS)
	{
		WinPageShow page_runway;
		if (m_curPage == WinPageShow::PAGE_LIST_SID_TRANS)
		{
			page_runway = WinPageShow::PAGE_LIST_SID_RNW;
		}
		if (m_curPage == WinPageShow::PAGE_LIST_STAR_TRANS)
		{
			page_runway = WinPageShow::PAGE_LIST_STAR_RNW;
		}
		if (m_curPage == WinPageShow::PAGE_LIST_APP_TRANS)
		{
			page_runway = WinPageShow::PAGE_LIST_APP_RNW;
		}
		auto iter = m_selectInfo.program.mapTrans.find(pItem->GetText());
		if (iter != m_selectInfo.program.mapTrans.end())
		{
			m_selectInfo.programTrans = iter->second;
		}
		else
		{
			m_selectInfo.programTrans.ClearName();
			m_selectInfo.programTrans.vWayPoint.clear();
		}

		if (m_selectInfo.program.mapRunway.size() > 1)
		{
			ShowPage(page_runway, m_showPageNode, m_curNodeIndex);
		}
		else
		{
			if (m_selectInfo.program.mapRunway.size() > 0)
			{
				m_selectInfo.programRunway = m_selectInfo.program.mapRunway.begin()->second;
			}
			else
			{
				m_selectInfo.programRunway = McduRunwayInfo();
			}
			//选择结束
			SelectSIDSTARFinished();
		}
	}
	else if (m_curPage == WinPageShow::PAGE_LIST_SID_RNW ||
		m_curPage == WinPageShow::PAGE_LIST_STAR_RNW ||
		m_curPage == WinPageShow::PAGE_LIST_APP_RNW)
	{
		auto iter = m_selectInfo.program.mapRunway.find(pItem->GetText());
		if (iter != m_selectInfo.program.mapRunway.end())
		{
			m_selectInfo.programRunway = iter->second;
		}
		else
		{
			m_selectInfo.programRunway.ClearName();
			m_selectInfo.programRunway.vWayPoint.clear();
		}
		//选择结束
		SelectSIDSTARFinished();
	}
	else
	{
#if 0
		if (0 == index)
		{
            if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
			{
                CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertFPLNodeByInputRouteString(FPLNodeIndex(m_curNodeIndex, -1));
			}
			this->SetVisible(false);
		}
		else if (1 == index)
		{
            if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
			{
                CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertFPLNodeByInputRouteString(FPLNodeIndex(m_curNodeIndex + 1, -1));
			}
			this->SetVisible(false);
		}
		else
		{
#endif
			switch (m_curPage)
			{
			case WinPageShow::PAGE_ACTION_SID:
			case WinPageShow::PAGE_ACTION_CHGSID:

				m_mapProgramInfo.clear();
				CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_showPageNode.belongAirport, ROUTE_TYPE_DEP_SID, m_mapProgramInfo);

				ShowPage(WinPageShow::PAGE_LIST_SID, m_showPageNode, m_curNodeIndex);

				break;
			case WinPageShow::PAGE_ACTION_STARAPP:
			case WinPageShow::PAGE_ACTION_CHGSTAR:
			case WinPageShow::PAGE_ACTION_CHGAPP:
			{
				auto text = pItem->GetText();
				if (text == m_cSelectArrival || text == m_cChangeArrival)
				{
					m_mapProgramInfo.clear();
					CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_showPageNode.belongAirport, ROUTE_TYPE_ARR_STAR, m_mapProgramInfo);
					ShowPage(WinPageShow::PAGE_LIST_STAR, m_showPageNode, m_curNodeIndex);
				}
				else if (text == m_cSelectApproach || text == m_cChangeApproach)
				{
					m_mapProgramInfo.clear();
					CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_showPageNode.belongAirport, ROUTE_TYPE_ARR_APPR, m_mapProgramInfo);
					ShowPage(WinPageShow::PAGE_LIST_APP, m_showPageNode, m_curNodeIndex);
				}
			}
				break;
			default:
				break;
			}
#if 0
		}
#endif
	}
}

void CHmsRouteNodeWin::SelectSIDSTARFinished()
{
	auto ConvertStu = [=](std::vector<WptNodeStu2D> &dst, std::vector<McduWayCtrlPoint> &src, const std::string &airport)
	{
		for (auto &c : src)
		{
			//app程序跑道后面还有点
			if (c.eFixType == FIX_TYPE_RNW)
			{
				McduAirportRunway runwayInfo;
				if (CHms424Database::GetInstance()->GetRunwayInfo(airport, c.fix.strFixName, runwayInfo))
				{
					c.fix.coordFix.dLatitude = runwayInfo.dLatitude;
					c.fix.coordFix.dLongitude = runwayInfo.dLongitude;
					c.nHeading = runwayInfo.nRunwayDirection;
					c.fDistance = runwayInfo.nRunwayLength;
					c.nAltitude[0].nAltitude = runwayInfo.nAltitude;
					c.ctrl.nAltitudeCnt = 1;
				}
				dst.push_back(WptNodeStu2D(c));
			}
			else
			{
				dst.push_back(WptNodeStu2D(c));
			}	
		}
	};

	FPL2DNodeInfoStu info;
	info.ePriType = FPL_TYPE_PROGRAM;
	info.belongAirport = m_showPageNode.belongAirport;
	info.programName = m_selectInfo.program.name;
	info.transName = m_selectInfo.programTrans.transName;
	info.rnwName = m_selectInfo.programRunway.runwayName;

	if (m_selectInfo.program.eRouteType == ROUTE_TYPE_DEP_SID)
	{
		info.eSubType = FPL_TYPE_PROGRAM_SID;

		//离场程序在前面插入跑道
		McduWayCtrlPoint runwayPoint;
		runwayPoint.eFixType = FIX_TYPE_RNW;
		//strcpy(runwayPoint.fix.strFixName, info.rnwName.c_str());
		runwayPoint.fix.SetName(info.rnwName);
		
		m_selectInfo.programRunway.vWayPoint.insert(m_selectInfo.programRunway.vWayPoint.begin(), runwayPoint);

		ConvertStu(info.vWayPoint, m_selectInfo.programRunway.vWayPoint, info.belongAirport);
		ConvertStu(info.vWayPoint, m_selectInfo.program.vWayPoint, info.belongAirport);
		ConvertStu(info.vWayPoint, m_selectInfo.programTrans.vWayPoint, info.belongAirport);
		if (info.transName.empty())
		{
			info.ident = info.rnwName + "." + info.programName;
		}
		else
		{
			info.ident = info.rnwName + "." + info.programName + "." + info.transName;
		} 
	}
	else if (m_selectInfo.program.eRouteType == ROUTE_TYPE_ARR_STAR)
	{
		info.eSubType = FPL_TYPE_PROGRAM_STAR;

		//进场程序在后面插入跑道
		McduWayCtrlPoint runwayPoint;
		runwayPoint.eFixType = FIX_TYPE_RNW;
		//strcpy(runwayPoint.fix.strFixName, info.rnwName.c_str());
		runwayPoint.fix.SetName(info.rnwName);

		m_selectInfo.programRunway.vWayPoint.insert(m_selectInfo.programRunway.vWayPoint.end(), runwayPoint);

		ConvertStu(info.vWayPoint, m_selectInfo.programTrans.vWayPoint, info.belongAirport);
		ConvertStu(info.vWayPoint, m_selectInfo.program.vWayPoint, info.belongAirport);
		ConvertStu(info.vWayPoint, m_selectInfo.programRunway.vWayPoint, info.belongAirport);
		if (info.transName.empty())
		{
			info.ident = info.programName + "." + info.rnwName;
		}
		else
		{
			info.ident = info.transName + "." + info.programName + "." + info.rnwName;
		}
	}
	else if (m_selectInfo.program.eRouteType == ROUTE_TYPE_ARR_APPR)
	{
		info.eSubType = FPL_TYPE_PROGRAM_APP;

		//进近程序已经有跑道了，不用插入
		ConvertStu(info.vWayPoint, m_selectInfo.programTrans.vWayPoint, info.belongAirport);
		ConvertStu(info.vWayPoint, m_selectInfo.program.vWayPoint, info.belongAirport);
		if (info.transName.empty())
		{
			info.ident = info.programName;
		}
		else
		{
			info.ident = info.transName + "." + info.programName;
		}
	}
	if (info.vWayPoint.size() > 0)
	{
        if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
		{
			//如果第一个点就是hold， 在前面插入一个点
			if (info.vWayPoint[0].ctrlPoint.eFixType == FIX_TYPE_HOLD_DEST ||
				info.vWayPoint[0].ctrlPoint.eFixType == FIX_TYPE_HOLD_TIME)
			{
				info.vWayPoint.insert(info.vWayPoint.begin(), info.vWayPoint[0]);
				info.vWayPoint[0].ctrlPoint.eFixType = FIX_TYPE_FIX;
			}

			ProcessSpecialPoint(info.vWayPoint);

			//共用段 跑道段 过度段 接头处有重复的点，这里删除重复的
			std::vector<int> vecDel;
            for (int i = 1; i < (int)info.vWayPoint.size(); ++i)
			{
				if (info.vWayPoint[i].brief.ident == info.vWayPoint[i - 1].brief.ident && 
					info.vWayPoint[i].ctrlPoint.eFixType != FIX_TYPE_HOLD_TIME && 
					info.vWayPoint[i].ctrlPoint.eFixType != FIX_TYPE_HOLD_DEST)
				{
					vecDel.push_back(i);
				}
				//数据错误的点
				if (info.vWayPoint[i].ctrlPoint.eFixType == FIX_TYPE_INIT)
				{
					vecDel.push_back(i);
				}
			}
			for (int i = vecDel.size() - 1; i >= 0; --i)
			{
				info.vWayPoint.erase(info.vWayPoint.begin() + vecDel[i]);
			}

            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertProgramInfo(info, FPLNodeIndex(m_curNodeIndex, -1));
		}
	}

	this->SetVisible(false);
}

void CHmsRouteNodeWin::SetSTARRunwayFilter(std::string str)
{
	m_strSTARRunwayFilter = str;
}

void CHmsRouteNodeWin::SetAPPRunwayFilter(std::string str)
{
	m_strAPPRunwayFilter = str;
}

void CHmsRouteNodeWin::UseProgram(std::string airport, std::string program, std::string trans, std::string runway, FPL2DNodeType type, int index)
{
	m_curNodeIndex = index;
	m_showPageNode.belongAirport = airport;

	std::map<std::string, McduProgramInfo> mapProgramInfo;

	if (type == FPL_TYPE_PROGRAM_SID)
	{
		CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_showPageNode.belongAirport, ROUTE_TYPE_DEP_SID, mapProgramInfo);
	}
	else if (type == FPL_TYPE_PROGRAM_STAR)
	{
		CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_showPageNode.belongAirport, ROUTE_TYPE_ARR_STAR, mapProgramInfo);
	}
	else if (type == FPL_TYPE_PROGRAM_APP)
	{
		CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_showPageNode.belongAirport, ROUTE_TYPE_ARR_APPR, mapProgramInfo);
	}
	std::map<std::string, McduProgramInfo>::iterator iter = mapProgramInfo.find(program);
	if (iter != mapProgramInfo.end())
	{
		m_selectInfo.program = iter->second;

		auto transIter = m_selectInfo.program.mapTrans.find(trans);
		if (transIter != m_selectInfo.program.mapTrans.end())
		{
			m_selectInfo.programTrans = transIter->second;
		}
		else
		{
			m_selectInfo.programTrans = McduTRANSITIONInfo();
		}
		
		auto runwayIter = m_selectInfo.program.mapRunway.find(runway);
		if (runwayIter != m_selectInfo.program.mapRunway.end())
		{
			m_selectInfo.programRunway = runwayIter->second;
		}
		else
		{
			m_selectInfo.programRunway = McduRunwayInfo();
		}

		SelectSIDSTARFinished();
	}

	this->SetVisible(false);
}

void CHmsRouteNodeWin::ProcessSpecialPoint(std::vector<WptNodeStu2D> &vecWptNode)
{
    for (int i = 1; i < (int)vecWptNode.size(); ++i)
	{
		auto &curWptNode = vecWptNode[i];
		auto &curCtrlPoint = vecWptNode[i].ctrlPoint;
		if (curCtrlPoint.eFixType == FIX_TYPE_COURSE_DISTANCE_FIX)
		{
			auto lastPoint = vecWptNode[i - 1].ctrlPoint;
			auto lastLonLat = Vec2(lastPoint.fix.coordFix.dLongitude, lastPoint.fix.coordFix.dLatitude);
			auto curLonLat = Vec2(curCtrlPoint.fix.coordFix.dLongitude, curCtrlPoint.fix.coordFix.dLatitude);

			Vec2 lonLat;
			auto dis1 = CalculateDistanceNM(lastLonLat.x, lastLonLat.y, curLonLat.x, curLonLat.y);
			bool bLess = dis1 < curCtrlPoint.fDistance ? true : false;
			Vec2 tmpLastLonLat;
			double tmpLastLen = 0;
			for (int i = 1; i < 1000; ++i)
			{
				auto tmp = CHmsNavMathHelper::NextPointLonLatByBearing(lastLonLat, i * 0.2 * KnotKMH, curCtrlPoint.nHeading);
				auto len = CalculateDistanceNM(tmp.x, tmp.y, curLonLat.x, curLonLat.y);
				if (bLess && len >= curCtrlPoint.fDistance)
				{
					if (fabs(tmpLastLen - curCtrlPoint.fDistance) <= fabs(len - curCtrlPoint.fDistance))
					{
						lonLat = tmpLastLonLat;
					}
					else
					{
						lonLat = tmp;
					}
					break;
				}
				if ((!bLess) && len <= curCtrlPoint.fDistance)
				{
					if (fabs(tmpLastLen - curCtrlPoint.fDistance) <= fabs(len - curCtrlPoint.fDistance))
					{
						lonLat = tmpLastLonLat;
					}
					else
					{
						lonLat = tmp;
					}
					break;
				}
				tmpLastLonLat = tmp;
				tmpLastLen = len;
			}

			char tmp[128] = {0};
			sprintf(tmp, "%s D%1.1f", curCtrlPoint.fix.strFixName, curCtrlPoint.fDistance);
			//strcpy(curCtrlPoint.fix.strFixName, tmp);
			std::string tmpStr = tmp;
			curCtrlPoint.fix.SetName(tmpStr);

			curCtrlPoint.fix.coordFix.dLongitude = lonLat.x;
			curCtrlPoint.fix.coordFix.dLatitude = lonLat.y;
			curWptNode.brief.ident = tmp;
			curWptNode.brief.lon = lonLat.x;
			curWptNode.brief.lat = lonLat.y;
		}
		else if (curCtrlPoint.eFixType == FIX_TYPE_COURSE_ALTITUDE)
		{
			auto lastPoint = vecWptNode[i - 1].ctrlPoint;
			auto lastLonLat = Vec2(lastPoint.fix.coordFix.dLongitude, lastPoint.fix.coordFix.dLatitude);
			float distance = 1.5f * KnotKMH; //km
			//计算上升距离
			if (vecWptNode[i - 1].ctrlPoint.ctrl.nAltitudeCnt >= 1)
			{
				float height = curCtrlPoint.nAltitude[0].nAltitude - vecWptNode[i - 1].ctrlPoint.nAltitude[0].nAltitude;
				height = height / MeterFeet;
				distance = height / tan(MATH_DEG_TO_RAD(3)) / 1000.0; 
			}
			auto lonLat = CHmsNavMathHelper::NextPointLonLatByBearing(lastLonLat, distance, curCtrlPoint.nHeading);

			char tmp[128] = {0};
			sprintf(tmp, "%dFT", curCtrlPoint.nAltitude[0].nAltitude);
			//strcpy(curCtrlPoint.fix.strFixName, tmp);
			std::string tmpStr = tmp;
			curCtrlPoint.fix.SetName(tmpStr);

			curCtrlPoint.fix.coordFix.dLongitude = lonLat.x;
			curCtrlPoint.fix.coordFix.dLatitude = lonLat.y;
			curWptNode.brief.ident = tmp;
			curWptNode.brief.lon = lonLat.x;
			curWptNode.brief.lat = lonLat.y;
		}
		else if (curCtrlPoint.eFixType == FIX_TYPE_COURSE_VECTORS)
		{
			auto lastPoint = vecWptNode[i - 1].ctrlPoint;
			auto lastLonLat = Vec2(lastPoint.fix.coordFix.dLongitude, lastPoint.fix.coordFix.dLatitude);
			auto lonLat = CHmsNavMathHelper::NextPointLonLatByBearing(lastLonLat, 3 * KnotKMH, curCtrlPoint.nHeading);

			curCtrlPoint.fix.coordFix.dLongitude = lonLat.x;
			curCtrlPoint.fix.coordFix.dLatitude = lonLat.y;
			curWptNode.brief.ident = "MANSEQ";
			curWptNode.brief.lon = lonLat.x;
			curWptNode.brief.lat = lonLat.y;
		}
		else if (curCtrlPoint.eFixType == FIX_TYPE_COURSE_RADIAL_TO_FIX || curCtrlPoint.eFixType == FIX_TYPE_COURSE_RADIAL_FROM_FIX)
		{
			if (i - 1 < 0)
			{
				continue;
			}
			if (curWptNode.ctrlPoint.nHeading == -1 || curWptNode.ctrlPoint.nLegData == -1)
			{
				continue;
			}
			auto &lastWpt = vecWptNode[i - 1];
			auto legData = curWptNode.ctrlPoint.nLegData;
			std::string direct = "FROM";
			if (curWptNode.ctrlPoint.eFixType == FIX_TYPE_COURSE_RADIAL_TO_FIX)
			{
				legData += 180;
				direct = "TO";
			}

			auto GetWptNormal = [](const WptNodeStu2D &wptNode, float angle)
			{
				Vec2 lonLat1 = Vec2(wptNode.brief.lon, wptNode.brief.lat);
				Vec2 lonLat2 = CHmsNavMathHelper::NextPointLonLatByBearing(lonLat1, 10, angle);
				Vec3 p1 = CHmsNavMathHelper::LonLatToEarthPoint(lonLat1);
				Vec3 p2 = CHmsNavMathHelper::LonLatToEarthPoint(lonLat2);
				Vec3 normal1;
				Vec3::cross(p1, p2, &normal1);
				return normal1.getNormalized();
			};
			Vec3 normal1 = GetWptNormal(lastWpt, curWptNode.ctrlPoint.nHeading);
			Vec3 normal2 = GetWptNormal(curWptNode, legData);
			Vec3 crossLine;
			Vec3::cross(normal1, normal2, &crossLine);
			crossLine.normalize();

			Ray ray( -2 * crossLine, crossLine);
			Sphere sphere(Vec3(0, 0, 0), 1.0f);
			Vec3 crossFirst, crossSecond;
			if (ray.intersects(sphere, crossFirst, crossSecond))
			{
				crossFirst.normalize();
				crossSecond.normalize();

				Vec2 lonLat = CHmsNavMathHelper::EarthPointToLonLat(crossFirst);
				//会有两个交点，判断一下
				if (fabsf(lonLat.x - curWptNode.brief.lon) > 150)
				{
					lonLat = CHmsNavMathHelper::EarthPointToLonLat(crossSecond);
				}

				char tmp[128] = { 0 };
				sprintf(tmp, "%d-%d %s %s",
					curWptNode.ctrlPoint.nHeading,
					curWptNode.ctrlPoint.nLegData,
					direct.c_str(),
					curWptNode.ctrlPoint.fix.strFixName
					);

				std::string tmpStr = tmp;
				curCtrlPoint.fix.SetName(tmpStr);

				curCtrlPoint.fix.coordFix.dLongitude = lonLat.x;
				curCtrlPoint.fix.coordFix.dLatitude = lonLat.y;
				curWptNode.brief.ident = tmp;
				curWptNode.brief.lon = lonLat.x;
				curWptNode.brief.lat = lonLat.y;
			}	
		}
	}
}
