
#include <string.h>
#include "CoordinateSystemMgr.h"

#include "Coordinate.h"

static  pCCoordinateMgr s_coordMgr = 0;
pCCoordinateMgr CCoordinateMgr_Create()
{
    pCCoordinateMgr pcc = (pCCoordinateMgr)malloc(sizeof(CCoordinateMgr));
    if (pcc == NULL)
    {
        printf("pCCoordinateMgr pcc = (pCCoordinateMgr)malloc(sizeof(CCoordinateMgr))  failed\n");
        exit(1);
    }
    memset(pcc, 0, sizeof(CCoordinateMgr));
    return pcc;
}

pCCoordinateMgr GetCCoordinateMgr()
{
    if (!s_coordMgr)
    {
        s_coordMgr = CCoordinateMgr_Create();
    }
    return s_coordMgr;
}

void CCoordinateMgr_Free(pCCoordinateMgr p)
{
    if (p)
    {
        free(p);
        p = 0;
    }
}



void CCoordinateMgr_Update(CVec3d lonlat)
{
    pCCoordinateMgr pcc = 0;
    if (pcc = GetCCoordinateMgr())
    {
        /*WGS84*/
        {
            {
                int iLon = lonlat._v[0], iLat = lonlat._v[1];
                if ((pcc->m_iLon != iLon)
                    || (pcc->m_iLat != iLat))
                {
                    pcc->m_iLon = iLon;
                    pcc->m_iLat = iLat;
                    pcc->m_OriginLonlat = lonlat;
                    pcc->m_wgs84OriginVec3d = Coordinate_DegreeLonLatAltToVec3d(iLon + 0.5, iLat + 0.5, 0.0);
                }
            }
        }
        /*MERCATOR*/
        {
            {
                int iLon = lonlat._v[0], iLat = lonlat._v[1];
                // 				if ((pcc->m_iLon != iLon)  
                // 					|| (pcc->m_iLat != iLat))
                {
                    pcc->m_mercatorOriginVec3d = Coordinate_DegLonLatToMercator(Vec_CreatVec3d(iLon + 0.5, iLat + 0.5, 0.0));
                }
            }
    }
    }
}

CVec3d CCoordinateMgr_GetRelativePoint()
{
    pCCoordinateMgr pcc = 0;
    if (pcc = GetCCoordinateMgr())
    {
        return pcc->m_wgs84OriginVec3d;
    }
    return Vec_CreatVec3d(0, 0, 0);
}

CVec3d CCoordinateMgr_GetWGS84RelativePoint(CVec3d pos)
{
    pCCoordinateMgr pcc = 0;
    if (pcc = GetCCoordinateMgr())
    {
        pos = Vec_Vec3dSubVec3d(&pos, &pcc->m_wgs84OriginVec3d);
        return pos;
    }
    return pos;
}

CVec3d CCoordinateMgr_GetMercatorRelativePoint(CVec3d pos)
{
    pCCoordinateMgr pcc = 0;
    if (pcc = GetCCoordinateMgr())
    {
        pos = Vec_Vec3dSubVec3d(&pos, &pcc->m_mercatorOriginVec3d);
        return pos;
    }
    return pos;
}
CVec3d CCoordinateMgr_GetMercatorRelative()
{
    return GetCCoordinateMgr()->m_mercatorOriginVec3d;
}