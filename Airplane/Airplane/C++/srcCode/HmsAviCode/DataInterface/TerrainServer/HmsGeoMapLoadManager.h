#pragma once
#include "HmsTerrainTiffReader.h"
#include "base/HmsTimer.h"
#include "HmsAviPf.h"
#include "HmsTerrainTile.h"
#include "HmsTerrainTileDescribe.h"
#include <thread>
#include <mutex>
#include "base/HmsImage.h"

class CHmsGeoMapLoadManager
{
public:
	CHmsGeoMapLoadManager();
	~CHmsGeoMapLoadManager();

	void InitManager(const std::string & geoTiffPath);

	void Start();

	void End();

	void SetMapCenter(const HmsAviPf::Vec2 & pos);

	void SetLayer(int nLayer){ m_nMapLayer = nLayer; }

	bool IsImageUpdate(){return m_bImageUpdate;}

	HmsAviPf::Image * GetImage(HmsAviPf::Vec2 & mapCenter, HmsAviPf::Rect & mapRect, HmsAviPf::Vec2 & mapDstOffset, float & fLonLatOffset);

	//设置使用加载
	void SetUsingLoading();

	//设置取消加载
	void SetUnusedLoading();

protected:
	void ThreadRunning();

	//检查地图是否需要更新，地图级别切换，或者地图移动等
	void CheckLoad();

	//加载缓冲中的地图文件
	bool LoadingMapTile();

	bool LoadIndexRawData(const TerrainTiffIndex & index, HmsTerrainRawData & rawData);

	inline static int GetCenterRow(){ return c_nMapTileRow / 2; }
	inline static int GetCenterCol(){ return c_nMapTileCol / 2; }


private:
	bool GetMapTileData(HmsTerrainRawData & rawData, CHmsTerrainTile * mapTile);
	void CleanTempMap();
	void AddTempMapTileToLoading(const CHmsTerrainTileDescribe & des);
	void UpdateImageData();

private:
	const static int c_nMapTileRow = 3;
	const static int c_nMapTileCol = 3;

	HmsAviPf::Vec2										m_posCenter;						//需要显示的地图中心位置
	CHmsTerrainTileDescribe									m_mapTileMapCenter;

	std::thread *										m_pThread;
	std::mutex											m_mutexThreadLoading;				//用于线程的数据加载锁
	bool												m_bThreadRun;
	int													m_nThreadLoadingUse;				//用于线程加载暂停，用于地图在后台运行时

	std::map<TerrainTiffIndex, HmsTerrainRawData*>			m_mapTempTerrainRawData;			//存储临时原始地形文件，其他级别的地形都从里面去获取
	int													m_nMapLayer;						//地图层级编号

	std::multimap<TerrainTiffIndex, CHmsTerrainTile*>			m_mapLoadTile;



	CHmsTerrainTile											m_mapTile[c_nMapTileRow][c_nMapTileCol];
	HmsAviPf::RefPtr<HmsAviPf::Image>						m_image;
	HmsAviPf::Vec2										m_mapCenter;
	HmsAviPf::Rect										m_mapRect;
	HmsAviPf::Vec2										m_mapDstByOffset;
	float												m_fLngLatOffset;
	bool												m_bImageUpdate;								//图像已经更新
};

