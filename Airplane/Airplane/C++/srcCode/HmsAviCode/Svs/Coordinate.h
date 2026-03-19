#ifndef _COORDINATE__H_
#define  _COORDINATE__H_

#include "Matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec3.h"

#ifdef __cplusplus
}
#endif

extern const double WGS_84_RADIUS_EQUATOR;
extern const double WGS_84_2_MERCATOR_LON_COEFFICIENT;
extern const double WGS_84_2_MERCATOR_LON_COEFFICIENT_RECIPROCAL;

/*******************************************************************************
名称：Coordinate_LonLatAltToXYZ
功能：经纬度转换为地心坐标系
参数：经度，纬度，海拔
时间 : 16:11:28 2015
用户 : XiaFei
版本：1.0
注意：经度，纬度均为 弧度
返回值：
*******************************************************************************/
void Coordinate_LonLatAltToXYZ(double longitude, double latitude, double height, double* X, double* Y, double* Z);
/*******************************************************************************
名称：Coordinate_LonLatAltToXYZ
功能：经纬度转换为地心坐标系
参数：经度，纬度，海拔
时间 : 16:11:28 2015
用户 : XiaFei
版本：1.0
注意：经度，纬度均为 欧拉角
返回值：
*******************************************************************************/
void Coordinate_DegreeLonLatAltToXYZ(double longitude, double latitude, double height, double* X, double* Y, double* Z);

/*******************************************************************************
名称：Coordinate_XYZToLonLatAlt
功能：地心坐标系转经纬高 http://www.colorado.edu/geography/gcraft/notes/datum/gif/xyzllh.gif
参数：
时间 :16:13:25 2015
用户 :
版本：1.0
注意：结果为弧度
返回值：
*******************************************************************************/
void Coordinate_XYZToLonLatAlt(double X, double Y, double Z, double* longitude, double* latitude, double* height);
/*******************************************************************************
名称：Coordinate_XYZToDegreeLonLatAlt
功能：转换得到欧拉角
参数：
时间 :16:20:18 2015
用户 :iphone
版本：1.0
注意：
返回值：
*******************************************************************************/
void Coordinate_XYZToDegreeLonLatAlt(double X, double Y, double Z, double* longitude, double* latitude, double* height);

/*******************************************************************************
名称：Coordinate_Vec3fLonlatAltToXYZ
功能：Vec3的弧度经纬高转换为地心坐标系
参数：
时间 : 16:24:07 2015
用户 : iphone
版本：1.0
注意：
返回值：
*******************************************************************************/
void Coordinate_Vec3fLonlatAltToXYZ(CVec3f lonlatalt, double* X, double* Y, double* Z);

/*******************************************************************************
名称：Coordinate_Vec3fLonlatAltToVec3d
功能：Vec3的弧度经纬高转换为地心坐标系
参数：
时间 : 16:26:13 2015
用户 : iphone
版本: 1.0
注意：
返回值：
*******************************************************************************/
CVec3d Coordinate_Vec3fLonlatAltToVec3d(CVec3f lonlatalt);
/*******************************************************************************
名称：Coordinate_Vec3fDegreeLonlatAltToVec3d
功能：Vec3的欧拉角经纬高转换为地心坐标系
参数：
时间 : 16:25:16 2015
用户 : iphone
版本: 1.0
注意：
返回值：
*******************************************************************************/
CVec3d Coordinate_Vec3fDegreeLonlatAltToVec3d(CVec3f lonlatalt);
CVec3d Coordinate_Vec3dDegreeLonlatAltToVec3d(const CVec3d lonlatalt);
CVec3d Coordinate_pVec3dDegreeLonlatAltToVec3d(const pCVec3d lonlatalt);

/*******************************************************************************
名称：Coordinate_Vec3dLonlatAltToXYZ
功能：Vec3d的弧度经纬高转换为地心坐标系
参数：
时间 : 16:27:00 2015
用户 : iphone
版本: 1.0
注意：
返回值：
*******************************************************************************/
void Coordinate_Vec3dLonlatAltToXYZ(CVec3d lonlatalt, double* X, double* Y, double* Z);
/*******************************************************************************
名称：Coordinate_Vec3dLonlatAltToVec3d
功能：Vec3d的弧度经纬高转换为地心坐标系
参数：
时间 : 16:28:48 2015
用户 : iphone
版本 : 1.0
注意：
返回值：
*******************************************************************************/
CVec3d Coordinate_Vec3dLonlatAltToVec3d(CVec3d lonlatalt);
/*******************************************************************************
名称：Coordinate_Vec3dDegreeLonlatAltToVec3d
功能：Vec3d的欧拉角经纬高转换为地心坐标系
参数：
时间 : 16:27:32 2015
用户 : iphone
版本 : 1.0
注意：
返回值：
*******************************************************************************/
CVec3d Coordinate_DegreeVec3dLonLatAltToVec3d(CVec3d lonlatalt);

/*******************************************************************************
名称：Coordinate_Vec3dToLonlatAltVec3f
功能：位置转弧度经纬高
参数：
时间 : 16:43:32 2015
用户 : iphone
版本 : 1.0
注意：
返回值：
*******************************************************************************/
CVec3f Coordinate_Vec3dToLonlatAltVec3f(CVec3d lonlatalt);
/*******************************************************************************
名称： Coordinate_Vec3dToDegreeLonlatAltVec3f
功能：位置转欧拉经纬高
参数：
时间 : 16:43:59 2015
用户 : iphone
版本 : 1.0
注意：
返回值：
*******************************************************************************/
CVec3f Coordinate_Vec3dToDegreeLonlatAltVec3f(CVec3d lonlatalt);
/*******************************************************************************
名称：Coordinate_Vec3dToLonlatAltVec3d
功能：地形坐标系转经纬高 弧度
参数：
时间 : 16:47:21 2015
用户 : iphone
版本 : 1.0
注意：
返回值：
*******************************************************************************/
CVec3d Coordinate_Vec3dToLonlatAltVec3d(CVec3d lonlatalt);
/*******************************************************************************
名称：
功能：地形坐标系转经纬高 欧拉度
参数：
时间 : 16:47:42 2015
用户 : iphone
版本 : 1.0
注意：
返回值：
*******************************************************************************/
CVec3d Coordinate_Vec3dToDegreeLonlatAltVec3d(CVec3d lonlatalt);

CVec3d Coordinate_LonLatAltToVec3d(double lon, double lat, double alt);
CVec3d Coordinate_DegreeLonLatAltToVec3d(double lon, double lat, double alt);

void Coordinate_ComputeLocalToWorldTransformFromLonLatAlt(pCMatrix localToWorld, double longitude, double latitude, double height);
void Coordinate_ComputeGroundCoordinateSystem(double longitude, double latitude, pCMatrix localToWorld);
CMatrix Coordinate_Vec3dLonlatToMatrix(CVec3d lonlat);
CMatrix Coordinate_Vec3dDegreeLonlatToMatrix(CVec3d lonlat);
CMatrix Coordinate_Vec3fLonlatToMatrix(CVec3f lonlat);
CMatrix Coordinate_Vec3fDegreeLonlatToMatrix(CVec3f lonlat);

/*******************************************************************************
名称：Coordinate_YawPitchRollDegreeToMatrix
功能：欧拉角 姿态矩阵
参数：俯仰 滚转 航向 单位°
时间:  15:36:29 4/28/2015
用户 : xiafei
版本：1.0
注意：单位°
返回值：CMatrix
*******************************************************************************/
CMatrix Coordinate_YawPitchRollDegreeToMatrix(double head, double pitch, double roll);
/*******************************************************************************
名称：
功能：姿态矩阵 弧度
参数：无
时间:  15:36:49 4/28/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
CMatrix Coordinate_YawPitchRollToMatrix(double head, double pitch, double roll);

/*******************************************************************************
名称：Coordinate_YawPitchRollVec3dDegreeToMatrix
功能：姿态度转矩阵
参数：无
时间:  15:05:48 7/14/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
CMatrix Coordinate_YawPitchRollVec3dDegreeToMatrix(CVec3d ypr);
CMatrix Coordinate_YawPitchRollVec3dToMatrix(CVec3d ypr);

/*******************************************************************************
名称：Coordinate_DegreeLonlatToMatrix
功能：lonlat角度转矩阵
参数：无
时间:  15:04:26 7/14/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
CMatrix Coordinate_DegreeLonlatToMatrix(double lon, double lat, double alt);
/*******************************************************************************
名称：Coordinate_LonlatToMatrix
功能：弧度lonlat转矩阵
参数：无
时间:  15:04:08 7/14/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
CMatrix Coordinate_LonlatToMatrix(double lon, double lat, double alt);

/*******************************************************************************
名称：Coordinate_LonLatToMercator
功能：
参数：无
时间:  17:16:11 8/24/2015
用户 : xiafei
版本：1.0
注意：纬度范围-85~85
返回值：
*******************************************************************************/
CVec3d  Coordinate_DegLonLatToMercator(CVec3d lonLat);

CVec3d  Coordinate_DegreeLonLatToMercator(double lon, double lat, double alt);
/*******************************************************************************
名称：
功能：墨卡托转WGS84
参数：无
时间:  17:16:37 8/24/2015
用户 : xiafei
版本：1.0
注意：纬度范围-85~85
返回值：
*******************************************************************************/
CVec3d Coordinate_MercatorToDegLonLat(CVec3d mercator);

CVec3d  Coordinate_DegLonLatXYZToMercator(double lon, double lat, double alt);

CVec3d  Coordinate_ptrDegLonLatToMercator(CVec3d *lonLat);

void Coordinate_Vec3dDegLonLatArcLon(CVec3d cameraLonlat, double arcLength, double *maxLon/*,double *maxLat*/);

#endif
