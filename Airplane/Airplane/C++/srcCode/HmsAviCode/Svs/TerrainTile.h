#ifndef _TERRAIN_TILE__H_
#define _TERRAIN_TILE__H_

#include "Node.h"
#include "MatrixNode.h"
#include "Matrix.h"
#include "TerrainHead.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec3.h"

#ifdef __cplusplus
}
#endif

//typedef struct _CTerrainTile
class CTerrainTile
{
public:
    /*节点掩码*/
    unsigned long m_mask;
    /*节点地形经纬度*/
    CVec3d m_longlatalt;
    /*整形的经纬度(所在的top level 地块的经纬度)*/
    int m_lon;
    int m_lat;
    /*是否隐藏不显示*/
    int m_hasHideTer;
    /*该节点层次*/
    int m_layer;
    /*地形节点的mat节点*/
    pCNode  m_terrain;
    /*自身节点*/
    pCNode node; //本结构数据，属于该node
    unsigned int m_parentsId;
    unsigned int m_Lod;
    unsigned int m_UniqueId;
    unsigned int m_id;
    char m_IdString[17];
    char m_parentIdString[17];
    int intile;

    CTerrainTile(){
        m_mask = 0;
        m_longlatalt = Vec_CreatVec3d(0, 0, 0);
        m_lon = 0;
        m_lat = 0;
        m_hasHideTer = 0;
        m_layer = 0;
        m_terrain = NULL;
        node = NULL;
        m_parentsId = 0;
        m_Lod = 0;
        m_UniqueId = 0;
        m_id = 0;
        m_IdString[0] = '\0';
        m_parentIdString[0] = '\0';
        intile = 0;
    }
};// CTerrainTile, *pCTerrainTile;
typedef CTerrainTile* pCTerrainTile;

pCTerrainTile CTerrainTile_Create(void);

void CTerrainTile_SetMask(pCTerrainTile p, unsigned long mask);

int CTerrainTile_Free(pCTerrainTile p);

bool CTerrainTile_Destory(pCTerrainTile p);

CVec3d	CTerrainTile_GetLonLat(pCTerrainTile p);

void setLonlat(const CVec3d lonlat);

void CTerrainTile_SetLonLat(pCTerrainTile p, int s, int t);

//CVec3d CTerrainTile_GetAltLonLat(pCTerrainTile p);

unsigned char CTerrainTile_GetLayer(unsigned long mask);

unsigned char CTerrainTile_GetCount(unsigned long mask, unsigned char layer);

void CTerrainTile_SetLayer(unsigned long* mask, unsigned char layer);

void CTerrainTile_SetCount(unsigned long* mask, unsigned char count, unsigned char layer);

unsigned long CTerrainTile_GetMaskByLonLatAndLayer(pCTerrainTile p, const CVec3d ll, unsigned char layer);

void CTerrainTile_GetLonLatLayerByMask(pCTerrainTile p, unsigned long mask, CVec3d *lonlat, unsigned char* layer);


void CTerrainTile_SetTerrain(pCTerrainTile p, pCMatrixTransform matT);


int CTerrainTile_HasChildTer(pCTerrainTile p);

CVec3d CTerrainTile_GetCenter(pCTerrainTile p);

CVec3d CTerrainTile_GetCenter_Mecartor(pCTerrainTile p);

void CTerrainTile_RemoveTerrain(pCTerrainTile p);

void CTerrainTile_SetLodAndId(pCTerrainTile ptr, unsigned int lod, unsigned int id);

void CTerrainTile_GetIdString(pCTerrainTile ptr);

void CTerrainTile_SetParentIdString(pCTerrainTile ptr, char* str);


#endif