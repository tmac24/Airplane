#pragma once

#include "HmsStandardDef.h"
#include "HmsNavSvsMgr.h"

class CHmsNavSvsTile;

class CHmsNavSvsTilePool
{
public:
    static CHmsNavSvsTilePool* GetInstance();
    static HmsUint64 GetIDByLayer(int nLayer, int nx, int ny);
    bool PreallocationTile(int count, std::vector<CHmsNavSvsTile*> &vec);
    CHmsNavSvsTile * GetOrCreateTile(int nLayer, int nx, int ny);
    void PushToPool(CHmsNavSvsTile *pTile, int nLayer, int nx, int ny);
    void MoveUnusedTileToGarbage(const SvsMgrLayerModelStu *pModelStu);

private:
    CHmsNavSvsTilePool();
    ~CHmsNavSvsTilePool();

private:
    CHmsNavSvsTile* m_tileGarbage[300];
    int m_nTileGarbageCount;
    SvsMgrLayerModelStu m_layerModel[SVS_MGR_LAYER_MAX - SVS_MGR_LAYER_MIN + 1];
};


