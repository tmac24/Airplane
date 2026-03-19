#include "TileDataMgrBase.h"
#include "../Svs/FileManager.h"
#include "../Svs/AltManager.h"
//#include "..\DataInterface\FsxDataServer\SimFsxDataDef.h"
#include "BarrierTileDataMgr.h"
#include <mutex>


NS_HMS_BEGIN

static std::mutex res_mutex;

BarrierTileDataMgr* BarrierTileDataMgr::m_pInstance = nullptr;

BarrierTileDataMgr::BarrierTileDataMgr()
{

}

BarrierTileDataMgr* BarrierTileDataMgr::getInstance()
{
    if (nullptr == m_pInstance)
    {
        std::unique_lock<std::mutex> mymutex(res_mutex);
        if (nullptr == m_pInstance)
        {
            m_pInstance = new BarrierTileDataMgr;
        }
    }
    return m_pInstance;
}

//extern "C" const char*  GetTawsDataPath();

void BarrierBlock::InitBarrierBlockData()
{
    unsigned int len;
    unsigned char* barrierData = nullptr;

    char filepath[256] = { 0 };
    char name[64] = { 0 };
    const char* taws_data_path = GetTawsDataPath();

    auto nLon = m_lon;
    auto nLat = m_lat;

    if (m_lon < 0)
    {
        nLon = m_lon + 1;
    }
    if (m_lat < 0)
    {
        nLat = m_lon + 1;
    }

    sprintf(name, "export_%03d_%03d.dat", abs(nLon), abs(nLat));
    sprintf(filepath, "%s/Obstacle/%sx%sy/%s", taws_data_path, m_lon >= 0 ? "+" : "-", m_lat >= 0 ? "+" : "-", name);

    len = LoadBinaryFile(filepath, &barrierData);

    if (nullptr == barrierData)
    {
        return;
    }

    int i = 0;
    int sizeofBarrier = sizeof(BarrierParameters);
    int recordNumber = len / sizeofBarrier;
    printf("num:%d\n", recordNumber);
    for (i = 0; i < recordNumber; i++)
    {
        BarrierParameters* barrie = new BarrierParameters;
        memcpy(barrie, barrierData + sizeofBarrier*i, sizeofBarrier);

        barrie->height = barrie->height * METET_PER_FEET;
        barrie->altitude = barrie->altitude * METET_PER_FEET;
        m_BarrierInfos.push_back(barrie);
    }

    if (nullptr != barrierData)
    {
        HMS_SAFE_DELETE_ARRAY(barrierData);
    }
}


BarrierBlock::BarrierBlock(int lon, int lat)
: DataBlock(lon, lat)
{
    InitBarrierBlockData();
}

BarrierBlock::BarrierBlock(const Vec2& lonlat)
: BarrierBlock(lonlat.x, lonlat.y)
{
}

BarrierBlock::BarrierBlock(const TileKey& key)
: BarrierBlock(key.first, key.second)
{
}

const std::vector<BarrierParameters*>& BarrierBlock::GetBarrierInfos(void)
{
    return m_BarrierInfos;
}


BarrierBlock::~BarrierBlock()
{
    for (auto c : m_BarrierInfos)
    {
        HMS_SAFE_DELETE(c);
    }
    m_BarrierInfos.clear();
}

const std::set<BarrierBlock*>& BarrierTileDataMgr::GetRenderBlocks()
{
    static int s_heartbeat = 0;
    int heartbeat = GetHeartBeat();
    if (s_heartbeat != heartbeat)
    {
        s_heartbeat = heartbeat;
        for (auto c : m_renderBlockBuffer)
        {
            HMS_SAFE_RELEASE_NULL(c);
        }
        m_renderBlockBuffer.clear();

        for (auto c : m_renderChunks)
        {
            BarrierBlock* pbb = static_cast<BarrierBlock*>(c.second);
            m_renderBlockBuffer.insert(pbb);
            HMS_SAFE_RETAIN(c.second);
        }
    }

    return m_renderBlockBuffer;
}


const std::vector<BarrierParameters*>& HmsAviPf::BarrierTileDataMgr::GetNearestBarrierInfors(const Vec2& pos)
{
    m_nearrestBarrierBuffer.clear();
    for (auto c : m_renderChunks)
    {
        BarrierBlock* pbb = static_cast<BarrierBlock*>(c.second);
        for (auto pbp : pbb->GetBarrierInfos())
        {
            m_nearrestBarrierBuffer.push_back(pbp);
        }
    }

    std::sort(m_nearrestBarrierBuffer.begin(), m_nearrestBarrierBuffer.end(),
        [pos](const BarrierParameters* a, const BarrierParameters* b){
        return Vec2(a->longitude, a->latitude).distanceSquared(pos) < Vec2(b->longitude, b->latitude).distanceSquared(pos);
    });

    return m_nearrestBarrierBuffer;
}

BarrierBlock* BarrierTileDataMgr::CreateDataBlock(const TileKey& tkey)
{
    return new BarrierBlock(tkey);
}

NS_HMS_END


