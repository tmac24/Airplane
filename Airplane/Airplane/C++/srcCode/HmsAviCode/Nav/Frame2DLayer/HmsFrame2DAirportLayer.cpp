#include "HmsFrame2DAirportLayer.h"
#include "../HmsFrame2DRoot.h"
#include "HmsFrame2DRouteLayer.h"

CHmsFrame2DAirportLayer::CHmsFrame2DAirportLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_Airport)
{
    m_updateTime = 0;
}

CHmsFrame2DAirportLayer::~CHmsFrame2DAirportLayer()
{
}

CHmsFrame2DAirportLayer* CHmsFrame2DAirportLayer::Create(Size layerSize)
{
    CHmsFrame2DAirportLayer *ret = new (std::nothrow) CHmsFrame2DAirportLayer();
    if (ret && ret->Init(layerSize))
    {
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

bool CHmsFrame2DAirportLayer::Init(Size layerSize)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    this->SetContentSize(layerSize);

    return true;
}

void CHmsFrame2DAirportLayer::Update(float delta)
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
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pFrame2DRoot)
			return;
#endif
        if (m_pFrame2DRoot->Frame2DPointToEarthPoint(this->GetContentSize() * 0.5, pos))
        {
            Vec2 lonLat = CHmsFrame2DInterface::EarthPointToLonLat(pos);

            auto lonOffset = fabsf(lonLat.x - m_lastLonLat.x);
            auto latOffset = fabsf(lonLat.y - m_lastLonLat.y);
            if (lonOffset > 5 || latOffset > 5)
            {
                for (auto &c : m_vecWptNode)
                {
                    if (c.pNode)
                    {
                        c.pNode->RemoveFromParent();
                    }
                }
                m_vecWptNode.clear();

                std::vector<T_Airport> vecData;
                float range = 5;
                CHms424Database::GetInstance()->GetAirportData(vecData, lonLat.y - range, lonLat.y + range, lonLat.x - range, lonLat.x + range);

                for (auto &stu : vecData)
                {         
                    WptNodeStu2D wptStu;
                    wptStu.brief.ident = stu.AirportIdent;
                    wptStu.brief.lon = stu.Longitude;
                    wptStu.brief.lat = stu.Latitude;
                    wptStu.brief.wType = WptPointType::WPT_AIRPORT;
                    wptStu.pNode = CHmsFrame2DRouteLayer::CreateRouteNode(wptStu.brief.wType, wptStu.brief.ident, 20, FIX_TYPE_FIX);
                    wptStu.earthPos = CHmsNavMathHelper::LonLatToEarthPoint(Vec2(wptStu.brief.lon, wptStu.brief.lat));
#if _NAV_SVS_LOGIC_JUDGE
					if (wptStu.pNode)
#endif
					this->AddChild(wptStu.pNode);

                    m_vecWptNode.push_back(wptStu);
                }
                Update2DElements();

                m_lastLonLat = lonLat;
            }
        }
    }
}

void CHmsFrame2DAirportLayer::Update2DElements()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
    HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
    for (auto &c : m_vecWptNode)
    {
        WptNodeStu2D &stu = c;
        if (stu.pNode)
        {
            if (m_earthLayer <= 7)
            {
                stu.pNode->SetVisible(false);
            }
            else
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
        }
    }
}

void CHmsFrame2DAirportLayer::SetEarthLayer(int n)
{
    if (m_earthLayer != n)
    {
        m_earthLayer = n;

        SetNeedUpdate2DEles(true);
    }
}