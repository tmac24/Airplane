#ifndef __COORDINATE_SYSTEM_MGR_H_
#define __COORDINATE_SYSTEM_MGR_H_

#include "GLHeader.h" 
#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec3.h"

#ifdef __cplusplus
}
#endif

typedef struct _CCoordinateMgr
{ 
	/*原点的经纬度  欧拉度 米制海拔*/
	CVec3d m_OriginLonlat;
	/*相对地心坐标系的原点的XYZ*/
	CVec3d m_wgs84OriginVec3d;

	CVec3d m_mercatorOriginVec3d;
	/*为了方便计算 做一个int的经纬度  小数点后一位 121.112236 =  1215*/
	int m_iLon,m_iLat ;
}CCoordinateMgr, *pCCoordinateMgr;


static pCCoordinateMgr CCoordinateMgr_Create();

pCCoordinateMgr GetCCoordinateMgr();
 
void CCoordinateMgr_Free(pCCoordinateMgr pnode);


CVec3d CCoordinateMgr_GetRelativePoint();

CVec3d CCoordinateMgr_GetWGS84RelativePoint(CVec3d pos);

CVec3d CCoordinateMgr_GetMercatorRelativePoint(CVec3d pos);

CVec3d CCoordinateMgr_GetMercatorRelative();
/*******************************************************************************
名称：CCoordinateMgr_Update
功能：根据视点的经纬度计算当前坐标相对原点的经纬度
参数：无
时间:  15:22:48 7/16/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
void CCoordinateMgr_Update(CVec3d lonlat);

#endif