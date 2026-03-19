#pragma once
#include <string>
#include <vector>
#include "base/HmsTypes.h"

using namespace std;

class CHmsAirportConstant
{
public:
    static HmsAviPf::Color3B s_airportWhite;
    static HmsAviPf::Color3B s_airportGray;
    static HmsAviPf::Color3B s_airportBlue;

    static HmsAviPf::Size s_buttonSize;
    static HmsAviPf::Size s_toolIconSize;
    static float s_listItemHeight;
    static float s_fontSize;
};


enum NavImageAreaType
{
    NAV_NONE = 0,//不可用
    NAV_TOP_VIEM = 5,//成比例和不成比例俯视图统一处理
    NAV_SIDE_INC_TO_RIGHT = 15//侧视图,X轴正向朝右
};

struct StarSidStu
{
    int id;
    std::string name;
    std::string icaoCode;
    bool isFavorite;
    std::string imagePath;
    bool bDrawplane;

    NavImageAreaType firstAreaType;
    std::string firstAreaInfo;
    NavImageAreaType secondAreaType;
    std::string secondAreaInfo;

    std::string runwayIdent;
    double runwayBearing;
    double runwayLon;
    double runwayLat;

    StarSidStu()
        :id(0)
        , name("")
        , icaoCode("")
        , isFavorite(false)
        , imagePath("")
        , bDrawplane(false)
        , firstAreaType(NAV_NONE)
        , firstAreaInfo("")
        , secondAreaType(NAV_NONE)
        , secondAreaInfo("")
        , runwayIdent("")
        , runwayBearing(0)
        , runwayLon(0)
        , runwayLat(0)
    {}
};

struct AirportInfoStu
{
    string icaoCode;
    string name;
    string lon;
    string lat;

    string elevation;
    string longestRunway;
    string magneticVar;
    string transitAlt;

    string atis;
    string clearance;
    string ground;
    string tower;
};

struct AirportInfoFrequenciesStu
{
    string type;
    string name;
    string describe;
    string freq;
};

struct AirportInfoRunwayStu
{
    string runwayIdent;
    string size;
    string bearing;
    string lonLat;
    string thresholdEle;
};

struct AirportInfoProceduresStu
{
    string type;
    string name;

    StarSidStu ssStu;
};