#include "Coordinate.h"
#include <math.h>
#include "MathCommon.h"


#if 1
const double WGS_84_RADIUS_EQUATOR = 6378137.0;
//const double WGS_84_RADIUS_POLAR = 6356752.3142;
//const double WGS_84_ECCENTRICITY_SQUARED = 0.00669438000426080651558348727667;
//const double WGS_84_RADIUS_POLAR_SQUARE = 40408299984087.05552164;
//const double WGS_84_RADIUS_EQUATOR_SQUARE = 40680631590769.0;
//const double WGS_84_E_DASH_SQUARE = 0.00673949675658688230040992463583;

const double WGS_84_2_MERCATOR_LON_COEFFICIENT = 111319.49079327333333333333333333;
const double WGS_84_2_MERCATOR_LON_COEFFICIENT_RECIPROCAL = 8.9831528411952336632359078167953e-6;

#else
#define  WGS_84_RADIUS_EQUATOR   6378137.0
#define  WGS_84_RADIUS_POLAR   6356752.3142
#define  WGS_84_ECCENTRICITY_SQUARED   0.00669438000426080651558348727667
#define  WGS_84_RADIUS_POLAR_SQUARE   40408299984087.05552164
#define  WGS_84_RADIUS_EQUATOR_SQUARE   40680631590769.0
#define  WGS_84_E_DASH_SQUARE  0.00673949675658688230040992463583
#endif

#if 0//废弃此方法。现将地球假定成标准球体。
void Coordinate_LonLatAltToXYZ(double longitude, double latitude, double height, double* X, double* Y, double* Z)
{
    double sin_latitude = sin(latitude);
    double cos_latitude = cos(latitude);
    double N = WGS_84_RADIUS_EQUATOR / sqrt(1.0 - WGS_84_ECCENTRICITY_SQUARED*sin_latitude*sin_latitude);    
    *X = (N + height)*cos_latitude*cos(longitude);
    *Y = (N + height)*cos_latitude*sin(longitude);
    *Z = (N*(1 - WGS_84_ECCENTRICITY_SQUARED) + height)*sin_latitude;   
}
#endif

//************************************
// Method:    Coordinate_LonLatAltToXYZ
// FullName:  Coordinate_LonLatAltToXYZ
// Access:    public 
// Desc:      将地球假定成标准球体。
// Returns:   void
// Qualifier:
// Parameter: double longitude
// Parameter: double latitude
// Parameter: double height
// Parameter: double * X
// Parameter: double * Y
// Parameter: double * Z
// Author:    yan.jiang
//************************************
void Coordinate_LonLatAltToXYZ(double longitude, double latitude, double height, double* X, double* Y, double* Z)
{
    double sin_latitude = sin(latitude);
    double cos_latitude = cos(latitude);
    double R2 = WGS_84_RADIUS_EQUATOR + height;
    *X = R2 * cos_latitude * cos(longitude);
    *Y = R2 * cos_latitude * sin(longitude);
    *Z = R2 * sin_latitude;
}

void Coordinate_DegreeLonLatAltToXYZ(double longitude, double latitude, double height, double* X, double* Y, double* Z)
{
    longitude *= DegreesToRadians;
    latitude *= DegreesToRadians;
    Coordinate_LonLatAltToXYZ(longitude, latitude, height, X, Y, Z);
}

#if 0//废弃此方法。现将地球假定成标准球体。
void Coordinate_XYZToLonLatAlt(double X, double Y, double Z, double* longitude, double* latitude, double* height)
{
    double p = sqrt(X*X + Y*Y);
    double theta = atan2(Z*WGS_84_RADIUS_EQUATOR, (p*WGS_84_RADIUS_POLAR));
    double sin_theta = sin(theta);
    double cos_theta = cos(theta);
    double sin_latitude = 0.0;
    double N = 0.0;
    *latitude = atan((Z + WGS_84_E_DASH_SQUARE*WGS_84_RADIUS_POLAR*sin_theta*sin_theta*sin_theta) /
        (p - WGS_84_ECCENTRICITY_SQUARED*WGS_84_RADIUS_EQUATOR*cos_theta*cos_theta*cos_theta));
    *longitude = atan2(Y, X);
    sin_latitude = sin(*latitude);
    N = WGS_84_RADIUS_EQUATOR / sqrt(1.0 - WGS_84_ECCENTRICITY_SQUARED*sin_latitude*sin_latitude);
    *height = p / cos(*latitude) - N;
}
#endif

//************************************
// Method:    Coordinate_XYZToLonLatAlt
// FullName:  Coordinate_XYZToLonLatAlt
// Access:    public 
// Desc:      将地球假定成标准球体。
// Returns:   void
// Qualifier:
// Parameter: double X
// Parameter: double Y
// Parameter: double Z
// Parameter: double * longitude
// Parameter: double * latitude
// Parameter: double * height
// Author:    yan.jiang
//************************************
void Coordinate_XYZToLonLatAlt(double X, double Y, double Z, double* longitude, double* latitude, double* height)
{
    double p = sqrt(X*X + Y*Y);
    double sin_latitude = 0.0;
    *latitude = atan(Z / p);
    *longitude = atan2(Y, X);
    sin_latitude = sin(*latitude);
    *height = p / cos(*latitude) - WGS_84_RADIUS_EQUATOR;
}

void Coordinate_XYZToDegreeLonLatAlt(double X, double Y, double Z, double* longitude, double* latitude, double* height)
{
    Coordinate_XYZToLonLatAlt(X, Y, Z, longitude, latitude, height);
    *longitude = *longitude*RadiansToDegrees;
    *latitude = *latitude*RadiansToDegrees;
}

void Coordinate_Vec3fLonlatAltToXYZ(CVec3f lonlatalt, double* X, double* Y, double* Z)
{
    Coordinate_LonLatAltToXYZ(lonlatalt._v[0], lonlatalt._v[1], lonlatalt._v[2], X, Y, Z);
}

CVec3d Coordinate_Vec3fDegreeLonlatAltToVec3d(CVec3f lonlatalt)
{
    CVec3d xyz;
    Coordinate_DegreeLonLatAltToXYZ(lonlatalt._v[0], lonlatalt._v[1], lonlatalt._v[2], &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return xyz;
}

CVec3d Coordinate_Vec3dDegreeLonlatAltToVec3d(const CVec3d lonlatalt)
{
    CVec3d xyz;
    Coordinate_DegreeLonLatAltToXYZ(lonlatalt._v[0], lonlatalt._v[1], lonlatalt._v[2], &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return xyz;
}

CVec3d Coordinate_pVec3dDegreeLonlatAltToVec3d(const pCVec3d lonlatalt)
{
    CVec3d xyz;
    Coordinate_DegreeLonLatAltToXYZ(lonlatalt->_v[0], lonlatalt->_v[1], lonlatalt->_v[2], &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return xyz;
}

CVec3d Coordinate_Vec3fLonlatAltToVec3d(CVec3f lonlatalt)
{
    CVec3d xyz;
    Coordinate_Vec3fLonlatAltToXYZ(lonlatalt, &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return xyz;
}

void Coordinate_Vec3dLonlatAltToXYZ(CVec3d lonlatalt, double* X, double* Y, double* Z)
{
    Coordinate_LonLatAltToXYZ(lonlatalt._v[0], lonlatalt._v[1], lonlatalt._v[2], X, Y, Z);
}
CVec3d Coordinate_DegreeVec3dLonLatAltToVec3d(CVec3d lonlatalt)
{
    CVec3d xyz;
    lonlatalt._v[0] *= DegreesToRadians;
    lonlatalt._v[1] *= DegreesToRadians;
    Coordinate_Vec3dLonlatAltToXYZ(lonlatalt, &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return xyz;
}
CVec3d Coordinate_Vec3dLonlatAltToVec3d(CVec3d lonlatalt)
{
    CVec3d xyz;
    Coordinate_Vec3dLonlatAltToXYZ(lonlatalt, &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return xyz;
}


CVec3f Coordinate_Vec3dToLonlatAltVec3f(CVec3d lonlatalt)
{
    CVec3d xyz;
    Coordinate_XYZToLonLatAlt(lonlatalt._v[0], lonlatalt._v[1], lonlatalt._v[2], &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return Vec_CreatVec3f(xyz._v[0], xyz._v[1], xyz._v[2]);
}

CVec3f Coordinate_Vec3dToDegreeLonlatAltVec3f(CVec3d xyz)
{
    CVec3d lonlatalt;
    Coordinate_XYZToDegreeLonLatAlt(xyz._v[0], xyz._v[1], xyz._v[2], &(lonlatalt._v[0]), &(lonlatalt._v[1]), &(lonlatalt._v[2]));
    return Vec_CreatVec3f(lonlatalt._v[0], lonlatalt._v[1], lonlatalt._v[2]);
}

CVec3d Coordinate_Vec3dToLonlatAltVec3d(CVec3d lonlatalt)
{
    CVec3d xyz;
    Coordinate_XYZToLonLatAlt(lonlatalt._v[0], lonlatalt._v[1], lonlatalt._v[2], &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return xyz;
}

CVec3d Coordinate_Vec3dToDegreeLonlatAltVec3d(CVec3d xyz)
{
    CVec3d lonlatalt;
    Coordinate_XYZToDegreeLonLatAlt(xyz._v[0], xyz._v[1], xyz._v[2], &(lonlatalt._v[0]), &(lonlatalt._v[1]), &(lonlatalt._v[2]));
    return lonlatalt;
}

CVec3d Coordinate_LonLatAltToVec3d(double lon, double lat, double alt)
{
    CVec3d xyz;
    Coordinate_LonLatAltToXYZ(lon, lat, alt, &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return xyz;
}

CVec3d Coordinate_DegreeLonLatAltToVec3d(double lon, double lat, double alt)
{
    CVec3d xyz;
    Coordinate_LonLatAltToXYZ(lon*DegreesToRadians, lat*DegreesToRadians, alt, &(xyz._v[0]), &(xyz._v[1]), &(xyz._v[2]));
    return xyz;
}

void Coordinate_ComputeLocalToWorldTransformFromLonLatAlt(pCMatrix localToWorld, double longitude, double latitude, double height)
{
    CVec3d XYZ = Coordinate_LonLatAltToVec3d(longitude, latitude, height);
    CMatrix_SetMakeTranslateVec3d(localToWorld, &XYZ);
    Coordinate_ComputeGroundCoordinateSystem(latitude, longitude, localToWorld);
}

void Coordinate_ComputeGroundCoordinateSystem(double longitude, double latitude, pCMatrix localToWorld)
{
    /*计算北东天坐标*/
    CVec3d    up = Vec_CreatVec3d(cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
    CVec3d   east = Vec_CreatVec3d(-sin(longitude), cos(longitude), 0);
    CVec3d   north = Vec_CrossProductVec3d(&up, &east);

    (localToWorld->_mat[0][0]) = east._v[0];
    (localToWorld->_mat[0][1]) = east._v[1];
    (localToWorld->_mat[0][2]) = east._v[2];
    (localToWorld->_mat[1][0]) = north._v[0];
    (localToWorld->_mat[1][1]) = north._v[1];
    (localToWorld->_mat[1][2]) = north._v[2];
    (localToWorld->_mat[2][0]) = up._v[0];
    (localToWorld->_mat[2][1]) = up._v[1];
    (localToWorld->_mat[2][2]) = up._v[2];
}

CMatrix Coordinate_Vec3dLonlatToMatrix(CVec3d lonlat)
{
    CMatrix mat;
    CVec3d XYZ = Coordinate_LonLatAltToVec3d(lonlat._v[0], lonlat._v[1], lonlat._v[2]);
    CMatrix_SetMakeTranslateVec3d(&mat, &XYZ);
    Coordinate_ComputeGroundCoordinateSystem(lonlat._v[0], lonlat._v[1], &mat);
    return mat;
}
CMatrix Coordinate_LonlatToMatrix(double lon, double lat, double alt)
{
    CMatrix mat;
    CVec3d XYZ = Coordinate_LonLatAltToVec3d(lon, lat, alt);
    CMatrix_SetMakeTranslateVec3d(&mat, &XYZ);
    Coordinate_ComputeGroundCoordinateSystem(lon, lat, &mat);
    return mat;
}

CMatrix Coordinate_DegreeLonlatToMatrix(double lon, double lat, double alt)
{
    return Coordinate_LonlatToMatrix(lon*DegreesToRadians, lat*DegreesToRadians, alt);
}

CMatrix Coordinate_Vec3dDegreeLonlatToMatrix(CVec3d lonlat)
{
    lonlat._v[0] *= DegreesToRadians;
    lonlat._v[1] *= DegreesToRadians;
    return Coordinate_Vec3dLonlatToMatrix(lonlat);
}

CMatrix Coordinate_Vec3fLonlatToMatrix(CVec3f lonlat)
{
    CVec3d XYZ = Vec_Vec3fToVec3d(lonlat);
    return Coordinate_Vec3dLonlatToMatrix(XYZ);
}

CMatrix Coordinate_Vec3fDegreeLonlatToMatrix(CVec3f lonlat)
{
    CVec3d XYZ = Vec_Vec3fToVec3d(lonlat);
    XYZ._v[0] *= DegreesToRadians;
    XYZ._v[1] *= DegreesToRadians;
    return Coordinate_Vec3dLonlatToMatrix(XYZ);
}

CMatrix Coordinate_YawPitchRollDegreeToMatrix(double head, double pitch, double roll)
{
    return Coordinate_YawPitchRollToMatrix(head*DegreesToRadians, pitch*DegreesToRadians, roll*DegreesToRadians);
}

CMatrix Coordinate_YawPitchRollToMatrix(double head, double pitch, double roll)
{
    CMatrix headMat = CMatrix_RotateAVec3f(head, Vec_CreatVec3f(0, 0, -1));
    CVec3 pitchAxes = CMatrix_Vec3fMultMatrixToVec3f(Vec_CreatVec3f(1, 0, 0), &headMat);
    CMatrix pitchMat = CMatrix_RotateAVec3f(pitch, pitchAxes);
    CMatrix attMat = CMatrix_MatrixMultMatrix(&headMat, &pitchMat);
    CVec3 rollAxes = CMatrix_Vec3fMultMatrixToVec3f(Vec_CreatVec3f(0, 1, 0), &attMat);
    CMatrix rollMat = CMatrix_RotateAVec3f(roll, rollAxes);
    attMat = CMatrix_MatrixMultMatrix(&attMat, &rollMat);
    return attMat;
}

CMatrix Coordinate_YawPitchRollVec3dDegreeToMatrix(CVec3d ypr)
{
    return Coordinate_YawPitchRollToMatrix(ypr._v[0] * DegreesToRadians, ypr._v[1] * DegreesToRadians, ypr._v[2] * DegreesToRadians);
}
CMatrix Coordinate_YawPitchRollVec3dToMatrix(CVec3d ypr)
{
    return Coordinate_YawPitchRollToMatrix(ypr._v[0], ypr._v[1], ypr._v[2]);
}


CVec3d  Coordinate_DegreeLonLatToMercator(double lon, double lat, double alt)
{
    lon = lon * WGS_84_2_MERCATOR_LON_COEFFICIENT;
    lat = WGS_84_2_MERCATOR_LON_COEFFICIENT*log(tan((PI / 360.0)*(90.0 + lat))) / (PI / 180.0);
    return Vec_CreatVec3d(lon, lat, alt);
}
CVec3d  Coordinate_DegLonLatToMercator(CVec3d lonLat)
{
    lonLat._v[0] = lonLat._v[0] * WGS_84_2_MERCATOR_LON_COEFFICIENT;
    lonLat._v[1] = WGS_84_2_MERCATOR_LON_COEFFICIENT*log(tan((PI / 360.0)*(90.0 + lonLat._v[1]))) / (PI / 180.0);
    return lonLat;
}

CVec3d  Coordinate_DegLonLatXYZToMercator(double lon, double lat, double alt)
{
    lon = lon * WGS_84_2_MERCATOR_LON_COEFFICIENT;
    lat = WGS_84_2_MERCATOR_LON_COEFFICIENT*log(tan((PI / 360.0)*(90.0 + lat))) / (PI / 180.0);
    return Vec_CreatVec3d(lon, lat, alt);
}
CVec3d  Coordinate_ptrDegLonLatToMercator(CVec3d *lonLat)
{
    return Coordinate_DegLonLatXYZToMercator(lonLat->_v[0], lonLat->_v[1], lonLat->_v[2]);
}

CVec3d Coordinate_MercatorToDegLonLat(CVec3d mercator)
{
    mercator._v[0] = mercator._v[0] * WGS_84_2_MERCATOR_LON_COEFFICIENT_RECIPROCAL;
    mercator._v[1] = 180.0 / PI*(2.0*atan(exp(mercator._v[1] * WGS_84_2_MERCATOR_LON_COEFFICIENT_RECIPROCAL *  PI / 180.0)) - PI / 2);
    return mercator;
}
void Coordinate_Vec3dDegLonLatArcLon(CVec3d cameraLonlat, double arcLength, double *maxLon/*,double *maxLat*/)
{
    int intLon = cameraLonlat._v[0], intLat = cameraLonlat._v[1];
    double lonWidth = arcLength;
    double lonRadius = cos(DegreesToRadians*cameraLonlat._v[1])*WGS_84_RADIUS_EQUATOR;
    *maxLon = lonWidth / (PI*lonRadius / 180.0);
    *maxLon = lonWidth*180.0 / (PI*lonRadius);
    //	double maxLat =  lonWidth/(PI*lonRadius/180.0);
}
