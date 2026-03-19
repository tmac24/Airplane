#include "TileDataMgrBase.h"
#include "Svs/FileManager.h"
#include "Svs/AltManager.h"
//#include "..\DataInterface\FsxDataServer\SimFsxDataDef.h"
#include "RunwayTileDataMgr.h"
#include <mutex>
#include "Calculate/CalculateZs.h"

NS_HMS_BEGIN

static std::mutex res_mutex;

RunwayTileDataMgr* RunwayTileDataMgr::m_pInstance = nullptr;

RunwayTileDataMgr::RunwayTileDataMgr()
{

}

RunwayTileDataMgr* RunwayTileDataMgr::getInstance()
{
	if (nullptr == m_pInstance)
	{
		std::unique_lock<std::mutex> mymutex(res_mutex);
		if (nullptr == m_pInstance)
		{
			m_pInstance = new RunwayTileDataMgr;
		}
	}
	return m_pInstance;
}


//extern "C" const char*  GetTawsDataPath();


void RunwayBlock::InitRunwayBlockData()
{
	char filepath[256] = { 0 };
	unsigned int len;
	unsigned char *binaryData = nullptr;

	char name[64] = { 0 };
	const char *taws_data_path = GetTawsDataPath();

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
	sprintf(filepath, "%s/Runway/%sx%sy/%s", taws_data_path, m_lon >= 0 ? "+" : "-", m_lat >= 0 ? "+" : "-", name);

	len = LoadBinaryFile(filepath, &binaryData);

	if (nullptr == binaryData)
	{
		//printf("runway data loading failed (%d, %d)\n",m_lon, m_lat);
		return;
	}

	const unsigned int sizeOfPerData = sizeof(RunwayParameters);
	int i = 0;
	int recordNumber = len / sizeOfPerData;
	for (i = 0; i < recordNumber; i++)
	{
		RunwayParameters* record = new RunwayParameters;
		memcpy(record, binaryData + sizeOfPerData * i, sizeOfPerData);
		m_RunwayInfos.push_back(record);
	}
	if (nullptr != binaryData)
	{
		HMS_SAFE_DELETE(binaryData);
	}

	//C3DAirportAnysysRunwayData(blk);
}


RunwayBlock::RunwayBlock(int lon, int lat) 
: DataBlock(lon,lat)
{
	InitRunwayBlockData();
}

RunwayBlock::RunwayBlock(const Vec2& lonlat)
: RunwayBlock(lonlat.x, lonlat.y)
{	
}

RunwayBlock::RunwayBlock(const TileKey& key)
: RunwayBlock(key.first, key.second)
{	
}

const std::vector<RunwayParameters*>& RunwayBlock::GetRunwayInfos(void)
{
	return m_RunwayInfos;
}


RunwayBlock::~RunwayBlock()
{
	for (auto c : m_RunwayInfos)
	{
		HMS_SAFE_DELETE(c);
	}
	m_RunwayInfos.clear();
}

const std::set<RunwayBlock*>& RunwayTileDataMgr::GetRenderBlocks()
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
			RunwayBlock* prb = static_cast<RunwayBlock*>(c.second);
			m_renderBlockBuffer.insert(prb);
			HMS_SAFE_RETAIN(c.second);
		}		
	}
	return m_renderBlockBuffer;
}

RunwayBlock* RunwayTileDataMgr::CreateDataBlock(const TileKey& tkey)
{
	return new RunwayBlock(tkey);
}

OutputInfo RunwayTileDataMgr::CalcNearestRunwayInfo(const double& lon, const double& lat)
{
    RunwayTileDataMgr::getInstance()->UpdateBlocks(HmsAviPf::Vec2((float)lon, (float)lat));
    OutputInfo oi;

    double distanceKmMin = 9999.0;//km
    double distanceTmpMin = 180.0;//degree
    RunwayParameters* pNearestRunwayParam = nullptr;

    auto runwayBlocks = RunwayTileDataMgr::getInstance()->GetRenderBlocks();
    for (auto b : runwayBlocks)
    {
        for (auto rwInfo : b->GetRunwayInfos())
        {
            double deltaLon = rwInfo->lon - lon;
            double deltaLat = rwInfo->lat - lat;

            double distanceTmp = sqrt(deltaLon * deltaLon + deltaLat * deltaLat);

            if (distanceTmp < distanceTmpMin)
            {
                distanceTmpMin = distanceTmp;
                pNearestRunwayParam = rwInfo;
            }
        }
    }
    if (nullptr != pNearestRunwayParam)
    {	//nearest runway existed. 
        //Calculate the distance
        auto distance = CalculateDistanceM(lon, lat, pNearestRunwayParam->lon, pNearestRunwayParam->lat);
        double AngleToRunway = ToAngle(CalculateAngle(lon, lat, pNearestRunwayParam->lon, pNearestRunwayParam->lat));

        memcpy(oi.nearestICAO_id, pNearestRunwayParam->ICAO_id, sizeof(oi.nearestICAO_id));
        memcpy(oi.nearestRunway_id, pNearestRunwayParam->runway_id, sizeof(oi.nearestRunway_id));
        oi.nearestAptDist = (float)distance;
        oi.nearestRunwayDist = (float)distance;
        oi.nearestRunwayAlt = (float)FeetToMeter(pNearestRunwayParam->altitude);
        oi.nearestRunwayAngle = (float)AngleToRunway;
    }
    else
    {
        //no runway nearby
        const double defaultDistance = 999.9E3;
        const double defauttAltitude = 0.0;

        memset(oi.nearestICAO_id, 0, sizeof(oi.nearestICAO_id));
        memset(oi.nearestRunway_id, 0, sizeof(oi.nearestRunway_id));
        oi.nearestAptDist = (float)defaultDistance;
        oi.nearestRunwayDist = (float)defaultDistance;
        oi.nearestRunwayAlt = (float)defauttAltitude;
    }

    return oi;
}

NS_HMS_END


