#pragma once
#include "SmartAviDataDef.h"
#include "../FMS/HmsFmsDataDef.h"
#include "../FMS/HmsDuControlData.h"
#include "../Calculate/HmsAlphaFilter.h"
#include "HmsDataBusStuDef.h"

struct HmsAftData
{
    bool   bLatLonValid;			///whether or not latitude and longitude is valid: 0 :false 1:true.
    bool   bGpsTrackValid;          ///whether or not the device have he true track 0:not valid, 1:GPS  2:Calculate
    double latitude;				///latitude,  unit degree.
    double longitude;				///longitude, unit degree.
    float  altitudeFt;				///this is used altitude by gps or pressure alt, unit Ft

    bool   bHeightAboveTerrainValid;   ///whether or not height is valid
    float  heightAboveTerrainMeter;    ///hight from the ground(meter)

    bool   bValidAttitude;
    bool   bValidAirData;
    bool   bValidADSBTraffic;
    bool   bValidADSBWeather;

    int	   nTrueHeadingDataFrom;	//0:invalid  1:GPS or Device	2: By Calculate
    int	   nTrueTrackDataFrom;		//0:invalid  1:GPS or Device	2: By Calculate

    float roll;						///roll angle,unit degree [-180, 180].
    float pitch;					///pitch angle, unit degree [-90, 90].
    float magneticHeading;			///yaw(Magnetic heading),unit degree [0,  360].
    float trueHeading;				///true heading, unit degree [0, 360]
    float trueTrack;				///true track,unit degree [0,  360].
    float sideSliAngle;				///Slip-indicator,data in [-2,2].
    float yawRate;					///Yaw rate, unit degree/sec.

    float groundSpeed;			    ///ground speed, unit KNOT[(-3200 to 3200 KTS].
    short verticalSpeed;			///unit feet/min[-32,000 to 32,000 feet/min].
    float geoAltitudeFt;			/// the Geometric Altitude , unit ft
    float mslAltitudeFt;			/// the MSL altitude, unit ft
    float GLode;

    float simGroundSpeed;           ///speed relative to the ground unit KNOT
    float speedEast;                ///speed to the east (relative to the ground) unit m/s
    float speedNorth;               ///speed to the north (relative to the ground) unit m/s
    float speedSkyUp;               ///speed to the vertical upward (relative to the ground) unit m/s
    float simTrack;                 ///simulated track

    short pressureAltitudeFt;			///Altitude in feet at 29.92 inHg.Unsigned word where sea level is represented by 5000 ft[-5,000 to 60,000 feet].
    float IndicatedAirspeed;

    float version;					///iLevil version.
    int   batteryLevel;				///battery Level[0, 100].
    bool  bDeviceConnected;			//the device was connected
    bool  bDeviceIsCharging;		//the device is charging;
    char  cOrgCommunicationType;
    char  cCommunicationType;		//B: Bluetooth  W: wifi	
    char  cProtocalType;			//G: GDL90		M: mavlink

};

struct HmsAttitudeCorrection
{
    float roll;
    float pitch;
};

class CHmsDataBus
{
public:
    CHmsDataBus();
    ~CHmsDataBus();

    void Init();

    void UpdateDataBus(float delta);

    HmsAftData * GetData(){ return &m_data; }

    std::map<std::string, HmsADSBShortTraffic> * GetADSBData() { return &m_mapADSBShortTraffic; }

    float GetAltitudeMeter();

    float GetAltitudeFeet();

    float GetGeoAltitudeMeter();

    float GetGeoAltitudeFeet();

    float GetMslAltitudeMeter();

    float GetMslAltitudeFeet();

    float GetHeightMeter();

    float GetHeightFeet();

    void SetFmsData(FmsCalData data){ fmsCalData = data; }

    FmsCalData & GetFmsData() { return fmsCalData; }

    CHmsDuControlData * GetDuControlData() { return m_duControlData; }

    void SetDeviceConnect(bool bConnected);

    void SetDataCorrection(bool bUse);

    void SetHeight(float height);

    void SetCommunicationType(char cType);
    void SetProtocalType(char cType);

    void CalculateTrueHeading();

protected:
    void FillData();

private:
    HmsAftData					m_data;
    HmsAttitudeCorrection		m_dataCorrection;
    FmsCalData					fmsCalData;			//FMS CTRL
    CHmsDuControlData			*m_duControlData;	//DU Interface control data	
    bool						m_bUseDataCorrrection;

    CHmsAlphaFilter				m_filterSpeed;
    CHmsAlphaFilter				m_filterAlt;
    CHmsAlphaFilter				m_filterVerticalSpeed;
    CHmsAlphaFilter				m_fliterYaw;

    std::map<std::string, HmsADSBShortTraffic> m_mapADSBShortTraffic;
};

