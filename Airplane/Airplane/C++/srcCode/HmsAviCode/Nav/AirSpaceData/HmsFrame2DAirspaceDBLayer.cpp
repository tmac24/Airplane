#include "HmsFrame2DAirspaceDBLayer.h"
#include "../HmsFrame2DRoot.h"

CHmsFrame2DAirspaceDBLayer::CHmsFrame2DAirspaceDBLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_AirspaceDB)
{
    m_updateTime = 0;

    CHmsAirspaceData::GetInstance();
}

CHmsFrame2DAirspaceDBLayer::~CHmsFrame2DAirspaceDBLayer()
{
}

CHmsFrame2DAirspaceDBLayer* CHmsFrame2DAirspaceDBLayer::Create(Size layerSize)
{
    CHmsFrame2DAirspaceDBLayer *ret = new (std::nothrow) CHmsFrame2DAirspaceDBLayer();
    if (ret && ret->Init(layerSize))
    {
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

bool CHmsFrame2DAirspaceDBLayer::Init(Size layerSize)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    this->SetContentSize(layerSize);

    m_pRouteLineDrawNode = CHmsMultLineDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLineDrawNode)
		return false;
#endif
    m_pRouteLineDrawNode->SetAnchorPoint(0, 0);
    m_pRouteLineDrawNode->SetContentSize(this->GetContentSize());
    m_pRouteLineDrawNode->SetPosition(0, 0);
    m_pRouteLineDrawNode->SetDrawColor(Color4F::BLUE);
    this->AddChild(m_pRouteLineDrawNode);

    return true;
}

void CHmsFrame2DAirspaceDBLayer::Update(float delta)
{
    if (m_earthLayer <= 7)
    {
        return;
    }
    m_updateTime += delta;
    if (m_updateTime > 1)
    {
        m_updateTime = 0;

        Vec3 pos;
        if (m_pFrame2DRoot->Frame2DPointToEarthPoint(this->GetContentSize() * 0.5, pos))
        {
            Vec2 lonLat = CHmsFrame2DInterface::EarthPointToLonLat(pos);

            auto lonOffset = fabsf(lonLat.x - m_lastLonLat.x);
            auto latOffset = fabsf(lonLat.y - m_lastLonLat.y);
            if (lonOffset > 5 || latOffset > 5)
            {
                m_vecAirspaceGroup.clear();
                CHmsAirspaceData::GetInstance()->GetAirspaceData(lonLat.x, lonLat.y, 5, m_vecAirspaceGroup);
                Update2DElements();
                ReDrawLines();

                m_lastLonLat = lonLat;
            }   
        }
    }
}

void CHmsFrame2DAirspaceDBLayer::Update2DElements()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLineDrawNode || !m_pFrame2DRoot)
		return;
#endif
    if (m_earthLayer <= 7)
    {
        m_pRouteLineDrawNode->clear();
        return;
    }
    HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
    for (auto &group : m_vecAirspaceGroup)
    {
        for (auto &stu : group.data)
        {
			for (int k = 0; k < (int)stu.vecInsertEarthPos.size(); ++k)
            {
                bool bVisible = false;
                if (pFrust)
                {
                    bVisible = !pFrust->isOutOfFrustum(stu.vecInsertEarthPos.at(k));
                }
                stu.vecInsertLonLatIsVisible[k] = bVisible;
                stu.vecInsert2DPos[k] = m_pFrame2DRoot->EarthPointToFramePoint(stu.vecInsertEarthPos[k]);
            }
        }
    }

    ReDrawLines();
}

void CHmsFrame2DAirspaceDBLayer::SetEarthLayer(int n)
{
    if (m_earthLayer != n)
    {
        m_earthLayer = n;

        SetNeedUpdate2DEles(true);
    }
}

void CHmsFrame2DAirspaceDBLayer::ReDrawLines()
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
    for (auto &group : m_vecAirspaceGroup)
    {
        for (auto &airspaceStu : group.data)
        {
            RouteLineDrawStu stu;
            stu.fLineWidth = 8;
            stu.color = Color4B::YELLOW;

            const std::vector<Vec2> &vec2DPos = airspaceStu.vecInsert2DPos;
            const std::vector<bool> &vecVisible = airspaceStu.vecInsertLonLatIsVisible;

            PushPointToStu(stu, vec2DPos, vecVisible);

            vecStu.push_back(stu);

            for (auto &c : stu.vecArrayVertexs)
            {
                m_pRouteLineDrawNode->DrawLineStrip(c, 4, Color4B::BLACK);
            }
        }
    }
}
