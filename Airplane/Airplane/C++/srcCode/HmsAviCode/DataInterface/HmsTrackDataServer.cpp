#include "HmsTrackDataServer.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"
#include "Nav/HmsNavMathHelper.h"
#define MAX_RECODE_TRACK_SIZE   3600 * 10


void HmsTrackCell::UpdatePos3D()
{
    pos3D = CHmsNavMathHelper::LonLatToEarthPoint(Vec2(pos.dLongitude,pos.dLatitude));
}

int CHmsTrackData::UpdatePostion(const HmsTrackPosition & htp)
{
    HmsTrackCell htc;
    htc.pos = htp.pos;
    
    if (m_listTrackPos.size() > 0)
    {
        auto deltaKm = m_listTrackPos.front().pos.GetDistanceKm(htp.pos);
        //100米记录一个点
        if (deltaKm < 0.1)
        {
            return 0;
        }
        else if (deltaKm > 10)
        {
            m_listTrackPos.clear();
            return 2;
        }
    }

    htc.UpdatePos3D();
    m_listTrackPos.push_front(htc);
    m_bListDirty = true;
    if (m_listTrackPos.size() > MAX_RECODE_TRACK_SIZE)
    {
        m_listTrackPos.pop_back();
    }

    return 1;
}

CHmsTrackDataServer::CHmsTrackDataServer()
{

}

bool CHmsTrackDataServer::Update()
{
    bool bUpdate = false;

    if (m_timeUpdate.GetElapsed() >= 1)
    {
        auto data = CHmsGlobal::GetInstance()->GetDataBus()->GetData();
        if (data->bDeviceConnected && data->bLatLonValid)
        {
            HmsTrackPosition htp;
            htp.pos.SetData(data->latitude, data->longitude);
            htp.altFt = data->altitudeFt;
            int nRet = m_data.UpdatePostion(htp);
            if (nRet)
            {       
                if (nRet == 2)
                {
                    m_dataRecord.RecordNewFile();
                }
                m_dataRecord.OnDataRecv(&htp, sizeof(htp));
            }
        }
    }
    
    return bUpdate;
}

void CHmsTrackDataServer::SetRecordCmd(bool bRecord)
{
    m_dataRecord.SetRecordCmd(bRecord);
}

void CHmsTrackDataServer::FlushCurrentRecord()
{
    m_dataRecord.FlushRecordFile();
}

extern std::string g_stringRecordTrackPath;
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

std::vector<HmsTrackPosition> CHmsTrackDataServer::GetHistoryTrackPos(const std::string & strName)
{
    if (m_strHistoryName == strName)
    {
        return m_vHistoryTrack;
    }
    else
    {
        FlushCurrentRecord();

        m_strHistoryName = strName;
        m_vHistoryTrack.clear();
        std::string strPath = g_stringRecordTrackPath + strName;
        std::ifstream ifs(strPath, std::ifstream::in | std::ifstream::binary);
        if (ifs.is_open())
        {
            HmsTrackPosition htp;
            while (ifs.good())
            {
                ifs.read((char*)&htp, sizeof(htp));
                if (ifs.fail())
                {
                    break;
                }
                m_vHistoryTrack.push_back(htp);
            }
            ifs.close();
        }
        
    }
    return m_vHistoryTrack;
}

HmsAviPf::Vec3 CHmsTrackDataServer::Get3DPostion(const EarthCoord2D & ec)
{
    return CHmsNavMathHelper::LonLatToEarthPoint(Vec2(ec.dLongitude, ec.dLatitude));
}

