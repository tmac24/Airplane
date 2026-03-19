#include "HmsAirspaceData.h"
#include "../HmsNavMathHelper.h"
#include "Calculate/CalculateZs.h"

CHmsAirspaceData* CHmsAirspaceData::GetInstance()
{
    static CHmsAirspaceData *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsAirspaceData;
    }
    return s_pIns;
}

void CHmsAirspaceData::GetAirspaceData(double lon, double lat, double offset, std::vector<AirspaceGroupStu> &vec)
{
    for (auto &group : m_vecAirspaceGroup)
    {
        if (fabs(group.longitude - lon) < offset && fabs(group.latitude - lat) < offset)
        {
            vec.push_back(group);
        }
    }
}

CHmsAirspaceData::CHmsAirspaceData()
{
    InitData();
    InsertEarthPos();
}

CHmsAirspaceData::~CHmsAirspaceData()
{

}

void CHmsAirspaceData::InitData()
{
    std::vector<DB_AirspaceData> vec;
    vec.reserve(8 * 10000);
    CHms424Database::GetInstance()->GetRestrectionAirSpaceData(vec);

    m_vecAirspaceGroup.reserve(13000);
    AirspaceGroupStu *pGroup = nullptr;
    for (auto &dbStu : vec)
    {
        if (dbStu.sequence == 10)
        {
            AirspaceGroupStu group;
            m_vecAirspaceGroup.push_back(group);
            pGroup = &m_vecAirspaceGroup.back();

            if (dbStu.longitude > 360 || dbStu.latitude > 360)
            {
                pGroup->longitude = dbStu.arcLon;
                pGroup->latitude = dbStu.arcLat;
            }
            else
            {
                pGroup->longitude = dbStu.longitude;
                pGroup->latitude = dbStu.latitude;
            }
        }
        if (pGroup)
        {
            auto ConvertRestType = [](DB_AirspaceData &dbStu, AirspaceDataStu &stu)
            {
                if (dbStu.restType == "Alert") { stu.restType = AirspaceRestType::RestType_Alert; return; }
                if (dbStu.restType == "Caution") { stu.restType = AirspaceRestType::RestType_Caution; return; }
                if (dbStu.restType == "Danger") { stu.restType = AirspaceRestType::RestType_Danger; return; }
                if (dbStu.restType == "Military Operations Area") { stu.restType = AirspaceRestType::RestType_MilitaryOperationsArea; return; }
                if (dbStu.restType == "National Security Area") { stu.restType = AirspaceRestType::RestType_NationalSecurityArea; return; }
                if (dbStu.restType == "Prohibited") { stu.restType = AirspaceRestType::RestType_Prohibited; return; }
                if (dbStu.restType == "Restricted") { stu.restType = AirspaceRestType::RestType_Restricted; return; }
                if (dbStu.restType == "Training") { stu.restType = AirspaceRestType::RestType_Training; return; }
                if (dbStu.restType == "Warning") { stu.restType = AirspaceRestType::RestType_Warning; return; }
                if (dbStu.restType == "Unspecified or Unknown") { stu.restType = AirspaceRestType::RestType_UnspecifiedorUnknown; return; }
            };
            auto ConvertBoundary = [](DB_AirspaceData &dbStu, AirspaceDataStu &stu)
            {
                if (dbStu.boundary.find('C') != std::string::npos) { stu.boundary = AirspaceBoundaryType::Boundary_Circle; return; }
                if (dbStu.boundary.find('G') != std::string::npos) { stu.boundary = AirspaceBoundaryType::Boundary_GreatCircle; return; }
                if (dbStu.boundary.find('H') != std::string::npos) { stu.boundary = AirspaceBoundaryType::Boundary_RhumbLine; return; }
                if (dbStu.boundary.find('L') != std::string::npos) { stu.boundary = AirspaceBoundaryType::Boundary_CounterClockwiseARC; return; }
                if (dbStu.boundary.find('R') != std::string::npos) { stu.boundary = AirspaceBoundaryType::Boundary_ClockwiseARC; return; }
            };

            AirspaceDataStu stu;
 
            ConvertRestType(dbStu, stu);
            stu.sequence = dbStu.sequence;
            ConvertBoundary(dbStu, stu);

            stu.longitude = dbStu.longitude;
            stu.latitude = dbStu.latitude;
            stu.arcLon = dbStu.arcLon;
            stu.arcLat = dbStu.arcLat;
            stu.arcDist = dbStu.arcDist;
            stu.arcBearing = dbStu.arcBearing;

            pGroup->data.push_back(stu);
        }
    }
}

void CHmsAirspaceData::InsertEarthPos()
{
    for (auto &group : m_vecAirspaceGroup)
    {
		for (int i = 0; i < (int)group.data.size(); ++i)
        {
            AirspaceDataStu *pCurStu = nullptr;
            AirspaceDataStu *pNextStu = nullptr;
            pCurStu = &group.data[i];
			if (i < (int)group.data.size() - 1)
            {
                pNextStu = &group.data[i + 1];
            }
            else
            {
                pNextStu = &group.data[0];
            }
            switch (pCurStu->boundary)
            {
            case AirspaceBoundaryType::Boundary_Circle:
                InsertEarthPosCircle(pCurStu, pNextStu);
                break;
            case AirspaceBoundaryType::Boundary_GreatCircle:
                InsertEarthPosGreatCircle(pCurStu, pNextStu);
                break;
            case AirspaceBoundaryType::Boundary_RhumbLine:
                InsertEarthPosRhumbLine(pCurStu, pNextStu);
                break;
            case AirspaceBoundaryType::Boundary_CounterClockwiseARC:
                InsertEarthPosCounterClockwiseARC(pCurStu, pNextStu);
                break;
            case AirspaceBoundaryType::Boundary_ClockwiseARC:
                InsertEarthPosClockwiseARC(pCurStu, pNextStu);
                break;
            default:
                break;
            }

            auto size = pCurStu->vecInsertEarthPos.size();
            pCurStu->vecInsert2DPos.resize(size);
            pCurStu->vecInsertLonLatIsVisible.resize(size);
        }
    }
}

//顺时针画圆
static void InsertCircle(Vec2 point, Vec2 center, std::vector<Vec3> &vecRet, float arcDegree, int segCount)
{
    Vec3 earthPoint = CHmsNavMathHelper::LonLatToEarthPoint(point);
    Vec3 earthCenter = CHmsNavMathHelper::LonLatToEarthPoint(center);
    float step = SIM_Pi * (arcDegree / 180) / segCount;
    for (int i = 0; i <= segCount; ++i)
    {
        Quaternion q(earthCenter, step * i);
        vecRet.push_back(q*earthPoint);
    }
};

void CHmsAirspaceData::InsertEarthPosCircle(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!pCurStu)
		return;
#endif
    Vec2 center = Vec2(pCurStu->arcLon, pCurStu->arcLat);
    Vec2 point = CHmsNavMathHelper::NextPointLonLatByBearing(center, pCurStu->arcDist * KnotKMH, 0);
    InsertCircle(point, center, pCurStu->vecInsertEarthPos, 360, 30);
}

void CHmsAirspaceData::InsertEarthPosGreatCircle(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!pCurStu || !pNextStu)
		return;
#endif
    InsertEarthPosRhumbLine(pCurStu, pNextStu);
}

void CHmsAirspaceData::InsertEarthPosRhumbLine(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!pCurStu || !pNextStu)
		return;
#endif
    CHmsNavMathHelper::InsertEarthPos(Vec2(pCurStu->longitude, pCurStu->latitude)
        , Vec2(pNextStu->longitude, pNextStu->latitude), pCurStu->vecInsertEarthPos, 3);
}

void CHmsAirspaceData::InsertEarthPosCounterClockwiseARC(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!pCurStu || !pNextStu)
		return;
#endif
    InsertEarthPosClockwiseARC(pCurStu, pNextStu, true);
}

void CHmsAirspaceData::InsertEarthPosClockwiseARC(AirspaceDataStu *pCurStu, AirspaceDataStu *pNextStu, bool bCounter)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!pCurStu || !pNextStu)
		return;
#endif
    Vec2 center = Vec2(pCurStu->arcLon, pCurStu->arcLat);
    Vec2 point = Vec2(pCurStu->longitude, pCurStu->latitude);
    Vec2 dst = Vec2(pNextStu->longitude, pNextStu->latitude);

    Vec3 centerXYZ = CHmsNavMathHelper::LonLatToEarthPoint(center);
    Vec3 pointXYZ = CHmsNavMathHelper::LonLatToEarthPoint(point);
    Vec3 dstXYZ = CHmsNavMathHelper::LonLatToEarthPoint(dst);

    double centerAngle = Vec3::angle(pointXYZ, dstXYZ);
    double pointAngle = Vec3::angle(centerXYZ, dstXYZ);
    double dstAngle = Vec3::angle(centerXYZ, pointXYZ);

    double tmp1 = (cos(centerAngle) - cos(pointAngle)*cos(dstAngle)) / (sin(pointAngle)*sin(dstAngle));
    tmp1 = HMS_BETWEEN_VALUE(tmp1, -1, 1);
    double c = HmsMath_acos(tmp1);
    c = MATH_RAD_TO_DEG_DOUBLE(c);

    if (bCounter)
    {
        InsertCircle(point, center, pCurStu->vecInsertEarthPos, -c, 30);
    }
    else
    {
        InsertCircle(point, center, pCurStu->vecInsertEarthPos, c, 30);
    }

    auto lastLonLat = CHmsNavMathHelper::EarthPointToLonLat(pCurStu->vecInsertEarthPos.back());
    auto dis = CalculateDistanceKM(lastLonLat.x, lastLonLat.y, dst.x, dst.y);
    if (dis > 3)
    {
        pCurStu->vecInsertEarthPos.clear();
        c = 360 - c;
        if (bCounter)
        {
            InsertCircle(point, center, pCurStu->vecInsertEarthPos, -c, 30);
        }
        else
        {
            InsertCircle(point, center, pCurStu->vecInsertEarthPos, c, 30);
        }
    }
}
