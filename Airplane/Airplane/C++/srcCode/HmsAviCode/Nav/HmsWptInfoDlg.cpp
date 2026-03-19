#include "HmsWptInfoDlg.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsNavComm.h"
#include "NavSvs/HmsNavSvsLayer.h"
#include "ToolBar/HmsAvionicsDisplayMgr.h"
#include "NavSvs/HmsNavSvsLoadDataThread.h"

CHmsWptInfoDlg::CHmsWptInfoDlg()
: HmsUiEventInterface(this)
, m_callbackFuncAdd(nullptr)
, m_lonLatSpace(2)
{

}

CHmsWptInfoDlg::~CHmsWptInfoDlg()
{

}

bool CHmsWptInfoDlg::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	const float marginLeft = 10;
	const float marginBottom = 90;

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
	pBgNode->DrawSolidRect(
		Rect(marginLeft, marginBottom, windowSize.width - marginLeft*2, windowSize.height - marginBottom*2),
		Color4F(Color3B::GRAY));

	const Size buttonSize = Size((windowSize.width - 10) / 4 - 10, 70);

	auto CreateButtonFunc = [=](const char *text, Vec2 pos, WptDlgPage page){

		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
			buttonSize, Color3B(0x00, 0x98, 0xff), Color3B::GRAY);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(text, 32, Color4B::BLACK);
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetPosition(pos);
			pItem->SetOnClickedFunc(std::bind(&CHmsWptInfoDlg::ShowPage, this, std::placeholders::_1, page));
			this->AddChild(pItem);
		}

		return pItem;
	};

    m_pButtonALL = CreateButtonFunc("ALL", Vec2(marginLeft + (buttonSize.width + marginLeft) * 0, marginLeft), WptDlgPage::PAGE_ALL);
    m_pButtonAPT = CreateButtonFunc("APT", Vec2(marginLeft + (buttonSize.width + marginLeft) * 1, marginLeft), WptDlgPage::PAGE_APT);
    m_pButtonNAV = CreateButtonFunc("NAV", Vec2(marginLeft + (buttonSize.width + marginLeft) * 2, marginLeft), WptDlgPage::PAGE_NAV);
    m_pButtonWPT = CreateButtonFunc("WPT", Vec2(marginLeft + (buttonSize.width + marginLeft) * 3, marginLeft), WptDlgPage::PAGE_WPT);

	m_pButtonCancel = CreateButtonFunc(
		"Cancel", 
		Vec2(marginLeft, windowSize.height - marginLeft - buttonSize.height), 
		WptDlgPage::PAGE_EXIT_CANCEL);

	m_pButtonDelete = CreateButtonFunc(
		"Delete", 
		Vec2(windowSize.width - marginLeft - buttonSize.width, windowSize.height - marginLeft - buttonSize.height), 
		WptDlgPage::PAGE_EXIT_DELETE);

    m_pButtonRoam = CreateButtonFunc(
        "Roam",
        Vec2(windowSize.width - marginLeft - buttonSize.width, windowSize.height - marginLeft - buttonSize.height),
        WptDlgPage::PAGE_EXIT_DELETE);
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pButtonRoam)
		return false;
#endif
    m_pButtonRoam->SetOnClickedFunc([=](CHmsUiButtonAbstract*)
    {
        CHmsNavSvsLayer::GetInstance()->SetRoamMode(m_lonlatClick.x, m_lonlatClick.y);
        CHmsAvionicsDisplayMgr::GetInstance()->GetToolBarBottom()->SetCurrentMode("3DView");
    });

	m_pLabelCancel = CHmsGlobal::CreateLanguageLabel("Add To Route", 32);
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLabelCancel)
		return false;
#endif
	m_pLabelCancel->SetAnchorPoint(0.5, 0.5);
	m_pLabelCancel->SetPosition(windowSize.width / 2, windowSize.height - marginBottom / 2);
	this->AddChild(m_pLabelCancel);

	m_pLabelDelete = CHmsGlobal::CreateLanguageLabel("Replace", 32);
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLabelDelete)
		return false;
#endif
	m_pLabelDelete->SetAnchorPoint(0.5, 0.5);
	m_pLabelDelete->SetPosition(windowSize.width / 2, windowSize.height - marginBottom / 2);
	this->AddChild(m_pLabelDelete);
	
	InitListBox(Rect(marginLeft, marginBottom, windowSize.width - marginLeft * 2, windowSize.height - marginBottom*2));

	this->RegisterAllNodeChild();

	return true;
}

void CHmsWptInfoDlg::InitListBox(const HmsAviPf::Rect & windowRect)
{
	m_listSize = windowRect.size - Size(20, 20);

	auto CreateListFunc = [=](){

		auto temp = CHmsUiVerticalScrollView::Create(m_listSize);
#if _NAV_SVS_LOGIC_JUDGE
		if (temp)
#endif
		{
			this->AddChild(temp);
			temp->SetAnchorPoint(Vec2(0, 0));
			temp->SetPosition(windowRect.origin + Vec2(10, 10));
			temp->SetItemVSpace(5);
		}
		return temp;
	};
	m_pList = CreateListFunc();


	for (int i = 0; i < 31; ++i)
	{
		auto pButton = CreateRouteItemButton(WptPointType::WPT_AIRPORT, "text", "text");
		m_vecUiButtonPool.pushBack(pButton);
	}
	m_nCurButtonPoolIndex = 0;
}

void CHmsWptInfoDlg::ShowPage(CHmsUiButtonAbstract *pBtn, WptDlgPage page)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pButtonALL
		|| !m_pButtonAPT
		|| !m_pButtonNAV
		|| !m_pButtonWPT)
		return;
#endif
	auto SetDataSource = [=](WptDlgPage t){

		m_nCurButtonPoolIndex = 0;
#if _NAV_SVS_LOGIC_JUDGE
		if (m_pList)
#endif
		m_pList->RemoveAllItem();
		m_wptNodeMap.clear();
		for (int i = 0; i < (int)m_vecAllNodeInfo.size(); ++i)
		{
			m_pList->RemoveAllItem();
			m_wptNodeMap.clear();
			for (unsigned int i = 0; i < m_vecAllNodeInfo.size(); ++i)
			{
				const WptNodeStu &stu = m_vecAllNodeInfo.at(i);
				if (i == 0)
				{
					auto temp = GetOrCreateUiButton(WptPointType::WPT_USERWPT, stu.des1.c_str(), stu.des2.c_str());
					m_pList->AddWindowItem(temp);
					m_wptNodeMap[temp] = stu;
					continue;
				}
				if (
					(t == WptDlgPage::PAGE_ALL) ||
					(t == WptDlgPage::PAGE_APT && stu.brief.wType == WptPointType::WPT_AIRPORT) ||
					(t == WptDlgPage::PAGE_NAV && (stu.brief.wType == WptPointType::WPT_VOR || stu.brief.wType == WptPointType::WPT_NDB)) ||
					(t == WptDlgPage::PAGE_WPT && (stu.brief.wType == WptPointType::WPT_WPT || stu.brief.wType == WptPointType::WPT_USERWPT))
					)
				{
					auto temp = GetOrCreateUiButton(stu.brief.wType, stu.des1.c_str(), stu.des2.c_str());
					m_pList->AddWindowItem(temp);
					m_wptNodeMap[temp] = stu;
				}
			}
			m_pList->ScrollToTop();
		}
	};
	m_pButtonALL->UnSelected();
	m_pButtonAPT->UnSelected();
	m_pButtonNAV->UnSelected();
	m_pButtonWPT->UnSelected();
	switch (page)
	{
	case WptDlgPage::PAGE_ALL:
		m_pButtonALL->Selected();
		SetDataSource(WptDlgPage::PAGE_ALL);
		break;
	case WptDlgPage::PAGE_APT:
		m_pButtonAPT->Selected();
		SetDataSource(WptDlgPage::PAGE_APT);
		break;
	case WptDlgPage::PAGE_NAV:
		m_pButtonNAV->Selected();
		SetDataSource(WptDlgPage::PAGE_NAV);
		break;
	case WptDlgPage::PAGE_WPT:
		m_pButtonWPT->Selected();
		SetDataSource(WptDlgPage::PAGE_WPT);
		break;
	case WptDlgPage::PAGE_EXIT_CANCEL:
		this->SetVisible(false);
		if (m_callbackFuncCancel)
		{
			m_callbackFuncCancel(m_callbackParam);
		}
		break;
	case WptDlgPage::PAGE_EXIT_DELETE:
		this->SetVisible(false);
		if (m_callbackFuncDelete)
		{
			m_callbackFuncDelete(m_callbackParam);
		}
		break;
	default:
		break;
	}
}

void CHmsWptInfoDlg::ShowDlg(float lon, float lat, WptDlgMode dlgMode, int params)
{
	m_callbackParam = params;

    m_lonlatClick = Vec2(lon, lat);

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pButtonCancel
		|| !m_pLabelCancel
		|| !m_pButtonDelete
		|| !m_pLabelDelete
		|| !m_pButtonRoam)
		return;
#endif

	if (dlgMode == WptDlgMode::WPTDLGMODE_ADD)
	{
		m_pButtonCancel->SetVisible(true);
		m_pLabelCancel->SetVisible(true);
		m_pButtonDelete->SetVisible(false);
		m_pLabelDelete->SetVisible(false);

        m_pButtonRoam->SetVisible(true);
	}
	else
	{
		m_pButtonCancel->SetVisible(true);
		m_pLabelCancel->SetVisible(false);
		m_pButtonDelete->SetVisible(true);
		m_pLabelDelete->SetVisible(true);

        m_pButtonRoam->SetVisible(false);
	}
	this->SetVisible(true);

	m_vecAllNodeInfo.clear();
	for (int i = 0; i < 4; ++i)
	{
		std::vector<WptNodeStu> vecRet;
		if (0 == i)
		{
			GetVorInfo(lon, lat, vecRet);
		}
		else if (1 == i)
		{
			GetAirport(lon, lat, vecRet);
		}
		else if (2 == i)
		{
			GetWaypoint(lon, lat, vecRet);
		}
		else if (3 == i)
		{
			GetNDBInfo(lon, lat, vecRet);
		}
		if (vecRet.size() > 10)
		{
			m_vecAllNodeInfo.insert(m_vecAllNodeInfo.end(), vecRet.begin(), vecRet.begin() + 10);
		}
		else
		{
			m_vecAllNodeInfo.insert(m_vecAllNodeInfo.end(), vecRet.begin(), vecRet.end());
		}
	}
	std::sort(m_vecAllNodeInfo.begin(), m_vecAllNodeInfo.end(), [](const WptNodeStu &stuA, const WptNodeStu &stuB){
		return stuA.distance < stuB.distance;
	});
	{
		auto clickStr = CHmsNavMathHelper::LonLatToString(lon, lat);
		WptNodeStu node;
		node.brief.wType = WptPointType::WPT_USERWPT;
		node.brief.lon = lon;
		node.brief.lat = lat;
		node.brief.ident = clickStr;

        Vec2 lonlat = Vec2(lon, lat);
        int altitude = CHmsGlobal::GetInstance()->GetTerrainServer()->GetAltitude(&lonlat);
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            char tmp[128] = { 0 };
            sprintf(tmp, "/%dm", altitude);
            std::string destmp = clickStr + tmp;
            node.des1 = destmp;
            node.des2 = destmp;
        }
        else
        {
            char tmp[128] = { 0 };
            sprintf(tmp, "/%dft", (int)MeterToFeet(altitude));
            std::string destmp = clickStr + tmp;
            node.des1 = destmp;
            node.des2 = destmp;
        }
		
		m_vecAllNodeInfo.insert(m_vecAllNodeInfo.begin(), node);
	}

	ShowPage(m_pButtonAPT, WptDlgPage::PAGE_NAV);
}

void CHmsWptInfoDlg::GetVorInfo(float lon, float lat, std::vector<WptNodeStu> &vecRet)
{
	auto dbInstance = CHms424Database::GetInstance();
#if _NAV_SVS_LOGIC_JUDGE
	if (!dbInstance)
		return;
#endif
	std::vector<T_VHFNavaid> vecVor;
	dbInstance->GetVHFNavaidData(vecVor, lat - m_lonLatSpace, lat + m_lonLatSpace, lon - m_lonLatSpace, lon + m_lonLatSpace);
    for (int i = 0; i < (int)vecVor.size(); ++i)
	{
		const T_VHFNavaid &stu = vecVor.at(i);
		double angle = CalculateAngle(lon, lat, stu.Longitude, stu.Latitude) / SIM_Pi * 180;
		double dis = CalculateDistanceNM(lon, lat, stu.Longitude, stu.Latitude);

		WptNodeStu node;
		node.brief.wType = WptPointType::WPT_VOR;
		node.brief.lon = stu.Longitude;
		node.brief.lat = stu.Latitude;
		node.brief.ident = stu.VorIdent;
		node.distance = dis;
		node.angle = angle;
		node.chType = stu.chType;
		node.freq = stu.VorFrequency;
		node.name = stu.VorName;

        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            node.distance = KnotToKMH(node.distance);
        }
        auto tmpUnit = CHmsNavComm::GetInstance()->AppendNMOrKMUnit("%0.2f", node.distance);
        node.des1 = CHmsGlobal::FormatString("%s - %s, %0.0f\xC2\xB0", node.brief.ident.c_str(), tmpUnit.c_str(), node.angle);
		node.des2 = CHmsGlobal::FormatString("%s; %0.2f; %s", node.chType.c_str(), node.freq, node.name.c_str());

		vecRet.push_back(node);
	}
	std::sort(vecRet.begin(), vecRet.end(), [](const WptNodeStu &stuA, const WptNodeStu &stuB){
		return stuA.distance < stuB.distance;
	});
}

void CHmsWptInfoDlg::GetNDBInfo(float lon, float lat, std::vector<WptNodeStu> &vecRet)
{
	auto dbInstance = CHms424Database::GetInstance();
#if _NAV_SVS_LOGIC_JUDGE
	if (!dbInstance)
		return;
#endif
	std::vector<T_NDBNavaid> vecNDB;
	dbInstance->GetNDBNavaidData(vecNDB, lat - m_lonLatSpace, lat + m_lonLatSpace, lon - m_lonLatSpace, lon + m_lonLatSpace);
    for (int i = 0; i < (int)vecNDB.size(); ++i)
	{
		const T_NDBNavaid &stu = vecNDB.at(i);
		double angle = CalculateAngle(lon, lat, stu.Longitude, stu.Latitude) / SIM_Pi * 180;
		double dis = CalculateDistanceNM(lon, lat, stu.Longitude, stu.Latitude);

		WptNodeStu node;
		node.brief.wType = WptPointType::WPT_NDB;
		node.brief.lon = stu.Longitude;
		node.brief.lat = stu.Latitude;
		node.brief.ident = stu.NDBIdent;
		node.distance = dis;
		node.angle = angle;
		node.chType = stu.chType;
		node.freq = stu.NDBFrequency;
		node.name = stu.NDBName;

        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            node.distance = KnotToKMH(node.distance);
        }
        auto tmpUnit = CHmsNavComm::GetInstance()->AppendNMOrKMUnit("%0.2f", node.distance);
		node.des1 = CHmsGlobal::FormatString("%s - %s, %0.0f\xC2\xB0", node.brief.ident.c_str(), tmpUnit.c_str(), node.angle);
		node.des2 = CHmsGlobal::FormatString("%s; %0.2f; %s", node.chType.c_str(), node.freq, node.name.c_str());

		vecRet.push_back(node);
	}
	std::vector<T_TerminalNDBNavaid> vecTerminalNDB;
	dbInstance->GetTerminalNDBNavaid(vecTerminalNDB, lat - m_lonLatSpace, lat + m_lonLatSpace, lon - m_lonLatSpace, lon + m_lonLatSpace);
    for (int i = 0; i < (int)vecTerminalNDB.size(); ++i)
	{
		const T_TerminalNDBNavaid &stu = vecTerminalNDB.at(i);
		double angle = CalculateAngle(lon, lat, stu.Longitude, stu.Latitude) / SIM_Pi * 180;
		double dis = CalculateDistanceNM(lon, lat, stu.Longitude, stu.Latitude);

		WptNodeStu node;
		node.brief.wType = WptPointType::WPT_NDB;
		node.brief.lon = stu.Longitude;
		node.brief.lat = stu.Latitude;
		node.brief.ident = stu.NDBIdent;
		node.distance = dis;
		node.angle = angle;
		node.chType = stu.chType;
		node.freq = stu.NDBFrequency;
		node.name = stu.NDBName;

        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            node.distance = KnotToKMH(node.distance);
        }
        auto tmpUnit = CHmsNavComm::GetInstance()->AppendNMOrKMUnit("%0.2f", node.distance);
		node.des1 = CHmsGlobal::FormatString("%s - %s, %0.0f\xC2\xB0", node.brief.ident.c_str(), tmpUnit.c_str(), node.angle);
		node.des2 = CHmsGlobal::FormatString("%s; %0.2f; %s", node.chType.c_str(), node.freq, node.name.c_str());

		vecRet.push_back(node);
	}

	std::sort(vecRet.begin(), vecRet.end(), [](const WptNodeStu &stuA, const WptNodeStu &stuB){
		return stuA.distance < stuB.distance;
	});
}

void CHmsWptInfoDlg::GetAirport(float lon, float lat, std::vector<WptNodeStu> &vecRet)
{
	auto dbInstance = CHms424Database::GetInstance();
#if _NAV_SVS_LOGIC_JUDGE
	if (!dbInstance)
		return;
#endif
	std::vector<T_Airport> vecAPT;
	dbInstance->GetAirportData(vecAPT, lat - m_lonLatSpace, lat + m_lonLatSpace, lon - m_lonLatSpace, lon + m_lonLatSpace);
    for (int i = 0; i < (int)vecAPT.size(); ++i)
	{
		const T_Airport &stu = vecAPT.at(i);
		double angle = CalculateAngle(lon, lat, stu.Longitude, stu.Latitude) / SIM_Pi * 180;
		double dis = CalculateDistanceNM(lon, lat, stu.Longitude, stu.Latitude);

		WptNodeStu node;
		node.brief.wType = WptPointType::WPT_AIRPORT;
		node.brief.lon = stu.Longitude;
		node.brief.lat = stu.Latitude;
		node.brief.ident = stu.AirportIdent;
		node.distance = dis;
		node.angle = angle;
		node.chType = stu.AreaCode;
		node.freq = 0;
		node.name = stu.AirportName;

        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            node.distance = KnotToKMH(node.distance);
        }
        auto tmpUnit = CHmsNavComm::GetInstance()->AppendNMOrKMUnit("%0.2f", node.distance);
		node.des1 = CHmsGlobal::FormatString("%s - %s, %0.0f\xC2\xB0", node.brief.ident.c_str(), tmpUnit.c_str(), node.angle);
		node.des2 = CHmsGlobal::FormatString("%s; %s", node.chType.c_str(), node.name.c_str());

		vecRet.push_back(node);
	}
	std::sort(vecRet.begin(), vecRet.end(), [](const WptNodeStu &stuA, const WptNodeStu &stuB){
		return stuA.distance < stuB.distance;
	});
}

void CHmsWptInfoDlg::GetWaypoint(float lon, float lat, std::vector<WptNodeStu> &vecRet)
{
	auto dbInstance = CHms424Database::GetInstance();
#if _NAV_SVS_LOGIC_JUDGE
	if (!dbInstance)
		return;
#endif
	std::vector<T_AirportWaypoint> vecWPT;
	dbInstance->GetAirportWaypointData(vecWPT, lat - m_lonLatSpace, lat + m_lonLatSpace, lon - m_lonLatSpace, lon + m_lonLatSpace);
	dbInstance->GetEnrouteWaypointData(vecWPT, lat - m_lonLatSpace, lat + m_lonLatSpace, lon - m_lonLatSpace, lon + m_lonLatSpace);
    for (int i = 0; i < (int)vecWPT.size(); ++i)
	{
		const T_AirportWaypoint &stu = vecWPT.at(i);
		double angle = CalculateAngle(lon, lat, stu.Longitude, stu.Latitude) / SIM_Pi * 180;
		double dis = CalculateDistanceNM(lon, lat, stu.Longitude, stu.Latitude);

		WptNodeStu node;
		node.brief.wType = WptPointType::WPT_WPT;
		node.brief.lon = stu.Longitude;
		node.brief.lat = stu.Latitude;
		node.brief.ident = stu.WpIdent;
		node.distance = dis;
		node.angle = angle;
		node.chType = stu.WpType;
		node.freq = 0;
		node.name = stu.RegionCode;

        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            node.distance = KnotToKMH(node.distance);
        }
        auto tmpUnit = CHmsNavComm::GetInstance()->AppendNMOrKMUnit("%0.2f", node.distance);
		node.des1 = CHmsGlobal::FormatString("%s - %s, %0.0f\xC2\xB0", node.brief.ident.c_str(), tmpUnit.c_str(), node.angle);
		node.des2 = CHmsGlobal::FormatString("%s; %s", node.chType.c_str(), node.name.c_str());

		vecRet.push_back(node);
	}

	GetUserWptInfo(lon, lat, vecRet);

	std::sort(vecRet.begin(), vecRet.end(), [](const WptNodeStu &stuA, const WptNodeStu &stuB){
		return stuA.distance < stuB.distance;
	});
}

void CHmsWptInfoDlg::GetUserWptInfo(float lon, float lat, std::vector<WptNodeStu> &vecRet)
{
	std::vector<UserWptStu> vecUserWpt;
	CHmsHistoryRouteDatabase::GetInstance()->GetUserWpt(vecUserWpt);
    for (int i = 0; i < (int)vecUserWpt.size(); ++i)
	{
		const UserWptStu &stu = vecUserWpt.at(i);
		if (fabsf(stu.lon - lon) <= m_lonLatSpace && fabsf(stu.lat - lat) <= m_lonLatSpace)
		{
			double angle = CalculateAngle(lon, lat, stu.lon, stu.lat) / SIM_Pi * 180;
			double dis = CalculateDistanceNM(lon, lat, stu.lon, stu.lat);

			WptNodeStu node;
			node.brief.wType = WptPointType::WPT_USERWPT;
			node.brief.lon = stu.lon;
			node.brief.lat = stu.lat;
			node.brief.ident = stu.name;
			node.distance = dis;
			node.angle = angle;

            if (CHmsNavComm::GetInstance()->IsMetric())
            {
                node.distance = KnotToKMH(node.distance);
            }
            auto tmpUnit = CHmsNavComm::GetInstance()->AppendNMOrKMUnit("%0.2f", node.distance);
			node.des1 = CHmsGlobal::FormatString("%s - %s, %0.0f\xC2\xB0", node.brief.ident.c_str(), tmpUnit.c_str(), node.angle);
			node.des2 = CHmsGlobal::FormatString("%s", CHmsNavMathHelper::LonLatToString(stu.lon, stu.lat).c_str());

			vecRet.push_back(node);
		}
	}
}

const char * CHmsWptInfoDlg::GetImagePathByWptType(WptPointType t)
{
	switch (t)
	{
	case WptPointType::WPT_VOR:
		return "res/NavImage/navHexagon.png";
		break;
	case WptPointType::WPT_WPT:
		return "res/NavImage/navTriangle.png";
		break;
	case WptPointType::WPT_USERWPT:
		return "res/NavImage/navDiamond.png";
		break;
	case WptPointType::WPT_NDB:
		return "res/NavImage/navRing.png";
		break;
	case WptPointType::WPT_AIRPORT:
		return "res/NavImage/navCicle.png";
		break;
	default:
		break;
	}
	return "res/NavImage/navCicle.png";
}

CHmsUiStretchButton * CHmsWptInfoDlg::CreateRouteItemButton(WptPointType t, const char *text1, const char *text2)
{
	auto pButton = CHmsUiStretchButton::Create("res/NavImage/green_edit.png",
		Size(m_listSize.width, 90)/*, Color3B(0xa8, 0x9c, 0x56), Color3B(0xa8, 0x9c, 0x56)*/);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pButton)
		return pButton;
#endif
	pButton->SetAnchorPoint(Vec2(0, 1));
	pButton->SetOnClickedFunc([=](CHmsUiButtonAbstract *pBtn){

		CHmsUiStretchButton *pTemp = dynamic_cast<CHmsUiStretchButton *>(pBtn);
		if (pTemp)
		{
			auto iter = m_wptNodeMap.find(pTemp);
			if (iter != m_wptNodeMap.end() && m_callbackFuncAdd)
			{
				m_callbackFuncAdd(iter->second.brief.wType, iter->second.brief.lon, iter->second.brief.lat, iter->second.brief.ident, m_callbackParam);
				this->SetVisible(false);
			}
		}
	});
	pButton->AddIcon(GetImagePathByWptType(t));
	auto temp = pButton->GetIconNode();
#if _NAV_SVS_LOGIC_JUDGE
	if (!temp)
		return pButton;
#endif
    temp->SetAnchorPoint(Vec2(0.5, 0.5));
	temp->SetPosition(20, 45);
	{
		auto label = CHmsGlobal::CreateLabel(text1, 26, Color4F::BLACK);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return pButton;
#endif
		label->UpdateTextString();
		label->SetAnchorPoint(0, 0);
		label->SetPosition(40, 45);
		label->SetTag(101);
		pButton->AddChild(label);
	}
	{
		auto label = CHmsGlobal::CreateLabel(text2, 20, Color4F::BLACK);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return pButton;
#endif
		label->UpdateTextString();
		label->SetAnchorPoint(0, 0);
		label->SetPosition(40, 10);
		label->SetTag(102);
		pButton->AddChild(label);
	}

	return pButton;
}

CHmsUiStretchButton * CHmsWptInfoDlg::GetOrCreateUiButton(WptPointType t, const char *text1, const char *text2)
{
	if (m_nCurButtonPoolIndex >= m_vecUiButtonPool.size())
	{
		auto pButton = CreateRouteItemButton(t, text1, text2);
#if _NAV_SVS_LOGIC_JUDGE
		if (pButton)
#endif
		m_vecUiButtonPool.pushBack(pButton);
	}
	auto pButton = m_vecUiButtonPool.at(m_nCurButtonPoolIndex);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pButton)
		return pButton;
#endif
	++m_nCurButtonPoolIndex;

	auto temp = pButton->GetIconNode();
#if _NAV_SVS_LOGIC_JUDGE
	if (!temp)
		return pButton;
#endif
    temp->InitWithFile(GetImagePathByWptType(t));
	{
		auto node = pButton->GetChildByTag(101);
		if (node)
		{
			auto label = dynamic_cast<CHmsLabel*>(node);
			if (label)
			{
				label->SetString(text1);
				label->UpdateTextString();
			}
		}
	}
	{
		auto node = pButton->GetChildByTag(102);
		if (node)
		{
			auto label = dynamic_cast<CHmsLabel*>(node);
			if (label)
			{
				label->SetString(text2);
				label->UpdateTextString();
			}
		}
	}


	return pButton;
}

bool CHmsWptInfoDlg::OnPressed(const Vec2 & posOrigin)
{
	HmsUiEventInterface::OnPressed(posOrigin);
	return true;
}



