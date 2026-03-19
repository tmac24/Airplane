#include "HmsMapNode.h"
#include "Calculate/PositionZs.h"
USING_NS_HMS;

float CHmsMapNode::s_nMaxMapLayer = 10;

CHmsMapNode::CHmsMapNode()
	: m_fRange(10)
	, m_mapHeading(0)
	, m_sizeMap(1000, 1000)
	, m_fRangePix(300)
	, m_bClipMap(false)
	, m_eCenterMode(HmsMapCenterMode::CurAftCenter)
	, m_mapLngPerTile(0)
	, m_nLayer(1)
	, m_fLayerScale(1)
{
}


CHmsMapNode::~CHmsMapNode()
{
}

bool CHmsMapNode::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	return true;
}

void CHmsMapNode::SetMapMode(HmsSitMapMode mapMode)
{
	//const int c_TerrainID = 346721;

	if (mapMode != HmsSitMapMode::none)
	{
		if (nullptr == m_layerMap)
		{
			m_layerMap = CHmsMapLayer::Create();
			//m_mapLayer->SetTag(c_TerrainID);
			m_layerMap->SetLocalZOrder(-10);

			m_layerMap->GetMapLoadManager()->SetScreenPix((int)m_sizeMap.width, (int)m_sizeMap.height);
		}

		if (m_layerMap)
		{
			m_layerMap->GetMapLoadManager()->SetMapFolder(mapMode);
			//m_layerMap->SetCurrentRange(m_fRange);
			OnMapRangeChange(m_fRange);

			//确定是否已经添加地图节点
			if (nullptr == m_layerMap->GetParent())
			{
				AddMapLayer(m_layerMap.get());
			}
		}
	}
	else
	{
		if (m_layerMap)
		{
			RemoveMapLayer(m_layerMap.get());//清除显示节点		
		}
	}

	m_eSitMapMode = mapMode;
}

void CHmsMapNode::SetMapData(const HmsAviPf::Vec2 & posCenter, float fHeading)
{
	if (HmsMapCenterMode::CurAftCenter == m_eCenterMode)
	{
		m_mapCenter = posCenter;
		m_mapHeading = fHeading;

		if (m_layerMap && m_layerMap->GetParent())
		{
			m_layerMap->SetCurrentAftData(posCenter, fHeading);
		}

		for (auto & c:m_mapLayer)
		{
			c.second->SetCurrentAftData(posCenter, fHeading);
		}
	}
	else
	{
		m_mapHeading = fHeading;
		if (m_layerMap && m_layerMap->GetParent())
		{
			m_layerMap->SetCurrentAftData(fHeading);
		}

		for (auto & c : m_mapLayer)
		{
			c.second->SetCurrentAftData(m_mapCenter, fHeading);
		}
	}
}

void CHmsMapNode::SetMapSize(const HmsAviPf::Size & size)
{
	m_sizeMap = size;
	if (m_layerMap)
	{
		m_layerMap->GetMapLoadManager()->SetScreenPix((int)size.width, (int)size.height);
	}
}

void CHmsMapNode::InitEventDispatcher()
{

// 	auto mouseListener = EventListenerMouse::create();
// 	mouseListener->onMouseScroll = [=](EventMouse* event)
// 	{
// 		auto fScrolY = event->getScrollY();
// 		auto fScale = pow(1.1, fScrolY);
// 
// 		OnMapScale(fScale);
// 	};
// 
// 	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

// 	auto listener = EventListenerTouchAllAtOnce::create();
// 
// 	listener->onTouchesMoved = [=](const std::vector<HmsAviPf::Touch*>& touches, HmsAviPf::CHmsEvent* event)
// 	{
// 		if (touches.size() == 1)
// 		{
// 			auto it = touches.begin();
// 			auto delta = (*it)->getDelta();
// 			OnMapMove(delta);
// 		}
// 		else
// 		{
// 			auto fScaleMax = 1.0f;
// 			auto fScaleMin = 1.0f;
// 
// 			auto distanceFun = [](const HmsAviPf::Vec2 & pos1, const HmsAviPf::Vec2 & pos2)
// 			{
// 				return sqrt((pos2.x - pos1.x)*(pos2.x - pos1.x) + (pos2.y - pos1.y)*(pos2.y - pos1.y));
// 			};
// 
// 			for (auto & c1 : touches)
// 			{
// 				for (auto & c2 : touches)
// 				{
// 					if (c1 != c2)
// 					{
// 						auto lastDist = distanceFun(c1->getPreviousLocation(), c2->getPreviousLocation());
// 						auto newDist = distanceFun(c1->getLocation(), c2->getLocation());
// 
// 						auto fScale = newDist / lastDist;
// 						fScaleMax = HMS_MAX(fScaleMax, fScale);
// 						fScaleMin = HMS_MIN(fScaleMin, fScale);
// 					}
// 				}
// 			}
// 
// 			if (fScaleMax * fScaleMin >= 1)
// 			{
// 				OnMapScale(fScaleMax);
// 			}
// 			else
// 			{
// 				OnMapScale(fScaleMin);
// 			}
// 		}
// 	};
// 
// 	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CHmsMapNode::OnMapScale(float fScale)
{
	float fTempRange = fScale * m_fRange;

	if (m_funcMapRangeCheck)
	{
		if (m_funcMapRangeCheck(fTempRange, m_fRange))
		{
			m_fRange = fTempRange;
			OnMapRangeChange(m_fRange);
		}
	}
	else
	{
		m_fRange = fTempRange;
		OnMapRangeChange(m_fRange);
	}
}

void CHmsMapNode::OnMapMove(const HmsAviPf::Vec2 & delta)
{
	if (HmsMapCenterMode::MoveAbleLayer == m_eCenterMode)
	{
		//m_mapHeading = 0;

		m_mapCenter = CHmsMapLayer::GetLngLatByOffset(m_mapCenter, m_mapHeading, m_nLayer, m_fLayerScale, delta);

		if (m_layerMap && m_layerMap->GetParent())
		{
			
			m_layerMap->SetCurrentAftData(m_mapCenter, m_mapHeading);
		}

		for (auto & c : m_mapLayer)
		{
			c.second->SetCurrentAftData(m_mapCenter, m_mapHeading);
		}
	}
}

void CHmsMapNode::SetMapRangeCheck(const LPFuncHmsMapRangeCheck & funCheck)
{
	m_funcMapRangeCheck = funCheck;
}

void CHmsMapNode::OnMapRangeChange(float fRange)
{
	auto  long1Deg = CHmsGlobal::GetKmPerLongitude(m_mapCenter.y);
	auto nTileCnt = 1 << 0;
	auto dLngPerTile = 360.0 / nTileCnt;

	auto lngPerPix = dLngPerTile / 256;		//每一个像素的经纬
	auto distanceKmPerPix = lngPerPix * long1Deg;

	//auto layerDesignDistance = fmax(m_sizeMap.width / 2.0, m_sizeMap.height / 2.0) * distanceKmPerPix; ;
	auto layerDesignDistance = m_fRangePix * distanceKmPerPix;
	auto fScale = layerDesignDistance / fRange;

	int nScale = fScale;

	int nLayer = 0;
	while (nScale >>= 1)
	{
		nLayer++;
	}

	if (nLayer > s_nMaxMapLayer)
	{
		nLayer = s_nMaxMapLayer;
	}

	if (m_layerMap)
	{
		float fLayerScale = fScale / pow(2, nLayer);
		m_layerMap->SetLayerAndScaled(nLayer, fLayerScale);
		
		m_mapLngPerTile = lngPerPix * fLayerScale / (1<<nLayer);

		for (auto & c:m_mapLayer)
		{
			c.second->SetLayerAndScaled(nLayer, fLayerScale);
		}

		m_nLayer = nLayer;
		m_fLayerScale = fLayerScale;
	}
	else
	{
		float fLayerScale = fScale / pow(2, nLayer);
		m_mapLngPerTile = lngPerPix * fLayerScale / (1 << nLayer);
		for (auto & c : m_mapLayer)
		{
			c.second->SetLayerAndScaled(nLayer, fLayerScale);
		}

		m_nLayer = nLayer;
		m_fLayerScale = fLayerScale;
	}

	UpdateDataForMoveableLayer();
}

void CHmsMapNode::SetRange(float fRange, bool bUpdate)
{
	m_fRange = fRange;
	if (bUpdate)
	{
		OnMapRangeChange(fRange);

		if (m_eCenterMode == HmsMapCenterMode::MoveAbleLayer)
		{
			OnMapMove(Vec2());
		}
	}
}

void CHmsMapNode::SetRangePix(float fRangePix)
{
	m_fRangePix = fRangePix;
}

void CHmsMapNode::AddMapLayer(HmsAviPf::CHmsNode * node)
{
	HmsAviPf::CHmsNode * pParent = this;

	if (m_bClipMap)
	{
		if (nullptr == m_nodeClip)
		{
			m_nodeClip = CHmsClippingNode::create();
			auto stencil = CHmsDrawNode::Create();
			stencil->DrawSolidRect(m_rectClip.getMinX(), m_rectClip.getMinY(), m_rectClip.getMaxX(), m_rectClip.getMaxY(), Color4F::WHITE); 
			m_nodeClip->setStencil(stencil);

			this->AddChild(m_nodeClip);
		}

		pParent = m_nodeClip;
	}

	if (pParent)
	{
		pParent->AddChild(node);
	}
}

void CHmsMapNode::AddMapLayer(const std::string & strLayerName, CHmsMapLayerInterface * pInterface, int nLayerID /*= 1*/)
{
	auto find = m_mapLayer.find(strLayerName);
	if (find != m_mapLayer.end())
	{
		
	}
	else
	{
		m_mapLayer[strLayerName] = pInterface;
		pInterface->SetLocalZOrder(nLayerID);
		AddMapLayer(pInterface);
	}
}

void CHmsMapNode::RemoveMapLayer(HmsAviPf::CHmsNode * node)
{
	if (node)
	{
		node->RemoveFromParent();
	}
}

void CHmsMapNode::SetClipRect(const HmsAviPf::Rect & rect)
{
	m_rectClip = rect;

	if (m_bClipMap)
	{
		if (m_nodeClip)
		{
			auto stencil = dynamic_cast<HmsAviPf::CHmsDrawNode*>(m_nodeClip->getStencil());
			if (stencil)
			{
				stencil->clear();
				//stencil->drawSolidRect(Vec2(m_rectClip.getMinX(), m_rectClip.getMinY()), Vec2(m_rectClip.getMaxX(), m_rectClip.getMaxY()), Color4F::WHITE);
				stencil->DrawSolidRect(m_rectClip.getMinX(), m_rectClip.getMinY(), m_rectClip.getMaxX(), m_rectClip.getMaxY(), Color4F::WHITE);
			}
		}
	}
}

void CHmsMapNode::SetMapMove(const HmsAviPf::Vec2 & delta)
{
	OnMapMove(delta);
}
#include "HmsAftObjectLayer.h"
void CHmsMapNode::SetCenterMode(HmsMapCenterMode eMode)
{
	m_eCenterMode = eMode;

	auto itFind = m_mapLayer.find("AftObj");
	if (itFind != m_mapLayer.end())
	{
		auto aftLayer = dynamic_cast<CHmsAftObjectLayer*>(itFind->second);
		if (aftLayer)
		{
			aftLayer->SetCurrentAftData(m_mapCenter, m_mapHeading);
			aftLayer->SetCurAftCenter(m_eCenterMode == HmsMapCenterMode::CurAftCenter);
		}
	}

	if (eMode == HmsMapCenterMode::MoveAbleLayer)
	{
		SetMapMove(Vec2(0, 0));
	}
}

void CHmsMapNode::UpdateDataForMoveableLayer()
{
	if (HmsMapCenterMode::MoveAbleLayer == m_eCenterMode)
	{
		if (m_layerMap && m_layerMap->GetParent())
		{
			m_layerMap->SetCurrentAftData(m_mapCenter, m_mapHeading);
		}
	}
}

void CHmsMapNode::UpdateMap()
{
	OnMapRangeChange(m_fRange);
}

void CHmsMapNode::SetLayerVisible(const std::string & strName, bool bVisible)
{
	auto itFind = m_mapLayer.find(strName);
	if (itFind != m_mapLayer.end())
	{
		itFind->second->SetVisible(bVisible);
	}
}

void CHmsMapNode::SetNorthMode(bool bNorth)
{
	auto itFind = m_mapLayer.find("AftObj");
	if (itFind != m_mapLayer.end())
	{
		auto aftLayer = dynamic_cast<CHmsAftObjectLayer*>(itFind->second);
		if (aftLayer)
		{
			aftLayer->SetNorthMode(bNorth);
		}
	}
}
