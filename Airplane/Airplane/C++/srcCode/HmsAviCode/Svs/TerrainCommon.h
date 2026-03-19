#ifndef _TERRAIN_COMMON__H_
#define _TERRAIN_COMMON__H_

#include <vector>
#include "Drawable.h"
#include "Node.h"
#include "TerrainTile.h"
#include "Matrix.h"
#include "line.h"

struct ToChildNode
{
    pCTerrainTile parent;
    pCTerrainTile child[4];
};
typedef ToChildNode* pToChildNode;

struct ToParentNode
{
    pCTerrainTile parent;
    pCNode pNodeMat;
};
typedef ToParentNode* pToParentNode;

struct TTerrain
{
    pCTerrainTile pTerTile;
    bool needKeep;
};
typedef TTerrain* pTTerrain;

struct CTerrainThread
{
    pCNode m_root;

    std::vector<pCTerrainTile> m_needAdded2;
    std::vector<pCTerrainTile> m_hasAdded2;
    std::vector<pCTerrainTile> m_needDeleted2;
    std::vector<pCTerrainTile> m_needCreatedP2;

    CVec3d m_lla;
    int m_lon;
    int m_lat;
    CVec3d m_eyePos;

    std::vector<pToChildNode> m_needCreatedC2;
    std::vector<pToChildNode> m_hasCreatedC2;
    std::vector<pToParentNode> m_hasCreatedP2;

    unsigned char m_curLayer;

    TTerrain lastTopTerrainTiles[9];//9 top level tile

    int m_dl_renderiId;

    CVec4d projection;
    CVec3d ypr;
    CVec3d centerlonlat;
    CVec3d startlonlat;
    CVec3d endlonlat;

    line2d viewprojection[2];

    CTerrainThread(){
        m_root = CNode_CreateNode(Node);

        m_lla = Vec_CreatVec3d(0, 0, 0);
        m_eyePos = Vec_CreatVec3d(0, 0, 0);
        m_curLayer = 0;
        memset(lastTopTerrainTiles, 0, sizeof(TTerrain)* 9);
        m_dl_renderiId = 0;
        projection = Vec_CreatVec4d(0, 0, 0, 0);
        ypr = Vec_CreatVec3d(0, 0, 0);
        centerlonlat = Vec_CreatVec3d(0, 0, 0);
        startlonlat = Vec_CreatVec3d(0, 0, 0);
        endlonlat = Vec_CreatVec3d(0, 0, 0);
        memset(viewprojection, 0, sizeof(line2d)* 2);
    }
};

typedef CTerrainThread* pCTerrainThread;


pTTerrain TTerrain_Create();
void TTerrain_Free(pTTerrain p);


#endif // !__HEADER__