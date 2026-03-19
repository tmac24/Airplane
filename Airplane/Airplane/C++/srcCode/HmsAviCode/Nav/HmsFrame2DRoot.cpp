#include "HmsFrame2DRoot.h"
#include "HmsNavInfoWindow.h"
#include "../FMS/HmsFMS.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsNavFplLayer.h"

USING_NS_HMS;

CHmsFrame2DRoot::CHmsFrame2DRoot()
: CHmsFrame2DInterface(this)
, m_earthLayer(0)
, m_planeYaw(0)
, m_funcDragButtonRelease(nullptr)
, m_bIsPressOnDragButton(false)
, m_bPlaneLonLatValid(false)
{

}

CHmsFrame2DRoot::~CHmsFrame2DRoot()
{

}

CHmsFrame2DRoot* CHmsFrame2DRoot::Create(HmsAviPf::Camera *pCamera, Size layerSize)
{
	CHmsFrame2DRoot *ret = new (std::nothrow) CHmsFrame2DRoot();
	if (ret && ret->Init(pCamera, layerSize) && pCamera)
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsFrame2DRoot::Init(HmsAviPf::Camera *pCamera, Size layerSize)
{
	if (!CHmsFrame2DInterface::Init(pCamera, layerSize))
	{
		return false;
	}
    CHmsNavComm::GetInstance()->SetCHmsFrame2DRoot(this);

	int localZOrder = 10000;

	m_pAdminAreasLayer = CHmsFrame2DAdminAreasLayer::Create(layerSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pAdminAreasLayer)
#endif
	{
	m_pAdminAreasLayer->SetAnchorPoint(Vec2(0, 0));
	m_pAdminAreasLayer->SetLocalZOrder(localZOrder++);
	m_pAdminAreasLayer->SetFrame2DRoot(this);
	m_pAdminAreasLayer->HideAdminAreasLayer();
	m_vecChildLayer.push_back(m_pAdminAreasLayer);
	this->AddChild(m_pAdminAreasLayer);
	}
    m_pAftTrackLayer = CHmsFrame2DAftTrackLayer::Create(layerSize);
    m_pAftTrackLayer->SetAnchorPoint(Vec2(0, 0));
    m_pAftTrackLayer->SetLocalZOrder(localZOrder++);
    m_pAftTrackLayer->SetFrame2DRoot(this);
    m_pAftTrackLayer->ShowTrackLayer();
    m_vecChildLayer.push_back(m_pAftTrackLayer);
    this->AddChild(m_pAftTrackLayer);

#if 0
    auto pADSBLayer = CHmsFrame2DADSBLayer::Create(layerSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (pADSBLayer)
#endif
	{
		pADSBLayer->SetAnchorPoint(Vec2(0, 0));
		pADSBLayer->SetLocalZOrder(localZOrder++);
		pADSBLayer->SetFrame2DRoot(this);
		m_vecChildLayer.push_back(pADSBLayer);
		this->AddChild(pADSBLayer);
	}
#endif

	auto pAirplane = CHmsFrame2DAirplaneLayer::Create(layerSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (pAirplane)
#endif
	{
		pAirplane->SetAnchorPoint(Vec2(0, 0));
		pAirplane->SetLocalZOrder(localZOrder++);
		pAirplane->SetFrame2DRoot(this);
		m_vecChildLayer.push_back(pAirplane);
		this->AddChild(pAirplane);
	}

	m_pAirspaceLayer = CHmsFrame2DAirspaceLayer::Create(layerSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pAirspaceLayer)
#endif
	{
		m_pAirspaceLayer->SetAnchorPoint(Vec2(0, 0));
		m_pAirspaceLayer->SetLocalZOrder(localZOrder++);
		m_pAirspaceLayer->SetFrame2DRoot(this);
		m_pAirspaceLayer->HideAirspaceLayer();
		m_vecChildLayer.push_back(m_pAirspaceLayer);
		this->AddChild(m_pAirspaceLayer);
	}

    m_pAirspaceDBLayer = CHmsFrame2DAirspaceDBLayer::Create(layerSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pAirspaceDBLayer)
#endif
	{
		m_pAirspaceDBLayer->SetAnchorPoint(Vec2(0, 0));
		m_pAirspaceDBLayer->SetLocalZOrder(localZOrder++);
		m_pAirspaceDBLayer->SetFrame2DRoot(this);
		m_pAirspaceDBLayer->SetLayerVisible(false);
		m_vecChildLayer.push_back(m_pAirspaceDBLayer);
		this->AddChild(m_pAirspaceDBLayer);
	}

    m_pAirportLayer = CHmsFrame2DAirportLayer::Create(layerSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pAirportLayer)
#endif
	{
		m_pAirportLayer->SetAnchorPoint(Vec2(0, 0));
		m_pAirportLayer->SetLocalZOrder(localZOrder++);
		m_pAirportLayer->SetFrame2DRoot(this);
		m_pAirportLayer->SetLayerVisible(false);
		m_vecChildLayer.push_back(m_pAirportLayer);
		this->AddChild(m_pAirportLayer);
	}

	auto pObsLayer = CHmsFrame2DObsLayer::Create(layerSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (pObsLayer)
#endif
	{
		pObsLayer->SetAnchorPoint(Vec2(0, 0));
		pObsLayer->SetLocalZOrder(localZOrder++);
		pObsLayer->SetFrame2DRoot(this);
		m_vecChildLayer.push_back(pObsLayer);
		this->AddChild(pObsLayer);
	}

	m_pRouteLayer = CHmsFrame2DRouteLayer::Create(layerSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pRouteLayer)
#endif
	{
		m_pRouteLayer->SetAnchorPoint(Vec2(0, 0));
		m_pRouteLayer->SetLocalZOrder(localZOrder++);
		m_pRouteLayer->SetFrame2DRoot(this);
		m_vecChildLayer.push_back(m_pRouteLayer);
		this->AddChild(m_pRouteLayer);
		m_pRouteLayer->RegisterPressNodeShort(HMS_CALLBACK_2(CHmsFrame2DRoot::OnNodePressShort, this));
		m_pRouteLayer->RegisterPressNodeLong(HMS_CALLBACK_2(CHmsFrame2DRoot::OnNodeOrLinePressLong, this));
		m_pRouteLayer->RegisterPressLineLong(HMS_CALLBACK_2(CHmsFrame2DRoot::OnNodeOrLinePressLong, this));
		m_pRouteLayer->RegisterEleChangeCallback(HMS_CALLBACK_0(CHmsFrame2DRoot::OnRouteEleChanged, this));
	}

	m_pAirspaceEditLayer = CHmsFrame2DAirspaceEditLayer::Create(layerSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pAirspaceEditLayer)
#endif
	{
		m_pAirspaceEditLayer->SetAnchorPoint(Vec2(0, 0));
		m_pAirspaceEditLayer->SetLocalZOrder(localZOrder++);
		m_pAirspaceEditLayer->SetFrame2DRoot(this);
		m_pAirspaceEditLayer->HideEditLayer();
		m_vecChildLayer.push_back(m_pAirspaceEditLayer);
		this->AddChild(m_pAirspaceEditLayer);
	}

	m_pWptInfoDlg = CHmsWptInfoDlg::Create(Size(700, 850));
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pWptInfoDlg)
#endif
	{
		m_pWptInfoDlg->SetAnchorPoint(Vec2(0, 0));
		m_pWptInfoDlg->SetVisible(false);
		m_pWptInfoDlg->SetCallbackAdd(std::bind(&CHmsFrame2DRoot::CallbackAdd, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
		m_pWptInfoDlg->SetCallbackCancel(HMS_CALLBACK_1(CHmsFrame2DRoot::CallbackCancel, this));
		m_pWptInfoDlg->SetCallbackDelete(HMS_CALLBACK_1(CHmsFrame2DRoot::CallbackDelete, this));
		m_pWptInfoDlg->SetLocalZOrder(localZOrder++);
		this->AddChild(m_pWptInfoDlg);
	}

	m_mapMeasure = CHmsMapMeasure::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_mapMeasure)
#endif
	{
		this->AddChild(m_mapMeasure);
	}

	m_pToolbox = CHmsFrame2DToolbox::Create(Size(300, 300));
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pToolbox)
#endif
	{
		m_pToolbox->SetAnchorPoint(Vec2(0.5, 0.5));
		m_pToolbox->SetVisible(false);
		m_pToolbox->SetLocalZOrder(localZOrder++);
		m_pToolbox->SetFrame2DRoot(this);
		this->AddChild(m_pToolbox);
	}

	Size dragBtnSize = Size(60, 60);
	m_dragButtonInitPos.x = dragBtnSize.width * 0.5;
    m_dragButtonInitPos.y = layerSize.height * 0.618 - 30 + dragBtnSize.height * 0.5;
	auto pDragButton = CHmsUiStretchButton::Create("res/localNormal.png", dragBtnSize, Color3B::WHITE, Color3B::WHITE);
#if _NAV_SVS_LOGIC_JUDGE
	if (pDragButton)
#endif
	{
		pDragButton->SetAnchorPoint(Vec2(0.5, 0.5));
		pDragButton->SetPosition(m_dragButtonInitPos);
		pDragButton->SetVisible(false);
		pDragButton->SetLocalZOrder(localZOrder++);
		this->AddChild(pDragButton);
		m_pDragButtonLonLat = pDragButton;
	}


	this->RegisterAllNodeChild();

	InitNameNode();

	return true;
}

void CHmsFrame2DRoot::Update(float delta)
{
    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
#if _NAV_SVS_LOGIC_JUDGE
	if (!dataBus)
		return;
#endif
    auto aftData = dataBus->GetData();
#if _NAV_SVS_LOGIC_JUDGE
	if (!aftData)
		return;
#endif
    if (aftData->bLatLonValid)
    {
        m_planeLonLat = Vec2(aftData->longitude, aftData->latitude);
        m_planeEarthPos = LonLatToEarthPoint(m_planeLonLat);
        m_planeYaw = aftData->trueHeading;
    }  
    m_bPlaneLonLatValid = aftData->bLatLonValid;

    FmsCalData gpsHsiData = dataBus->GetFmsData();
    if (gpsHsiData.valid)
    {
        auto index = gpsHsiData.flySegment;
        m_flySegment = ConvertIntIndexToFPLIndex(index);
    }
	
	for (auto &c : m_vecChildLayer)
	{
		if (!c->IsVisible())
		{
			continue;
		}
		c->Update(delta);

		if (c->GetNeedUpdate2DEles())
		{
			c->Update2DElements();
		}
		c->SetNeedUpdate2DEles(false);
	}
	//会有数据同步，窗口不可见也要调用
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsNavComm::GetInstance()->GetCHmsNavFplLayer() 
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pEditAirspace)
		return;
#endif
	CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pEditAirspace->Update(delta);
}

void CHmsFrame2DRoot::OnRouteEleChanged()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	const auto &vecWptNode = m_pRouteLayer->GetWptNodes();

    std::vector<WptNodeStu2D> vec;
    for (auto &c : vecWptNode)
    {
        vec.insert(vec.end(), c.vWayPoint.begin(), c.vWayPoint.end());
    }
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsGlobal::GetInstance()->GetDataBus()
		|| !CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData())
		return;
#endif
    CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData()->SetEnroute(vec);

#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pNavInfoWin)
		return;
#endif
    CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pNavInfoWin->SetWptNodeData(vecWptNode);
}

void CHmsFrame2DRoot::InitNameNode()
{
	m_map2dLayerNameNode.insert("2dRoute", m_pRouteLayer);
	m_map2dLayerNameNode.insert("2dAirspace", m_pAirspaceLayer);
	m_map2dLayerNameNode.insert("2dAirsapceEdit", m_pAirspaceEditLayer);
	m_map2dLayerNameNode.insert("2dAirspaceDB", m_pAirspaceDBLayer);
	m_map2dLayerNameNode.insert("2dAdminAreas", m_pAdminAreasLayer);
    m_map2dLayerNameNode.insert("2dAftTrackLayer", m_pAftTrackLayer);
}

bool CHmsFrame2DRoot::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pDragButtonLonLat 
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pKeyBoard
		|| !m_pWptInfoDlg
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pRouteNodeWin
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pSearchShape
		|| !m_pToolbox)
		return false;
#endif
	if (m_pDragButtonLonLat->IsVisible() && m_pDragButtonLonLat->GetRectFromParent().containsPoint(posOrigin))
	{
		m_bIsPressOnDragButton = true;

        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pKeyBoard->SetVisible(false);

		return true;
	}
	else
	{
		m_bIsPressOnDragButton = false;

		//包含3个对话框，先让对话框响应事件
		if (HmsUiEventInterface::OnPressed(posOrigin))
		{
			return true;
		}
		else
		{
            if (m_pWptInfoDlg->IsVisible())
            {
                m_pWptInfoDlg->ShowPage(nullptr, WptDlgPage::PAGE_EXIT_CANCEL);
            }
            CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->HideRouteNodeWin();
			m_pToolbox->SetVisible(false);
            CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pSearchShape->SetVisible(false);
		}

		return false;
	}
}

void CHmsFrame2DRoot::OnMove(const HmsAviPf::Vec2 & posOrigin)
{
	if (m_bIsPressOnDragButton)
	{
#if _NAV_SVS_LOGIC_JUDGE
		if (m_pDragButtonLonLat)
#endif
		m_pDragButtonLonLat->SetPosition(posOrigin);
	}
	else
	{
		HmsUiEventInterface::OnMove(posOrigin);
	}
}

void CHmsFrame2DRoot::OnReleased(const HmsAviPf::Vec2 & posOrigin)
{
	if (m_bIsPressOnDragButton)
	{
		if (m_funcDragButtonRelease)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (!m_pDragButtonLonLat)
				return;
#endif
			auto pos = m_pDragButtonLonLat->GetPoistion();
			Vec3 earthPos;
			if (Frame2DPointToEarthPoint(pos, earthPos))
			{
				auto lonLat = EarthPointToLonLat(earthPos);
				m_funcDragButtonRelease(lonLat.x, lonLat.y);
			}
		}
	}
	else
	{
		HmsUiEventInterface::OnReleased(posOrigin);
	}
}

void CHmsFrame2DRoot::OnNodePressShort(WptNodeStu2D *pStu, int params)
{
	FrameEventState state = (FrameEventState)params;
	if (state == FrameEventState::DRAG_OLD_NODE)
	{
		if (pStu)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_pToolbox && pStu->pNode)
#endif
			{
				m_pToolbox->SetPosition(pStu->pNode->GetPoistion());
				m_pToolbox->SetCurSelectIndex(FindNodeIndex(pStu));
				m_pToolbox->SetVisible(true);
			}

            if (m_pWptInfoDlg->IsVisible())
            {
                m_pWptInfoDlg->ShowPage(nullptr, WptDlgPage::PAGE_EXIT_CANCEL);
		}
	}
}
}

void CHmsFrame2DRoot::OnNodeOrLinePressLong(WptNodeStu2D *pStu, int params)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (pStu && m_pWptInfoDlg && pStu->pNode)
#else
	if (pStu)
#endif
	{
		m_pWptInfoDlg->SetPosition(GetShowDlgPos(pStu->pNode->GetPoistion()));
		m_pWptInfoDlg->ShowDlg(pStu->brief.lon, pStu->brief.lat, WptDlgMode::WPTDLGMODE_MOD, params);

        m_pToolbox->SetVisible(false);
	}
}

void CHmsFrame2DRoot::OnClickOnEarth(const HmsAviPf::Vec2 & pos)
{
	Vec3 earthPos;
	if (!Frame2DPointToEarthPoint(pos, earthPos))
	{
		return;
	}
	Vec2 lonLat = EarthPointToLonLat(earthPos);

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pWptInfoDlg || !m_pRouteLayer)
		return;
#endif
	m_pWptInfoDlg->SetPosition(GetShowDlgPos(pos));
	m_pWptInfoDlg->ShowDlg(lonLat.x, lonLat.y, WptDlgMode::WPTDLGMODE_ADD, (int)m_pRouteLayer->GetFrameEventState());
}

void CHmsFrame2DRoot::Update2DElements()
{
	
}

void CHmsFrame2DRoot::InsertWptNode(WptPointType t, float lon, float lat, std::string ident, FPLNodeIndex index, bool bExpand /*= true*/)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->InsertWptNode(t, lon, lat, ident, index, bExpand);
}

void CHmsFrame2DRoot::InsertWptNode(WptNodeBrief brief, FPLNodeIndex index, bool bExpand /*= true*/)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->InsertWptNode(brief, index, bExpand);
}

void CHmsFrame2DRoot::DeleteWptNode(FPLNodeIndex index, bool bExpand /*= true*/, bool bDetectAirport /*= true*/)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->DeleteWptNode(index, bExpand, bDetectAirport);
}

void CHmsFrame2DRoot::CallbackAdd(WptPointType t, float lon, float lat, std::string ident, int params)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->CallbackAdd(t, lon, lat, ident, params);
}

void CHmsFrame2DRoot::CallbackCancel(int params)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->CallbackCancel(params);

    m_pToolbox->SetVisible(false);
}

void CHmsFrame2DRoot::CallbackDelete(int params)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->CallbackDelete(params);

    m_pToolbox->SetVisible(false);
}

HmsAviPf::Vec2 CHmsFrame2DRoot::GetShowDlgPos(const HmsAviPf::Vec2 & posOrigin)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pWptInfoDlg)
		return Vec2(0,0);
#endif
	auto dlgSize = m_pWptInfoDlg->GetContentSize();
	auto winSize = this->GetContentSize();
	Vec2 p = posOrigin;
	if (p.x + dlgSize.width > winSize.width) p.x = winSize.width - dlgSize.width;
    if (p.y + dlgSize.height > (winSize.height * 0.618 - 30)) p.y = winSize.height * 0.618 - 30 - dlgSize.height;

	return p;
}

void CHmsFrame2DRoot::Reverse()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->Reverse();
}

void CHmsFrame2DRoot::ClearAll()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->ClearAll();
}

void CHmsFrame2DRoot::SetEarthLayer(int n)
{
	if (m_earthLayer != n)
	{
		m_earthLayer = n;
		for (auto &c : m_vecChildLayer)
		{
			c->SetEarthLayer(n);
		}
	}
}

void CHmsFrame2DRoot::InsertProgramInfo(FPL2DNodeInfoStu &info, FPLNodeIndex curStuIndex)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->InsertProgramInfo(info, curStuIndex);
}

FPLNodeIndex CHmsFrame2DRoot::FindNodeIndex(WptNodeStu2D *stu)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return FPLNodeIndex(0,0);
#endif
	return m_pRouteLayer->FindNodeIndex(stu);
}

std::vector<FPLNodeIndex> CHmsFrame2DRoot::FindSameProgram(std::string airport, FPL2DNodeType subType)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return vector<FPLNodeIndex>(0);
#endif
	return m_pRouteLayer->FindSameProgram(airport, subType);
}

bool CHmsFrame2DRoot::InsertFPLNodeByInputRouteString(FPLNodeIndex index)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer 
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pNavInfoWin)
		return false;
#endif
    auto str = CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pWptAddSearch->GetInputRouteString();
	if (m_pRouteLayer->InsertFPLNodeByInputRouteString(index, str))
	{
        return true;
	}
    else
    {
        return false;
    }
}

void CHmsFrame2DRoot::DirectTo(FPLNodeIndex index)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	m_pRouteLayer->DirectTo(index);
}

void CHmsFrame2DRoot::ShowProcedureDlg()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer 
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pProcedure)
		return;
#endif
	ProcedureParamStu airportSID;
	ProcedureParamStu airportSTAR;
	const auto &vecWptNode = m_pRouteLayer->GetWptNodes();

	auto IsAirport = [=](ProcedureParamStu &stu, int vecIndex)
	{
		auto info = vecWptNode[vecIndex];
		if (info.ePriType == FPL2DNodeType::FPL_TYPE_POINT)
		{
			if (info.vWayPoint[0].brief.wType == WptPointType::WPT_AIRPORT)
			{
				stu.airportIdent = info.ident;
				stu.lon = info.vWayPoint[0].brief.lon;
				stu.lat = info.vWayPoint[0].brief.lat;
				stu.index = vecIndex;
			}
		}
	};

	if (vecWptNode.size() == 1)
	{
		IsAirport(airportSID, 0);
	}
	else if (vecWptNode.size() >= 2)
	{
		IsAirport(airportSID, 0);
		IsAirport(airportSTAR, vecWptNode.size() - 1);
	}

    CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pProcedure->SetSIDSTARAirport(airportSID, airportSTAR);
    CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pProcedure->ShowPage(ProcedurePageShow::PAGE_LIST_BRIEF);
}

void CHmsFrame2DRoot::SaveRoute()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
		return;
#endif
	const auto &vecWptNode = m_pRouteLayer->GetWptNodes();
	std::string name;
	std::vector<int> vec;
	if (m_pRouteLayer->GetRouteInfoForSave(name, vec))
	{
        if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pHistoryRouteInputName)
		{
            CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pHistoryRouteInputName->ShowDlg(name, vec, [=](std::string name){

                if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pHistoryRouteDlg)
				{
                    CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pHistoryRouteDlg->SaveRoute(vecWptNode, name);
				}
			});
		}
	}
}

void CHmsFrame2DRoot::OpenHistoryRouteDlg()
{
	if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer() && CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pHistoryRouteDlg)
	{
        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pHistoryRouteDlg->OpenDialog();
	}
}

void CHmsFrame2DRoot::InsertProgram(std::string airport, std::string program, std::string trans, std::string runway, FPL2DNodeType type)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	const auto &vecWptNode = m_pRouteLayer->GetWptNodes();
	int index = -1;
	for (int i = 0; i < (int)vecWptNode.size(); ++i)
	{
		if (vecWptNode[i].ePriType == FPL_TYPE_POINT && vecWptNode[i].ident == airport)
		{
			index = i;
			break;
		}
	}
	if (index < 0)
	{
		return;
	}
	if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer() && CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pRouteNodeWin)
	{
        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pRouteNodeWin->UseProgram(airport, program, trans, runway, type, index);
	}
}

FPLNodeIndex CHmsFrame2DRoot::ConvertIntIndexToFPLIndex(int index)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return FPLNodeIndex(0,0);
#endif
	return m_pRouteLayer->ConvertIntIndexToFPLIndex(index);
}

void CHmsFrame2DRoot::Resize(const HmsAviPf::Size & size, int nLevel)
{
	this->SetContentSize(size);
}

void CHmsFrame2DRoot::ShowInputAltitude(FPLNodeIndex index)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	auto &vecWptNode = m_pRouteLayer->m_vecWptNode;

	if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer() && CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pInputAltitude)
	{
		if (index.primaryIndex >= 0 && index.primaryIndex < (int)vecWptNode.size())
		{
			if (index.subIndex >= 0 && index.subIndex < (int)vecWptNode[index.primaryIndex].vWayPoint.size())
			{
				auto pNodeStu2D = &vecWptNode[index.primaryIndex].vWayPoint[index.subIndex];

                CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pInputAltitude->ShowDlg(pNodeStu2D->brief.attr.altStu, 
                    [=, &vecWptNode](FmcAltitude altStu[2]){

					pNodeStu2D->brief.attr.altStu[0] = altStu[0];
					pNodeStu2D->brief.attr.altStu[1] = altStu[1];
					for (int i = index.primaryIndex + 1; i < (int)vecWptNode.size(); ++i)
					{
						if (vecWptNode[i].ePriType != FPL_TYPE_POINT)
						{
							break;
						}
						for (auto &c : vecWptNode[i].vWayPoint)
						{
                            if (c.brief.attr.altStu[0].nAltitude == -1 && c.brief.attr.altStu[1].nAltitude == -1)
                            {
                                c.brief.attr.altStu[0] = altStu[0];
                                c.brief.attr.altStu[1] = altStu[1];
                            }						
						}
					}
					m_pRouteLayer->SetEleChanged(true);
					m_pRouteLayer->SetNeedUpdate2DEles(true);
					m_pRouteLayer->SetNeedSync(true);
				});
			}
		}
	}
}

void CHmsFrame2DRoot::ShowSearchShape(FPLNodeIndex index)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return;
#endif
	auto &vecWptNode = m_pRouteLayer->m_vecWptNode;

	if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer() && CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pSearchShape)
	{
		if (index.primaryIndex >= 0 && index.primaryIndex < (int)vecWptNode.size())
		{
			if (index.subIndex >= 0 && index.subIndex < (int)vecWptNode[index.primaryIndex].vWayPoint.size())
			{
				auto pNodeStu2D = &vecWptNode[index.primaryIndex].vWayPoint[index.subIndex];

                CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pSearchShape->ShowDlg(pNodeStu2D->brief.attr.searchShape, 
                    [=](SearchShapeStu stu){

					pNodeStu2D->brief.attr.searchShape = stu;

					m_pRouteLayer->SetEleChanged(true);
					m_pRouteLayer->SetNeedUpdate2DEles(true);
					m_pRouteLayer->SetNeedSync(true);
				});
			}
		}
	}
}

void CHmsFrame2DRoot::ShowAirspace(bool bShow)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pAirspaceLayer)
		return;
#endif
	if (bShow)
	{
		m_pAirspaceLayer->ShowAirspaceLayer();
	}
	else
	{
		m_pAirspaceLayer->HideAirspaceLayer();
	}
}

bool CHmsFrame2DRoot::AirspaceIsVisiable()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pAirspaceLayer)
		return false;
#endif
	return m_pAirspaceLayer->IsVisible();
}

void CHmsFrame2DRoot::ShowEditAirspaceDlg()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsNavComm::GetInstance()->GetCHmsNavFplLayer() 
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pEditAirspace)
		return;
#endif
    CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pEditAirspace->ShowDlg();
}

Frustum* CHmsFrame2DRoot::GetFrustum()
{
	HmsAviPf::Frustum *pFrust = nullptr;
	if (m_fNearPct != 0)
	{
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pCamera)
			return pFrust;
#endif
		pFrust = m_pCamera->GetCameraFrustum(m_fNearPct);
	}
	return pFrust;
}

const WptNodeStu2D* CHmsFrame2DRoot::GetWptNodeStu2D(FPLNodeIndex index)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLayer)
		return nullptr;
#endif
	return m_pRouteLayer->GetWptNodeStu2D(index);
}

FPLNodeIndex CHmsFrame2DRoot::GetFlySegment()
{
	return m_flySegment;
}

HmsAviPf::Vec2 CHmsFrame2DRoot::GetPlaneLonLat()
{
	return m_planeLonLat;
}

HmsAviPf::Vec3 CHmsFrame2DRoot::GetPlaneEarthPos()
{
	return m_planeEarthPos;
}

float CHmsFrame2DRoot::GetPlaneYaw()
{
	return m_planeYaw;
}

void CHmsFrame2DRoot::SetNeedUpdate2DEles(bool b)
{
	for (auto &c : m_vecChildLayer)
	{
		c->SetNeedUpdate2DEles(b);
	}
}

CHmsMapMeasure* CHmsFrame2DRoot::GetMapMeasure()
{
	return m_mapMeasure;
}

void CHmsFrame2DRoot::ShowUserWptEditDlg()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pUserWptEditDlg)
		return;
#endif
    CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pUserWptEditDlg->ShowDlg();
}

void CHmsFrame2DRoot::ShowOrHideDragButton(bool bShow, std::function<void(float, float)> funcOnRelease)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pDragButtonLonLat)
		return;
#endif
	if (bShow)
	{
		m_pDragButtonLonLat->SetPosition(m_dragButtonInitPos);
	}
	m_pDragButtonLonLat->SetVisible(bShow);
	m_funcDragButtonRelease = funcOnRelease;
}

void CHmsFrame2DRoot::ShowAirspaceEditLayer(const std::vector<AirspaceEditStu> &vec)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pAirspaceEditLayer)
		return;
#endif
	m_pAirspaceEditLayer->ShowEditLayer(vec);
}

void CHmsFrame2DRoot::HideAirspaceEditLayer()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pAirspaceEditLayer)
		return;
#endif
	m_pAirspaceEditLayer->HideEditLayer();
}

void CHmsFrame2DRoot::UpdateAirspaceEditData(const std::vector<AirspaceEditStu> &vecStu)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()
		|| !CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pEditAirspace)
		return;
#endif
    CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->m_pEditAirspace->UpdateAirspaceEditData(vecStu);
}

CHmsFrame2DAirspaceDBLayer* CHmsFrame2DRoot::GetAirspaceDBLayer()
{
    return m_pAirspaceDBLayer;
}

CHmsFrame2DAirportLayer* CHmsFrame2DRoot::GetAirportLayer()
{
    return m_pAirportLayer;
}

CHmsFrame2DLayerBase* CHmsFrame2DRoot::GetFrame2DLayer(NavFrame2DLayerType type)
{
    for (auto &c : m_vecChildLayer)
    {
        if (c->GetLayerType() == type)
        {
            return c;
        }
    }
    return nullptr;
}

void CHmsFrame2DRoot::Set2DLayerDisplayByName(const std::string & strLayerName, bool bShow)
{
	auto node = Get2DLayerByName(strLayerName);
	if (node)
	{
		node->SetVisible(bShow);
	}
}

bool CHmsFrame2DRoot::Is2DLayerDisplay(const std::string & strLayerName)
{
	auto node = Get2DLayerByName(strLayerName);
	if (node)
	{
		return node->IsVisible();
	}
	return false;
}

HmsAviPf::CHmsNode * CHmsFrame2DRoot::Get2DLayerByName(const std::string & strLayerName)
{
	auto itFind = m_map2dLayerNameNode.find(strLayerName);
	if (itFind != m_map2dLayerNameNode.end())
	{
		if (itFind->second)
		{
			return itFind->second;
		} 
	}
	return nullptr;
}

bool CHmsFrame2DRoot::IsPlaneLonLatValid()
{
	return m_bPlaneLonLatValid;
}

