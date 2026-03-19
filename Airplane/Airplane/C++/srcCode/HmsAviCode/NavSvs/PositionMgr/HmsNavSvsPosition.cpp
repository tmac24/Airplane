#include "HmsNavSvsPosition.h"
#include "NavSvs/HmsNavSvsLoadDataThread.h"

CHmsNavSvsPositionBase::CHmsNavSvsPositionBase()
{
    m_lon = 103.9;
    m_lat = 30.5;
    m_height = 2000;
    m_yaw = 0;
    m_pitch = 0;
    m_roll = 0;
    m_speed = 0;
}

CHmsNavSvsPositionBase::~CHmsNavSvsPositionBase()
{
    m_type = NavSvsPositionMgrType::POSMGR_FIX;
}

void CHmsNavSvsPositionBase::Update(float delta)
{

}

int CHmsNavSvsPositionBase::GetAltAboveGround(float lon, float lat, float alt)
{
    if (fabs(lon) <= 180 && fabs(lat) <= 90)
    {
        Vec2 pos = Vec2(lon, lat);
        auto terrainAlt = CHmsGlobal::GetInstance()->GetTerrainServer()->GetAltitude(&pos) + 30;
        if (alt < terrainAlt)
        {
            return terrainAlt;
        }
        else
        {
            return alt;
        }
    }
    else
    {
        return alt;
    }
}
