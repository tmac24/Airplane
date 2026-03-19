#ifndef _3DTERRAIN_THREAD__H_
#define _3DTERRAIN_THREAD__H_

#include "TerrainCommon.h"

static pCTerrainThread C3DTerrainManager_Create();

//void C3DTerrainManager_FillRenderBuffer();

//void C3DTerrainManager_RenderFilledBuffer();

pCTerrainThread C3DTerrainManager_GetTerrainMgr();

void C3DTerrainManager_SetLonLatAlt(const CVec3d  lla);

void C3DTerrainManager_UpdateTerrain_multiThread();
//void C3DTerrainManager_UpdateTerrain();

bool C3DTerrainManager_FindChildTer(pCTerrainThread pTerrainMgr, pCTerrainTile curTerrainTile, unsigned char layer);

void C3DTerrainManager_updateMount();

pCMatrixTransform C3DTerrainManager_CreateTile(pCTerrainTile pTerTile, double lon, double lat, unsigned int count, double titleSpan_degree, unsigned char layer);

pCDrawable CTerrainManager_CreateBorder2(const pCTerrainTile tile, const double sLon, const double eLon, const double sLat, const double eLat,
    const unsigned int w, const unsigned int h, const double unit, const CVec3d centerPos);

//added by @wcc 
void CTerrainManager_RegistRenderList();

//void CTerrainManager_DrawTerrain();



#endif
