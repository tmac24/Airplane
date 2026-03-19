#include <HmsAviPf.h>
#include "HmsTerrainServer.h"
#include "../../HmsGlobal.h"

USING_NS_HMS;

CHmsTerrainServer::CHmsTerrainServer()
: m_pThread(nullptr)
, m_bThreadRun(true)
, m_bImageUpdate(false)
, m_nThreadLoadingUse(0)
{

}

CHmsTerrainServer::~CHmsTerrainServer()
{
    End();
}

void CHmsTerrainServer::InitManager(const std::string & geoTiffPath)
{
    CHmsTerrainTiffReader::SetGeoTiffPath(geoTiffPath);
}

void CHmsTerrainServer::Start()
{
    if (nullptr == m_pThread)
    {
        m_bThreadRun = true;
        m_pThread = new std::thread(&CHmsTerrainServer::ThreadRunning, this);
    }
}

void CHmsTerrainServer::End()
{
    m_bThreadRun = false;
    if (m_pThread)
    {
        m_pThread->join();
        delete m_pThread;
        m_pThread = nullptr;
    }
}

void CHmsTerrainServer::ThreadRunning()
{
    while (m_bThreadRun)
    {
        bool bDoLoad = false;
        TerrainTiffIndex gti;

        while (0 == m_nThreadLoadingUse)
        {
            //如果暂停加载，线程暂停
#ifdef  __vxworks
            sleep(10);
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif

            if (!m_bThreadRun)
            {
                break;;
            }
        }

        while (m_qLoadAction.size())
        {
            m_mutexThreadLoading.lock();
            auto action = m_qLoadAction.front();
            m_qLoadAction.pop();
            m_mutexThreadLoading.unlock();
            if (action)
            {
                action();
            }
            SetUnusedLoading();
        }
    }
}

void CHmsTerrainServer::SetUsingLoading()
{
    m_nThreadLoadingUse++;
    if (m_nThreadLoadingUse <= 0)
    {
        m_nThreadLoadingUse = 1;
    }
}

void CHmsTerrainServer::SetUnusedLoading()
{
    m_nThreadLoadingUse--;
    if (m_nThreadLoadingUse < 0)
    {
        m_nThreadLoadingUse = 0;
    }
}

void CHmsTerrainServer::GetAltDataByAsyn(std::vector<AltitudeDataStu> * vAltData, const HmsAltLoadCallback & loadBack)
{
    if (vAltData)
    {
        m_mutexThreadLoading.lock();
        m_qLoadAction.push(HMS_CALLBACK_0(CHmsTerrainServer::OnLoadAltData, this, vAltData, loadBack));
        m_mutexThreadLoading.unlock();
        SetUsingLoading();
    }
    else if (loadBack)
    {
        loadBack(false, vAltData);
    }
}

void CHmsTerrainServer::OnLoadAltData(std::vector<AltitudeDataStu> * vAltData, const HmsAltLoadCallback & loadBack)
{
    bool bRet = false;

    if (vAltData)
    {
        Vec2 pos;
        for (auto & c : *vAltData)
        {
            pos.set(c.lon, c.lat);
            c.altitude = GetAltitude(&pos);
        }
    }

    if (loadBack)
    {
        loadBack(bRet, vAltData);
    }
}

bool CHmsTerrainServer::LoadIndexRawData(const TerrainTiffIndex *index, HmsTerrainRawData *pRawData)
{
    static CHmsTerrainTiffReader s_reader;
    if (s_reader.GetTerrainData(index->nLong, index->nLat, *pRawData))
    {
        return true;
    }
    return false;
}

HmsTerrainRawData* CHmsTerrainServer::GetTerrainRawData(const HmsAviPf::Vec2 *pVec2)
{
    static std::map<int, int> s_mapTifReadFail;
    if (m_mapTerrainRawData.size() > 9)
    {
        for (auto iter = m_mapTerrainRawData.begin(); iter != m_mapTerrainRawData.end(); ++iter)
        {
            delete iter->second;
            iter->second = nullptr;
        }
        m_mapTerrainRawData.clear();
    }

    TerrainTiffIndex index = TerrainTiffIndex::GetIndex(*pVec2);
    HmsTerrainRawData *pRawData = nullptr;
    auto iter = m_mapTerrainRawData.find(index.nIndex);
    if (iter != m_mapTerrainRawData.end())
    {
        pRawData = iter->second;
    }
    else
    {
        if (s_mapTifReadFail.find(index.nIndex) == s_mapTifReadFail.end())
        {
            CHmsTime time;
            time.RecordCurrentTime();
            printf("\n begin read tif  %d_%d \n", index.nLong, index.nLat);

            pRawData = new HmsTerrainRawData();
            if (LoadIndexRawData(&index, pRawData))
            {
                m_mapTerrainRawData[index.nIndex] = pRawData;
                printf("\n end read tif, elapse time %f \n", time.GetElapsed());
            }
            else
            {
                if (pRawData)
                {
                    delete pRawData;
                    pRawData = nullptr;
                }
                s_mapTifReadFail[index.nIndex] = 1;
                printf("\n end read tif failed, elapse time %f \n", time.GetElapsed());
            }
        }
    }
    return pRawData;
}

int CHmsTerrainServer::GetAltitude(const HmsAviPf::Vec2 *pVec2)
{
    std::lock_guard<std::mutex> locker(m_mutexAlt);

    HmsTerrainRawData *pRawTerrainData = GetTerrainRawData(pVec2);
    int alt = 0;
    if (pRawTerrainData)
    {
        alt = pRawTerrainData->GetAltitude(*pVec2);
    }
    return alt;
}









