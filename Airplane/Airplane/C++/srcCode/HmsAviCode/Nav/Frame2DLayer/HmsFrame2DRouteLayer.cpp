#include "HmsFrame2DRouteLayer.h"
#include "../HmsFrame2DRoot.h"

CHmsFrame2DRouteLayer::CHmsFrame2DRouteLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_Route)
, m_pCurDragNodeStu2D(nullptr)
, m_pClickDelayAction(nullptr)
, m_bEnableClickDelay(false)
, m_funcPressNodeShort(nullptr)
, m_funcPressNodeLong(nullptr)
, m_funcPressLineShort(nullptr)
, m_funcPressLineLong(nullptr)
, m_funcEleChangeCallback(nullptr)
, m_routeNodeFontSize(20)
{
    
}

CHmsFrame2DRouteLayer::~CHmsFrame2DRouteLayer()
{

}

CHmsFrame2DRouteLayer* CHmsFrame2DRouteLayer::Create(Size layerSize)
{
	CHmsFrame2DRouteLayer *ret = new (std::nothrow) CHmsFrame2DRouteLayer();
	if (ret && ret->Init(layerSize))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}
bool CHmsFrame2DRouteLayer::Init(Size layerSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	this->SetContentSize(layerSize);

	m_pRouteLineDrawNode = CHmsRouteLineDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pRouteLineDrawNode)
#endif
	{
		m_pRouteLineDrawNode->SetAnchorPoint(0, 0);
		m_pRouteLineDrawNode->SetContentSize(this->GetContentSize());
		m_pRouteLineDrawNode->SetPosition(0, 0);
		m_pRouteLineDrawNode->SetTexturePath("res/NavImage/navLine.png");
		this->AddChild(m_pRouteLineDrawNode);
	}

	//CHmsUISyncDataMgr::GetInstance()->RegisterUiCallback(ESyncDataID::DATA_ID_FLIGHT_PLAN, HMS_CALLBACK_3(CHmsFrame2DRouteLayer::OnSyncDataUpdate, this));

	this->RegisterAllNodeChild();

	return true;
}

void CHmsFrame2DRouteLayer::Update(float delta)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	auto segIndex = m_pFrame2DRoot->GetFlySegment();
	if (!(m_flySegment == segIndex))
	{
		m_flySegment = segIndex;
		ReDrawLines();
	}
	if (m_bNeedUseSyncData)
	{
		//UseSyncData();
		
		if (m_bEleChanged)
		{
			ProcessNodeEnableOrNot();
			if (m_funcEleChangeCallback)
			{
				m_funcEleChangeCallback();
			}
			SetEleChanged(false);
		}
		//来自同步的数据变化不需要再同步
		SetNeedSync(false);
	}
	else
	{
		if (m_bEleChanged)
		{
			ProcessNodeEnableOrNot();
			if (m_funcEleChangeCallback)
			{
				m_funcEleChangeCallback();
			}
			SetEleChanged(false);
		}
	}

	if (GetNeedSync())
	{
		//SendSyncData();

		SetNeedSync(false);
	}
}
#if 0
void CHmsFrame2DRouteLayer::SendSyncData()
{
	auto ConvertFPLNodeToHistory = [=](const FPL2DNodeInfoStu &node, HistoryRouteOnePointStu &hisStu)
	{
		if (node.ePriType == FPL_TYPE_POINT)
		{
			const auto &brief = node.vWayPoint[0].brief;
			hisStu.ePriType = FPL_TYPE_POINT;
			hisStu.eSubType = FPL_TYPE_POINT;
			hisStu.pointAttr.type = brief.wType;
			hisStu.pointAttr.lon = brief.lon;
			hisStu.pointAttr.lat = brief.lat;
			//strcpy(hisStu.pointAttr.ident, brief.ident.c_str());
			hisStu.pointAttr.SetIdent(brief.ident);
			hisStu.pointAttr.altStu[0] = brief.attr.altStu[0];
			hisStu.pointAttr.altStu[1] = brief.attr.altStu[1];
			hisStu.pointAttr.searchShape = brief.attr.searchShape;
		}
		else if (node.ePriType == FPL_TYPE_PROGRAM)
		{
			hisStu.ePriType = node.ePriType;
			hisStu.eSubType = node.eSubType;
			//strcpy(hisStu.programAttr.belongAirport, node.belongAirport.c_str());
			//strcpy(hisStu.programAttr.programName, node.programName.c_str());
			//strcpy(hisStu.programAttr.transName, node.transName.c_str());
			//strcpy(hisStu.programAttr.rnwName, node.rnwName.c_str());
			hisStu.programAttr.SetBelongAirport(node.belongAirport);
			hisStu.programAttr.SetProgramName(node.programName);
			hisStu.programAttr.SetTransName(node.transName);
			hisStu.programAttr.SetRnwName(node.rnwName);
		}
	};

	std::vector<HistoryRouteOnePointStu> vecStu;
	vecStu.resize(m_vecWptNode.size());
	std::vector<FplSyncProgramAttrStu> vecSyncAttr;
	for (int i = 0; i < m_vecWptNode.size(); ++i)
	{
		ConvertFPLNodeToHistory(m_vecWptNode[i], vecStu[i]);

		if (m_vecWptNode[i].ePriType == FPL_TYPE_PROGRAM)
		{
			for (int j = 0; j < m_vecWptNode[i].vWayPoint.size(); ++j)
			{
				FplSyncProgramAttrStu attrStu;
				attrStu.index = FPLNodeIndex(i, j);
				attrStu.attr = m_vecWptNode[i].vWayPoint[j].brief.attr;
				vecSyncAttr.push_back(attrStu);
			}
		}
	}

	int len1 = vecStu.size() * sizeof(HistoryRouteOnePointStu);
	int len2 = vecSyncAttr.size() * sizeof(FplSyncProgramAttrStu);
	int lenAll = len1 + len2 + sizeof(int) * 2;
	char *pSyncData = new char[lenAll]();
	if (pSyncData)
	{
		char *pTmp = pSyncData;

		memcpy(pTmp, &len1, sizeof(int));
		pTmp += sizeof(int);
		memcpy(pTmp, vecStu.data(), len1);
		pTmp += len1;

		memcpy(pTmp, &len2, sizeof(int));
		pTmp += sizeof(int);
		memcpy(pTmp, vecSyncAttr.data(), len2);
		pTmp += len2;

		if (lenAll < 65535)
		{
			CHmsUISyncDataMgr::GetInstance()->SendSyncDataToMS(ESyncDataID::DATA_ID_FLIGHT_PLAN, pSyncData, lenAll);
		}

		delete[] pSyncData;
	}
}

void CHmsFrame2DRouteLayer::OnSyncDataUpdate(ESyncDataID dataID, char *data, int length)
{
	char *pTmp = data;

	int len1 = 0;
	memcpy(&len1, pTmp, sizeof(int));
	pTmp += sizeof(int);
	std::vector<HistoryRouteOnePointStu> vecStu;
	if (len1 > 0)
	{
		vecStu.resize(len1 / sizeof(HistoryRouteOnePointStu));
		memcpy(vecStu.data(), pTmp, len1);
		pTmp += len1;
	}

	int len2 = 0;
	memcpy(&len2, pTmp, sizeof(int));
	pTmp += sizeof(int);
	std::vector<FplSyncProgramAttrStu> vecSyncAttr;
	if (len2 > 0)
	{
		vecSyncAttr.resize(len2 / sizeof(FplSyncProgramAttrStu));
		memcpy(vecSyncAttr.data(), pTmp, len2);
		pTmp += len2;
	}

	//===========使用同步数据
	m_syncMutex.lock();
	m_vecSyncStu = vecStu;
	m_vecSyncAttr = vecSyncAttr;
	m_bNeedUseSyncData = true;
	m_syncMutex.unlock();
}

void CHmsFrame2DRouteLayer::UseSyncData()
{
	std::lock_guard<std::mutex> locker(m_syncMutex);

	m_bNeedUseSyncData = false;

	ClearAll();
	for (auto &c : m_vecSyncStu)
	{
		if (c.ePriType == FPL_TYPE_POINT)
		{
			WptNodeBrief brief;
			brief.wType = c.pointAttr.type;
			brief.lon = c.pointAttr.lon;
			brief.lat = c.pointAttr.lat;
			brief.ident = c.pointAttr.ident;
			brief.attr.altStu[0] = c.pointAttr.altStu[0];
			brief.attr.altStu[1] = c.pointAttr.altStu[1];
			brief.attr.searchShape = c.pointAttr.searchShape;

			InsertWptNode(brief, FPLNodeIndex(), false);
		}
	}
	for (auto &c : m_vecSyncStu)
	{
		if (c.ePriType == FPL_TYPE_PROGRAM)
		{
			m_pFrame2DRoot->InsertProgram(
				c.programAttr.belongAirport,
				c.programAttr.programName,
				c.programAttr.transName,
				c.programAttr.rnwName,
				c.eSubType);
		}
	}
	//同步程序中点的属性
	for (auto &c : m_vecSyncAttr)
	{
		if (c.index.primaryIndex >= 0 && c.index.primaryIndex < m_vecWptNode.size())
		{
			if (c.index.subIndex >= 0 && c.index.subIndex < m_vecWptNode[c.index.primaryIndex].vWayPoint.size())
			{
				m_vecWptNode[c.index.primaryIndex].vWayPoint[c.index.subIndex].brief.attr = c.attr;
			}
		}
	}
}
#endif

void CHmsFrame2DRouteLayer::Update2DElements()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
	for (auto &c : m_vecWptNode)
	{
		for (auto &e : c.vWayPoint)
		{
			WptNodeStu2D &stu = e;
			if (stu.pNode)
			{
				bool bVisible = false;
				if (pFrust)
				{
					bVisible = !pFrust->isOutOfFrustum(stu.earthPos);
				}
				stu.pNode->SetVisible(bVisible && stu.bEnableNode);
				if (bVisible)
				{
					auto pos = m_pFrame2DRoot->EarthPointToFramePoint(stu.earthPos);
					stu.pNode->SetPosition(pos);
				}
			}

			for (int k = 0; k < (int)stu.vecInsertEarthPos.size(); ++k)
			{
				bool bVisible = false;
				if (pFrust)
				{
					bVisible = !pFrust->isOutOfFrustum(stu.vecInsertEarthPos.at(k));
				}
				stu.vecInsertLonLatIsVisible[k] = bVisible && stu.bEnableNode;
				stu.vecInsert2DPos[k] = m_pFrame2DRoot->EarthPointToFramePoint(stu.vecInsertEarthPos[k]);
			}
		}
	}
	ReDrawLines();
}

void CHmsFrame2DRouteLayer::SetEarthLayer(int n)
{
	if (m_earthLayer != n)
	{
		m_earthLayer = n;
		
		CalInsertLonLat(FPLNodeIndex(), FPLNodeIndex(m_vecWptNode.size(), 0));

		SetNeedUpdate2DEles(true);
	}
}

bool CHmsFrame2DRouteLayer::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
	m_frameEventState = FrameEventState::DRAG_NULL;
	m_clickDelayParamsStu.bExecuted = false;
	m_posPress = posOrigin;

	if (TestPressOnOldNode(posOrigin))
	{
		AddClickDelay();
		return true;
	}
	else
	{
		if (TestPressOnLine(posOrigin))
		{
			AddClickDelay();
			return true;
		}
	}
	return false;
}

void CHmsFrame2DRouteLayer::OnMove(const HmsAviPf::Vec2 & posOrigin)
{
	if (m_frameEventState == FrameEventState::DRAG_OLD_NODE || m_frameEventState == FrameEventState::DRAG_NEW_NODE)
	{
		auto delta = posOrigin - m_posPress;
		if (delta.length() > 5)
		{
			//如果移动的距离很远，就手动调用延时函数，然后取消延时
			if (m_bEnableClickDelay)
			{
				PressClickDelay();
				CancelClickDelay();
			}
			auto newPos = m_posImageWhenPress + delta;
			Vec3 earthPos;
			if (m_pFrame2DRoot && m_pFrame2DRoot->Frame2DPointToEarthPoint(newPos, earthPos) && m_pCurDragNodeStu2D)
			{
				m_pCurDragNodeStu2D->pNode->SetPosition(newPos);

				m_pCurDragNodeStu2D->earthPos = earthPos;
				Vec2 lonLat = m_pFrame2DRoot->EarthPointToLonLat(earthPos);
				m_pCurDragNodeStu2D->brief.lon = lonLat.x;
				m_pCurDragNodeStu2D->brief.lat = lonLat.y;

				auto dis = FindNodeIndex(m_pCurDragNodeStu2D);
				CalInsertLonLat(FPLNodeIndex(dis.primaryIndex - 2, 0), FPLNodeIndex(dis.primaryIndex + 2, 0));

				ReDrawLines();
				DrawMoveCicle(newPos);
			}
		}
	}
}

void CHmsFrame2DRouteLayer::OnReleased(const HmsAviPf::Vec2 & posOrigin)
{
	CancelClickDelay();

	if (m_frameEventState == FrameEventState::DRAG_OLD_NODE || m_frameEventState == FrameEventState::DRAG_NEW_NODE)
	{
		//如果延时函数已经响应，可以弹出窗口
		if (m_clickDelayParamsStu.bExecuted)
		{
			if (m_funcPressNodeLong && m_frameEventState == FrameEventState::DRAG_OLD_NODE)
			{
				m_funcPressNodeLong(m_pCurDragNodeStu2D, (int)m_frameEventState);
			}
			else if (m_funcPressLineLong && m_frameEventState == FrameEventState::DRAG_NEW_NODE)
			{
				m_funcPressLineLong(m_pCurDragNodeStu2D, (int)m_frameEventState);
			}
		}
		else
		{
			if (m_frameEventState == FrameEventState::DRAG_OLD_NODE)
			{
				if (m_funcPressNodeShort)
				{
					//还未执行延时函数，所以不能用 m_pCurDragNodeStu2D
					m_funcPressNodeShort(m_clickDelayParamsStu.pNodeStu2D, (int)m_frameEventState);
				}
			}
		}
	}

	m_frameEventState = FrameEventState::DRAG_NULL;
	m_clickDelayParamsStu.bExecuted = false;
}

void CHmsFrame2DRouteLayer::CallbackAdd(WptPointType t, float lon, float lat, std::string ident, int params)
{
	FrameEventState state = (FrameEventState)params;

	if (state == FrameEventState::DRAG_OLD_NODE || state == FrameEventState::DRAG_NEW_NODE)
	{
		auto index = FindNodeIndex(m_pCurDragNodeStu2D);
		if (m_pFrame2DRoot && index.primaryIndex > -1 && index.subIndex > -1)
		{
			m_pCurDragNodeStu2D->brief.wType = t;
			m_pCurDragNodeStu2D->brief.lon = lon;
			m_pCurDragNodeStu2D->brief.lat = lat;
			m_pCurDragNodeStu2D->brief.ident = ident;
			m_pCurDragNodeStu2D->earthPos = m_pFrame2DRoot->LonLatToEarthPoint(Vec2(lon, lat));

			if (m_pCurDragNodeStu2D->pNode)
			{
				m_pCurDragNodeStu2D->pNode->RemoveFromParent();
			}
            m_pCurDragNodeStu2D->pNode = CreateRouteNode(t, ident, m_routeNodeFontSize, FIX_TYPE_FIX);
            this->AddChild(m_pCurDragNodeStu2D->pNode);
			m_pCurDragNodeStu2D->pNode->SetPosition(m_pFrame2DRoot->EarthPointToFramePoint(m_pCurDragNodeStu2D->earthPos));

			auto index = FindNodeIndex(m_pCurDragNodeStu2D);
			if (m_vecWptNode[index.primaryIndex].ePriType == FPL2DNodeType::FPL_TYPE_PROGRAM)
			{
				ExpandOneProgram(index);
			}
			else
			{
				m_vecWptNode[index.primaryIndex].ePriType = FPL2DNodeType::FPL_TYPE_POINT;
				m_vecWptNode[index.primaryIndex].eSubType = FPL2DNodeType::FPL_TYPE_POINT;
				m_vecWptNode[index.primaryIndex].ident = ident;
			}
		}
	}
	else if (state == FrameEventState::DRAG_NULL)
	{
		InsertWptNode(t, lon, lat, ident, FPLNodeIndex());
	}

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

void CHmsFrame2DRouteLayer::CallbackCancel(int params)
{
	FrameEventState state = (FrameEventState)params;
	if (state == FrameEventState::DRAG_OLD_NODE)
	{
		auto index = FindNodeIndex(m_pCurDragNodeStu2D);
		if (index.primaryIndex > -1 && index.subIndex > -1)
		{
			*(m_pCurDragNodeStu2D) = m_copyNode;
			auto index = FindNodeIndex(m_pCurDragNodeStu2D);
			//CalInsertLonLat(FPLNodeIndex(index.primaryIndex - 1, -1), FPLNodeIndex(index.primaryIndex + 2, -1));
		}
	}
	else if (state == FrameEventState::DRAG_NEW_NODE)
	{
		auto index = FindNodeIndex(m_pCurDragNodeStu2D);
		if (index.primaryIndex > -1 && index.subIndex > -1)
		{
			DeleteWptNode(index, false);
		}
	}

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

void CHmsFrame2DRouteLayer::CallbackDelete(int params)
{
	FrameEventState state = (FrameEventState)params;

	if (state == FrameEventState::DRAG_OLD_NODE || state == FrameEventState::DRAG_NEW_NODE)
	{
		auto index = FindNodeIndex(m_pCurDragNodeStu2D);
		if (index.primaryIndex > -1 && index.subIndex > -1)
		{
			DeleteWptNode(index);
		}
	}

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

void CHmsFrame2DRouteLayer::Reverse()
{
	for (int i = (int)m_vecWptNode.size() - 1; i >= 0; --i)
	{
		if (m_vecWptNode[i].ePriType == FPL2DNodeType::FPL_TYPE_PROGRAM)
		{
			DeleteWptNode(FPLNodeIndex(i, -1), false);
		}
	}

	for (int i = 0; i < (int)m_vecWptNode.size() / 2; ++i)
	{
		auto first = m_vecWptNode[i];
        m_vecWptNode[i] = m_vecWptNode[(int)m_vecWptNode.size() - 1 - i];
        m_vecWptNode[(int)m_vecWptNode.size() - 1 - i] = first;
	}
	//CalInsertLonLat(FPLNodeIndex(0, -1), FPLNodeIndex(m_vecWptNode.size(), -1));

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

void CHmsFrame2DRouteLayer::ClearAll()
{
	for (auto &c : m_vecWptNode)
	{
		for (auto &e : c.vWayPoint)
		{
			if (e.pNode)
			{
				e.pNode->RemoveFromParent();
			}
		}
	}
	m_vecWptNode.clear();

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

void CHmsFrame2DRouteLayer::InsertWptNode(WptPointType t, float lon, float lat, std::string ident, FPLNodeIndex index, bool bExpand /*= true*/)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	WptNodeStu2D stu2D;
	stu2D.brief.wType = t;
	stu2D.brief.lon = lon;
	stu2D.brief.lat = lat;
	stu2D.brief.ident = ident;
	stu2D.earthPos = m_pFrame2DRoot->LonLatToEarthPoint(Vec2(lon, lat));
    stu2D.pNode = CreateRouteNode(t, stu2D.brief.ident, m_routeNodeFontSize, FIX_TYPE_FIX);
#if _NAV_SVS_LOGIC_JUDGE
	if (stu2D.pNode)
#endif
	{
		this->AddChild(stu2D.pNode);
		stu2D.pNode->SetPosition(m_pFrame2DRoot->EarthPointToFramePoint(stu2D.earthPos));
	}

	if (-1 == index.primaryIndex || index.primaryIndex >= (int)m_vecWptNode.size())
	{
		FPL2DNodeInfoStu info;
		info.ePriType = FPL_TYPE_POINT;
		info.eSubType = FPL_TYPE_POINT;
		info.ident = stu2D.brief.ident;
		info.vWayPoint.push_back(stu2D);

		m_vecWptNode.push_back(info);
		index.primaryIndex = m_vecWptNode.size() - 1;
	}
	else
	{
		auto &info = m_vecWptNode.at(index.primaryIndex);
		if (info.ePriType == FPL2DNodeType::FPL_TYPE_PROGRAM && index.subIndex > 0)
		{
			//在这个组插入，要展开组
			info.vWayPoint.insert(info.vWayPoint.begin() + index.subIndex, stu2D);
			if (bExpand)
			{
				ExpandOneProgram(index);
			}
		}
		else
		{
			//在一级索引插入
			FPL2DNodeInfoStu info;
			info.ePriType = FPL_TYPE_POINT;
			info.eSubType = FPL_TYPE_POINT;
			info.ident = stu2D.brief.ident;
			info.vWayPoint.push_back(stu2D);
			m_vecWptNode.insert(m_vecWptNode.begin() + index.primaryIndex, info);
		}
	}
	
	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

void CHmsFrame2DRouteLayer::InsertWptNode(WptNodeBrief brief, FPLNodeIndex index, bool bExpand /*= true*/)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	WptNodeStu2D stu2D;
	stu2D.brief = brief;
	stu2D.earthPos = m_pFrame2DRoot->LonLatToEarthPoint(Vec2(stu2D.brief.lon, stu2D.brief.lat));
    stu2D.pNode = CreateRouteNode(stu2D.brief.wType, stu2D.brief.ident, m_routeNodeFontSize, FIX_TYPE_FIX);
#if _NAV_SVS_LOGIC_JUDGE
	if (stu2D.pNode)
#endif
	{
		this->AddChild(stu2D.pNode);
		stu2D.pNode->SetPosition(m_pFrame2DRoot->EarthPointToFramePoint(stu2D.earthPos));
	}

	if (-1 == index.primaryIndex || index.primaryIndex >= (int)m_vecWptNode.size())
	{
		FPL2DNodeInfoStu info;
		info.ePriType = FPL_TYPE_POINT;
		info.eSubType = FPL_TYPE_POINT;
		info.ident = stu2D.brief.ident;
		info.vWayPoint.push_back(stu2D);

		m_vecWptNode.push_back(info);
		index.primaryIndex = m_vecWptNode.size() - 1;
	}
	else
	{
		auto &info = m_vecWptNode.at(index.primaryIndex);
		if (info.ePriType == FPL2DNodeType::FPL_TYPE_PROGRAM && index.subIndex > 0)
		{
			//在这个组插入，要展开组
			info.vWayPoint.insert(info.vWayPoint.begin() + index.subIndex, stu2D);
			if (bExpand)
			{
				ExpandOneProgram(index);
			}
		}
		else
		{
			//在一级索引插入
			FPL2DNodeInfoStu info;
			info.ePriType = FPL_TYPE_POINT;
			info.eSubType = FPL_TYPE_POINT;
			info.ident = stu2D.brief.ident;
			info.vWayPoint.push_back(stu2D);
			m_vecWptNode.insert(m_vecWptNode.begin() + index.primaryIndex, info);
		}
	}

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

void CHmsFrame2DRouteLayer::DeleteWptNode(FPLNodeIndex index, bool bExpand /*= true*/, bool bDetectAirport /*= true*/)
{
	bool bIsAirport = false;
	std::string airport;

	if (index.primaryIndex < 0 || index.primaryIndex >= (int)m_vecWptNode.size())
	{
		return;
	}

	auto &nodeInfo = m_vecWptNode.at(index.primaryIndex);

	if ((index.subIndex == -1) || (nodeInfo.vWayPoint.size() == 1))
	{
		if (nodeInfo.vWayPoint[0].brief.wType == WptPointType::WPT_AIRPORT)
		{
			bIsAirport = true;
			airport = nodeInfo.vWayPoint[0].brief.ident;
		}
		//删除一级索引
		for (auto &c : nodeInfo.vWayPoint)
		{
			if (c.pNode)
			{
				c.pNode->RemoveFromParent();
			}
		}
		m_vecWptNode.erase(m_vecWptNode.begin() + index.primaryIndex);
	}
	else
	{
		//删除二级索引
		if (nodeInfo.vWayPoint.at(index.subIndex).pNode)
		{
			nodeInfo.vWayPoint.at(index.subIndex).pNode->RemoveFromParent();
		}
		nodeInfo.vWayPoint.erase(nodeInfo.vWayPoint.begin() + index.subIndex);

		if (bExpand)
		{
			ExpandOneProgram(index);
		}
	}
	if (bDetectAirport && bIsAirport)
	{
		DeleteAirportProgram(airport);
	}

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

void CHmsFrame2DRouteLayer::InsertProgramInfo(FPL2DNodeInfoStu &info, FPLNodeIndex curStuIndex)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	auto index = curStuIndex;

	//删除已经存在的同类型程序
	auto vecIndex = FindSameProgram(info.belongAirport, info.eSubType);
	if (vecIndex.size() > 0)
	{
		for (int i = vecIndex.size() - 1; i >= 0; --i)
		{
			if (vecIndex[i].primaryIndex < curStuIndex.primaryIndex)
			{
				--curStuIndex.primaryIndex;
			}
			DeleteWptNode(vecIndex[i], false);
		}
		//插入的位置就是之前删除的位置，如果是进场程序，查询进近程序的位置，插入在前面
		index = vecIndex[0];
		if (info.eSubType == FPL_TYPE_PROGRAM_STAR)
		{
			auto vecTmp = FindSameProgram(info.belongAirport, FPL_TYPE_PROGRAM_APP);
			if (vecTmp.size() > 0)
			{
				index = vecTmp[0];
			}
		}
	}
	else
	{
		//此时index是机场位置
		if (info.eSubType == FPL_TYPE_PROGRAM_SID)
		{
			++index.primaryIndex;
		}
		else if (info.eSubType == FPL_TYPE_PROGRAM_STAR)
		{
			auto vecIndex = FindSameProgram(info.belongAirport, FPL_TYPE_PROGRAM_APP);
			if (vecIndex.size() > 0)
			{
				index = vecIndex[0];
			}
		}
	}

	for (auto &c : info.vWayPoint)
	{
		c.earthPos = m_pFrame2DRoot->LonLatToEarthPoint(Vec2(c.brief.lon, c.brief.lat));
        c.pNode = CreateRouteNode(c.brief.wType, c.brief.ident, m_routeNodeFontSize, c.ctrlPoint.eFixType);
#if _NAV_SVS_LOGIC_JUDGE
		if (c.pNode)
#endif
		{
			this->AddChild(c.pNode);
			c.pNode->SetPosition(m_pFrame2DRoot->EarthPointToFramePoint(c.earthPos));
		}
	}
	if (-1 == index.primaryIndex || index.primaryIndex >= (int)m_vecWptNode.size())
	{
		m_vecWptNode.push_back(info);
		index.primaryIndex = m_vecWptNode.size() - 1;
	}
	else
	{
		m_vecWptNode.insert(m_vecWptNode.begin() + index.primaryIndex, info);
	}
	
	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

FPLNodeIndex CHmsFrame2DRouteLayer::FindNodeIndex(WptNodeStu2D *stu)
{
	FPLNodeIndex index;
	for (const auto &c : m_vecWptNode)
	{
		++index.primaryIndex;
		index.subIndex = -1;

		for (const auto &e : c.vWayPoint)
		{
			++index.subIndex;
			if (&e == stu)
			{
				return index;
			}
		}
	}
	return FPLNodeIndex();
}

void CHmsFrame2DRouteLayer::ExpandOneProgram(FPLNodeIndex index)
{
	if (index.primaryIndex < 0 || index.primaryIndex >= (int)m_vecWptNode.size())
	{
		return;
	}
	FPL2DNodeInfoStu tmp = m_vecWptNode[index.primaryIndex];
	DeleteWptNode(FPLNodeIndex(index.primaryIndex, -1));

	int pos = index.primaryIndex;
	for (auto &c : tmp.vWayPoint)
	{
        c.pNode = CreateRouteNode(c.brief.wType, c.brief.ident, m_routeNodeFontSize, c.ctrlPoint.eFixType);
#if _NAV_SVS_LOGIC_JUDGE
		if (c.pNode)
#endif
        this->AddChild(c.pNode);

		FPL2DNodeInfoStu info;
		info.ePriType = FPL_TYPE_POINT;
		info.eSubType = FPL_TYPE_POINT;
		info.ident = c.brief.ident;
		info.vWayPoint.push_back(c);

		m_vecWptNode.insert(m_vecWptNode.begin() + pos, info);
		++pos;
	}
	
	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

std::vector<FPLNodeIndex> CHmsFrame2DRouteLayer::FindSameProgram(std::string airport, FPL2DNodeType subType)
{
	std::vector<FPLNodeIndex> vecIndex;
	FPLNodeIndex index;
	for (const auto &c : m_vecWptNode)
	{
		++index.primaryIndex;
		if (c.ePriType == FPL_TYPE_PROGRAM)
		{
			if (c.eSubType == subType && c.belongAirport == airport)
			{
				vecIndex.push_back(index);
			}
		}
	}
	return vecIndex;
}

void CHmsFrame2DRouteLayer::DeleteAirportProgram(std::string airport)
{
	if (m_vecWptNode.size() > 0)
	{
		for (int i = m_vecWptNode.size() - 1; i >= 0; --i)
		{
			auto &c = m_vecWptNode[i];
			if (c.ePriType == FPL_TYPE_PROGRAM && c.belongAirport == airport)
			{
				DeleteWptNode(FPLNodeIndex(i, -1), false);
			}
		}
	}

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

bool CHmsFrame2DRouteLayer::InsertFPLNodeByInputRouteString(FPLNodeIndex index, std::string &str)
{
	std::vector<WptNodeBrief> vecRet;
    if (!DecodeInputRouteString(vecRet, str))
    {
        return false;
    }
	
	if (vecRet.empty())
	{
		return false;
	}
	//-1 表示添加在最后面
	if (index.primaryIndex == -1)
	{
		for (auto &c : vecRet)
		{
			InsertWptNode(c.wType, c.lon, c.lat, c.ident, index, false);
		}
	}
	else
	{
        //在中间插入
		for (int i = vecRet.size() - 1; i >= 0; --i)
		{
			auto &c = vecRet[i];
			InsertWptNode(c.wType, c.lon, c.lat, c.ident, index, false);
		}
	}

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);

	return true;
}

bool CHmsFrame2DRouteLayer::DecodeInputRouteString(std::vector<WptNodeBrief> &vecRet, std::string strRoute)
{
	bool bRet = false;
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return bRet;
#endif
	std::vector<std::string> listStr;
	CHmsNavMathHelper::StrSplit(strRoute, " ", listStr);
	if (listStr.size() >= 1)
	{
		float lon = 0;
		float lat = 0;
		auto dbInstance = CHms424Database::GetInstance();
		if (!dbInstance)
		{
			return bRet;
		}
		for (auto &ident : listStr)
		{
            if (ident.empty())
            {
                continue;
            }

            bRet = false;

			if (ident == "D")
			{
				WptNodeBrief stu;
				auto ll = m_pFrame2DRoot->GetPlaneLonLat();
				stu.wType = WptPointType::WPT_USERWPT;
				stu.lon = ll.x;
				stu.lat = ll.y;
				stu.ident = CHmsNavMathHelper::LonLatToString(stu.lon, stu.lat);
				vecRet.push_back(stu);
                bRet = true;
			}
			else if (CHmsNavMathHelper::StrLonLatToFloat(lon, lat, ident))
			{
				WptNodeBrief stu;
				stu.wType = WptPointType::WPT_USERWPT;
				stu.lon = lon;
				stu.lat = lat;
				stu.ident = CHmsNavMathHelper::LonLatToString(stu.lon, stu.lat);
				vecRet.push_back(stu);
                bRet = true;
			}
            else
            {
                std::vector<std::string> vecSplit;
                CHmsNavMathHelper::StrSplit(ident, ":", vecSplit);
                if (vecSplit.size() == 1)
                {
                    //机场可以是1 也可以是2， 自定义航路点是1
                    if (CHmsHistoryRouteDatabase::GetInstance()->GetUserWptLonLatByIdent(lon, lat, ident))
                    {
                        WptNodeBrief stu;
                        stu.wType = WptPointType::WPT_USERWPT;
                        stu.lon = lon;
                        stu.lat = lat;
                        stu.ident = ident;
                        vecRet.push_back(stu);
                        bRet = true;
                    }
                    else if (dbInstance->GetAirportLonLatByIdent(lon, lat, ident))
                    {
                        WptNodeBrief stu;
                        stu.wType = WptPointType::WPT_AIRPORT;
                        stu.lon = lon;
                        stu.lat = lat;
                        stu.ident = ident;
                        vecRet.push_back(stu);
                        bRet = true;
                    }
                }
                else if (vecSplit.size() == 2)
                {
                    //机场 航路点 VOR NDB
                    if (dbInstance->GetAirportLonLatByIdent(lon, lat, vecSplit[0], vecSplit[1]))
                    {
                        WptNodeBrief stu;
                        stu.wType = WptPointType::WPT_AIRPORT;
                        stu.lon = lon;
                        stu.lat = lat;
                        stu.ident = vecSplit[0];
                        vecRet.push_back(stu);
                        bRet = true;
                    }
                    else if (dbInstance->GetWaypointLonLatByIdent(lon, lat, vecSplit[0], vecSplit[1]))
                    {
                        WptNodeBrief stu;
                        stu.wType = WptPointType::WPT_WPT;
                        stu.lon = lon;
                        stu.lat = lat;
                        stu.ident = vecSplit[0];
                        vecRet.push_back(stu);
                        bRet = true;
                    }
                    else if (dbInstance->GetVorLonLatByIdent(lon, lat, vecSplit[0], vecSplit[1]))
                    {
                        WptNodeBrief stu;
                        stu.wType = WptPointType::WPT_VOR;
                        stu.lon = lon;
                        stu.lat = lat;
                        stu.ident = vecSplit[0];
                        vecRet.push_back(stu);
                        bRet = true;
                    }
                    else if (dbInstance->GetNDBLonLatByIdent(lon, lat, vecSplit[0], vecSplit[1]))
                    {
                        WptNodeBrief stu;
                        stu.wType = WptPointType::WPT_NDB;
                        stu.lon = lon;
                        stu.lat = lat;
                        stu.ident = vecSplit[0];
                        vecRet.push_back(stu);
                        bRet = true;
                    }
                }               
            }
            if (!bRet)
            {
                break;
            }
		}
	}

    return bRet;
}

void CHmsFrame2DRouteLayer::DirectTo(FPLNodeIndex index)
{
	if (index.primaryIndex < 0 || index.primaryIndex >= (int)m_vecWptNode.size())
	{
		return;
	}
	//删除前面的点
    for (int i = (int)m_vecWptNode.size() - 1; i >= 0; --i)
	{
		if (i < index.primaryIndex)
		{
			DeleteWptNode(FPLNodeIndex(i, -1), false, false);
		}
	}
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	WptNodeBrief firstNode;
	auto ll = m_pFrame2DRoot->GetPlaneLonLat();
	firstNode.wType = WptPointType::WPT_USERWPT;
	firstNode.lon = ll.x;
	firstNode.lat = ll.y;
	firstNode.ident = CHmsNavMathHelper::LonLatToString(firstNode.lon, firstNode.lat);

	index.primaryIndex = 0;
	if (m_vecWptNode[index.primaryIndex].ePriType == FPL_TYPE_POINT)
	{
		//目标是点直接插入
		InsertWptNode(firstNode.wType, firstNode.lon, firstNode.lat, firstNode.ident, FPLNodeIndex(0, -1), false);
	}
	else
	{
		//目标是程序，先展开程序，再删除前面的点
		ExpandOneProgram(index);
		if (index.subIndex >= 0 && index.subIndex < (int)m_vecWptNode.size())
		{
			for (int i = m_vecWptNode.size() - 1; i >= 0; --i)
			{
				if (i < index.subIndex)
				{
					DeleteWptNode(FPLNodeIndex(i, -1), false, false);
				}
			}
			InsertWptNode(firstNode.wType, firstNode.lon, firstNode.lat, firstNode.ident, FPLNodeIndex(0, -1), false);
		}
	}

    //第一个点设为飞机高度
    if (m_vecWptNode.size() > 0)
    {
        if (m_vecWptNode[0].vWayPoint.size() > 0)
        {
            auto pDataBus = CHmsGlobal::GetInstance()->GetDataBus();
            if (pDataBus)
            {
                auto &stu = m_vecWptNode[0].vWayPoint[0];
                stu.brief.attr.altStu[0].uLimit.bitType.bAt = 1;
                stu.brief.attr.altStu[0].nAltitude = pDataBus->GetMslAltitudeFeet();
            }  
        }
    }

	SetEleChanged(true);
	SetNeedUpdate2DEles(true);
	SetNeedSync(true);
}

bool CHmsFrame2DRouteLayer::GetRouteInfoForSave(std::string &nameToSave, std::vector<int> &vecIndex)
{
	time_t nowtime = time(NULL);
	struct tm *local = localtime(&nowtime);
	char tmpTime[32] = { 0 };
	sprintf(tmpTime, "%d%d%d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
	std::string strTime = tmpTime;

	std::string name;
	if (m_vecWptNode.size() >= 2)
	{
		name += m_vecWptNode[0].ident + "_" + m_vecWptNode.back().ident + "_" + strTime;
		std::vector<int> vec;
		vec.push_back(m_vecWptNode[0].ident.size());
		vec.push_back(m_vecWptNode[0].ident.size() + m_vecWptNode.back().ident.size() + 1);
		vec.push_back(name.size() - 1);

		nameToSave = name;
		vecIndex = vec;
		return true;
	}
	return false;
}

FPLNodeIndex CHmsFrame2DRouteLayer::ConvertIntIndexToFPLIndex(int index)
{
	int pos = -1;
	FPLNodeIndex nodeIndex;
	for (auto &c : m_vecWptNode)
	{
		++nodeIndex.primaryIndex;
		nodeIndex.subIndex = -1;
		for (auto &e : c.vWayPoint)
		{
			++nodeIndex.subIndex;
			++pos;
			if (pos == index)
			{
				return nodeIndex;
			}
		}
	}
	return FPLNodeIndex();
}

void CHmsFrame2DRouteLayer::ReDrawLines()
{
	auto PushPointToStu = [=](RouteLineDrawStu &stu, const std::vector<Vec2> &vec2DPos, const std::vector<bool> &vecVisible)
	{
		std::vector<Vec2> vecLines;
        for (int k = 0; k < (int)vec2DPos.size(); ++k)
		{
			if (vecVisible[k])
			{
				vecLines.push_back(vec2DPos[k]);
			}
			else
			{
				if (vecLines.size() > 0)
				{
					stu.vecArrayVertexs.push_back(vecLines);
					vecLines.clear();
				}
			}
		}
		if (vecLines.size() > 0)
		{
			stu.vecArrayVertexs.push_back(vecLines);
			vecLines.clear();
		}
	};

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLineDrawNode)
		return;
#endif
	m_pRouteLineDrawNode->clear();
	std::vector<RouteLineDrawStu> vecStu;

	//===============画航路
	if (m_vecWptNode.size() < 2)
	{
		return;
	}

	bool bAfterRunway = false;
	FPLNodeIndex index;

	for (auto &c : m_vecWptNode)
	{
		++index.primaryIndex;
		index.subIndex = -1;

		for (auto &e : c.vWayPoint)
		{
			if (!e.bEnableNode)
			{
				continue;
			}
			if (c.eSubType == FPL_TYPE_PROGRAM_APP && e.ctrlPoint.eFixType == FIX_TYPE_RNW)
			{
				bAfterRunway = true;
			}
			++index.subIndex;

			WptNodeStu2D &wptNode = e;
#if _NAV_SVS_LOGIC_JUDGE
			if (!wptNode.pNode)
				continue;
#endif
			wptNode.pNode->SetColor(Color3B::WHITE);
			if (index.primaryIndex == 0 && index.subIndex == 0)
			{
				if (m_flySegment.primaryIndex <= 0)
				{
					wptNode.pNode->SetColor(Color3B::MAGENTA);
				}
				continue;
			}

			Color4B color;
			if (m_flySegment == index)
			{
				color = Color4B::MAGENTA;
				if (bAfterRunway && c.eSubType == FPL_TYPE_PROGRAM_APP && e.ctrlPoint.eFixType != FIX_TYPE_RNW)
				{
					color.a = 20;
				}
				wptNode.pNode->SetColor(Color3B::MAGENTA);
			}
			else if (index < m_flySegment)
			{
				color = Color4B::GRAY;
				if (bAfterRunway && c.eSubType == FPL_TYPE_PROGRAM_APP && e.ctrlPoint.eFixType != FIX_TYPE_RNW)
				{
					color.a = 20;
				}
			}
			else
			{
				color = Color4B::GREEN;
				if (bAfterRunway && c.eSubType == FPL_TYPE_PROGRAM_APP && e.ctrlPoint.eFixType != FIX_TYPE_RNW)
				{
					color.a = 20;
				}
			}

			RouteLineDrawStu stu;
			stu.fLineWidth = 12;
			stu.color = color;

			const std::vector<Vec2> &vec2DPos = wptNode.vecInsert2DPos;
			const std::vector<bool> &vecVisible = wptNode.vecInsertLonLatIsVisible;

			PushPointToStu(stu, vec2DPos, vecVisible);

			vecStu.push_back(stu);
		}
	}
	m_pRouteLineDrawNode->DrawLines(vecStu);
}

void CHmsFrame2DRouteLayer::DrawMoveCicle(const Vec2 & pos)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLineDrawNode)
		return;
#endif
	m_pRouteLineDrawNode->DrawRing(pos, 35, 10, 40, Color4F::BLUE);
}

bool CHmsFrame2DRouteLayer::DotInLine(const Vec2 & pos, const Vec2 & posA, const Vec2 & posB, Vec2 & verticalPos, float fError /*= 15*/)
{
	auto a = pos - posA;
	auto b = posB - posA;
	auto abDot = a.dot(b);
	if (abDot < 0.0f)
	{
		return false;
	}

	auto bLenSq = b.getLengthSq();
	auto c = abDot / bLenSq * b;
	verticalPos = pos + c;

	if (c.getLengthSq() > bLenSq)
	{
		return false;
	}

	auto e = a - c;

	if (e.getLengthSq() <= fError*fError)
	{
		return true;
	}

	return false;
}

void CHmsFrame2DRouteLayer::CalInsertLonLat(FPLNodeIndex beginIndex, FPLNodeIndex endIndex)
{
	if (m_vecWptNode.size() < 2)
	{
		return;
	}

	auto DetectEdge = [](const int &edge1, int &dst, const int &edge2)
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
	DetectEdge(0, beginIndex.primaryIndex, m_vecWptNode.size() - 1);
	DetectEdge(0, endIndex.primaryIndex, m_vecWptNode.size());
	DetectEdge(beginIndex.primaryIndex, beginIndex.primaryIndex, endIndex.primaryIndex);

	m_vecWptNode[0].vWayPoint[0].vecInsertLonLatIsVisible.clear();
	m_vecWptNode[0].vWayPoint[0].vecInsertEarthPos.clear();
	m_vecWptNode[0].vWayPoint[0].vecInsert2DPos.clear();

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();

	WptNodeStu2D *pLastStu = nullptr;
	WptNodeStu2D *pCurStu = nullptr;
	for (int i = beginIndex.primaryIndex; i < endIndex.primaryIndex; ++i)
	{
		auto &c = m_vecWptNode.at(i);
		for (auto &e : c.vWayPoint)
		{
			if (!e.bEnableNode)
			{
				e.vecInsertLonLatIsVisible.clear();
				e.vecInsertEarthPos.clear();
				e.vecInsert2DPos.clear();
				continue;
			}
			if (!pLastStu)
			{
				pLastStu = &e;
				continue;
			}
			else
			{
				pCurStu = &e;

				if (pCurStu->brief.attr.searchShape.type == SHAPE_END)
				{
					if (pLastStu->brief.attr.searchShape.type == SHAPE_SPIRAL)
					{
						ProcessInsertSpiral(pLastStu, pCurStu, pFrust);
					}
					else if (pLastStu->brief.attr.searchShape.type == SHAPE_RECT_SPIRAL)
					{
						ProcessInsertRectSpiral(pLastStu, pCurStu, pFrust);
					}
					else if (pLastStu->brief.attr.searchShape.type == SHAPE_RECT)
					{
						ProcessInsertRect(pLastStu, pCurStu, pFrust);
					}
				}
				else
				{
					if (pCurStu->ctrlPoint.eFixType == FIX_TYPE_HOLD_TIME || pCurStu->ctrlPoint.eFixType == FIX_TYPE_HOLD_DEST)
					{
						ProcessInsertHoldAt(pLastStu, pCurStu, pFrust);
					}
					else
					{
						ProcessInsertNormal(pLastStu, pCurStu, pFrust);
					}
				}
				pLastStu = pCurStu;
				pCurStu = nullptr;
			}
		}
	}
}

void CHmsFrame2DRouteLayer::ProcessInsertNormal(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust)
{
	if (pLastStu == nullptr || pCurStu == nullptr || m_pFrame2DRoot == nullptr)
	{
		return;
	}
	pCurStu->vecInsertLonLatIsVisible.clear();
	pCurStu->vecInsertEarthPos.clear();
	pCurStu->vecInsert2DPos.clear();
	//30km插值，用于画线, 根据级别确定插值距离
	int nLayer = m_earthLayer;
	if (nLayer < 5)
	{
		nLayer = 5;
	}
	auto dis = 30 * pow(2, (8 - nLayer));
	if (dis < 1)
	{
		dis = 1;
	}
	std::vector<Vec3> vecEarthPos;
	m_pFrame2DRoot->InsertEarthPos(Vec2(pLastStu->brief.lon, pLastStu->brief.lat), Vec2(pCurStu->brief.lon, pCurStu->brief.lat), vecEarthPos, dis);
	for (const auto &earthPos : vecEarthPos)
	{
		if (pFrust)
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(!pFrust->isOutOfFrustum(earthPos));
		}
		else
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(true);
		}
		pCurStu->vecInsertEarthPos.push_back(earthPos);
		pCurStu->vecInsert2DPos.push_back(m_pFrame2DRoot->EarthPointToFramePoint(earthPos));
	}
}

void CHmsFrame2DRouteLayer::ProcessInsertHoldAt(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust)
{
	if (pLastStu == nullptr || pCurStu == nullptr || m_pFrame2DRoot == nullptr)
	{
		return;
	}
	pCurStu->vecInsertLonLatIsVisible.clear();
	pCurStu->vecInsertEarthPos.clear();
	pCurStu->vecInsert2DPos.clear();

	double legLen = 0;
	if (pCurStu->ctrlPoint.eFixType == FIX_TYPE_HOLD_TIME)
	{
		legLen = 3 * KnotKMH;//nm
	}
	else
	{
		legLen = pCurStu->ctrlPoint.nLegData * KnotKMH;
	}

	auto inboundFix1 = Vec2(pCurStu->brief.lon, pCurStu->brief.lat);
	auto inboundFix2 = CHmsNavMathHelper::NextPointLonLatByBearing(inboundFix1, legLen, pCurStu->ctrlPoint.nHeading + 180);

	auto radius = legLen / SIM_Pi;
	double angle = 0;
	if (pCurStu->ctrlPoint.ctrl.nDirectType == -1)
	{
		angle = pCurStu->ctrlPoint.nHeading - 90;
	}
	else
	{
		angle = pCurStu->ctrlPoint.nHeading + 90;
	}
	auto outboundFix1 = CHmsNavMathHelper::NextPointLonLatByBearing(inboundFix1, radius * 2, angle);
	auto outboundFix2 = CHmsNavMathHelper::NextPointLonLatByBearing(inboundFix2, radius * 2, angle);

	auto fixEndCenter = CHmsNavMathHelper::NextPointLonLatByBearing(inboundFix1, radius, angle);
	auto outEndCenter = CHmsNavMathHelper::NextPointLonLatByBearing(inboundFix2, radius, angle);

	//顺时针画圆
	auto InsertCircle = [=](Vec2 point, Vec2 center, std::vector<Vec3> &vecRet)
	{
		Vec3 earthPoint = CHmsNavMathHelper::LonLatToEarthPoint(point);
		Vec3 earthCenter = CHmsNavMathHelper::LonLatToEarthPoint(center);
		for (int i = 1; i < 30; ++i)
		{
			Quaternion q(earthCenter, SIM_Pi / 30 * i);
			vecRet.push_back(q*earthPoint);
		}
	};

	std::vector<Vec3> vecEarthPos;
	const float insertDis = 0.3f * KnotKMH;
	if (pCurStu->ctrlPoint.ctrl.nDirectType == -1)
	{
		m_pFrame2DRoot->InsertEarthPos(inboundFix1, inboundFix2, vecEarthPos, insertDis);
		InsertCircle(inboundFix2, outEndCenter, vecEarthPos);
		m_pFrame2DRoot->InsertEarthPos(outboundFix2, outboundFix1, vecEarthPos, insertDis);
		InsertCircle(outboundFix1, fixEndCenter, vecEarthPos);
	}
	else
	{
		m_pFrame2DRoot->InsertEarthPos(outboundFix1, outboundFix2, vecEarthPos, insertDis);
		InsertCircle(outboundFix2, outEndCenter, vecEarthPos);
		m_pFrame2DRoot->InsertEarthPos(inboundFix2, inboundFix1, vecEarthPos, insertDis);
		InsertCircle(inboundFix1, fixEndCenter, vecEarthPos);
	}
	//填补缺口
	if (vecEarthPos.size() > 0)
	{
		vecEarthPos.push_back(vecEarthPos[0]);
	}

	for (const auto &earthPos : vecEarthPos)
	{
		if (pFrust)
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(!pFrust->isOutOfFrustum(earthPos));
		}
		else
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(true);
		}
		pCurStu->vecInsertEarthPos.push_back(earthPos);
		pCurStu->vecInsert2DPos.push_back(m_pFrame2DRoot->EarthPointToFramePoint(earthPos));
	}
}

void CHmsFrame2DRouteLayer::ProcessInsertSpiral(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust)
{
	if (pLastStu == nullptr || pCurStu == nullptr)
	{
		return;
	}
	pCurStu->vecInsertLonLatIsVisible.clear();
	pCurStu->vecInsertEarthPos.clear();
	pCurStu->vecInsert2DPos.clear();

	auto shapeStu = pLastStu->brief.attr.searchShape;
	float spaceSpeed = shapeStu.space / 360.0;
	float angleSpace = 10;
	float radius = -angleSpace * spaceSpeed;
	float angle = shapeStu.angle - angleSpace;

	std::vector<Vec3> vecEarthPos;
	vecEarthPos.push_back(pLastStu->earthPos);
	for (int i = 0; i < 2000; ++i)
	{
		angle += angleSpace;
		radius += angleSpace * spaceSpeed;

		auto ll = CHmsNavMathHelper::NextPointLonLatByBearing(Vec2(pLastStu->brief.lon, pLastStu->brief.lat), radius * KnotKMH, angle);
		auto earthPos = CHmsNavMathHelper::LonLatToEarthPoint(ll);
		vecEarthPos.push_back(earthPos);

		if (radius > shapeStu.radius)
		{
			pCurStu->brief.lon = ll.x;
			pCurStu->brief.lat = ll.y;
			pCurStu->earthPos = earthPos;
			break;
		}
	}
	for (auto &earthPos : vecEarthPos)
	{
		if (pFrust)
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(!pFrust->isOutOfFrustum(earthPos));
		}
		else
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(true);
		}
		pCurStu->vecInsertEarthPos.push_back(earthPos);
		pCurStu->vecInsert2DPos.push_back(m_pFrame2DRoot->EarthPointToFramePoint(earthPos));
	}
}

void CHmsFrame2DRouteLayer::ProcessInsertRectSpiral(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust)
{
	if (pLastStu == nullptr || pCurStu == nullptr)
	{
		return;
	}
	pCurStu->vecInsertLonLatIsVisible.clear();
	pCurStu->vecInsertEarthPos.clear();
	pCurStu->vecInsert2DPos.clear();

	auto shapeStu = pLastStu->brief.attr.searchShape;
	float angleSpace = 90;
	float radius = 0;
	float angle = shapeStu.angle - angleSpace;

	std::vector<Vec3> vecEarthPos;
	Vec2 lastLonLat = CHmsNavMathHelper::EarthPointToLonLat(pLastStu->earthPos);
	Vec2 curLonLat;
	Vec2 originOne = lastLonLat;
	Vec2 originTwo;
	for (int i = 0; i < 1000; ++i)
	{
		if (0 == i)
		{
			angle += angleSpace;
			curLonLat = CHmsNavMathHelper::NextPointLonLatByBearing(lastLonLat, shapeStu.space * KnotKMH, angle);
			CHmsNavMathHelper::InsertEarthPos(lastLonLat, curLonLat, vecEarthPos, 3.0f * KnotKMH);
			vecEarthPos.pop_back();
			originTwo = curLonLat;

			lastLonLat = curLonLat;
		}
		else
		{
			if (i % 4 == 1)
			{
				radius += shapeStu.space * 1.414213;
				angle = shapeStu.angle + 45;
			}
			else
			{
				angle += 90;
			}
			if (i % 4 == 0)
			{
				curLonLat = CHmsNavMathHelper::NextPointLonLatByBearing(originTwo, radius * KnotKMH, angle);
			}
			else
			{
				curLonLat = CHmsNavMathHelper::NextPointLonLatByBearing(originOne, radius * KnotKMH, angle);
			}
			CHmsNavMathHelper::InsertEarthPos(lastLonLat, curLonLat, vecEarthPos, 3.0f * KnotKMH);
			//vecEarthPos.pop_back();

			lastLonLat = curLonLat;
		}

		if (radius > shapeStu.radius)
		{
			pCurStu->brief.lon = curLonLat.x;
			pCurStu->brief.lat = curLonLat.y;
			pCurStu->earthPos = CHmsNavMathHelper::LonLatToEarthPoint(curLonLat);

			vecEarthPos.push_back(pCurStu->earthPos);

			break;
		}
	}
	for (auto &earthPos : vecEarthPos)
	{
		if (pFrust)
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(!pFrust->isOutOfFrustum(earthPos));
		}
		else
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(true);
		}
		pCurStu->vecInsertEarthPos.push_back(earthPos);
		pCurStu->vecInsert2DPos.push_back(m_pFrame2DRoot->EarthPointToFramePoint(earthPos));
	}
}

void CHmsFrame2DRouteLayer::ProcessInsertRect(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust)
{
	if (pLastStu == nullptr || pCurStu == nullptr)
	{
		return;
	}
	pCurStu->vecInsertLonLatIsVisible.clear();
	pCurStu->vecInsertEarthPos.clear();
	pCurStu->vecInsert2DPos.clear();

	auto shapeStu = pLastStu->brief.attr.searchShape;
	Vec2 originOne;
	Vec2 origin;
	Vec2 originTwo;
	origin = CHmsNavMathHelper::EarthPointToLonLat(pLastStu->earthPos);
	if (shapeStu.posInSide == 0)
	{
		originOne = origin;
	}
	else
	{
		originOne = CHmsNavMathHelper::NextPointLonLatByBearing(origin, shapeStu.posInSide * shapeStu.height * KnotKMH,
			shapeStu.angle + 180);
	}
	if (shapeStu.posInSide == 1)
	{
		originTwo = origin;
	}
	else
	{
		originTwo = CHmsNavMathHelper::NextPointLonLatByBearing(origin, (1 - shapeStu.posInSide) * shapeStu.height * KnotKMH,
			shapeStu.angle);
	}
	auto CreateFix = [=](Vec2 origin, float angle, float space, float maxDis, std::vector<Vec2> &vecLonLat)
	{
		for (int i = 1; i < 1000; ++i)
		{
			auto tmp = CHmsNavMathHelper::NextPointLonLatByBearing(origin, i * space * KnotKMH, angle);
			vecLonLat.push_back(tmp);
			if (i * space > maxDis)
			{
				break;
			}
		}
	};
	std::vector<Vec2> vecOnePoints;
	CreateFix(originOne, shapeStu.angle + 90, shapeStu.space, shapeStu.width, vecOnePoints);
	std::vector<Vec2> vecTwoPoints;
	CreateFix(originTwo, shapeStu.angle + 90, shapeStu.space, shapeStu.width, vecTwoPoints);
	std::vector<Vec2> vecAllPoints;
	if (shapeStu.posInSide != 1)
	{
		vecAllPoints.push_back(origin);
	}
	vecTwoPoints.insert(vecTwoPoints.begin(), originTwo);

	if (vecTwoPoints.size() >= 2 && vecOnePoints.size() >= 2)
	{
		for (int i = 0; i < 2000; ++i)
		{
			if (vecTwoPoints.size() - i * 2 < 2)
			{
				break;
			}
			vecAllPoints.push_back(vecTwoPoints[i * 2 + 0]);
			vecAllPoints.push_back(vecTwoPoints[i * 2 + 1]);

			if (vecOnePoints.size() - i * 2 < 2)
			{
				break;
			}
			vecAllPoints.push_back(vecOnePoints[i * 2 + 0]);
			vecAllPoints.push_back(vecOnePoints[i * 2 + 1]);
		}
	}
	std::vector<Vec3> vecEarthPos;
    for (int i = 1; i < (int)vecAllPoints.size(); ++i)
	{
		CHmsNavMathHelper::InsertEarthPos(vecAllPoints[i - 1], vecAllPoints[i], vecEarthPos, 3 * KnotKMH);
        if (i == (int)vecAllPoints.size() - 1)
		{
			pCurStu->brief.lon = vecAllPoints.back().x;
			pCurStu->brief.lat = vecAllPoints.back().y;
			pCurStu->earthPos = CHmsNavMathHelper::LonLatToEarthPoint(Vec2(pCurStu->brief.lon, pCurStu->brief.lat));
		}
	}
	for (auto &earthPos : vecEarthPos)
	{
		if (pFrust)
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(!pFrust->isOutOfFrustum(earthPos));
		}
		else
		{
			pCurStu->vecInsertLonLatIsVisible.push_back(true);
		}
		pCurStu->vecInsertEarthPos.push_back(earthPos);
		pCurStu->vecInsert2DPos.push_back(m_pFrame2DRoot->EarthPointToFramePoint(earthPos));
	}
}

bool CHmsFrame2DRouteLayer::TestPressOnOldNode(const HmsAviPf::Vec2 & posOrigin)
{
	for (auto &c : m_vecWptNode)
	{
		for (auto &e : c.vWayPoint)
		{
			if (!e.pNode->IsVisible())
			{
				continue;
			}
			auto rect = e.pNode->GetRectFromParent();
			if (rect.containsPoint(posOrigin))
			{
				m_clickDelayParamsStu.pNodeStu2D = &e;
				m_frameEventState = FrameEventState::DRAG_OLD_NODE;

				return true;
			}
		}
	}
	return false;
}

bool CHmsFrame2DRouteLayer::TestPressOnLine(const HmsAviPf::Vec2 & posOrigin)
{
	Vec2 verticalPos;

	FPLNodeIndex index;
	for (auto &c : m_vecWptNode)
	{
		++index.primaryIndex;
		index.subIndex = -1;

		for (auto &e : c.vWayPoint)
		{
			++index.subIndex;

			const std::vector<Vec2> &vecPos = e.vecInsert2DPos;
			if (vecPos.empty())
			{
				continue;
			}
            for (int k = 0; k < (int)vecPos.size() - 1; ++k)
			{
				if (!e.vecInsertLonLatIsVisible[k])
				{
					continue;
				}
				if (DotInLine(posOrigin, vecPos[k], vecPos[k + 1], verticalPos))
				{
					m_clickDelayParamsStu.posOrigin = posOrigin;
					m_clickDelayParamsStu.index = index;
					m_frameEventState = FrameEventState::DRAG_NEW_NODE;

					return true;
				}
			}
		}
	}

	return false;
}

void CHmsFrame2DRouteLayer::ProcessNodeEnableOrNot()
{
	for (auto &c : m_vecWptNode)
	{
		for (auto &e : c.vWayPoint)
		{
			e.bEnableNode = true;
		}
	}
	std::map<std::string, int> mapAirport;
	for (auto &c : m_vecWptNode)
	{
		for (auto &e : c.vWayPoint)
		{
			//如果有进离场程序就不在地图上显示机场
			if (c.ePriType == FPL_TYPE_PROGRAM)
			{
				mapAirport.insert(std::make_pair(c.belongAirport, 0));

				//如果有进近程序，就不在地图上显示进场程序跑道
				if (c.eSubType == FPL_TYPE_PROGRAM_STAR)
				{
					auto vecIndex = FindSameProgram(c.belongAirport, FPL_TYPE_PROGRAM_APP);
					if (vecIndex.size() > 0)
					{
						auto &last = c.vWayPoint[c.vWayPoint.size() - 1];
						if (last.ctrlPoint.eFixType == FIX_TYPE_RNW)
						{
							last.bEnableNode = false;
						}
					}
				}
			}
		}
	}
	for (auto &c : m_vecWptNode)
	{
		for (auto &e : c.vWayPoint)
		{
			if (e.brief.wType == WptPointType::WPT_AIRPORT && mapAirport.find(e.brief.ident) != mapAirport.end())
			{
				e.bEnableNode = false;
			}
		}
	}

	//类型为SHAPE_END的点是插值时自动插入的，重新插值时要先删除这个点
	for (int i = m_vecWptNode.size() - 1; i >= 0; --i)
	{
		if (m_vecWptNode[i].vWayPoint.size() > 0)
		{
			if (m_vecWptNode[i].vWayPoint[0].brief.attr.searchShape.type == SHAPE_END)
			{
				DeleteWptNode(FPLNodeIndex(i, -1), false, false);
			}
		}
	}
	//重新插入结束点
	for (int i = m_vecWptNode.size() - 1; i >= 0; --i)
	{
		if (m_vecWptNode[i].ePriType != FPL_TYPE_PROGRAM && m_vecWptNode[i].vWayPoint.size() > 0)
		{
			auto type = m_vecWptNode[i].vWayPoint[0].brief.attr.searchShape.type;
			if (type == SHAPE_SPIRAL || type == SHAPE_RECT_SPIRAL || type == SHAPE_RECT)
			{
				WptNodeStu2D tmpStu2D;
				tmpStu2D.brief.ident = "SearchEnd";
				tmpStu2D.brief.attr.searchShape.type = SHAPE_END;
				tmpStu2D.ctrlPoint.eFixType = FIX_TYPE_FIX;
				tmpStu2D.pNode = CHmsNode::Create();
				this->AddChild(tmpStu2D.pNode);

				FPL2DNodeInfoStu stu;
				stu.ePriType = FPL_TYPE_POINT;
				stu.eSubType = FPL_TYPE_POINT;
				stu.ident = tmpStu2D.brief.ident;
				stu.vWayPoint.push_back(tmpStu2D);

				m_vecWptNode.insert(m_vecWptNode.begin() + i + 1, stu);
			}
		}
	}

	CalInsertLonLat(FPLNodeIndex(), FPLNodeIndex(m_vecWptNode.size(), 0));
}

CHmsNode * CHmsFrame2DRouteLayer::CreateRouteNode(WptPointType type, std::string &strText, float textSize, FIX_TYPE fixType)
{
	auto pParent = CHmsNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!pParent)
		return nullptr;
#endif
	pParent->SetAnchorPoint(Vec2(0.5, 0.5));
	pParent->SetContentSize(Size(150, 50));

	if (fixType == FIX_TYPE_COURSE_ALTITUDE ||
		fixType == FIX_TYPE_COURSE_VECTORS ||
		fixType == FIX_TYPE_COURSE_RADIAL_FROM_FIX ||
		fixType == FIX_TYPE_COURSE_RADIAL_TO_FIX)
	{

	}
	else
	{
		std::string str = CHmsWptInfoDlg::GetImagePathByWptType(type);
		auto pIcon = CHmsImageNode::Create(str.c_str());
		pIcon->SetAnchorPoint(Vec2(0.5, 0.5));
        pIcon->SetPosition(pParent->GetContentSize() * 0.5);
		pParent->AddChild(pIcon);
	}
    auto pLabel = CHmsGlobal::CreateLabel(strText.c_str(), textSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pLabel)
		return pParent;
#endif
    pLabel->SetAnchorPoint(0, 0.5);
    pLabel->SetPosition(Vec2(pParent->GetContentSize().width * 0.5 + 30, pParent->GetContentSize().height * 0.5));
    pLabel->SetTextColor(Color4B::BLACK);

	auto pBgImage = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pBgImage)
		return pParent;
#endif
	pBgImage->SetAnchorPoint(Vec2(0, 0.5));
    pBgImage->SetPosition(pLabel->GetPoistion() - Vec2(10, 0));
    pBgImage->SetContentSize(pLabel->GetContentSize() + Size(20, 0));
	pBgImage->SetColor(Color3B(0x9d, 0xb0, 0xfa));
	pBgImage->SetOpacity(200);
	pParent->AddChild(pBgImage);

    pParent->AddChild(pLabel);

	return pParent;
}

void CHmsFrame2DRouteLayer::AddClickDelay()
{
	CancelClickDelay();

	m_bEnableClickDelay = true;

	m_pClickDelayAction = CHmsActionDelayCallback::Create(0.8f, HMS_CALLBACK_0(CHmsFrame2DRouteLayer::PressClickDelay, this));

	this->RunAction(m_pClickDelayAction);
}
void CHmsFrame2DRouteLayer::CancelClickDelay()
{
	m_bEnableClickDelay = false;
	if (m_pClickDelayAction)
	{
		this->StopActioin(m_pClickDelayAction);
		m_pClickDelayAction = nullptr;
	}
}
void CHmsFrame2DRouteLayer::PressClickDelay()
{
	if (m_bEnableClickDelay)
	{
		m_bEnableClickDelay = false;
		m_pClickDelayAction = nullptr;

		m_clickDelayParamsStu.bExecuted = true;

		if (m_frameEventState == FrameEventState::DRAG_OLD_NODE)
		{
			if (m_clickDelayParamsStu.pNodeStu2D)
			{
				m_pCurDragNodeStu2D = m_clickDelayParamsStu.pNodeStu2D;
				m_copyNode = *m_pCurDragNodeStu2D;
				m_frameEventState = FrameEventState::DRAG_OLD_NODE;
#if _NAV_SVS_LOGIC_JUDGE
				if (!m_pCurDragNodeStu2D)
					return;
#endif
				m_posImageWhenPress = m_pCurDragNodeStu2D->pNode->GetPoistion();
			}
		}
		else if (m_frameEventState == FrameEventState::DRAG_NEW_NODE)
		{
			Vec3 earthPos;
#if _NAV_SVS_LOGIC_JUDGE
			if (!m_pFrame2DRoot)
				return;
#endif
			m_pFrame2DRoot->Frame2DPointToEarthPoint(m_clickDelayParamsStu.posOrigin, earthPos);
			auto lonLat = m_pFrame2DRoot->EarthPointToLonLat(earthPos);
			auto str = CHmsNavMathHelper::LonLatToString(lonLat.x, lonLat.y);
			auto index = m_clickDelayParamsStu.index;
			InsertWptNode(WptPointType::WPT_USERWPT, lonLat.x, lonLat.y, str, index, false);

			m_pCurDragNodeStu2D = &m_vecWptNode[index.primaryIndex].vWayPoint[index.subIndex];
			m_frameEventState = FrameEventState::DRAG_NEW_NODE;
			m_posImageWhenPress = m_pCurDragNodeStu2D->pNode->GetPoistion();
		}
	}
}

void CHmsFrame2DRouteLayer::RegisterPressNodeShort(t_RouteClickCallback func)
{
	m_funcPressNodeShort = func;
}

void CHmsFrame2DRouteLayer::RegisterPressNodeLong(t_RouteClickCallback func)
{
	m_funcPressNodeLong = func;
}

void CHmsFrame2DRouteLayer::RegisterPressLineShort(t_RouteClickCallback func)
{
	m_funcPressLineShort = func;
}

void CHmsFrame2DRouteLayer::RegisterPressLineLong(t_RouteClickCallback func)
{
	m_funcPressLineLong = func;
}

void CHmsFrame2DRouteLayer::RegisterEleChangeCallback(t_RouteEleChangeCallback func)
{
	m_funcEleChangeCallback = func;
}

FrameEventState CHmsFrame2DRouteLayer::GetFrameEventState()
{
	return m_frameEventState;
}

const std::vector<FPL2DNodeInfoStu>& CHmsFrame2DRouteLayer::GetWptNodes()
{
	return m_vecWptNode;
}

const WptNodeStu2D* CHmsFrame2DRouteLayer::GetWptNodeStu2D(FPLNodeIndex index)
{
	if (index.primaryIndex >= 0 && index.primaryIndex < (int)m_vecWptNode.size())
	{
		if (index.subIndex >= 0 && index.subIndex < (int)m_vecWptNode[index.primaryIndex].vWayPoint.size())
		{
			return &m_vecWptNode[index.primaryIndex].vWayPoint[index.subIndex];
		}
	}
	return nullptr;
}