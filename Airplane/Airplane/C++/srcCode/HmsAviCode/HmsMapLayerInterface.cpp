#include "HmsMapLayerInterface.h"
#include "Calculate/CalculateZs.h"
USING_NS_HMS;

double LngToPixelByGoogle(double dLng, int zoom);
double PixelToLngByGoogle(double dPixel, int zoom);
double LatToPixelByGoogle(double dLat, int zoom);
double PixelToLatByGoogle(double dPixedY, int zoom);

CHmsMapLayerInterface::CHmsMapLayerInterface()
	: m_nLayer(3)
	, m_fScale(1)
	, m_fCurHeading(0)
	, m_pMapDescribe(nullptr)
{
}


CHmsMapLayerInterface::~CHmsMapLayerInterface()
{
}

HmsAviPf::Vec2 CHmsMapLayerInterface::GetGooglePosition(double dX, double dY)
{
	return Vec2(LngToPixelByGoogle(dX, m_nLayer), -LatToPixelByGoogle(dY, m_nLayer));
}

HmsAviPf::Vec2 CHmsMapLayerInterface::GetGooglePosition(const CSimEarthCoord & posEc)
{
	return GetGooglePosition(posEc.dLongitude, posEc.dLatitude);
}

HmsAviPf::Vec2 CHmsMapLayerInterface::GetWGSPositionByPix(double dPixX, double dPixY)
{
	return Vec2(PixelToLngByGoogle(dPixX, m_nLayer), PixelToLatByGoogle(fabs(dPixY), m_nLayer));
}

void CHmsMapLayerInterface::UpdateLayer()
{

}

void CHmsMapLayerInterface::SetLayerAndScaled(int nLayer, float fLayerScale)
{
	SetLayer(nLayer);
	SetScale(fLayerScale);
	UpdateLayer();
}

void CHmsMapLayerInterface::SetCurrentAftData(const HmsAviPf::Vec2 & pos, float fHeading)
{
	HmsAviPf::Vec2 posTemp = GetGooglePosition(pos.x, pos.y);
	
	auto posLayer = HmsAviPf::Vec2(-posTemp.x, -posTemp.y);
	m_root->SetPosition(posLayer*m_fScale);

	this->SetRotation(-fHeading);
	m_fCurHeading = fHeading;
	m_posCenter = pos;
	OnLayerPosition(posLayer, m_fScale);
}

void CHmsMapLayerInterface::AddMapLayerChild(HmsAviPf::CHmsNode * pNode)
{
	if (m_root)
	{
		m_root->AddChild(pNode);
	}
}

bool CHmsMapLayerInterface::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_root = CHmsNode::Create();
	this->AddChild(m_root);

	return true;
}

#include "HmsMapLoadManager.h"
CSimEarthCoord CHmsMapLayerInterface::GetLngLatByOffset(const HmsAviPf::Vec2 & posCenter, const HmsAviPf::Vec2 & deltaScreen)
{
	//还原地图旋转前的位置
	HmsAviPf::Vec2 deltaMap = deltaScreen.rotateByAngle(Vec2(), ToRadian(-m_fCurHeading));
	//获取缩放之后的偏移
	deltaMap *= 1 / m_fScale;

	//地图中心位置的像素点
	HmsAviPf::Vec2 posPixCenter = GetGooglePixelPosByLngLat(posCenter, m_nLayer);
	HmsAviPf::Vec2 posPix = Vec2(posPixCenter.x + deltaMap.x, posPixCenter.y - deltaMap.y);

	auto ec = GetLngLatByGooglePixel(posPix.x, posPix.y, m_nLayer);

	return ec;
}

CSimEarthCoord CHmsMapLayerInterface::GetLngLatByOffsetNotHeading(const HmsAviPf::Vec2 & posCenter, const HmsAviPf::Vec2 & deltaScreen)
{
	HmsAviPf::Vec2 deltaMap = deltaScreen;
	//获取缩放之后的偏移
	deltaMap *= 1 / m_fScale;

	//地图中心位置的像素点
	HmsAviPf::Vec2 posPixCenter = GetGooglePixelPosByLngLat(posCenter, m_nLayer);
	HmsAviPf::Vec2 posPix = Vec2(posPixCenter.x + deltaMap.x, posPixCenter.y - deltaMap.y);

	auto ec = GetLngLatByGooglePixel(posPix.x, posPix.y, m_nLayer);

	return ec;
}

CSimEarthCoord CHmsMapLayerInterface::GetEarthCoordByTouch(const HmsAviPf::Touch * touch)
{
	auto centerPos = GetGooglePosition(m_posCenter);
	centerPos *= m_fScale;
	auto tempNode = CHmsNode::Create();
	tempNode->SetPosition(centerPos);
	m_root->AddChild(tempNode);

	Vec2 point = touch->GetLocation();
	auto space = tempNode->ConvertToNodeSpaceAR(point);
	auto ec = GetLngLatByOffsetNotHeading(m_posCenter, space);
	m_root->RemoveChild(tempNode);

	return ec;
}

HmsAviPf::Vec2 CHmsMapLayerInterface::GetCurDrawPosition(const CSimEarthCoord & ec)
{
	auto pos = GetGooglePosition(ec);
	pos *= m_fScale;
	return pos;
}
