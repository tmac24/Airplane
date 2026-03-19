#include "HmsFrame2DAirplaneLayer.h"
#include "../HmsFrame2DRoot.h"

CHmsFrame2DAirplaneLayer::CHmsFrame2DAirplaneLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_Airplane)
{

}

CHmsFrame2DAirplaneLayer::~CHmsFrame2DAirplaneLayer()
{
}

CHmsFrame2DAirplaneLayer* CHmsFrame2DAirplaneLayer::Create(Size layerSize)
{
	CHmsFrame2DAirplaneLayer *ret = new (std::nothrow) CHmsFrame2DAirplaneLayer();
	if (ret && ret->Init(layerSize))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsFrame2DAirplaneLayer::Init(Size layerSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	this->SetContentSize(layerSize);

	//m_pImagePlaneTop = CHmsImageNode::Create("res/airport/plane.png");
    m_pImagePlaneTop = CHmsImageNode::Create("res/AftIcon/heliZ9.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pImagePlaneTop)
		return false;
#endif
	m_pImagePlaneTop->SetColor(Color3B::BLUE);
	m_pImagePlaneTop->SetAnchorPoint(Vec2(0.5f, 0.5f));
	m_pImagePlaneTop->SetVisible(false);
	this->AddChild(m_pImagePlaneTop);
	

	return true;
}

void CHmsFrame2DAirplaneLayer::Update(float delta)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot || !m_pImagePlaneTop)
		return;
#endif
	if (m_pFrame2DRoot->IsPlaneLonLatValid())
	{
		auto earthPos = m_pFrame2DRoot->GetPlaneEarthPos();
		m_posPlaneTop = earthPos;

		Vec2 lonLat = m_pFrame2DRoot->NextPointLonLatByBearing(m_pFrame2DRoot->GetPlaneLonLat(), 10, m_pFrame2DRoot->GetPlaneYaw());
		auto nextEarthPos = m_pFrame2DRoot->LonLatToEarthPoint(lonLat);

		auto screenAft = m_pFrame2DRoot->EarthPointToFramePoint(earthPos);
		auto screenDir = m_pFrame2DRoot->EarthPointToFramePoint(nextEarthPos);
		m_pImagePlaneTop->SetPosition(screenAft);
		auto temp = screenDir - screenAft;
		auto angle = temp.getAngle();
		m_pImagePlaneTop->SetRotation(ToAngle(SIM_Pi / 2 - angle));

		if (!m_pImagePlaneTop->IsVisible())
		{
			m_pImagePlaneTop->SetVisible(true);
			Update2DElements();
		}
	}
	else
	{
		m_pImagePlaneTop->SetVisible(false);
	}
}

void CHmsFrame2DAirplaneLayer::Update2DElements()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot || !m_pImagePlaneTop)
		return;
#endif
	if (m_pFrame2DRoot->IsPlaneLonLatValid())
	{
		HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
		bool bVisible = false;
		if (pFrust)
		{
			bVisible = !pFrust->isOutOfFrustum(m_posPlaneTop);
		}
		m_pImagePlaneTop->SetVisible(bVisible);
	}
	else
	{
		m_pImagePlaneTop->SetVisible(false);
	}
}
