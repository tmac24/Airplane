
#include <HmsGE/math/HmsMath.h>
#include "TerrainTile.h"
#include "Drawable.h"
#include "Node.h"
#include "AltManager.h"
#include "Coordinate.h"


pCTerrainTile CTerrainTile_Create(void)
{
    //pCTerrainTile p = (pCTerrainTile)malloc(sizeof(CTerrainTile));
    //if (p == NULL)
    //{
    //    printf("pCTerrainTile p = (pCTerrainTile)malloc(sizeof(CTerrainTile)) failed\n");
    //    exit(1);
    //}
    //memset(p, 0, sizeof(CTerrainTile));
    auto p = new CTerrainTile;
    return p;
}

bool CTerrainTile_Destory(pCTerrainTile p)
{
    if (p)
    {
        /*typedef struct _CTerrainTile
        {
        unsigned long m_mask;
        CVec3d m_longlatalt;
        int m_lon, m_lat;
        int m_hasHideTer;
        int m_layer;
        pCNode  m_terrain;
        pCNode  node;
        CMatrix mat;
        }CTerrainTile,*pCTerrainTile;*/
        /*pCNode node = p->node;
        pCNode matnode = p->m_terrain;
        pCMatrixTransform matde = (pCMatrixTransform)(matnode->pNodeData);*/

        //free(p);
        delete p;
    }
    return true;
}

int CTerrainTile_Free(pCTerrainTile ptr)
{
    if (ptr)
    {
        if (ptr->m_terrain)
        {
            if (ptr->node)
            {
                CNode_DetachChild(ptr->node, ptr->m_terrain);
            }
            if (CNode_FreeNode(ptr->m_terrain))
            {
                ptr->m_terrain = NULL;
            }
        }
        if (CNode_FreeNode(ptr->node))
            ptr->node = NULL;
        ptr = NULL;
    }

    return 1;
}

void CTerrainTile_SetMask(pCTerrainTile p, unsigned long mask)
{
    if (p)
    {
        p->m_mask = mask;
    }
}

#if 0
unsigned long CTerrainTile_GetMask(pCTerrainTile p)
{
    return (0 == p) ? 0 : p->m_mask;
};
#endif

CVec3d	CTerrainTile_GetLonLat(pCTerrainTile p)
{
    if (NULL == p)
    {
        return Vec_CreatVec3d(0.0, 0.0, 0.0);
    }

    unsigned char layer = CTerrainTile_GetLayer(p->m_mask);
    unsigned char tmp;
    double tLon = p->m_lon;
    double tLat = p->m_lat;
    unsigned char i;
    for (i = 0; i <= layer; i++)
    {
        tmp = CTerrainTile_GetCount(p->m_mask, i);
        tLat += LayerMap[i].tileSpan*(tmp & 1);
        tLon += LayerMap[i].tileSpan*(tmp >> 1);
    }
    return Vec_CreatVec3d(tLon, tLat, 0.0);
}

unsigned char CTerrainTile_GetLayer(unsigned long mask)
{
    unsigned long tmp64 = mask;
    return tmp64 >> 24;
}

unsigned char CTerrainTile_GetCount(unsigned long mask, unsigned char layer)
{
    unsigned long tmp64 = mask;
    unsigned char tmp = tmp64 >> (22 - layer * 2);
    return tmp & 3;
}

void CTerrainTile_SetLayer(unsigned long* mask, unsigned char layer)
{
#if 0
    unsigned long _mask = *mask;
    unsigned long tmp = 0x00FFFFFF;
    _mask &= tmp;
    tmp = layer;
    _mask |= tmp << 24;
    *mask = _mask;
#endif
    *mask = (*mask & 0x00FFFFFF) | (layer << 24 & 0xFF000000);
}

void CTerrainTile_SetCount(unsigned long* mask, unsigned char count, unsigned char layer)
{
    unsigned long _mask = *mask;
    unsigned long tmp64 = 0x3;
    tmp64 = tmp64 << (22 - layer * 2);
    _mask &= ~tmp64;
    tmp64 = count;
    tmp64 = tmp64 << (22 - layer * 2);
    _mask |= tmp64;
    *mask = _mask;
}

unsigned long CTerrainTile_GetMaskByLonLatAndLayer(pCTerrainTile p, const CVec3d lonlat, unsigned char layer)
{
    double tLon = lonlat._v[0] + p->m_lon;
    double tLat = lonlat._v[1] + p->m_lat;
    unsigned long mask = 0;
    unsigned char tmp;
    unsigned char i;
    CTerrainTile_SetLayer(&mask, layer);

    for (i = 0; i <= layer; i++)
    {
        tmp = (unsigned char)(tLon / LayerMap[i].tileSpan) % 2;
        tmp = tmp << 1;
        tmp += (unsigned char)(tLat / LayerMap[i].tileSpan) % 2;
        CTerrainTile_SetCount(&mask, tmp, i);
    }
    return mask;
}

void CTerrainTile_GetLonLatLayerByMask(pCTerrainTile p, unsigned long mask, CVec3d *lonlat, unsigned char* layer)
{
    if (p && lonlat && layer)
    {
        *layer = CTerrainTile_GetLayer(mask);

        unsigned char tmp;
        double tLon = p->m_lon;
        double tLat = p->m_lat;
        for (unsigned char i = 0; i <= *layer; i++)
        {
            tmp = CTerrainTile_GetCount(mask, i);
            tLat += LayerMap[i].tileSpan*(tmp & 1);
            tLon += LayerMap[i].tileSpan*(tmp >> 1);
        }
        lonlat->_v[0] = tLon;
        lonlat->_v[1] = tLat;
    }
}

void CTerrainTile_SetLonLat(pCTerrainTile p, int s, int t)
{
    if (p)
    {
        p->m_lon = s;
        p->m_lat = t;
    }
}

void CTerrainTile_SetTerrain(pCTerrainTile p, pCMatrixTransform matT)
{
    if (p && matT)
    {
        CNode_RemoveAllChildAndDelete(p->node);
        p->m_terrain = matT->node;
        CNode_AddChild(p->node, p->m_terrain);
    }
}

int CTerrainTile_HasChildTer(pCTerrainTile ptr)
{
    //unsigned int num = c_vector_size(ptr->node->childrenArrary);
    unsigned int num = ptr->node->childrenArrary2.size();

    if (ptr->m_terrain)
    {
        return num > 1;
    }
    return num > 0;
}

CVec3d CTerrainTile_GetCenter(pCTerrainTile p)
{
    CVec3d ll;
    unsigned char layer;
    CTerrainTile_GetLonLatLayerByMask(p, p->m_mask, &ll, &layer);
    double eLon = ll._v[0] + LayerMap[layer].tileSpan;
    double eLat = ll._v[1] + LayerMap[layer].tileSpan;
    if (eLon > 180.0)
    {
        eLon = 180.0;
    }
    if (eLat > 90.0)
    {
        eLat = 90.0;
    }
    double tlon = (eLon + ll._v[0])*0.5;
    double tlat = (eLat + ll._v[1])*0.5;
    float alt = CAltManager_GetAltByLonLat(tlon, tlat);
    return Coordinate_DegreeLonLatAltToVec3d(tlon, tlat, alt);
}

CVec3d CTerrainTile_GetCenter_Mecartor(pCTerrainTile p)
{
    CVec3d ll;
    unsigned char layer;
    float alt;
    double eLon;
    double eLat;
    double tlon;
    double tlat;

    CTerrainTile_GetLonLatLayerByMask(p, p->m_mask, &ll, &layer);
    eLon = ll._v[0] + LayerMap[layer].tileSpan;
    eLat = ll._v[1] + LayerMap[layer].tileSpan;
    if (eLon > 180.0){ eLon = 180.0; }
    if (eLat > 90.0) { eLat = 90.0; }
    tlon = (eLon + ll._v[0])*0.5;
    tlat = (eLat + ll._v[1])*0.5;
    alt = CAltManager_GetAltByLonLat(tlon, tlat);
    return Coordinate_DegreeLonLatToMercator(tlon, tlat, alt);
}

#if 0
CVec3d CTerrainTile_GetAltLonLat(pCTerrainTile p)
{
    return (0 == p) ? Vec_CreatVec3d(0.0, 0.0, 0.0) : Vec_CreatVec3d(p->m_lon, p->m_lat, 0.0);
}
#endif


void CTerrainTile_RemoveTerrain(pCTerrainTile p)
{
    if (p && p->m_terrain)
    {
        if (CNode_DetachChild(p->node, p->m_terrain))
        {
            /*		printf("移除 p->m_terrain：%s\n",p->m_terrain->name);*/
            if (CNode_FreeNode(p->m_terrain))
            {
                /*	CNode_Destory(p->m_terrain); */
                p->m_terrain = NULL;
            }
            else
            {
                printf("CNode_FreeNode p->m_terrain：%s %s\n", p->m_terrain->name, "失败");
            }
        }
        else
        {
            printf("移除 p->m_terrain：%s %s\n", p->m_terrain->name, "失败");
        }
        /*	removeChild(m_terrain);
            m_terrain = NULL;*/
    }
}

void CTerrainTile_SetParentIdString(pCTerrainTile ptr, char* str)
{
    if (ptr && str)
    {
        memcpy_op(ptr->m_parentIdString, str, HMS_MIN(16, strlen(str)));
    }
}

void CTerrainTile_SetLodAndId(pCTerrainTile ptr, unsigned int lod, unsigned int id)
{
    if (ptr)
    {
        unsigned int  newid = ptr->m_parentsId << 2;
        newid += id;
        ptr->m_UniqueId = newid;
        ptr->m_Lod = lod;
        ptr->m_id = id;

        CTerrainTile_GetIdString(ptr);
    }
}

void CTerrainTile_GetIdString(pCTerrainTile ptr)
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
    //printf("\t%d,%d创建%d级别父节点：%s的%d号节点%d,id: %s\n",ptr->m_lon,ptr->m_lat,ptr->m_Lod,ptr->m_parentIdString,ptr->m_id,ptr->m_UniqueId,ptr->m_IdString);
}
