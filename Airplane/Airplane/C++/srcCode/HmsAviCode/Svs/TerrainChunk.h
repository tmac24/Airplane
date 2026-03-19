#ifndef _TERRAIN_CHUNK__H_
#define _TERRAIN_CHUNK__H_

#include "Node.h"
#include "GLHeader.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec3.h"

#ifdef __cplusplus
}
#endif

typedef struct _CTerrainChunk
{
    double m_lon, m_lat;
    double m_center_lon, m_center_lat;
    unsigned int m_parentsId;
    unsigned int m_Lod;
    unsigned int m_UniqueId;
    unsigned int m_id;
    char m_IdString[17];
    char m_parentIdString[17];
    pCNode  m_terrain;
    double m_rou;
    CVec3d m_center;
    unsigned char m_leaf;
    unsigned char m_isRoot;
}CTerrainChunk, *pCTerrainChunk;

unsigned char CTerrainChunk_Destroy(pCTerrainChunk ptr);
pCTerrainChunk CTerrainChunk_Create();
void  CTerrainChunk_Update(CVec3d pos);
void CTerrainChunk_SetParentIdString(pCTerrainChunk ptr, char* str);
void CTerrainChunk_GetIdString(pCTerrainChunk ptr);
void CTerrainChunk_SetLodAndId(pCTerrainChunk ptr, unsigned int lod, unsigned int id);


#endif