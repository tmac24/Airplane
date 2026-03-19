#pragma once

#include "math/HmsMath.h"

#include "mathd/Vec2d.h"
#include "mathd/Vec3d.h"
#include "mathd/Mat4d.h"
#include "mathd/Quaterniond.h"
#include <vector>
#include "HmsNavSvsCommDef.h"

using namespace HmsAviPf;

#define SVS_BETWEEN_VALUE(_value_, _min_, _max_) ((_value_) < (_min_) ? (_min_) : (_value_) < (_max_) ? (_value_) : (_max_))

class CHmsNavSvsMathHelper
{
public:
    static double MercatorPosYToLatDegree(double dPosY);
    static double MercatorPosYToLatRadian(double dPosY);

    static Vec2 GetTileXYByLonLat(int nLayer, Vec2d lonLat);
    static Vec2d GetTileLonLatByXY(int nLayer, Vec2 xy);

    static void XYZAxisToMat4d(Mat4d *pDstRotate, Vec3d *xaxis, Vec3d *yaxis, Vec3d *zaxis);
    static void XYZAxisAndPosToMat4d(Mat4d *pDstRotate, Vec3d *xaxis, Vec3d *yaxis, Vec3d *zaxis, Vec3d *pos);
    static void GetCurUVN(const Vec2d *lonLat, Vec3d *side, Vec3d *back, Vec3d *up);
    static void YawPitchRollToMat(double yaw, double pitch, double roll, Mat4d *pDstMat4d);
    static void GetLonLatRotate(const Vec2d *pLonLat, Mat4d *pDstRotate);
    static void CaculateAttitude(const CalAttitudeStu *pAttitudeStu, Mat4d *pDstMat4d,
        Vec3d *pXAxis, Vec3d *pYAxis, Vec3d *pZAxis, Vec3d *pOriginPosd);

    static Vec2 EarthPointToLonLat(const Vec3 *pos);
    static Vec3 LonLatToEarthPoint(const Vec2 *lonLat);
    static Vec2 NextPointLonLatByBearing(const Vec2 *curPos, float dis, float bearing);
    static void InsertLonLat(const Vec2 *lonLat1, const Vec2 *lonLat2, std::vector<Vec2> &vecLonLatSerials, float dis);
    static void InsertEarthPos(const Vec2 *lonLat1, const Vec2 *lonLat2, std::vector<Vec3> &vecEarthSerials, float dis);

    static Vec2d EarthPointToLonLat_Double(const Vec3d *pos);
    static Vec3d LonLatToEarthPoint_Double(const Vec2d *lonLat);
    static Vec2d NextPointLonLatByBearing_Double(const Vec2d *curPos, double dis, double bearing);
    static void InsertLonLat_Double(const Vec2d *lonLat1, const Vec2d *lonLat2, std::vector<Vec2d> &vecLonLatSerials, double dis);
    static void InsertEarthPos_Double(const Vec2d *lonLat1, const Vec2d *lonLat2, std::vector<Vec3d> &vecEarthSerials, double dis);
    static void GetMatFromMat4d(Mat4 *pDstMat, const Mat4d *pMat);
    static Vec3d LonLatAltToXYZ_Double(const Vec2d *LngLat, double height);

    static double MeterToWorldUnit(double meter);
    static double WorldUnitToMeter(double value);
};

