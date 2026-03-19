#pragma once

#include "base/HmsTimer.h"
#include "SimEarthCoord.h"
#include "HmsMapDef.h"
#include "base/HmsImage.h"
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

// enum class HmsSitMapMode
// {
// 	none,				//无地图
// 	terrian,			//地形
// 	roadmap,			//街道
// 	satellite,			//卫星
// };

double LngToPixelByGoogle(double dLng, int zoom);
double PixelToLngByGoogle(double dPixel, int zoom);
double LatToPixelByGoogle(double dLat, int zoom);
double PixelToLatByGoogle(double dPixedY, int zoom);

HmsAviPf::Vec2 GetGooglePixelPosByLngLat(HmsAviPf::Vec2 lngLat, int zoom);

CSimEarthCoord GetLngLatByGooglePixel(double dPixX, double dPixY, int Zoom);

class CHmsMapLoadManager;

struct TileIndex
{
	unsigned int nLng;
	unsigned int nLat;
	unsigned int nLayer;

	TileIndex()
	{
		nLng = 0;
		nLat = 0;
		nLayer = 0;
	}

	std::string GetString()
	{
		char strOut[64];
		sprintf(strOut, "%d_%d_%d", nLayer, nLng, nLat);
		return strOut;
	}

	HmsAviPf::Vec2 GetLeftTopPosition()
	{
		return HmsAviPf::Vec2(nLng * 256, nLat * 256);
	}

	bool operator<(const TileIndex & right) const 
	{ 
		if (nLayer < right.nLayer)
		{
			return true;
		}
		else if (nLayer == right.nLayer)
		{
			if (nLat < right.nLat)
			{
				return true;
			}
			else if (nLat == right.nLat)
			{
				return (nLng < right.nLng);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	bool operator>(const TileIndex & right) const
	{
		if (nLayer > right.nLayer)
		{
			return true;
		}
		else if (nLayer == right.nLayer)
		{
			if (nLat > right.nLat)
			{
				return true;
			}
			else if (nLat == right.nLat)
			{
				return (nLng > right.nLng);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	bool operator==(const TileIndex & right) const
	{
		return (nLayer == right.nLayer) && (nLat == right.nLat) && (nLng == right.nLng);
	}
};

struct MapTile
{
	TileIndex		 index;
	HmsAviPf::Image * pImage;

	MapTile()
	{
		pImage = nullptr;
	}
};

#define Tile_Load_Func std::function<void(HmsMapTile*, HmsAviPf::Texture2D*)>

struct HmsMapTile :public HmsAviPf::Ref
{
	TileIndex						index;
	HmsAviPf::Image *				pImage;
	HmsAviPf::CHmsImageNode *		pSprite;
	CHmsMapLoadManager *	pLoadManager;

	HmsMapTile()
	{
		pImage = nullptr;
		pSprite = nullptr;
		pLoadManager = nullptr;
	}

	~HmsMapTile()
	{
		HMS_SAFE_RELEASE_NULL(pImage);
		HMS_SAFE_RELEASE_NULL(pSprite);
	}

	void OnLoad(HmsAviPf::Texture2D *pTexture)
	{
		if (s_funTileLoad)
		{
			this->autorelease();
			s_funTileLoad(this, pTexture);
		}
	}

	static Tile_Load_Func s_funTileLoad;
};

struct MapFolder 
{
	std::string strFolder;
	std::string strSuffix;
};

#define TILE_FUNC  std::function<void(HmsMapTile*)>
#define TILE_LAYER_FUNC  std::function<void(int)>
#define TILE_SOURCE_FUNC std::function<void(void)>

class CHmsMapLoadManager
{
public:
	CHmsMapLoadManager();
	~CHmsMapLoadManager();

	void Init(const TILE_FUNC  & funcLoad, const TILE_FUNC & funcRemove, const TILE_LAYER_FUNC & funcChangeLayer, const TILE_SOURCE_FUNC & funcChangeSource, int nLayer = 4);

	void SetLayer(int nLayer);

	void SetScreenPix(unsigned int nWidth, unsigned int nHeight);

	void SetMapCenter(const HmsAviPf::Vec2 & pos);

	void Update();

	void SetMapFolder(const std::string & strFolder);

	bool GetLayerMap(std::vector<int> & vDirectory);

	void SetMapFolder(HmsSitMapMode eMap);

	static void AddMapFolder(HmsSitMapMode eMap, const std::string & strFolder, const std::string & strSuffix);

protected:
	void OnMapTileLoaded(MapTile & mapTile);

	void CheckCurPos();

	void CheckRemove();

	void CheckResponse();

	TileIndex GetIndexByLngLat(const HmsAviPf::Vec2 & pos);

	void OnRootFolderChange();

	void LoadMapTileAsync(HmsMapTile * pMapTile);

	void RunLoadMapTile();


private:
	unsigned int GetLayerTile(int nLayer);

	void UpdateTileInfo();

	inline bool CheckTile(const TileIndex & index);

private:
	HmsAviPf::Vec2		m_vecMapCenter;					//地图中心点
	TileIndex			m_indexCenter;

	HmsSitMapMode			m_eCurSitMap;					//记录当前使用的路径ID

	int										m_nLayerCurrent;			//当前图层
	int										m_nNeedTile;				//当前图层需要的贴图数量
	unsigned int							m_nScreenWidth;				//屏幕宽度
	unsigned int							m_nScreenHeight;			//屏幕高度


	std::map<TileIndex, HmsMapTile*>		m_mapTile;

	TILE_FUNC								m_funcLoad;
	TILE_FUNC								m_funcRemove;
	TILE_LAYER_FUNC							m_funcChangeLayer;
	TILE_SOURCE_FUNC						m_funcChangeSource;

	std::thread								* m_threadLoading;
	std::condition_variable					m_conditionSleep;
	std::mutex								m_mutexRequest;
	std::queue<HmsMapTile*>					m_queueRequest;

	std::mutex								m_mutexResponse;
	std::queue<HmsMapTile*>					m_queueResponse;
	bool									m_bRun;

	static std::map<HmsSitMapMode, MapFolder>		s_mapMapFolder;
};

