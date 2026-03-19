#include "TerrainChunk.h"
#include "Coordinate.h"



static pCTerrainChunk pchunk = 0;;
pCTerrainChunk CTerrainChunk_Create()
{
    pCTerrainChunk chunk = (pCTerrainChunk)malloc(sizeof(CTerrainChunk));
    if (!chunk)
    {
        LogWrite(LOG_LEVEL_FATAL, "??????? pCTerrainChunk chunk =  (pCTerrainChunk)malloc(sizeof(CTerrainChunk)) ==null ");
        //system("pause");//xyh
        return 0;
    }
    memset(chunk, 0, sizeof(CTerrainChunk));
    return chunk;
}

unsigned char  CTerrainChunk_Destroy(pCTerrainChunk chunk)
{
    if (chunk->m_terrain)
    {

    }
    free(chunk);
    chunk = 0;
    return 1;
}

void  CTerrainChunk_Update(CVec3d pos)
{
    if (!pchunk)
    {
        pchunk = CTerrainChunk_Create();
    }
    {
        int intLon = pos._v[0], intLat = pos._v[1];
        pos = Coordinate_Vec3dToDegreeLonlatAltVec3d(pos);
        intLon = pos._v[0], intLat = pos._v[1];
    }
}


void CTerrainChunk_SetParentIdString(pCTerrainChunk ptr, char* str)
{
    if (ptr && str)
    {
        unsigned int strl = strlen(str);
        unsigned int len = 16 < strl ? 16 : strl;
        memcpy(ptr->m_parentIdString, str, len);
    }
}
void CTerrainChunk_GetIdString(pCTerrainChunk ptr)
{
    char isNor = 0;

    unsigned int id = ptr->m_UniqueId;
    unsigned int i = 1;

    for (i = 1; i < 17; i++)
    {
        unsigned int mask = 3;
        mask = mask << ((16 - i) * 2);
        mask = mask&id;
        mask = mask >> ((16 - i) * 2);
        if (mask)
        {
            isNor = 1;
        }
        if (isNor)
        {
            sprintf(ptr->m_IdString, "%s%d", ptr->m_IdString, mask);
        }
    }
    if (!isNor)
    {
        sprintf(ptr->m_IdString, "%d", 0);
    }
    //	printf("\t%d,%d????%d???????%s??%d????%d,id: %s\n",(int)ptr->m_lon,(int)ptr->m_lat,ptr->m_Lod,ptr->m_parentIdString,ptr->m_id,ptr->m_UniqueId,ptr->m_IdString);
}
void CTerrainChunk_SetLodAndId(pCTerrainChunk ptr, unsigned int lod, unsigned int id)
{
    if (ptr)
    {
        unsigned int  newid = ptr->m_parentsId << 2;
        newid += id;
        ptr->m_UniqueId = newid;
        ptr->m_Lod = lod;
        ptr->m_id = id;

        CTerrainChunk_GetIdString(ptr);
    }
}
