#include <HmsAviPf.h>
#include "HmsGlobal.h"
#include "HmsMapLayer.h"
#include "Calculate/CalculateZs.h"
USING_NS_HMS;

#ifndef HMS_MAX
#define HMS_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define HMS_MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif // !HMS_MAX



CHmsMapLayer::CHmsMapLayer()
	: m_nLayer(0)
	, m_nLayerDown(0)
	, m_nTileCnt(1)
	, m_dLngPerTile(1.0)
	, m_dLatPerTile(1.0)
	, m_pNodeRoot(nullptr)
	, m_scaleOrgShowTile(1.0f, 1.0f)
	, m_posCenter(104.06f, 30.67f)
	, m_fScaleLayer(1.0f)
	, m_fScaleLayerDown(1.0f)
	, m_pLoadManager(nullptr)
{
	
}



CHmsMapLayer::~CHmsMapLayer()
{
	HMS_SAFE_RELEASE_NULL(m_pNodeRoot);
}

bool CHmsMapLayer::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_pNodeRoot = CHmsNode::Create();
	m_pNodeRoot->retain();
	this->AddChild(m_pNodeRoot);

	//m_pLoadManager = CHmsGlobal::GetInstance()->GetMapLoadManager();
	if (nullptr == m_pLoadManager)
	{
		m_pLoadManager = new CHmsMapLoadManager();
	}

	//InitEventDispatcher();

	//初始化加载器
	m_pLoadManager->Init(
		HMS_CALLBACK_1(CHmsMapLayer::OnLoadMapTile, this),
		HMS_CALLBACK_1(CHmsMapLayer::OnRemoveMapTile, this),
		HMS_CALLBACK_1(CHmsMapLayer::OnChangeLayer, this),
		HMS_CALLBACK_0(CHmsMapLayer::OnChangeMapSource, this)
		,12
		);
	//设置路径


	std::vector<int> vIntLayer;
	if (m_pLoadManager->GetLayerMap(vIntLayer))
	{
		for (auto c :vIntLayer)
		{
			GetOrCreateMapTileNode(c);
		}
	}

	//设置地图中心点
	SetMapCenter(m_posCenter);

	//设置更新
	//this->SetScheduleUpdate();
	return true;
}

void CHmsMapLayer::InitEventDispatcher()
{
#if 0
	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseScroll = [=](EventMouse* event)
	{
		auto fScrolY = event->getScrollY();
		auto fScale = m_fScaleLayer;//this->getScale();
		fScale = fScale * pow(1.1 , fScrolY);
		
		OnScaleLayer(fScale);
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

	auto listener = EventListenerTouchAllAtOnce::create();

	listener->onTouchesMoved = [=](const std::vector<HmsAviPf::Touch*>& touches, HmsAviPf::CHmsEvent* event)
	{
		if (touches.size() == 1)
		{
			auto it = touches.begin();
			auto delta = (*it)->getDelta();
			m_posCenter += -(m_dLatPerTile / 256) * delta;
		}
		else
		{
			auto fScaleMax = 1.0f;
			auto fScaleMin = 1.0f;

			auto distanceFun = [](const HmsAviPf::Vec2 & pos1, const HmsAviPf::Vec2 & pos2)
			{
				return sqrt((pos2.x - pos1.x)*(pos2.x - pos1.x) + (pos2.y - pos1.y)*(pos2.y - pos1.y));
			};

			for (auto & c1:touches)
			{
				for (auto & c2:touches)
				{
					if (c1 != c2)
					{
						auto lastDist = distanceFun(c1->getPreviousLocation(), c2->getPreviousLocation());
						auto newDist = distanceFun(c1->getLocation(), c2->getLocation());

						auto fScale = newDist / lastDist;
						fScaleMax = HMS_MAX(fScaleMax, fScale);
						fScaleMin = HMS_MIN(fScaleMin, fScale);
					}
				}
			}

			if (fScaleMax * fScaleMin >= 1)
			{
				OnScaleLayer(m_fScaleLayer*fScaleMax);
			}
			else
			{
				OnScaleLayer(m_fScaleLayer*fScaleMin);
			}
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif
}

void CHmsMapLayer::update(float delta)
{
	SetMapCenter(m_posCenter);
	m_pLoadManager->Update();
}

void CHmsMapLayer::OnChangeLayer(int nLayer)
{
	if (m_nLayer == nLayer)
	{
		return;
	}
	
	if (0 == m_nLayer)
	{
		//初始化
		m_nLayer = nLayer;
		m_nTileCnt = 1 << nLayer;
		m_dLngPerTile = 360.0 / m_nTileCnt;
		m_dLatPerTile = 180.0 / m_nTileCnt;
		m_fScaleLayerDown = m_fScaleLayer;
	}
	else
	{
		if (m_nLayerDown)
		{
			//如果之前存在地图
			GetOrCreateMapTileNode(m_nLayerDown)->SetVisible(false);
			//清空图层缓存数据

		}

		//将当前图层转换为背景图层,不更新，为了防止加载是闪屏,或者没内容
		m_nLayerDown = m_nLayer;
		m_nLayer = nLayer;
		m_nTileCnt = 1 << nLayer;
		m_dLngPerTile = 360.0 / m_nTileCnt;
		m_dLatPerTile = 180.0 / m_nTileCnt;
		m_fScaleLayerDown = m_fScaleLayer;

		int nScale = m_nLayer - m_nLayerDown;
		m_fScaleLayerDown = m_fScaleLayer;
		m_fScaleLayer *= pow(2, -nScale);
		auto downNode = GetOrCreateMapTileNode(m_nLayerDown);
		downNode->SetScale(m_fScaleLayerDown);
		downNode->SetLocalZOrder(0);
		downNode->SetVisible(true);

		auto curNode = GetCurrentNode();
		curNode->SetLocalZOrder(10);
		curNode->SetScale(m_fScaleLayer);
		curNode->SetVisible(true);
	}
}

void CHmsMapLayer::OnLoadMapTile(HmsMapTile * pMapTile)
{
	if (pMapTile)
	{
		pMapTile->pSprite->SetAnchorPoint(Vec2(0, 1.0));
		Vec2 pos = pMapTile->index.GetLeftTopPosition();
		pMapTile->pSprite->SetPosition(Vec2(pos.x, -pos.y));
		pMapTile->pSprite->SetTag(pMapTile->index.nLayer);
		pMapTile->pSprite->SetName(pMapTile->index.GetString());

		auto node = GetOrCreateMapTileNode(pMapTile->index.nLayer);
		node->AddChild(pMapTile->pSprite);
	}
}

#if 0
{
	Texture2D *pt2d;
	RenderTexture * texture = RenderTexture::create(800, 600);
	texture->setName("123");
	texture->begin();
	pMapTile->pSprite->visit();
	texture->end();

	texture->newImage(false);
	texture->saveToFile("123.png");
}
#endif

void CHmsMapLayer::OnRemoveMapTile(HmsMapTile * pMapTile)
{
	if (pMapTile && pMapTile->pSprite)
	{
		auto node = GetOrCreateMapTileNode(m_nLayer);
		node->RemoveChildByName(pMapTile->index.GetString());
	}
}

void CHmsMapLayer::SetMapCenter(const HmsAviPf::Vec2 & pos)
{
	HmsAviPf::Vec2 posTemp = GetGooglePixelPosByLngLat(pos, m_nLayer);
	auto node = GetCurrentNode();
	node->SetPosition(HmsAviPf::Vec2(-posTemp.x, posTemp.y)*m_fScaleLayer);

	//如果底层图层存在
	if (m_nLayerDown)
	{
		node = GetOrCreateMapTileNode(m_nLayerDown);
		auto posTempDown = GetGooglePixelPosByLngLat(pos, m_nLayerDown);
		node->SetPosition(HmsAviPf::Vec2(-posTempDown.x, posTempDown.y)*m_fScaleLayerDown);
	}

	m_pLoadManager->SetMapCenter(pos);
	m_posCenter = pos;
}

void CHmsMapLayer::SetCurrentHeading(float fHeading)
{
	m_pNodeRoot->SetRotation(-fHeading);
}

HmsAviPf::CHmsNode *  CHmsMapLayer::GetOrCreateMapTileNode(int nLayer)
{
	auto it = m_mapNodeMapTile.find(nLayer);
	if (it != m_mapNodeMapTile.end())
	{
		return it->second;
	}
	else
	{
		auto node = CHmsNode::Create();
		m_pNodeRoot->AddChild(node);
		m_mapNodeMapTile.insert(std::pair<int, HmsAviPf::CHmsNode*>(nLayer, node));
		node->retain();
		return node;
	}
}

HmsAviPf::CHmsNode * CHmsMapLayer::GetCurrentNode()
{
	return GetOrCreateMapTileNode(m_nLayer);
}

void CHmsMapLayer::OnScaleLayer(float fScale)
{	
	auto curNode = GetCurrentNode();

	curNode->SetScale(fScale);
	m_fScaleLayer = fScale;
	if (m_nLayerDown)
	{
		m_fScaleLayerDown = m_fScaleLayer * pow(2, m_nLayer - m_nLayerDown);
		GetOrCreateMapTileNode(m_nLayerDown)->SetScale(m_fScaleLayerDown);
	}
	

	if (fScale > 1.8f)
	{
		auto layer = GetUpLayer();
		if (m_nLayer != layer)
		{
			m_pLoadManager->SetLayer(layer);
		}
	}
	else if (fScale < 0.6f)
	{
		auto layer = GetLowLayer();
		if (m_nLayer != layer)
		{
			m_pLoadManager->SetLayer(layer);
		}
	}
}

int CHmsMapLayer::GetLowLayer()
{
	auto it = m_mapNodeMapTile.find(m_nLayer);
	if (it != m_mapNodeMapTile.begin())
	{
		it--;
	}
	return it->first;
}

int CHmsMapLayer::GetUpLayer()
{
	auto it = m_mapNodeMapTile.find(m_nLayer);
	if (it != m_mapNodeMapTile.end())
	{
		it++;
	}

	if (it != m_mapNodeMapTile.end())
	{
		return it->first;
	}
	else
	{
		return m_nLayer;
	}
}

void CHmsMapLayer::SetCurrentAftData(const HmsAviPf::Vec2 & pos, float fHeading)
{
	SetMapCenter(pos);
	SetCurrentHeading(fHeading);
	m_pLoadManager->Update();
}

void CHmsMapLayer::SetCurrentAftData(float fHeading)
{
	SetCurrentHeading(fHeading);
}

#include "Calculate/PositionZs.h"

void CHmsMapLayer::SetCurrentRange(float fRange)
{
	//EarthCoord2D ecCur(data->pos.dLatitude, data->pos.dLongitude);
	EarthCoord2D ecCur(m_posCenter.y, m_posCenter.x);
	EarthCoord2D ecLong = ecCur;
	EarthCoord2D ecLat = ecCur;
	ecLong.dLongitude += 1.0;
	ecLat.dLatitude += 1.0;

	auto  long1Deg = ecCur.GetDistanceKm(EarthCoord2D(ecLong));
	auto  lat1Deg = ecCur.GetDistanceKm(EarthCoord2D(ecLat));

	auto lngPerPix = m_dLngPerTile / 256;		//每一个像素的经纬
	auto distanceKmPerPix = lngPerPix * long1Deg;

	auto layerDesignDistance = 405 * distanceKmPerPix;

	auto fScale =  layerDesignDistance / fRange;

	OnScaleLayer(fScale);
}

void CHmsMapLayer::OnChangeMapSource()
{
	std::vector<int> vIntLayer;
	if (m_pLoadManager->GetLayerMap(vIntLayer))
	{
		for (auto c : vIntLayer)
		{
			GetOrCreateMapTileNode(c);
		}
	}
}

void CHmsMapLayer::SetLayerAndScaled(int nLayer, float fLayerScale)
{
	if (m_nLayer == nLayer)
	{
		m_fScaleLayer = fLayerScale;

		if (m_nLayerDown)
		{
			int nScale = m_nLayer - m_nLayerDown;
			m_fScaleLayerDown = m_fScaleLayer * pow(2, nScale);

			auto downNode = GetOrCreateMapTileNode(m_nLayerDown);
			downNode->SetScale(m_fScaleLayerDown);
			downNode->SetLocalZOrder(0);
			downNode->SetVisible(true);
		}

		auto curNode = GetCurrentNode();
		curNode->SetLocalZOrder(10);
		curNode->SetScale(m_fScaleLayer);
		curNode->SetVisible(true);
		return;
	}

	if (0 == m_nLayer)
	{
		//初始化
		m_nLayer = nLayer;
		m_nTileCnt = 1 << nLayer;
		m_dLngPerTile = 360.0 / m_nTileCnt;
		m_dLatPerTile = 180.0 / m_nTileCnt;
		m_fScaleLayer = fLayerScale;

		m_nLayerDown = 0;
		m_fScaleLayerDown = 1.0;

		if (m_pLoadManager)
		{
			m_pLoadManager->SetLayer(m_nLayer);
		}
	}
	else
	{
		if (m_nLayerDown)
		{
			//如果之前存在地图
			GetOrCreateMapTileNode(m_nLayerDown)->SetVisible(false);
			//清空图层缓存数据

		}

		//将当前图层转换为背景图层,不更新，为了防止加载是闪屏,或者没内容
		m_nLayerDown = m_nLayer;
		m_nLayer = nLayer;
		m_nTileCnt = 1 << nLayer;
		m_dLngPerTile = 360.0 / m_nTileCnt;
		m_dLatPerTile = 180.0 / m_nTileCnt;
		m_fScaleLayer = fLayerScale;

		int nScale = m_nLayer - m_nLayerDown;
		m_fScaleLayerDown = m_fScaleLayer * pow(2, nScale);

		auto downNode = GetOrCreateMapTileNode(m_nLayerDown);
		downNode->SetScale(m_fScaleLayerDown);
		downNode->SetLocalZOrder(0);
		downNode->SetVisible(true);

		auto curNode = GetCurrentNode();
		curNode->SetLocalZOrder(10);
		curNode->SetScale(m_fScaleLayer);
		curNode->SetVisible(true);

		if (m_pLoadManager)
		{
			m_pLoadManager->SetLayer(m_nLayer);
		}
	}
}

HmsAviPf::Vec2 CHmsMapLayer::GetLngLatByOffset(const HmsAviPf::Vec2 & deltaScreen)
{
	//还原地图旋转前的位置
	HmsAviPf::Vec2 deltaMap = deltaScreen.rotateByAngle(Vec2(), ToRadian(m_pNodeRoot->GetRotation()));
	//获取缩放之后的偏移
	deltaMap *= 1/m_fScaleLayer;
	
	//地图中心位置的像素点
	HmsAviPf::Vec2 posPixCenter = GetGooglePixelPosByLngLat(m_posCenter, m_nLayer);
	HmsAviPf::Vec2 posPix = Vec2(posPixCenter.x-deltaMap.x, posPixCenter.y+deltaMap.y);

	auto ec = GetLngLatByGooglePixel(posPix.x, posPix.y, m_nLayer);

	return Vec2(ec.dLongitude, ec.dLatitude);
}

HmsAviPf::Vec2 CHmsMapLayer::GetLngLatByOffset(const HmsAviPf::Vec2 & posCenter, float fHeading, int nLayer, float fLayerScale, const HmsAviPf::Vec2 & deltaScreen)
{
	//还原地图旋转前的位置
	HmsAviPf::Vec2 deltaMap = deltaScreen.rotateByAngle(Vec2(), ToRadian(-fHeading));
	//获取缩放之后的偏移
	deltaMap *= 1 / fLayerScale;

	//地图中心位置的像素点
	HmsAviPf::Vec2 posPixCenter = GetGooglePixelPosByLngLat(posCenter, nLayer);
	HmsAviPf::Vec2 posPix = Vec2(posPixCenter.x - deltaMap.x, posPixCenter.y + deltaMap.y);

	auto ec = GetLngLatByGooglePixel(posPix.x, posPix.y, nLayer);

	return Vec2(ec.dLongitude, ec.dLatitude);
}


