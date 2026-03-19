#include "HmsProcedureDlg.h"
#include "../HmsFrame2DRoot.h"
#include "Language/HmsLanguageMananger.h"
#include "../HmsRouteNodeWin.h"

CHmsProcedureDlg::CHmsProcedureDlg()
: HmsUiEventInterface(this)
{
	m_pProcedureEarth2D = nullptr;
}

CHmsProcedureDlg::~CHmsProcedureDlg()
{
}

bool CHmsProcedureDlg::Init(const HmsAviPf::Size & size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(size);

	float topHeight = 110;

	Vec2 useArea_pos = Vec2(10, 10);
	Size useArea_size = size - Size(20, 10 + topHeight);

	auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (pBgNode)
#endif
	{
		pBgNode->SetAnchorPoint(Vec2(0, 0));
		pBgNode->SetPosition(0, 0);
		this->AddChild(pBgNode);
		pBgNode->DrawSolidRect(Rect(Vec2(0, 0), size), Color4F(Color3B(0x0e, 0x19, 0x25)));
		pBgNode->DrawSolidRect(Rect(useArea_pos.x, useArea_pos.y, useArea_size.width, useArea_size.height),
			Color4F(Color3B::GRAY));
		//画边框
		{
			std::vector<Vec2> vec;
			vec.push_back(Vec2(0, 0));
			vec.push_back(Vec2(size.width, 0));
			vec.push_back(Vec2(size.width, size.height));
			vec.push_back(Vec2(0, size.height));
			vec.push_back(Vec2(0, 0));
			pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
		}
	}

	auto CreateButton = [=](Size s, Vec2 pos, const char * text)
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(text, 32, Color4B::WHITE, true);
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetPosition(pos);
			this->AddChild(pItem);
		}
		return pItem;
	};

	Size btnSize = Size(260, 90);
	CHmsUiStretchButton *pItem = nullptr;
	pItem = CreateButton(btnSize, Vec2(useArea_pos.x, useArea_pos.y + useArea_size.height + 10), "Back");
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetTextColor(Color4B::GREEN);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			BackTo();
		});
		pItem = CreateButton(btnSize, Vec2(useArea_pos.x + useArea_size.width - btnSize.width, useArea_pos.y + useArea_size.height + 10), "Close");
		pItem->SetTextColor(Color4B::RED);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			this->SetVisible(false);
		});
	}


	auto titleLabel = CHmsGlobal::CreateLanguageLabel("Procedure Preview", 32, true);
#if _NAV_SVS_LOGIC_JUDGE
	if (titleLabel)
#endif
	{
		titleLabel->SetAnchorPoint(0.5, 0);
		titleLabel->SetPosition(Vec2(useArea_pos.x + useArea_size.width * 0.5, useArea_pos.y + useArea_size.height + 10));
		titleLabel->SetTextColor(Color4B::WHITE);
		this->AddChild(titleLabel);
	}


	InitModules(Rect(useArea_pos, useArea_size));

	this->RegisterAllNodeChild();
	return true;
}

void CHmsProcedureDlg::InitModules(const HmsAviPf::Rect & windowRect)
{
	InitLeftList(windowRect);
	InitRightEarth(windowRect);
}

void CHmsProcedureDlg::InitLeftList(const HmsAviPf::Rect & windowRect)
{
	Size buttonSize = Size(400, 95);
	Size boxSize = Size(600, windowRect.size.height - buttonSize.height);

	auto pListBox = CHmsListBox::CreateWithImage(
		NULL,
		NULL,
		"res/airport/blue_selbox.png",
		"res/airport/star_normal.png",
		NULL);
#if _NAV_SVS_LOGIC_JUDGE
	if (pListBox)
#endif
	{
		this->AddChild(pListBox);
		m_pListBox = pListBox;

		pListBox->SetBoxSize(boxSize);
		pListBox->SetScrollbarWidth(4);
		pListBox->SetBacktopButtonVisible(false);
		pListBox->SetSelectWithCheck(true);
		pListBox->SetItemHeight(95);
		pListBox->SetAnchorPoint(Vec2(0, 0));
		pListBox->SetPosition(windowRect.origin + Vec2(0, buttonSize.height));
		pListBox->SetItemNormalTextColor(Color4B::BLACK);
		pListBox->SetItemSelectChangeCallBack(HMS_CALLBACK_3(CHmsProcedureDlg::OnListBoxItemClick, this));
	}

	auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
		buttonSize - Size(0, 20), Color3B(0x35, 0x98, 0xdc), Color3B(0x1e, 0x37, 0x4f));
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->AddText("Add To Route", 32, Color4B::WHITE, true);
		pItem->SetAnchorPoint(Vec2(0.5, 0));
		pItem->SetPosition(windowRect.origin.x + boxSize.width / 2, windowRect.origin.y + 10);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			SelectSIDSTARFinished();
		});
		this->AddChild(pItem);

		m_pBtnAddToRoute = pItem;
	}
}

void CHmsProcedureDlg::InitRightEarth(const HmsAviPf::Rect & windowRect)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pListBox)
		return;
#endif
	auto listBoxSzie = m_pListBox->GetContentSize();

	auto earthFrame = CHmsNavEarthFrame::Create(windowRect.size - Size(listBoxSzie.width, 0));
#if _NAV_SVS_LOGIC_JUDGE
	if (!earthFrame)
		return;
#endif
	CHmsProcedureEarth2D *pFrame2D = nullptr;
	pFrame2D = CHmsProcedureEarth2D::Create(earthFrame->GetCamera(), earthFrame->GetContentSize());
#if _NAV_SVS_LOGIC_JUDGE
	if (!pFrame2D)
		return;
#endif
	pFrame2D->SetAnchorPoint(Vec2(0, 0));
	pFrame2D->SetPosition(Vec2(0, 0));
	earthFrame->AddChild(pFrame2D);
	earthFrame->SetFrame2DRoot(pFrame2D);

	auto navNode = earthFrame->CreateNavViewer();
#if _NAV_SVS_LOGIC_JUDGE
	if (!navNode)
		return;
#endif
	navNode->SetAnchorPoint(Vec2(0, 0));
	navNode->SetPosition(windowRect.origin.x + listBoxSzie.width, windowRect.origin.y);
	navNode->AddChild(earthFrame);
	this->AddChild(navNode);

	m_pEarthFrame = earthFrame;
	m_pProcedureEarth2D = pFrame2D;

	{
		auto pItem = CHmsFixSizeUiImageButton::CreateWithImage("res/airport/imagePage/zoomIn.png",Size(50, 50));
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return;
#endif
		pItem->SetColor(Color3B(0, 71, 157));
		pItem->SetCallbackFunc(HMS_CALLBACK_2(CHmsProcedureDlg::ChangeBtnColor, this));
		pItem->SetAnchorPoint(Vec2(0.5, 0.5));
		pItem->SetPosition(windowRect.origin.x + windowRect.size.width - 35, windowRect.origin.y + windowRect.size.height - 35);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			if (m_pEarthFrame)
			{
				m_pEarthFrame->MoveNear();
			}
		});
		this->AddChild(pItem);
	}
	{
		auto pItem = CHmsFixSizeUiImageButton::CreateWithImage("res/airport/imagePage/zoomOut.png", Size(50, 50));
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return;
#endif
		pItem->SetColor(Color3B(0, 71, 157));
		pItem->SetCallbackFunc(HMS_CALLBACK_2(CHmsProcedureDlg::ChangeBtnColor, this));
		pItem->SetAnchorPoint(Vec2(0.5, 0.5));
		pItem->SetPosition(windowRect.origin.x + windowRect.size.width - 35, windowRect.origin.y + windowRect.size.height - 95);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			if (m_pEarthFrame)
			{
				m_pEarthFrame->MoveFar();
			}
		});
		this->AddChild(pItem);
	}
}

void CHmsProcedureDlg::ChangeBtnColor(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	auto pNode = dynamic_cast<CHmsNode*>(pBtn);
	switch (state)
	{
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_normal:
		if (pNode)
		{
			pNode->SetColor(Color3B(0, 71, 157));
			pNode->SetScale(1.0f);
		}
		break;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected:
		if (pNode)
		{
			pNode->SetColor(Color3B::WHITE);
			pNode->SetScale(1.1f);
		}
		break;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_disable:
		break;
	default:
		break;
	}
}

bool CHmsProcedureDlg::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
	HmsUiEventInterface::OnPressed(posOrigin);
	return true;
}

void CHmsProcedureDlg::SetSIDSTARAirport(ProcedureParamStu sid, ProcedureParamStu star)
{
	m_airportSID = sid;
	m_airportSTAR = star;
}

void CHmsProcedureDlg::ShowPage(ProcedurePageShow page)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pListBox)
		return;
#endif
	auto AppendListItem = [=](const char * text)
	{
        m_pListBox->AppendItem(text, false, false, 32, true);
	};

	//不可见表示是打开窗口
	if (!this->IsVisible())
	{
		auto str = CHmsMapTileSql::GetInstance().GetMapTypeName();
		if (m_lastMapTypeName != str)
		{
			m_lastMapTypeName = str;
			if (m_pEarthFrame)
			{
				m_pEarthFrame->ReloadEarthTile();
			}
		}
	}

	this->SetVisible(true);
	m_curPage = page;
	m_pListBox->Clear();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnAddToRoute)
		return;
#endif
	m_pBtnAddToRoute->SetVisible(false);

	if (page == ProcedurePageShow::PAGE_LIST_BRIEF)
	{
		if (m_airportSID.airportIdent.size() > 0)
		{
			m_mapProgramInfo.clear();
			CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_airportSID.airportIdent, ROUTE_TYPE_DEP_SID, m_mapProgramInfo);

			std::stringstream stream;
			stream << m_cDepartureStr << " (" << m_mapProgramInfo.size() << ")" << m_cLinkString << "From " << m_airportSID.airportIdent;
			AppendListItem(stream.str().c_str());
		}
		if (m_airportSTAR.airportIdent.size() > 0)
		{
			{
				m_mapProgramInfo.clear();
				CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_airportSTAR.airportIdent, ROUTE_TYPE_ARR_STAR, m_mapProgramInfo);

				std::stringstream stream;
				stream << m_cArrivalStr << " (" << m_mapProgramInfo.size() << ")" << m_cLinkString << "Into " << m_airportSTAR.airportIdent;
				AppendListItem(stream.str().c_str());
			}
			{
				m_mapProgramInfo.clear();
				CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_airportSTAR.airportIdent, ROUTE_TYPE_ARR_APPR, m_mapProgramInfo);

				std::stringstream stream;
				stream << m_cApproachStr << " (" << m_mapProgramInfo.size() << ")" << m_cLinkString << "Into " << m_airportSTAR.airportIdent;
				AppendListItem(stream.str().c_str());
			}
		}

		McduProgramInfo mcduProgramInfo;
		SetRouteDataToEarth2D(mcduProgramInfo, "", "");
	}
	else if (page == ProcedurePageShow::PAGE_LIST_SID || 
		page == ProcedurePageShow::PAGE_LIST_STAR || 
		page == ProcedurePageShow::PAGE_LIST_APP)
	{
		m_pListBox->Clear();
		
		for (auto &c : m_mapProgramInfo)
		{
			std::string strRnw;
			for (auto &r : c.second.mapRunway)
			{
				strRnw += r.first + " ";
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

		if (m_pEarthFrame)
		{
			if (page == ProcedurePageShow::PAGE_LIST_SID)
			{
				m_pEarthFrame->SetPlaneAlwaysCenter(false);
				m_pEarthFrame->SetViewCenterByLonLat(m_airportSID.lon, m_airportSID.lat);
				m_pEarthFrame->SetCurLayerToTen();
			}
			else
			{
				m_pEarthFrame->SetPlaneAlwaysCenter(false);
				m_pEarthFrame->SetViewCenterByLonLat(m_airportSTAR.lon, m_airportSTAR.lat);
				m_pEarthFrame->SetCurLayerToTen();
			}
		}
	}
	else if (page == ProcedurePageShow::PAGE_LIST_SID_TRANS ||
		page == ProcedurePageShow::PAGE_LIST_STAR_TRANS ||
		page == ProcedurePageShow::PAGE_LIST_APP_TRANS)
	{
		AppendListItem("No Transition");
		for (auto &c : m_selectInfo.program.mapTrans)
		{
			AppendListItem(c.second.transName);
		}
	}
	else if (page == ProcedurePageShow::PAGE_LIST_SID_RNW ||
		page == ProcedurePageShow::PAGE_LIST_STAR_RNW ||
		page == ProcedurePageShow::PAGE_LIST_APP_RNW)
	{
		for (auto &c : m_selectInfo.program.mapRunway)
		{
			AppendListItem(c.first.c_str());
		}
	}
}

void CHmsProcedureDlg::OnListBoxItemClick(CHmsListBoxItem *pItem, int index, bool selected)
{
	if (!selected || index < 0 || !pItem)
	{
		return;
	}
	if (m_curPage == ProcedurePageShow::PAGE_LIST_BRIEF)
	{
		auto text = pItem->GetText();
		if (text.find(m_cDepartureStr) == 0)
		{
			m_mapProgramInfo.clear();
			CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_airportSID.airportIdent, ROUTE_TYPE_DEP_SID, m_mapProgramInfo);
			ShowPage(ProcedurePageShow::PAGE_LIST_SID);
			SetRouteDataToEarth2D(m_mapProgramInfo);
		}
		else if (text.find(m_cArrivalStr) == 0)
		{
			m_mapProgramInfo.clear();
			CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_airportSTAR.airportIdent, ROUTE_TYPE_ARR_STAR, m_mapProgramInfo);
			ShowPage(ProcedurePageShow::PAGE_LIST_STAR);
			SetRouteDataToEarth2D(m_mapProgramInfo);
		}
		else if (text.find(m_cApproachStr) == 0)
		{
			m_mapProgramInfo.clear();
			CHmsSSADatabase::GetInstance()->GetMcduProgramInfo(m_airportSTAR.airportIdent, ROUTE_TYPE_ARR_APPR, m_mapProgramInfo);
			ShowPage(ProcedurePageShow::PAGE_LIST_APP);
			SetRouteDataToEarth2D(m_mapProgramInfo);
		}
	}
	else if (m_curPage == ProcedurePageShow::PAGE_LIST_SID ||
		m_curPage == ProcedurePageShow::PAGE_LIST_STAR ||
		m_curPage == ProcedurePageShow::PAGE_LIST_APP)
	{
		ProcedurePageShow page_trans, page_runway;
		if (m_curPage == ProcedurePageShow::PAGE_LIST_SID)
		{
			page_trans = ProcedurePageShow::PAGE_LIST_SID_TRANS;
			page_runway = ProcedurePageShow::PAGE_LIST_SID_RNW;
		}
		if (m_curPage == ProcedurePageShow::PAGE_LIST_STAR)
		{
			page_trans = ProcedurePageShow::PAGE_LIST_STAR_TRANS;
			page_runway = ProcedurePageShow::PAGE_LIST_STAR_RNW;
		}
		if (m_curPage == ProcedurePageShow::PAGE_LIST_APP)
		{
			page_trans = ProcedurePageShow::PAGE_LIST_APP_TRANS;
			page_runway = ProcedurePageShow::PAGE_LIST_APP_RNW;
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
				ShowPage(page_trans);
				SetRouteDataToEarth2D(m_selectInfo.program, m_cAll, m_cAll);
			}
			else
			{
				m_selectInfo.programTrans.ClearName();
				m_selectInfo.programTrans.vWayPoint.clear();

				if (m_selectInfo.program.mapRunway.size() > 1)
				{
					ShowPage(page_runway);
					SetRouteDataToEarth2D(m_selectInfo.program, m_selectInfo.programTrans.transName, m_cAll);
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
					ShowAddToRouteButton();
					SetRouteDataToEarth2D(m_selectInfo.program, m_selectInfo.programTrans.transName, m_selectInfo.programRunway.runwayName);
				}
			}
		}
	}
	else if (m_curPage == ProcedurePageShow::PAGE_LIST_SID_TRANS ||
		m_curPage == ProcedurePageShow::PAGE_LIST_STAR_TRANS ||
		m_curPage == ProcedurePageShow::PAGE_LIST_APP_TRANS)
	{
		ProcedurePageShow page_runway;
		if (m_curPage == ProcedurePageShow::PAGE_LIST_SID_TRANS)
		{
			page_runway = ProcedurePageShow::PAGE_LIST_SID_RNW;
		}
		if (m_curPage == ProcedurePageShow::PAGE_LIST_STAR_TRANS)
		{
			page_runway = ProcedurePageShow::PAGE_LIST_STAR_RNW;
		}
		if (m_curPage == ProcedurePageShow::PAGE_LIST_APP_TRANS)
		{
			page_runway = ProcedurePageShow::PAGE_LIST_APP_RNW;
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
			ShowPage(page_runway);
			SetRouteDataToEarth2D(m_selectInfo.program, m_selectInfo.programTrans.transName, m_cAll);
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
			ShowAddToRouteButton();
			SetRouteDataToEarth2D(m_selectInfo.program, m_selectInfo.programTrans.transName, m_selectInfo.programRunway.runwayName);
		}
	}
	else if (m_curPage == ProcedurePageShow::PAGE_LIST_SID_RNW ||
		m_curPage == ProcedurePageShow::PAGE_LIST_STAR_RNW ||
		m_curPage == ProcedurePageShow::PAGE_LIST_APP_RNW)
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
		ShowAddToRouteButton();
		SetRouteDataToEarth2D(m_selectInfo.program, m_selectInfo.programTrans.transName, m_selectInfo.programRunway.runwayName);
	}
}

void CHmsProcedureDlg::ShowAddToRouteButton()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnAddToRoute)
		return;
#endif
	m_pBtnAddToRoute->SetVisible(true);
}

void CHmsProcedureDlg::SelectSIDSTARFinished()
{
    if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
	{
		if (m_selectInfo.program.eRouteType == ROUTE_TYPE_DEP_SID)
		{
            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertProgram(
				m_airportSID.airportIdent, 
				m_selectInfo.program.name, 
				m_selectInfo.programTrans.transName, 
				m_selectInfo.programRunway.runwayName, 
				FPL_TYPE_PROGRAM_SID);
		}
		else if (m_selectInfo.program.eRouteType == ROUTE_TYPE_ARR_STAR)
		{
            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertProgram(
				m_airportSTAR.airportIdent,
				m_selectInfo.program.name,
				m_selectInfo.programTrans.transName,
				m_selectInfo.programRunway.runwayName,
				FPL_TYPE_PROGRAM_STAR);
		}
		else if (m_selectInfo.program.eRouteType == ROUTE_TYPE_ARR_APPR)
		{
            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertProgram(
				m_airportSTAR.airportIdent,
				m_selectInfo.program.name, 
				m_selectInfo.programTrans.transName, 
				m_selectInfo.programRunway.runwayName,
				FPL_TYPE_PROGRAM_APP);
		}	
	}

	this->SetVisible(false);
}

void CHmsProcedureDlg::BackTo()
{
	switch (m_curPage)
	{
	case ProcedurePageShow::PAGE_LIST_SID:
	case ProcedurePageShow::PAGE_LIST_STAR:
	case ProcedurePageShow::PAGE_LIST_APP:
		ShowPage(ProcedurePageShow::PAGE_LIST_BRIEF);
		break;

	case ProcedurePageShow::PAGE_LIST_SID_RNW:
		if (m_selectInfo.program.mapTrans.size() > 0)
		{
			ShowPage(ProcedurePageShow::PAGE_LIST_SID_TRANS);
		}
		else
		{
			ShowPage(ProcedurePageShow::PAGE_LIST_SID);
		}
		break;
	case ProcedurePageShow::PAGE_LIST_SID_TRANS:
		ShowPage(ProcedurePageShow::PAGE_LIST_SID);
		break;
	
	case ProcedurePageShow::PAGE_LIST_STAR_RNW:
		if (m_selectInfo.program.mapTrans.size() > 0)
		{
			ShowPage(ProcedurePageShow::PAGE_LIST_STAR_TRANS);
		}
		else
		{
			ShowPage(ProcedurePageShow::PAGE_LIST_STAR);
		}
		break;
	case ProcedurePageShow::PAGE_LIST_STAR_TRANS:
		ShowPage(ProcedurePageShow::PAGE_LIST_STAR);
		break;
	
	case ProcedurePageShow::PAGE_LIST_APP_RNW:
		if (m_selectInfo.program.mapTrans.size() > 0)
		{
			ShowPage(ProcedurePageShow::PAGE_LIST_APP_TRANS);
		}
		else
		{
			ShowPage(ProcedurePageShow::PAGE_LIST_APP);
		}
		break;
	case ProcedurePageShow::PAGE_LIST_APP_TRANS:
		ShowPage(ProcedurePageShow::PAGE_LIST_APP);
		break;
	
	default:
		break;
	}
}

void CHmsProcedureDlg::SetRouteDataToEarth2D(std::map<std::string, McduProgramInfo> &mapProgram)
{
	int maxCount = mapProgram.size() > 50 ? 50 : mapProgram.size();
	std::vector<PreviewRouteStu> vec(maxCount);
	int i = 0;
	for (auto &c : mapProgram)
	{
		TransMcduToWpt2DNode(vec[i], c.second, m_cAll, m_cAll);
		++i;
		if (i >= maxCount)
		{
			break;
		}
	}
	if (m_pProcedureEarth2D)
	{
		m_pProcedureEarth2D->SetRouteData(vec);
	}
}

void CHmsProcedureDlg::SetRouteDataToEarth2D(McduProgramInfo &program, std::string transName, std::string runwayName)
{
	std::vector<PreviewRouteStu> vec;
	vec.resize(1);
	TransMcduToWpt2DNode(vec[0], program, transName, runwayName);
	if (m_pProcedureEarth2D)
	{
		m_pProcedureEarth2D->SetRouteData(vec);
	}
}


void CHmsProcedureDlg::TransMcduToWpt2DNode(PreviewRouteStu &stu, McduProgramInfo program, std::string transName, std::string runwayName)
{
	stu.programName = program.name;
	stu.proType = program.eRouteType;

	std::map< std::string, std::vector<WptNodeStu2D> > mapBase;
	for (auto &rnw : program.mapRunway)
	{
		if (runwayName == m_cAll || runwayName == rnw.first)
		{
			std::vector<WptNodeStu2D> vecbase;
			if (program.eRouteType == ROUTE_TYPE_DEP_SID)
			{
				auto vecWayPoint = rnw.second.vWayPoint;
				vecWayPoint.insert(vecWayPoint.end(), program.vWayPoint.begin(), program.vWayPoint.end());

				McduWayCtrlPoint runwayPoint;
				runwayPoint.eFixType = FIX_TYPE_RNW;
				runwayPoint.fix.SetName(rnw.first);
				vecWayPoint.insert(vecWayPoint.begin(), runwayPoint);

				ConvertMCDUToWPT2D(vecbase, vecWayPoint, m_airportSID.airportIdent, program.eRouteType);
			}
			else if (program.eRouteType == ROUTE_TYPE_ARR_STAR)
			{
				auto vecWayPoint = rnw.second.vWayPoint;
				vecWayPoint.insert(vecWayPoint.begin(), program.vWayPoint.begin(), program.vWayPoint.end());

				McduWayCtrlPoint runwayPoint;
				runwayPoint.eFixType = FIX_TYPE_RNW;
				runwayPoint.fix.SetName(rnw.first);
				vecWayPoint.insert(vecWayPoint.end(), runwayPoint);

				ConvertMCDUToWPT2D(vecbase, vecWayPoint, m_airportSTAR.airportIdent, program.eRouteType);
			}
			else if (program.eRouteType == ROUTE_TYPE_ARR_APPR)
			{
				auto vecWayPoint = program.vWayPoint;
				ConvertMCDUToWPT2D(vecbase, vecWayPoint, m_airportSTAR.airportIdent, program.eRouteType);
			}

			CHmsRouteNodeWin::ProcessSpecialPoint(vecbase);	

			mapBase.insert(std::make_pair(rnw.first, vecbase));

			char tmp[128] = {0};
			sprintf(tmp, "No Transition %s", rnw.first.c_str());
			stu.mapRoute.insert(std::make_pair(tmp, vecbase));
		}
	}

	for (auto &trans : program.mapTrans)
	{
		if (transName == m_cAll || transName == trans.first)
		{
			for (auto &rnw : mapBase)
			{
				std::vector<WptNodeStu2D> vecTrans;
				if (program.eRouteType == ROUTE_TYPE_DEP_SID)
				{
					ConvertMCDUToWPT2D(vecTrans, trans.second.vWayPoint, m_airportSID.airportIdent, program.eRouteType);
					//vecTrans.insert(vecTrans.begin(), rnw.second.begin(), rnw.second.end());
				}
				else if (program.eRouteType == ROUTE_TYPE_ARR_STAR || program.eRouteType == ROUTE_TYPE_ARR_APPR)
				{
					ConvertMCDUToWPT2D(vecTrans, trans.second.vWayPoint, m_airportSTAR.airportIdent, program.eRouteType);
					//vecTrans.insert(vecTrans.end(), rnw.second.begin(), rnw.second.end());
				}
				CHmsRouteNodeWin::ProcessSpecialPoint(vecTrans);

				char tmp[128] = {0};
				sprintf(tmp, "%s %s", trans.first.c_str(), rnw.first.c_str());
				stu.mapRoute.insert(std::make_pair(tmp, vecTrans));
			}
		}
	}
}

void CHmsProcedureDlg::ConvertMCDUToWPT2D(
	std::vector<WptNodeStu2D> &dst, std::vector<McduWayCtrlPoint> &src, const std::string &airport, ROUTE_TYPE type)
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

	//删除数据错误的点
	for (int i = dst.size() - 1; i >= 0; --i)
	{
		if (dst[i].ctrlPoint.eFixType == FIX_TYPE_INIT)
		{
			dst.erase(dst.begin() + i);
		}
	}
}
