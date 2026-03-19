#include "HmsFrame2DAirspaceEditLayer.h"
#include "../HmsFrame2DRoot.h"

CHmsFrame2DAirspaceEditLayer::CHmsFrame2DAirspaceEditLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_AirspaceEdit)
, m_pCurDragNodeStu2D(nullptr)
, m_curPressIndex(-1)
, m_pRouteLineDrawNode(nullptr)
{
    
}

CHmsFrame2DAirspaceEditLayer::~CHmsFrame2DAirspaceEditLayer()
{
}

CHmsFrame2DAirspaceEditLayer* CHmsFrame2DAirspaceEditLayer::Create(Size layerSize)
{
	CHmsFrame2DAirspaceEditLayer *ret = new (std::nothrow) CHmsFrame2DAirspaceEditLayer();
	if (ret && ret->Init(layerSize))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsFrame2DAirspaceEditLayer::Init(Size layerSize)
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

void CHmsFrame2DAirspaceEditLayer::Update(float delta)
{

}

void CHmsFrame2DAirspaceEditLayer::Update2DElements()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
	for (auto &e : m_vecWptNode)
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
	ReDrawLines();
}

void CHmsFrame2DAirspaceEditLayer::SetEarthLayer(int n)
{
	if (m_earthLayer != n)
	{
		m_earthLayer = n;

		CalInsertLonLat(-1, m_vecWptNode.size());

		SetNeedUpdate2DEles(true);
	}
}

void CHmsFrame2DAirspaceEditLayer::ProcessInsertNormal(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust)
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

void CHmsFrame2DAirspaceEditLayer::ReDrawLines()
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
		stu.color = Color4B::GRAY;

		const std::vector<Vec2> &vec2DPos = wptNode.vecInsert2DPos;
		const std::vector<bool> &vecVisible = wptNode.vecInsertLonLatIsVisible;

		PushPointToStu(stu, vec2DPos, vecVisible);

		vecStu.push_back(stu);
	}
	m_pRouteLineDrawNode->DrawLines(vecStu);
}

void CHmsFrame2DAirspaceEditLayer::ShowEditLayer(const std::vector<AirspaceEditStu> &vecStu)
{
	this->SetVisible(true);
	m_vecStu = vecStu;

	UpdateWptNode();
}

void CHmsFrame2DAirspaceEditLayer::HideEditLayer()
{
	this->SetVisible(false);
}

void CHmsFrame2DAirspaceEditLayer::UpdateWptNode()
{
	for (auto &c : m_vecWptNode)
	{
		if (c.pNode)
		{
			c.pNode->RemoveFromParent();
		}
	}
	m_vecWptNode.clear();
	for (auto &c : m_vecStu)
	{
		WptNodeStu2D tmpStu2D;
		tmpStu2D.brief.ident = c.name;
		tmpStu2D.brief.lon = c.lon;
		tmpStu2D.brief.lat = c.lat;
		tmpStu2D.pNode = CreateRouteNode(c.name, 20);
		tmpStu2D.earthPos = CHmsNavMathHelper::LonLatToEarthPoint(Vec2(c.lon, c.lat));

		m_vecWptNode.push_back(tmpStu2D);
	}

	CalInsertLonLat(-1, m_vecWptNode.size());

	SetNeedUpdate2DEles(true);
}

CHmsNode * CHmsFrame2DAirspaceEditLayer::CreateRouteNode(std::string &strText, float textSize)
{
    auto pParent = CHmsNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!pParent)
		return nullptr;
#endif
    pParent->SetAnchorPoint(Vec2(0.5, 0.5));
    pParent->SetContentSize(Size(150, 50));
    this->AddChild(pParent);

    {
        std::string str = CHmsWptInfoDlg::GetImagePathByWptType(WptPointType::WPT_USERWPT);
        auto pIcon = CHmsImageNode::Create(str.c_str());
#if _NAV_SVS_LOGIC_JUDGE
		if (!pIcon)
			return nullptr;
#endif
        pIcon->SetAnchorPoint(Vec2(0.5, 0.5));
        pIcon->SetPosition(pParent->GetContentSize() * 0.5);
        pParent->AddChild(pIcon);
    }
    auto pLabel = CHmsGlobal::CreateLabel(strText.c_str(), textSize);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pLabel)
		return nullptr;
#endif
    pLabel->SetAnchorPoint(0, 0.5);
    pLabel->SetPosition(Vec2(pParent->GetContentSize().width * 0.5 + 30, pParent->GetContentSize().height * 0.5));
    pLabel->SetTextColor(Color4B::BLACK);

    auto pBgImage = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pBgImage)
		return nullptr;
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

void CHmsFrame2DAirspaceEditLayer::CalInsertLonLat(int beginIndex, int endIndex)
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

bool CHmsFrame2DAirspaceEditLayer::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
	m_pCurDragNodeStu2D = nullptr;
	m_curPressIndex = -1;
	m_posPress = posOrigin;
	if (TestPressOnOldNode(posOrigin))
	{
		return true;
	}

	return false;
}

void CHmsFrame2DAirspaceEditLayer::OnMove(const HmsAviPf::Vec2 & posOrigin)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
	if (m_pCurDragNodeStu2D)
	{
		auto delta = posOrigin - m_posPress;
		if (delta.length() > 5)
		{
			auto newPos = m_posImageWhenPress + delta;
			Vec3 earthPos;
			if (m_pFrame2DRoot->Frame2DPointToEarthPoint(newPos, earthPos))
			{
				m_pCurDragNodeStu2D->pNode->SetPosition(newPos);
				m_pCurDragNodeStu2D->earthPos = earthPos;
				Vec2 lonLat = m_pFrame2DRoot->EarthPointToLonLat(earthPos);
				m_pCurDragNodeStu2D->brief.lon = lonLat.x;
				m_pCurDragNodeStu2D->brief.lat = lonLat.y;
		
				CalInsertLonLat(m_curPressIndex - 1, m_curPressIndex + 2);
				ReDrawLines();
			}
		}
	}
}

void CHmsFrame2DAirspaceEditLayer::OnReleased(const HmsAviPf::Vec2 & posOrigin)
{
	if (m_pCurDragNodeStu2D)
	{
		std::vector<AirspaceEditStu> vecStu;
		for (auto &c : m_vecWptNode)
		{
			AirspaceEditStu stu;
			stu.name = c.brief.ident;
			stu.lon = c.brief.lon;
			stu.lat = c.brief.lat;
			vecStu.push_back(stu);
		}
		if (m_pFrame2DRoot)
		{
			m_pFrame2DRoot->UpdateAirspaceEditData(vecStu);
		}
	}
}

bool CHmsFrame2DAirspaceEditLayer::TestPressOnOldNode(const HmsAviPf::Vec2 & posOrigin)
{
    for (int i = 0; i < (int)m_vecWptNode.size(); ++i)
	{
		auto &e = m_vecWptNode[i];
#if _NAV_SVS_LOGIC_JUDGE
		if (!e.pNode)
			continue;
#endif
		if (!e.pNode->IsVisible())
		{
			continue;
		}
		auto rect = e.pNode->GetRectFromParent();
		if (rect.containsPoint(posOrigin))
		{
			m_pCurDragNodeStu2D = &e;
			m_curPressIndex = i;
#if _NAV_SVS_LOGIC_JUDGE
			if (m_pCurDragNodeStu2D->pNode)
#endif
			m_posImageWhenPress = m_pCurDragNodeStu2D->pNode->GetPoistion();
			
			return true;
		}
	}
	return false;
}


