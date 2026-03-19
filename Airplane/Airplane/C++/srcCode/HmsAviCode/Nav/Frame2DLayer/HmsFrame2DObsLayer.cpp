#include "HmsFrame2DObsLayer.h"
#include "../HmsFrame2DRoot.h"

CHmsFrame2DObsLayer::CHmsFrame2DObsLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_Obs)
{
	m_planeSegStu.vecScale.reserve(8);
}

CHmsFrame2DObsLayer::~CHmsFrame2DObsLayer()
{
}

CHmsFrame2DObsLayer* CHmsFrame2DObsLayer::Create(Size layerSize)
{
	CHmsFrame2DObsLayer *ret = new (std::nothrow) CHmsFrame2DObsLayer();
	if (ret && ret->Init(layerSize))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsFrame2DObsLayer::Init(Size layerSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	this->SetContentSize(layerSize);

	m_pAircraftWptLineNode = CHmsRouteLineDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pAircraftWptLineNode)
#endif
	{
		m_pAircraftWptLineNode->SetAnchorPoint(0, 0);
		m_pAircraftWptLineNode->SetContentSize(this->GetContentSize());
		m_pAircraftWptLineNode->SetPosition(0, 0);
		m_pAircraftWptLineNode->SetTexturePath("res/airport/bg2.png");
		this->AddChild(m_pAircraftWptLineNode);
	}

	m_pObsFullLineNode = CHmsRouteLineDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pObsFullLineNode)
#endif
	{
		m_pObsFullLineNode->SetAnchorPoint(0, 0);
		m_pObsFullLineNode->SetContentSize(this->GetContentSize());
		m_pObsFullLineNode->SetPosition(0, 0);
		m_pObsFullLineNode->SetTexturePath("res/airport/bg2.png");
		this->AddChild(m_pObsFullLineNode);
	}

	m_pObsDashLineNode = CHmsRouteLineDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pObsDashLineNode)
#endif
	{
		m_pObsDashLineNode->SetAnchorPoint(0, 0);
		m_pObsDashLineNode->SetContentSize(this->GetContentSize());
		m_pObsDashLineNode->SetPosition(0, 0);
		m_pObsDashLineNode->SetTexturePath("res/NavImage/navLine_dash.png");
		this->AddChild(m_pObsDashLineNode);
	}

	return true;
}

void CHmsFrame2DObsLayer::Update(float delta)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	auto segIndex = m_pFrame2DRoot->GetFlySegment();
	auto tmpNode = m_pFrame2DRoot->GetWptNodeStu2D(segIndex);
	if (!tmpNode || m_pFrame2DRoot->IsPlaneLonLatValid() == false)
	{
		ClearAllLine();
		return;
	}
	auto planeLonLat = m_pFrame2DRoot->GetPlaneLonLat();
	auto wptLonLat = Vec2(tmpNode->brief.lon, tmpNode->brief.lat);
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsGlobal::GetInstance()->GetDataBus())
		return;
#endif
	auto ctrlData = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
#if _NAV_SVS_LOGIC_JUDGE
	if (!ctrlData)
		return;
#endif
	float fObsAngle = ctrlData->GetCurCourse();
	bool bIsObsMode = ctrlData->IsObsMode();
	if (m_planeSegStu.wptLonLat != wptLonLat)
	{
		m_planeSegStu.wptLonLat = wptLonLat;
		if (bIsObsMode)
		{
			m_planeSegStu.bIsObsMode = bIsObsMode;
			m_planeSegStu.fObsAngle = fObsAngle;
			ReInsertObsLine3Dpos(fObsAngle);
			ReDrawObsLine();
		}	
		else
		{
			m_planeSegStu.bIsObsMode = bIsObsMode;
			m_planeSegStu.fObsAngle = fObsAngle;
			ClearObsData();
			ReDrawObsLine();
		}

		m_planeSegStu.planeLonLat = planeLonLat;
		ReInsertPlaneLine3Dpos();
		ReDrawPlaneLine();
	}
	else
	{
		if (bIsObsMode)
		{
			if (m_planeSegStu.bIsObsMode != bIsObsMode || m_planeSegStu.fObsAngle != fObsAngle)
			{
				m_planeSegStu.bIsObsMode = bIsObsMode;
				m_planeSegStu.fObsAngle = fObsAngle;
				ReInsertObsLine3Dpos(fObsAngle);
				ReDrawObsLine();
			}
		}
		else
		{
			if (m_planeSegStu.bIsObsMode != bIsObsMode)
			{
				m_planeSegStu.bIsObsMode = bIsObsMode;
				m_planeSegStu.fObsAngle = fObsAngle;
				ClearObsData();
				ReDrawObsLine();
			}
		}
		if (m_planeSegStu.planeLonLat != planeLonLat)
		{
			m_planeSegStu.planeLonLat = planeLonLat;
			ReInsertPlaneLine3Dpos();
			ReDrawPlaneLine();
		}
	}
}

void CHmsFrame2DObsLayer::Update2DElements()
{
	UpdateObsLine2DPos();
	ReDrawObsLine();

	UpdatePlaneLine2DPos();
	ReDrawPlaneLine();
}

CHmsNode * CHmsFrame2DObsLayer::CreateObsScaleNode(std::string &strText, float textSize)
{
	auto pParent = CHmsNode::Create();
	if (pParent)
	{
		pParent->SetAnchorPoint(Vec2(0.5, 0.5));
		pParent->SetContentSize(Size(100, 26));
		this->AddChild(pParent);

		std::string str = CHmsWptInfoDlg::GetImagePathByWptType(WptPointType::WPT_AIRPORT);
		auto pIcon = CHmsImageNode::Create(str.c_str());
		if (pIcon)
		{
			pIcon->SetAnchorPoint(Vec2(0.5, 0.5));
			pIcon->SetPosition(Vec2(50, 13));
			pIcon->SetScale(0.3f);
			pParent->AddChild(pIcon);
		}

		auto pBgImage = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png");
		if (pBgImage)
		{
			pBgImage->SetAnchorPoint(Vec2(0, 0.5));
			pBgImage->SetPosition(Vec2(65, 13));
			if (strText.size() > 15)
			{
				pBgImage->SetContentSize(Size(strText.size() * 9, 25));
			}
			else
			{
				pBgImage->SetContentSize(Size(strText.size() * 11, 25));
			}
			pBgImage->SetColor(Color3B::GRAY);
			pBgImage->SetOpacity(200);
			pParent->AddChild(pBgImage);
		}

		auto pLabel = CHmsGlobal::CreateLabel(strText.c_str(), textSize);
		if (pLabel)
		{
			pLabel->SetAnchorPoint(0, 0.5);
			pLabel->SetPosition(Vec2(65, 13));
			pLabel->SetTextColor(Color4B::BLACK);
			pParent->AddChild(pLabel);
		}
	}

	return pParent;
}

void CHmsFrame2DObsLayer::InsertDot(Vec2 lonLat1, Vec2 lonLat2, NavLineAttrStu &lineAttr)
{
	lineAttr.Clear();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
	std::vector<Vec2> vecLonLat;
	CHmsNavMathHelper::InsertLonLatNotEqual(lonLat1, lonLat2, vecLonLat);
    for (int i = 0; i < (int)vecLonLat.size(); ++i)
	{
		auto earthPos = m_pFrame2DRoot->LonLatToEarthPoint(vecLonLat[i]);
		if (pFrust)
		{
			lineAttr.vecIsVisible.push_back(!pFrust->isOutOfFrustum(earthPos));
		}
		else
		{
			lineAttr.vecIsVisible.push_back(false);
		}
		lineAttr.vecEarthPos.push_back(earthPos);
		lineAttr.vec2DPos.push_back(m_pFrame2DRoot->EarthPointToFramePoint(earthPos));
	}
}

void CHmsFrame2DObsLayer::ReInsertObsLine3Dpos(float angle)
{
#if 0
	double dis = CalculateDistanceKM(
		m_planeSegStu.planeLonLat.x, m_planeSegStu.planeLonLat.y,
		m_planeSegStu.wptLonLat.x, m_planeSegStu.wptLonLat.y);

	dis *= 2.6;
	if (dis < 100) dis = 100;
	if (dis > 800) dis = 500;
#else
    double dis = CHmsNavComm::GetInstance()->GetNMOrKM(30);
#endif

	ClearObsData();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	auto nextLonLat = m_pFrame2DRoot->NextPointLonLatByBearing(m_planeSegStu.wptLonLat, dis*0.5, angle);
	InsertDot(m_planeSegStu.wptLonLat, nextLonLat, m_planeSegStu.obsFull);

	nextLonLat = m_pFrame2DRoot->NextPointLonLatByBearing(m_planeSegStu.wptLonLat, dis*0.5, angle + 180);
	InsertDot(m_planeSegStu.wptLonLat, nextLonLat, m_planeSegStu.obsDash);

	//OBS刻度
	static int scaleArray[] = { 5, 10, 15 };
	int scaleCount = 0;
	std::vector<float> vecAngle;
	vecAngle.push_back(angle);
	vecAngle.push_back(angle + 180);
	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
	for (auto lineAngle : vecAngle)
	{
		for (int scaleIndex = 0; scaleIndex < 3; ++scaleIndex)
		{
            auto scaleNum = CHmsNavComm::GetInstance()->GetNMOrKM(scaleArray[scaleIndex]);
			if (scaleCount >= (int)m_planeSegStu.vecScale.size())
			{
                auto scaleStr = CHmsNavComm::GetInstance()->AppendNMOrKMUnit("%d", scaleArray[scaleIndex]);
				auto label = CreateObsScaleNode(scaleStr, 14);
				OBSScaleLabelStu obsStu;
				obsStu.nodeLabel = label;
				m_planeSegStu.vecScale.push_back(obsStu);
			}
			
			auto ll = m_pFrame2DRoot->NextPointLonLatByBearing(m_planeSegStu.wptLonLat, scaleNum, lineAngle);
			auto earthPositon = CHmsNavMathHelper::LonLatToEarthPoint(ll);

			auto &obsStu = m_planeSegStu.vecScale.at(scaleCount);
			obsStu.bEnable = true;
			obsStu.earthPos = earthPositon;
			CHmsNode * pTempNode = obsStu.nodeLabel;
			if (pTempNode)
			{
				pTempNode->SetPosition(m_pFrame2DRoot->EarthPointToFramePoint(earthPositon));
				if (pFrust)
				{
					pTempNode->SetVisible(!pFrust->isOutOfFrustum(earthPositon));
				}
				else
				{
					pTempNode->SetVisible(false);
				}
			}
			++scaleCount;
		}
	}
}

void CHmsFrame2DObsLayer::ReDrawObsLine()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pObsFullLineNode || !m_pObsDashLineNode)
		return;
#endif
	m_pObsFullLineNode->clear();
	m_pObsDashLineNode->clear();

	if (m_planeSegStu.bIsObsMode)
	{
		std::vector<Vec2> vecLines;
		for (int k = 0; k < (int)m_planeSegStu.obsFull.vec2DPos.size(); ++k)
		{
			if (m_planeSegStu.obsFull.vecIsVisible[k])
			{
				vecLines.push_back(m_planeSegStu.obsFull.vec2DPos[k]);
			}
		}
		m_pObsFullLineNode->DrawLines(vecLines.data(), vecLines.size(), 6, Color4B::BLUE);

		vecLines.clear();
		for (int k = 0; k < (int)m_planeSegStu.obsDash.vec2DPos.size(); ++k)
		{
			if (m_planeSegStu.obsDash.vecIsVisible[k])
			{
				vecLines.push_back(m_planeSegStu.obsDash.vec2DPos[k]);
			}
		}
		m_pObsDashLineNode->DrawDashLines(vecLines.data(), vecLines.size(), 6, Color4B::BLUE);
	}
}

void CHmsFrame2DObsLayer::UpdateObsLine2DPos()
{
	if (!m_planeSegStu.bIsObsMode)
	{
		return;
	}
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
	for (int i = 0; i < (int)m_planeSegStu.obsFull.vecEarthPos.size(); ++i)
	{
		bool bIsVisiable = false;
		if (pFrust)
		{
			bIsVisiable = !pFrust->isOutOfFrustum(m_planeSegStu.obsFull.vecEarthPos[i]);
		}
		m_planeSegStu.obsFull.vec2DPos[i] = m_pFrame2DRoot->EarthPointToFramePoint(m_planeSegStu.obsFull.vecEarthPos[i]);
		m_planeSegStu.obsFull.vecIsVisible[i] = bIsVisiable;
	}
	for (int i = 0; i < (int)m_planeSegStu.obsDash.vecEarthPos.size(); ++i)
	{
		bool bIsVisiable = false;
		if (pFrust)
		{
			bIsVisiable = !pFrust->isOutOfFrustum(m_planeSegStu.obsDash.vecEarthPos[i]);
		}
		m_planeSegStu.obsDash.vec2DPos[i] = m_pFrame2DRoot->EarthPointToFramePoint(m_planeSegStu.obsDash.vecEarthPos[i]);
		m_planeSegStu.obsDash.vecIsVisible[i] = bIsVisiable;
	}
	for (auto &c : m_planeSegStu.vecScale)
	{
		bool bIsVisiable = false;
		if (pFrust)
		{
			bIsVisiable = !pFrust->isOutOfFrustum(c.earthPos);
		}
		if (c.nodeLabel)
		{
			c.nodeLabel->SetPosition(m_pFrame2DRoot->EarthPointToFramePoint(c.earthPos));
			c.nodeLabel->SetVisible(bIsVisiable);
		}
	}
}

void CHmsFrame2DObsLayer::ReInsertPlaneLine3Dpos()
{
#if 0
	InsertDot(m_planeSegStu.planeLonLat, m_planeSegStu.wptLonLat, m_planeSegStu.airplaneLine);
#endif
}

void CHmsFrame2DObsLayer::ReDrawPlaneLine()
{
#if 0
	m_pAircraftWptLineNode->clear();
	std::vector<Vec2> vecLines;
	for (int k = 0; k < m_planeSegStu.airplaneLine.vec2DPos.size(); ++k)
	{
		if (m_planeSegStu.airplaneLine.vecIsVisible[k])
		{
			vecLines.push_back(m_planeSegStu.airplaneLine.vec2DPos[k]);
		}
	}
	m_pAircraftWptLineNode->DrawLines(vecLines.data(), vecLines.size(), 6, Color4B::MAGENTA);
#endif
}

void CHmsFrame2DObsLayer::UpdatePlaneLine2DPos()
{
#if 0
	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();

	for (int i = 0; i < m_planeSegStu.airplaneLine.vecEarthPos.size(); ++i)
	{
		bool bIsVisiable = false;
		if (pFrust)
		{
			bIsVisiable = !pFrust->isOutOfFrustum(m_planeSegStu.airplaneLine.vecEarthPos[i]);
		}
		m_planeSegStu.airplaneLine.vec2DPos[i] = m_pFrame2DRoot->EarthPointToFramePoint(m_planeSegStu.airplaneLine.vecEarthPos[i]);
		m_planeSegStu.airplaneLine.vecIsVisible[i] = bIsVisiable;
	}
#endif
}

void CHmsFrame2DObsLayer::ClearObsData()
{
	for (auto &scale : m_planeSegStu.vecScale)
	{
		scale.bEnable = false;
		scale.nodeLabel->SetVisible(false);
	}
	m_planeSegStu.obsFull.Clear();
	m_planeSegStu.obsDash.Clear();
}

void CHmsFrame2DObsLayer::ClearAllLine()
{
    if (m_planeSegStu.bIsObsMode)
	{
		ClearObsData();
		m_planeSegStu.bIsObsMode = false;
		m_planeSegStu.fObsAngle = 0;

		m_planeSegStu.planeLonLat = Vec2();
		m_planeSegStu.wptLonLat = Vec2();
		m_planeSegStu.airplaneLine.Clear();
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pAircraftWptLineNode || !m_pObsFullLineNode || !m_pObsDashLineNode)
			return;
#endif
		m_pAircraftWptLineNode->clear();
		m_pObsFullLineNode->clear();
		m_pObsDashLineNode->clear();
	}
}
