#include "HmsNavSvsPositionFixPos.h"

CHmsNavSvsPositionFixPos::CHmsNavSvsPositionFixPos()
{
    m_type = NavSvsPositionMgrType::POSMGR_FIX;
}

CHmsNavSvsPositionFixPos::~CHmsNavSvsPositionFixPos()
{

}

CHmsNavSvsPositionFixPos* CHmsNavSvsPositionFixPos::GetInstance()
{
    static CHmsNavSvsPositionFixPos *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsPositionFixPos;
    }
    return s_pIns;
}

void CHmsNavSvsPositionFixPos::SetLonLatHeight(double lon, double lat, double height)
{
    if (fabs(lon) <= 180 && fabs(lat) <= 90)
    {
        m_lon = lon;
        m_lat = lat;
    }
    m_height = height;
}

void CHmsNavSvsPositionFixPos::SetYawPitchRoll(double yaw, double pitch, double roll)
{
    m_yaw = 0;
    m_pitch = 0;
    m_roll = 0;
}

void CHmsNavSvsPositionFixPos::SetSpeed(double speed)
{
    m_speed = speed;
}
