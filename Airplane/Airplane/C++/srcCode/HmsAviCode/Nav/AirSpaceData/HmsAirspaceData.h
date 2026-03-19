#pragma once
#include <vector>
#include <map>
#include "Database/Hms424Database.h"
#include "math/HmsMath.h"
#include "HmsStandardDef.h"

enum class AirspaceRestType
{
    RestType_Alert = 0,
    RestType_Caution,
    RestType_Danger,
    RestType_MilitaryOperationsArea,
    RestType_NationalSecurityArea,
    RestType_Prohibited,
    RestType_Restricted,
    RestType_Training,
    RestType_Warning,
    RestType_UnspecifiedorUnknown
};

enum class AirspaceBoundaryType
{
    Boundary_Circle = 0,
    Boundary_GreatCircle,
    Boundary_RhumbLine,
    Boundary_CounterClockwiseARC,
    Boundary_ClockwiseARC
};

struct AirspaceDataStu
{
    AirspaceRestType restType;
    int sequence;
    AirspaceBoundaryType boundary;
    double latitude;
    double longitude;
    double arcLat;
    double arcLon;
    double arcDist;
    double arcBearing;

    bool bEnableNode;//false时不参与插值，不显示
    std::vector<bool> vecInsertLonLatIsVisible;
    std::vector<HmsAviPf::Vec3> vecInsertEarthPos;
    std::vector<HmsAviPf::Vec2> vecInsert2DPos;

#if _NAV_SVS_LOGIC_JUDGE
	AirspaceDataStu()
	{
		restType = AirspaceRestType::RestType_Alert;
		boundary = AirspaceBoundaryType::Boundary_Circle;
		sequence = 0;
		latitude = 0.0;
		longitude = 0.0;
		arcLat = 0.0;
		arcLon = 0.0;
		arcDist = 0.0;
		arcBearing = 0.0;
		bEnableNode = true;
	}
#endif
};

struct AirspaceGroupStu
{
    double latitude;
    double longitude;
    std::vector<AirspaceDataStu> data;
#if _NAV_SVS_LOGIC_JUDGE
	AirspaceGroupStu()
	{
		latitude = 0.0;
		longitude = 0.0;
	}
#endif
};

class CHmsAirspaceData
{
public:
    static CHmsAirspaceData* GetInstance();

    void GetAirspaceData(double lon, double lat, double offset, std::vector<AirspaceGroupStu> &vec);

protected:
    CHmsAirspaceData();
    ~CHmsAirspaceData();
    void InitData();
    void InsertEarthPos();

    void InsertEarthPosCircle(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu);
    void InsertEarthPosGreatCircle(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu);
    void InsertEarthPosRhumbLine(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu);
    void InsertEarthPosCounterClockwiseARC(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu);
    void InsertEarthPosClockwiseARC(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu, bool bCounter = false);
private:
    std::vector<AirspaceGroupStu> m_vecAirspaceGroup;
};

