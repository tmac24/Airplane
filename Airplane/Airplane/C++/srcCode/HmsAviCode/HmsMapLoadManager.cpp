#include <HmsAviPf.h>
#include "HmsMapLoadManager.h"
#include "HmsMapReader.h"
#include "render/HmsTextureCache.h"
#include "Nav/HmsMapTileSql.h"

#define  ZS_PI 3.14159265358979323846
#define  ZS_E 2.718281828459

double LngToPixelByGoogle(double dLng, int zoom)
{
	return (dLng + 180.0)*(256 << zoom) / 360;
}

double PixelToLngByGoogle(double dPixel, int zoom)
{
	return dPixel * 360 / (256 << zoom) - 180;
}

double LatToPixelByGoogle(double dLat, int zoom)
{
	double siny = sin(dLat*ZS_PI / 180.0);
	double y = log((1 + siny) / (1 - siny));
	return (128 << zoom)*(1 - y / (2 * ZS_PI));
}

double PixelToLatByGoogle(double dPixedY, int zoom)
{
	double y = 2 * ZS_PI * (1 - dPixedY / (128 << zoom));
	double z = pow(ZS_E, y);
	double siny = (z - 1) / (z + 1);
    return HmsMath_asin(siny) * 180 / ZS_PI;
}

HmsAviPf::Vec2 GetGooglePixelPosByLngLat(HmsAviPf::Vec2 lngLat, int zoom)
{
	HmsAviPf::Vec2 pos(0, 0);
	pos.x = LngToPixelByGoogle(lngLat.x, zoom);
	pos.y = LatToPixelByGoogle(lngLat.y, zoom);
	return pos;
}

CSimEarthCoord GetLngLatByGooglePixel(double dPixX, double dPixY, int Zoom)
{
	CSimEarthCoord ec(PixelToLngByGoogle(dPixX, Zoom), PixelToLatByGoogle(dPixY, Zoom));
	return ec;
}

CHmsMapLoadManager::CHmsMapLoadManager()
	: m_funcLoad(nullptr)
	, m_funcRemove(nullptr)
	, m_funcChangeLayer(nullptr)
	, m_funcChangeSource(nullptr)
	, m_nLayerCurrent(1)
	, m_nScreenWidth(3300)
	, m_nScreenHeight(2080)
	, m_nNeedTile(1)
	, m_eCurSitMap(HmsSitMapMode::none)
	, m_threadLoading(nullptr)
	, m_bRun(true)
{
	UpdateTileInfo();
	m_indexCenter.nLat = -1;
	m_indexCenter.nLng = -1;

	CHmsMapTileSql &mapTileSql = CHmsMapTileSql::GetInstanceBackup();
	mapTileSql.ChangeMapTypeName("satellite");


	HmsMapTile::s_funTileLoad = [=](HmsMapTile * pTile, HmsAviPf::Texture2D * pTexture)
	{
		if (pTile && pTile->pLoadManager)
		{
			
			auto & mapTile = pTile->pLoadManager->m_mapTile;

			auto find = mapTile.find(pTile->index);
			if (find != mapTile.end())
			{
				auto & pSprite = find->second->pSprite;

				if (pSprite)
				{
					if (pTexture)
					{
						auto lastTexture = pSprite->GetTexture();
						pSprite->SetTexture(pTexture);
					}
					else
					{
						auto pFailedTexture = HmsAviPf::CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage("res/loadFailed.png");
						pSprite->SetTexture(pFailedTexture);
					}
					pTile->retain();
					return;
					pSprite->autorelease();
					pSprite = nullptr;
				}

				if (pTexture)
				{
					pSprite = HmsAviPf::CHmsImageNode::CreateWithTexture(pTexture);
				}
				else
				{
					pSprite = HmsAviPf::CHmsImageNode::Create("res/loadFailed.png");
				}

				if (pSprite)
				{
					pSprite->retain();
				}

				if (m_funcLoad)
				{
					pTile->pLoadManager->m_funcLoad(find->second);
				}
			}
			
		}
	};
}


CHmsMapLoadManager::~CHmsMapLoadManager()
{

}

void CHmsMapLoadManager::SetMapCenter(const HmsAviPf::Vec2 & pos)
{
	m_vecMapCenter = pos;
}

void CHmsMapLoadManager::SetScreenPix(unsigned int nWidth, unsigned int nHeight)
{
	m_nScreenWidth = nWidth;
	m_nScreenHeight = nHeight;

	UpdateTileInfo();
}

unsigned int CHmsMapLoadManager::GetLayerTile(int nLayer)
{
	return 1 << nLayer;
}

void CHmsMapLoadManager::UpdateTileInfo()
{
	auto nMax = (m_nScreenWidth > m_nScreenHeight) ? m_nScreenWidth : m_nScreenHeight;
	auto nNeedTile = (nMax + 2 * 256) / 256;		//加宽至少256
	auto nLayerTile = GetLayerTile(m_nLayerCurrent);

	m_nNeedTile = (nNeedTile > nLayerTile) ? nLayerTile : nNeedTile;
}

void CHmsMapLoadManager::Init(const TILE_FUNC & funcLoad, const TILE_FUNC & funcRemove, const TILE_LAYER_FUNC & funcChangeLayer, const TILE_SOURCE_FUNC & funcChangeSource, int nLayer)
{
	m_funcLoad = funcLoad;
	m_funcRemove = funcRemove;
	m_funcChangeLayer = funcChangeLayer;
	m_funcChangeSource = funcChangeSource;
	SetLayer(nLayer);

// 	HmsMapTile::s_funTileLoad = [=](HmsMapTile * pTile, HmsAviPf::Texture2D * pTexture)
// 	{
// 		if (pTile)
// 		{
// 			auto find = m_mapTile.find(pTile->index);
// 			if (find != m_mapTile.end())
// 			{
// 				auto & pSprite = find->second->pSprite;
// 
// 				if (pSprite)
// 				{
// 					pSprite->autorelease();
// 					pSprite = nullptr;
// 				}
// 
// 				if (pTexture)
// 				{
// 					pSprite = HmsAviPf::CHmsImageNode::createWithTexture(pTexture);
// 				}
// 				else
// 				{
// 					pSprite = HmsAviPf::CHmsImageNode::create("loadFailed.png");
// 				}
// 
// 				if (pSprite)
// 				{
// 					pSprite->retain();
// 				}
// 				
// 				if (m_funcLoad)
// 				{
// 					m_funcLoad(find->second);
// 				}
// 			}
// 		}
// 	};
}

void CHmsMapLoadManager::Update()
{
	CheckCurPos();

	//检查删除
	CheckRemove();

	CheckResponse();
}

void CHmsMapLoadManager::CheckCurPos()
{
	auto index = GetIndexByLngLat(m_vecMapCenter);
	if (m_indexCenter == index)
	{
		//不做任何操作
	}
	else
	{
		//当前地图块更改
		//检测当前地图是否存在
		auto tileCnt = GetLayerTile(m_nLayerCurrent);
		auto nOneSide = (unsigned int)(m_nNeedTile / 2);
		unsigned int nLngBegin = (index.nLng + tileCnt - nOneSide) % tileCnt;
		unsigned int nLatBegin = (index.nLat + tileCnt - nOneSide) % tileCnt;
		auto nCnt = nOneSide * 2 + 1;
		unsigned int nLngEnd = nLngBegin + nCnt;
		unsigned int nLatEnd = nLatBegin + nCnt;

		m_indexCenter = index;

		for (auto nLng = nLngBegin; nLng < nLngEnd; nLng++)
		{
			for (auto nLat = nLatBegin; nLat < nLatEnd; nLat++)
			{
				TileIndex temp = index;
				temp.nLat = nLat;
				temp.nLng = nLng;
				if (!CheckTile(temp))
				{
					auto hmt = new HmsMapTile();
					if (hmt)
					{
						hmt->index = temp;
						hmt->retain();
						m_mapTile.insert(std::pair<TileIndex, HmsMapTile*>(temp, hmt));
						hmt->pLoadManager = this;

						LoadMapTileAsync(hmt);
					}
				}
			}
		}
	}
}

void CHmsMapLoadManager::CheckRemove()
{

	std::vector<TileIndex> vRemoveIndex;

	for (auto & it:m_mapTile)
	{
		auto & index = it.first;
		if (index.nLayer != m_nLayerCurrent)
		{
			;
		}
		else
		{
			int temp = (index.nLat - m_indexCenter.nLat);
			int temp2 = (index.nLng - m_indexCenter.nLng);

			if (it.second)
			{
				if ((abs(temp) > m_nNeedTile / 2) || (abs(temp2) > m_nNeedTile / 2))
				{
					if (m_funcRemove)
					{
						m_funcRemove(it.second);
					}
					if (it.second->pImage)
					{
						it.second->pImage->autorelease();
						it.second->pImage = nullptr;
					}
					if (it.second->pSprite)
					{
						it.second->pSprite->autorelease();
						it.second->pSprite = nullptr;
					}
					it.second->autorelease();
					it.second = nullptr;
					vRemoveIndex.push_back(it.first);
				}
			}
		}
	}

	for (auto & it:vRemoveIndex)
	{
		m_mapTile.erase(it);
	}
}

void CHmsMapLoadManager::SetLayer(int nLayer)
{
	m_nLayerCurrent = nLayer;

	if (m_funcChangeLayer)
	{
		m_funcChangeLayer(m_nLayerCurrent);
	}

	UpdateTileInfo();
}

TileIndex CHmsMapLoadManager::GetIndexByLngLat(const HmsAviPf::Vec2 & pos)
{
	TileIndex index;

	auto tileCnt = GetLayerTile(m_nLayerCurrent);
	auto fLngPerTile = 360.0f / tileCnt;
	auto fLatPerTile = 180.0f / tileCnt;

	index.nLayer = m_nLayerCurrent;
	index.nLng = floor(fmod((pos.x + 180.0f),360.0) / fLngPerTile);
	//错的
	//index.nLat = floor((90.0f - pos.y)/fLatPerTile);
	index.nLat = floor(LatToPixelByGoogle(pos.y, m_nLayerCurrent) / 256);

	return index;
}

bool CHmsMapLoadManager::CheckTile(const TileIndex & index)
{
	return (m_mapTile.find(index) != m_mapTile.end());
}

void CHmsMapLoadManager::SetMapFolder(const std::string & strFolder)
{
	CHmsMapReader::SetRootFolder(strFolder);
}

void CHmsMapLoadManager::SetMapFolder(HmsSitMapMode eMap)
{
	auto itFind = s_mapMapFolder.find(eMap);
	if (itFind != s_mapMapFolder.end())
	{
		if (m_eCurSitMap != eMap)
		{
			m_eCurSitMap = eMap;
			CHmsMapReader::SetRootFolder(itFind->second.strFolder, itFind->second.strSuffix);

			OnRootFolderChange();
		}
	}
}

bool CHmsMapLoadManager::GetLayerMap(std::vector<int> & vDirectory)
{
	return CHmsMapReader::GetLayerMap(vDirectory);
}

void CHmsMapLoadManager::AddMapFolder(HmsSitMapMode eMap, const std::string & strFolder, const std::string & strSuffix)
{
	MapFolder mapFolder;
	mapFolder.strFolder = strFolder;
	mapFolder.strSuffix = strSuffix;
	s_mapMapFolder.insert(std::pair<HmsSitMapMode, MapFolder>(eMap, mapFolder));
}

void CHmsMapLoadManager::OnRootFolderChange()
{
	//从新加载
	if (m_funcChangeSource)
	{
		m_funcChangeSource();
	}

	for (auto & c:m_mapTile)
	{
		auto temp = c.first;
		auto strPath = CHmsMapReader::GetTilePath(temp.nLayer, temp.nLng, temp.nLat);
		HmsAviPf::CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImageAsync(strPath.c_str(), HMS_CALLBACK_1(HmsMapTile::OnLoad, c.second));
	}
}

void CHmsMapLoadManager::LoadMapTileAsync(HmsMapTile * pMapTile)
{
	m_mutexRequest.lock();
	m_queueRequest.push(pMapTile);
	m_mutexRequest.unlock();

	if (m_threadLoading == nullptr)
	{
		// create a new thread to load images
		m_threadLoading = new std::thread(&CHmsMapLoadManager::RunLoadMapTile, this);
		m_bRun = true;
	}

	m_conditionSleep.notify_one();
}

void CHmsMapLoadManager::RunLoadMapTile()
{
	HmsMapTile * pMapTile = nullptr;
	CHmsMapTileSql &mapTileSql = CHmsMapTileSql::GetInstanceBackup();
	std::mutex signalMutex;
	std::unique_lock<std::mutex> signal(signalMutex);

	while (m_bRun)
	{
		m_mutexRequest.lock();
		if (m_queueRequest.empty())
		{
			pMapTile = nullptr;
		}
		else
		{
			pMapTile = m_queueRequest.front();
			m_queueRequest.pop();
		}
		m_mutexRequest.unlock();

		if (nullptr == pMapTile)
		{
			//wait
			m_conditionSleep.wait(signal);
			continue;
		}

		if (nullptr == pMapTile->pImage)
		{
			pMapTile->pImage = new Image();
		}

		if (!mapTileSql.GetImageFromDB(pMapTile->index.nLayer, pMapTile->index.nLng, pMapTile->index.nLat, pMapTile->pImage))
		{
			delete pMapTile->pImage;
			pMapTile->pImage = nullptr;
		}

		m_mutexResponse.lock();
		m_queueResponse.push(pMapTile);
		m_mutexResponse.unlock();
	}
}

void CHmsMapLoadManager::CheckResponse()
{
	m_mutexResponse.lock();
	while (m_queueResponse.size())
	{
		HmsMapTile * pMapTile = m_queueResponse.front();
		m_queueResponse.pop();
		Texture2D * texture = nullptr;
		if (pMapTile && pMapTile->pImage)
		{
			texture = new Texture2D();
			if (texture)
			{
				texture->autorelease();
				if (!texture->initWithImage(pMapTile->pImage))
				{
					texture = nullptr;
				}
			}
			
			pMapTile->OnLoad(texture);
		}
	}
	
	m_mutexResponse.unlock();
}

std::map<HmsSitMapMode, MapFolder> CHmsMapLoadManager::s_mapMapFolder;

Tile_Load_Func HmsMapTile::s_funTileLoad = nullptr;
