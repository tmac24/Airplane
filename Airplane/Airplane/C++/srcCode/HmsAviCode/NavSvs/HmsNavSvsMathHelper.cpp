#include "HmsNavSvsMathHelper.h"
#include "Calculate/CalculateZs.h"
#include "math/HmsMathBase.h"

#define ZS_E 2.718281828459	
#define ZS_PI 3.14159265358979323846
//#define SIM_Pi 3.14159265358979323846

double CHmsNavSvsMathHelper::MercatorPosYToLatDegree(double dPosY)
{
    double z = pow(ZS_E, dPosY * 2);
    double siny = (z - 1) / (z + 1);
    return HmsMath_asin(siny) * 180 / ZS_PI;
}

double CHmsNavSvsMathHelper::MercatorPosYToLatRadian(double dPosY)
{
    double z = pow(ZS_E, dPosY * 2);
    double siny = (z - 1) / (z + 1);
    return HmsMath_asin(siny);
}

Vec2 CHmsNavSvsMathHelper::GetTileXYByLonLat(int nLayer, Vec2d lonLat)
{
    Vec2 ret;
    int nLayerCnt = 1 << nLayer;
    int nLayerHalfCnt = 1 << (nLayer - 1);

    ret.x = (int)((lonLat.x + 180) * nLayerHalfCnt / 180.0);

    double tmpy = log(tan((90 + lonLat.y)*SIM_Pi / 360)) / (SIM_Pi / 180);
    tmpy = (180.0 - tmpy) / (180.0 / nLayerHalfCnt);
    ret.y = (int)tmpy;

    return ret;
}

Vec2d CHmsNavSvsMathHelper::GetTileLonLatByXY(int nLayer, Vec2 xy)
{
    Vec2d ret;
    int nLayerCnt = 1 << nLayer;
    int nLayerHalfCnt = 1 << (nLayer - 1);

    double xStart = (xy.x - nLayerHalfCnt)*180.0 / nLayerHalfCnt;
    ret.x = xStart;

    double yStart = (nLayerHalfCnt - xy.y)*SIM_Pi / nLayerHalfCnt;
    ret.y = MATH_RAD_TO_DEG(MercatorPosYToLatRadian(yStart));
    return ret;
}

void CHmsNavSvsMathHelper::XYZAxisToMat4d(Mat4d *pDstRotate, Vec3d *xaxis, Vec3d *yaxis, Vec3d *zaxis)
{
    pDstRotate->m[0] = xaxis->x;
    pDstRotate->m[1] = xaxis->y;
    pDstRotate->m[2] = xaxis->z;
    pDstRotate->m[3] = 0.0;

    pDstRotate->m[4] = yaxis->x;
    pDstRotate->m[5] = yaxis->y;
    pDstRotate->m[6] = yaxis->z;
    pDstRotate->m[7] = 0.0;

    pDstRotate->m[8] = zaxis->x;
    pDstRotate->m[9] = zaxis->y;
    pDstRotate->m[10] = zaxis->z;
    pDstRotate->m[11] = 0.0;
}

void CHmsNavSvsMathHelper::XYZAxisAndPosToMat4d(Mat4d *pDstRotate, Vec3d *xaxis, Vec3d *yaxis, Vec3d *zaxis, Vec3d *pos)
{
    XYZAxisToMat4d(pDstRotate, xaxis, yaxis, zaxis);

    pDstRotate->m[12] = pos->x;
    pDstRotate->m[13] = pos->y;
    pDstRotate->m[14] = pos->z;
    pDstRotate->m[15] = 1.0;
}

void CHmsNavSvsMathHelper::GetCurUVN(const Vec2d *lonLat, Vec3d *side, Vec3d *back, Vec3d *up)
{
    Vec2d nextLonLat;
    nextLonLat.x = lonLat->x;
    nextLonLat.y = lonLat->y - 1;

    Vec3d nextXYZ = LonLatToEarthPoint_Double(&nextLonLat);
    Vec3d curXYZ = LonLatToEarthPoint_Double(lonLat);
    Vec3d::cross(curXYZ, nextXYZ, side);
    side->normalize();

    *up = curXYZ.getNormalized();

    Vec3d::cross(*side, *up, back);
    back->normalize();
};

void CHmsNavSvsMathHelper::YawPitchRollToMat(double yaw, double pitch, double roll, Mat4d *pDstMat4d)
{
    Vec3d xaxis = { 1, 0, 0 };
    Vec3d yaxis = { 0, 1, 0 };
    Vec3d zaxis = { 0, 0, 1 };

    Mat4d yawMat;
    Mat4d::createRotation(yaxis, MATH_DEG_TO_RAD_DOUBLE(yaw), &yawMat);

    Vec3d pitchAxes;
    yawMat.transformVector(xaxis, &pitchAxes);
    Mat4d pitchMat;
    Mat4d::createRotation(pitchAxes, MATH_DEG_TO_RAD_DOUBLE(pitch), &pitchMat);

    Mat4d pitchYawMat;
    pitchYawMat = pitchMat * yawMat;

    Vec3d rollAxes;
    pitchYawMat.transformVector(zaxis, &rollAxes);
    Mat4d rollMat;
    Mat4d::createRotation(rollAxes, MATH_DEG_TO_RAD_DOUBLE(roll), &rollMat);

    Mat4d attMat;
    attMat = rollMat * pitchYawMat;

    *pDstMat4d = attMat;
}

void CHmsNavSvsMathHelper::GetLonLatRotate(const Vec2d *pLonLat, Mat4d *pDstRotate)
{
    Vec3d xaxis, yaxis, zaxis;
    GetCurUVN(pLonLat, &xaxis, &zaxis, &yaxis);

    XYZAxisToMat4d(pDstRotate, &xaxis, &yaxis, &zaxis);
}

void CHmsNavSvsMathHelper::CaculateAttitude(const CalAttitudeStu *pAttitudeStu, Mat4d *pDstMat4d,
    Vec3d *pXAxis, Vec3d *pYAxis, Vec3d *pZAxis, Vec3d *pOriginPosd)
{
    double planeHeight = pAttitudeStu->Height + 3;
    Vec2d planeLonLat = pAttitudeStu->LngLat;
    double yaw = pAttitudeStu->yaw;
    double pitch = pAttitudeStu->pitch;
    double roll = pAttitudeStu->roll;

    Mat4d attitudeMat;
    CHmsNavSvsMathHelper::YawPitchRollToMat(yaw, pitch, roll, &attitudeMat);

    Mat4d rotateMat;
    CHmsNavSvsMathHelper::GetLonLatRotate(&planeLonLat, &rotateMat);

    Vec3d xaxis = { 1, 0, 0 };
    Vec3d yaxis = { 0, 1, 0 };
    Vec3d zaxis = { 0, 0, 1 };

    Mat4d finalAttMat;
    finalAttMat = rotateMat * attitudeMat;

    finalAttMat.transformVector(xaxis, &xaxis);
    finalAttMat.transformVector(yaxis, &yaxis);
    finalAttMat.transformVector(zaxis, &zaxis);

    Vec3d originPosd = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&planeLonLat, planeHeight);
    originPosd = originPosd;

    CHmsNavSvsMathHelper::XYZAxisAndPosToMat4d(pDstMat4d, &xaxis, &yaxis, &zaxis, &originPosd);

    if (pXAxis)
    {
        *pXAxis = xaxis;
    }
    if (pYAxis)
    {
        *pYAxis = yaxis;
    }
    if (pZAxis)
    {
        *pZAxis = zaxis;
    }
    if (pOriginPosd)
    {
        *pOriginPosd = originPosd;
    }
}

Vec2 CHmsNavSvsMathHelper::EarthPointToLonLat(const Vec3 *pos)
{
    double lat = HmsMath_asin(0 - pos->y);
    float lon = atan2f(pos->x, pos->z);
    Vec2 v;
    v.x = MATH_RAD_TO_DEG(lon);
    v.y = MATH_RAD_TO_DEG(lat);

    return v;
}

Vec3 CHmsNavSvsMathHelper::LonLatToEarthPoint(const Vec2 *lonLat)
{
    Vec3 ret;
    float lon = MATH_DEG_TO_RAD(lonLat->x);
    float lat = MATH_DEG_TO_RAD(lonLat->y);

    ret.x = sinf(lon) * cos(lat);
    ret.y = -sin(lat);
    ret.z = cos(lon) * cos(lat);

    return ret;
}

Vec2 CHmsNavSvsMathHelper::NextPointLonLatByBearing(const Vec2 *curPos, float dis, float bearing)
{
    const float earthR = (float)EarthRadiusM / 1000.0f; //km
    float cRadian = dis / earthR;
    float tempRadian = MATH_DEG_TO_RAD(90 - curPos->y);
    float bearingRadian = MATH_DEG_TO_RAD(bearing);
    float aRadian = HmsMath_acos(cos(tempRadian) * cos(cRadian) + sin(tempRadian) * sin(cRadian) * cos(bearingRadian));

    float c = HmsMath_asin(sin(cRadian) * sin(bearingRadian) / sin(aRadian));

    Vec2 ret;
    ret.x = curPos->x + MATH_RAD_TO_DEG(c);
    if (ret.x > 180) ret.x -= 360;
    ret.y = 90 - MATH_RAD_TO_DEG(aRadian);
    return ret;
}

void CHmsNavSvsMathHelper::InsertLonLat(const Vec2 *lonLat1, const Vec2 *lonLat2, std::vector<Vec2> &vecLonLatSerials, float dis)
{
    std::vector<Vec3> vecEarth;
    CHmsNavSvsMathHelper::InsertEarthPos(lonLat1, lonLat2, vecEarth, dis);
    for (int i = 0; i < (int)vecEarth.size(); ++i)
    {
        Vec2 v2 = CHmsNavSvsMathHelper::EarthPointToLonLat(&vecEarth[i]);
        vecLonLatSerials.push_back(v2);
    }
}

void CHmsNavSvsMathHelper::InsertEarthPos(const Vec2 *lonLat1, const Vec2 *lonLat2, std::vector<Vec3> &vecEarthSerials, float dis)
{
    const float earthR = (float)EarthRadiusM / 1000.0f; //km
    float bR = MATH_DEG_TO_RAD(90 - lonLat1->y);
    float aR = MATH_DEG_TO_RAD(90 - lonLat2->y);
    float cR = HmsMath_acos(cos(bR) * cos(aR) + sin(bR) * sin(aR) * cos(MATH_DEG_TO_RAD(lonLat2->x - lonLat1->x)));

    int count = earthR * cR / dis;

    Vec3 pos1 = CHmsNavSvsMathHelper::LonLatToEarthPoint(lonLat1);
    Vec3 pos2 = CHmsNavSvsMathHelper::LonLatToEarthPoint(lonLat2);

    Vec3 axis;
    Vec3::cross(pos1, pos2, &axis);
    axis.normalize();

    vecEarthSerials.push_back(pos1);
    for (int i = 1; i < count; ++i)
    {
        Quaternion q = Quaternion(axis, cR / count*i);
        Vec3 pos3 = q * pos1;
        vecEarthSerials.push_back(pos3);
    }
    vecEarthSerials.push_back(pos2);
}

Vec2d CHmsNavSvsMathHelper::EarthPointToLonLat_Double(const Vec3d *pos)
{
    double lat = HmsMath_asin(0 - pos->y);
    double lon = atan2(pos->x, pos->z);
    Vec2d v;
    v.x = MATH_RAD_TO_DEG_DOUBLE(lon);
    v.y = MATH_RAD_TO_DEG_DOUBLE(lat);

    return v;
}

Vec3d CHmsNavSvsMathHelper::LonLatToEarthPoint_Double(const Vec2d *lonLat)
{
    Vec3d ret;
    double lon = MATH_DEG_TO_RAD_DOUBLE(lonLat->x);
    double lat = MATH_DEG_TO_RAD_DOUBLE(lonLat->y);

    ret.x = sin(lon) * cos(lat);
    ret.y = -sin(lat);
    ret.z = cos(lon) * cos(lat);

    return ret;
}

Vec2d CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(const Vec2d *curPos, double dis, double bearing)
{
    const double earthR = EarthRadiusM / 1000.0; //km
    double cRadian = dis / earthR;
    double tempRadian = MATH_DEG_TO_RAD_DOUBLE(90 - curPos->y);
    double bearingRadian = MATH_DEG_TO_RAD_DOUBLE(bearing);
    double aRadian = HmsMath_acos(cos(tempRadian) * cos(cRadian) + sin(tempRadian) * sin(cRadian) * cos(bearingRadian));

    double c = HmsMath_asin(sin(cRadian) * sin(bearingRadian) / sin(aRadian));

    Vec2d ret;
    ret.x = curPos->x + MATH_RAD_TO_DEG_DOUBLE(c);
    if (ret.x > 180)
    {
        ret.x -= 360;
    }
    ret.y = 90 - MATH_RAD_TO_DEG_DOUBLE(aRadian);
    return ret;
}

void CHmsNavSvsMathHelper::InsertLonLat_Double(const Vec2d *lonLat1, const Vec2d *lonLat2, std::vector<Vec2d> &vecLonLatSerials, double dis)
{
    std::vector<Vec3d> vecEarth;
    InsertEarthPos_Double(lonLat1, lonLat2, vecEarth, dis);
    for (int i = 0; i < (int)vecEarth.size(); ++i)
    {
        Vec2d v2 = EarthPointToLonLat_Double(&vecEarth[i]);
        vecLonLatSerials.push_back(v2);
    }
}

void CHmsNavSvsMathHelper::InsertEarthPos_Double(const Vec2d *lonLat1, const Vec2d *lonLat2, std::vector<Vec3d> &vecEarthSerials, double dis)
{
    const double earthR = EarthRadiusM / 1000.0; //km
    double bR = MATH_DEG_TO_RAD_DOUBLE(90 - lonLat1->y);
    double aR = MATH_DEG_TO_RAD_DOUBLE(90 - lonLat2->y);
    double cR = HmsMath_acos(cos(bR) * cos(aR) + sin(bR) * sin(aR) * cos(MATH_DEG_TO_RAD_DOUBLE(lonLat2->x - lonLat1->x)));

    int count = earthR * cR / dis;

    Vec3d pos1 = LonLatToEarthPoint_Double(lonLat1);
    Vec3d pos2 = LonLatToEarthPoint_Double(lonLat2);

    Vec3d axis;
    Vec3d::cross(pos1, pos2, &axis);
    axis.normalize();

    vecEarthSerials.push_back(pos1);
    for (int i = 1; i < count; ++i)
    {
        Quaterniond q = Quaterniond(axis, cR / count*i);
        Vec3d pos3 = q * pos1;
        vecEarthSerials.push_back(pos3);
    }
    vecEarthSerials.push_back(pos2);
}

void CHmsNavSvsMathHelper::GetMatFromMat4d(Mat4 *pDstMat, const Mat4d *pMat)
{
    for (int i = 0; i < 16; ++i)
    {
        pDstMat->m[i] = pMat->m[i];
    }
}

Vec3d CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(const Vec2d *LngLat, double height)
{
    Vec3d ret = CHmsNavSvsMathHelper::LonLatToEarthPoint_Double(LngLat);
    ret = ret * (height / EarthRadiusM + 1.0);
    return ret;
}

double CHmsNavSvsMathHelper::MeterToWorldUnit(double meter)
{
    return meter * (1.0 / EarthRadiusM);
}

double CHmsNavSvsMathHelper::WorldUnitToMeter(double value)
{
    return value * EarthRadiusM;
}


