#pragma once
#include "HmsTerrainTiffReader.h"
#include "base/HmsTimer.h"
#include "HmsAviPf.h"
#include "HmsTerrainTile.h"
#include "HmsTerrainTileDescribe.h"
#include <thread>
#include <mutex>
#include "base/HmsImage.h"
#include "../../Nav/HmsNavDataStruct.h"
#include "math/HmsMath.h"

typedef std::function<void(bool, std::vector<AltitudeDataStu> *)>			HmsAltLoadCallback;
typedef std::function<void()>												HmsTerrainActionFunc;

/**
*use to load terrain
*param   
*returns 
*by [1/17/2018 song.zhang] 
*/
class CHmsTerrainServer
{
public:
	CHmsTerrainServer();
	~CHmsTerrainServer();

	void InitManager(const std::string & geoTiffPath);

	void Start();

	void End();

	//设置使用加载
	void SetUsingLoading();

	//设置取消加载
	void SetUnusedLoading();

	/**
	*get the altitude by the interval distance Asynchronous
	*param   
	*returns 
	*by [1/17/2018 song.zhang] 
	*/
	void GetAltDataByAsyn(std::vector<AltitudeDataStu> * vAltData, const HmsAltLoadCallback & loadBack);
    int GetAltitude(const HmsAviPf::Vec2 *pVec2);

protected:
	void ThreadRunning();

	//检查地图是否需要更新，地图级别切换，或者地图移动等
	void CheckLoad();

	//加载缓冲中的地图文件
	bool LoadingMapTile();

	void OnLoadAltData(std::vector<AltitudeDataStu> * vAltData, const HmsAltLoadCallback & loadBack);

    bool LoadIndexRawData(const TerrainTiffIndex *index, HmsTerrainRawData *pRawData);
    HmsTerrainRawData* GetTerrainRawData(const HmsAviPf::Vec2 *pVec2);

private:
	std::thread *										m_pThread;
	std::mutex											m_mutexThreadLoading;				//用于线程的数据加载锁
    std::mutex                                          m_mutexAlt;
	bool												m_bThreadRun;
	int													m_nThreadLoadingUse;				//用于线程加载暂停，用于地图在后台运行时

	TerrainTiffIndex									m_indexCurPosition;					//当前所在位置的索引，用于清空缓存的时候，不把当前数据清除。

    std::map<int, HmsTerrainRawData*>		m_mapTerrainRawData;			//存储临时原始地形文件，其他级别的地形都从里面去获取
	std::queue<HmsTerrainActionFunc>					m_qLoadAction;

	bool												m_bImageUpdate;						//图像已经更新
};

