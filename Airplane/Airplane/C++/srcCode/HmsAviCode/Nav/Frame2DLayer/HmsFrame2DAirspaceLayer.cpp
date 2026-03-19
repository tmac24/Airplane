#include "HmsFrame2DAirspaceLayer.h"
#include "../HmsFrame2DRoot.h"

CHmsFrame2DAirspaceLayer::CHmsFrame2DAirspaceLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_AirspaceUser)
, m_pRouteLineDrawNode(nullptr)
{
    
}

CHmsFrame2DAirspaceLayer::~CHmsFrame2DAirspaceLayer()
{
}

CHmsFrame2DAirspaceLayer* CHmsFrame2DAirspaceLayer::Create(Size layerSize)
{
	CHmsFrame2DAirspaceLayer *ret = new (std::nothrow) CHmsFrame2DAirspaceLayer();
	if (ret && ret->Init(layerSize))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsFrame2DAirspaceLayer::Init(Size layerSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	this->SetContentSize(layerSize);

	m_pRouteLineDrawNode = CHmsRouteLineDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLineDrawNode)
		return false;
#endif
	m_pRouteLineDrawNode->SetAnchorPoint(0, 0);
	m_pRouteLineDrawNode->SetContentSize(this->GetContentSize());
	m_pRouteLineDrawNode->SetPosition(0, 0);
	m_pRouteLineDrawNode->SetTexturePath("res/NavImage/navLine.png");
	this->AddChild(m_pRouteLineDrawNode);

	return true;
}

void CHmsFrame2DAirspaceLayer::Update(float delta)
{

}

void CHmsFrame2DAirspaceLayer::Update2DElements()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
	for (auto &e : m_vecWptNode)
	{
		for (int k = 0; k < (int)e.vecInsertEarthPos.size(); ++k)
		{
			bool bVisible = false;
			if (pFrust)
			{
				bVisible = !pFrust->isOutOfFrustum(e.vecInsertEarthPos.at(k));
			}
			e.vecInsertLonLatIsVisible[k] = bVisible;
			e.vecInsert2DPos[k] = m_pFrame2DRoot->EarthPointToFramePoint(e.vecInsertEarthPos[k]);
		}
	}
	ReDrawLines();
}

void CHmsFrame2DAirspaceLayer::SetEarthLayer(int n)
{
	if (m_earthLayer != n)
	{
		m_earthLayer = n;

		CalInsertLonLat(-1, m_vecWptNode.size());

		SetNeedUpdate2DEles(true);
	}
}

void CHmsFrame2DAirspaceLayer::UpdateAirspaceData(const std::vector<AirspaceEditStu> &vecStu)
{
	for (auto &c : m_vecWptNode)
	{
		if (c.pNode)
		{
			c.pNode->RemoveFromParent();
		}
	}
	m_vecWptNode.clear();
	for (auto &c : vecStu)
	{
		WptNodeStu2D tmpStu2D;
		tmpStu2D.brief.lon = c.lon;
		tmpStu2D.brief.lat = c.lat;
		tmpStu2D.pNode = CHmsNode::Create();
		tmpStu2D.earthPos = CHmsNavMathHelper::LonLatToEarthPoint(Vec2(c.lon, c.lat));
#if _NAV_SVS_LOGIC_JUDGE
		if (tmpStu2D.pNode)
#endif
		this->AddChild(tmpStu2D.pNode);

		m_vecWptNode.push_back(tmpStu2D);
	}

	CalInsertLonLat(-1, m_vecWptNode.size());

	SetNeedUpdate2DEles(true);
	SetEleChanged(true);
}

void CHmsFrame2DAirspaceLayer::ProcessInsertNormal(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust)
{
	if (pLastStu == nullptr || pCurStu == nullptr)
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
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
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

void CHmsFrame2DAirspaceLayer::ReDrawLines()
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
	//==============画空域
	for (auto &wptNode : m_vecWptNode)
	{
		RouteLineDrawStu stu;
		stu.fLineWidth = 8;
		stu.color = Color4B::YELLOW;

		const std::vector<Vec2> &vec2DPos = wptNode.vecInsert2DPos;
		const std::vector<bool> &vecVisible = wptNode.vecInsertLonLatIsVisible;

		PushPointToStu(stu, vec2DPos, vecVisible);

		vecStu.push_back(stu);
	}
	m_pRouteLineDrawNode->DrawLines(vecStu);
}

void CHmsFrame2DAirspaceLayer::ShowAirspaceLayer()
{
	this->SetVisible(true);

	std::vector<AirspaceEditStu> vec;
	CHmsHistoryRouteDatabase::GetInstance()->GetAirspace(vec);
	UpdateAirspaceData(vec);
}

void CHmsFrame2DAirspaceLayer::HideAirspaceLayer()
{
	this->SetVisible(false);
}

void CHmsFrame2DAirspaceLayer::CalInsertLonLat(int beginIndex, int endIndex)
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
	DetectEdge(0, beginIndex, m_vecWptNode.size() - 1);
	DetectEdge(0, endIndex, m_vecWptNode.size());
	DetectEdge(beginIndex, beginIndex, endIndex);

	m_vecWptNode[0].vecInsertLonLatIsVisible.clear();
	m_vecWptNode[0].vecInsertEarthPos.clear();
	m_vecWptNode[0].vecInsert2DPos.clear();

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();

	WptNodeStu2D *pLastStu = nullptr;
	WptNodeStu2D *pCurStu = nullptr;
	for (int i = beginIndex; i < endIndex; ++i)
	{
		auto &e = m_vecWptNode.at(i);
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

			ProcessInsertNormal(pLastStu, pCurStu, pFrust);

			pLastStu = pCurStu;
			pCurStu = nullptr;
		}
	}
	//空域是一个闭合的图形，每次插值都单独处理最后一个点与第一个点的连接
	ProcessInsertNormal(&m_vecWptNode.back(), &m_vecWptNode.front(), pFrust);
}


