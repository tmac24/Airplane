#include "HmsNavSvsPositionRoam.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "NavSvs/HmsNavSvsLoadDataThread.h"

CHmsNavSvsPositionRoam::CHmsNavSvsPositionRoam()
{
    m_type = NavSvsPositionMgrType::POSMGR_ROAM;

    m_pitchSpeed = 0;
    m_rollSpeed = 0;
    m_rightSpeed = 0;
    m_bRun = true;
}

CHmsNavSvsPositionRoam::~CHmsNavSvsPositionRoam()
{

}

CHmsNavSvsPositionRoam* CHmsNavSvsPositionRoam::GetInstance()
{
    static CHmsNavSvsPositionRoam *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsPositionRoam;
    }
    return s_pIns;
}

void CHmsNavSvsPositionRoam::Update(float delta)
{
    if (!m_bRun)
    {
        return;
    }
    if (m_rightSpeed != 0)
    {
        auto yaw = m_yaw;
        if (m_rightSpeed < 0)
        {
            yaw -= 90;
        }
        else
        {
            yaw += 90;
        }
        Vec2d lonlat = Vec2d(m_lon, m_lat);
        lonlat = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&lonlat, fabsf(m_rightSpeed) / 3600 * delta, yaw);
        SetLonLat(lonlat.x, lonlat.y);
    }
    if (m_speed != 0)
    {
        Vec2d lonlat = Vec2d(m_lon, m_lat);
        lonlat = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&lonlat, m_speed / 3600 * delta, m_yaw);
        SetLonLat(lonlat.x, lonlat.y);
    }
    
    m_height = GetAltAboveGround(m_lon, m_lat, m_height);
}

void CHmsNavSvsPositionRoam::SetLonLat(double lon, double lat)
{
    if (fabs(lon) <= 180 && fabs(lat) <= 90)
    {
        m_lon = lon;
        m_lat = lat;

        m_height = GetAltAboveGround(m_lon, m_lat, m_height);
    }
}

void CHmsNavSvsPositionRoam::SetHeight(double height)
{
    m_height = height;
}

void CHmsNavSvsPositionRoam::SetSpeed(double speed)
{
    m_speed = speed;
}

void CHmsNavSvsPositionRoam::SetPitchSpeed(float v)
{
    m_pitchSpeed = v;
}

void CHmsNavSvsPositionRoam::SetRollSpeed(float v)
{
    m_rollSpeed = v;
}

void CHmsNavSvsPositionRoam::SetRightSpeed(float v)
{
    m_rightSpeed = v;
}

void CHmsNavSvsPositionRoam::SetPitch(float v)
{
    m_pitch = v;

    m_pitch = SVS_BETWEEN_VALUE(m_pitch, -89, 89);
}

void CHmsNavSvsPositionRoam::SetYaw(float v)
{
    m_yaw = v;

    while (m_yaw < 0)
    {
        m_yaw += 360;
    }
    while (m_yaw > 360)
    {
        m_yaw -= 360;
    }
}

void CHmsNavSvsPositionRoam::Play()
{
    m_bRun = true;
}

void CHmsNavSvsPositionRoam::Pause()
{
    m_bRun = false;
}
