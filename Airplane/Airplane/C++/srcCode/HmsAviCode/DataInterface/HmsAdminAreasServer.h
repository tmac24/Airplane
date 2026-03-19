#pragma once
#include "base/HmsTimer.h"
#include "HmsAviPf.h"
#include <thread>
#include <mutex>
#include "Database/HmsGeomDef.h"
#include "Database/HmsAdminAreaDatabase.h"


enum class ADMIN_AREAS_DATA_STATE
{
    ActionNone,					//the data is not in action
    ActionRequest,				//is request the action
    ActionLoading,				//is loading the data
    ActionRespond,				//load complete,and wait for respond 
    ActionUsed,					//the data is used
};

struct HmsAdminAreasServerDataCell
{
    ADMIN_AREAS_DATA_STATE		eState;
    int							nLayer;
    std::vector<HmsGeomData *>	vGeomData;
};

typedef std::function<void(HmsAdminAreasServerDataCell * dataCell)>	 HmsAdminAreasDataCallback;
typedef std::function<void()>										 HmsAdminAreasActionFunc;

/**
*use to load Admin Areas
*param
*returns
*by [10/9/2019 song.zhang]
*/
class CHmsAdminAreasServer
{
public:
    CHmsAdminAreasServer();
    ~CHmsAdminAreasServer();

    static CHmsAdminAreasServer * GetInstance();

    void Start();

    void End();

    //设置使用加载
    void SetUsingLoading();

    //设置取消加载
    void SetUnusedLoading();

    void GetDataByAsyn(const HmsGeomBoundary & boundary, HmsAdminAreasServerDataCell * dataCell, const HmsAdminAreasDataCallback & callback);

    std::vector<HmsGeomData> & GetChinaAmdin(){ return m_dataChinaAdmin; }

protected:
    void Init();

    void ThreadRunning();

    //检查地图是否需要更新，地图级别切换，或者地图移动等
    void CheckLoad();

    int FixAdminAreaDescribeLayer(int nLayer, const HmsAdminAreaDescribe & describe);

    bool GetGeomDataByDescribe(const HmsAdminAreaDescribe & describe, const HmsAviPf::Rect & rect, int nLayer, std::vector<HmsGeomData *> & m_vGeomData);
    bool GetGeomData(const HmsAviPf::Rect & rect, int nLayer, std::vector<HmsGeomData *> & m_vGeomData);

    void OnLoadData(const HmsGeomBoundary & boundary, HmsAdminAreasServerDataCell * dataCell, const HmsAdminAreasDataCallback & callback);


private:
    void CleanTempMap();

private:
    std::thread *										m_pThread;
    std::mutex											m_mutexThreadLoading;				//用于线程的数据加载锁
    bool												m_bThreadRun;
    int													m_nThreadLoadingUse;				//用于线程加载暂停，用于地图在后台运行时

    std::queue<HmsAdminAreasActionFunc>					m_qLoadAction;

    bool												m_bImageUpdate;						//图像已经更新

    std::map<std::string, HmsAdminAreaDescribe>			m_mapAdminAreaDescribe;

    CHmsAdminAreaDatabase								m_db;

    int													m_nCurLayer;					//the current layer index

    std::vector<HmsGeomData>                            m_dataChinaAdmin;
};

