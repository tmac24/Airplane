#pragma once

#include "NavSvs/HmsNavSvsCommDef.h"

class CHmsNavSvsPositionBase
{
public:
    CHmsNavSvsPositionBase();
    virtual ~CHmsNavSvsPositionBase();

    virtual void Update(float delta);
    virtual int GetAltAboveGround(float lon, float lat, float alt);
    inline NavSvsPositionMgrType GetPositionMgrType(){ return m_type; };
    inline double GetLon(){ return m_lon; };
    inline double GetLat(){ return m_lat; };
    inline double GetHeight(){ return m_height; };
    inline double GetYaw(){ return m_yaw; };
    inline double GetPitch(){ return m_pitch; };
    inline double GetRoll(){ return m_roll; };
    inline double GetSpeed(){ return m_speed; };

protected:
    NavSvsPositionMgrType m_type;
    double m_lon;
    double m_lat;
    double m_height;//米
    double m_yaw;
    double m_pitch;
    double m_roll;
    double m_speed;
};


