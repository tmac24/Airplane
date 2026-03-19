#include "HmsNavSvsTilePool.h"
#include "HmsNavSvsTile.h"

CHmsNavSvsTilePool::CHmsNavSvsTilePool()
{
    memset(&m_tileGarbage, 0, sizeof(m_tileGarbage));
    m_nTileGarbageCount = 0;

    memset(&m_layerModel, 0, sizeof(m_layerModel));
}

CHmsNavSvsTilePool::~CHmsNavSvsTilePool()
{

}

CHmsNavSvsTilePool* CHmsNavSvsTilePool::GetInstance()
{
    static CHmsNavSvsTilePool *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsTilePool();
    }
    return s_pIns;
}

HmsUint64 CHmsNavSvsTilePool::GetIDByLayer(int nLayer, int nx, int ny)
{
    HmsUint64 a = nLayer;
    a = a << 56;
    HmsUint64 b = nx;
    b = b << 28;
    HmsUint64 c = ny;
    return (a | b | c);
}

bool CHmsNavSvsTilePool::PreallocationTile(int count, std::vector<CHmsNavSvsTile*> &vec)
{
    if (count > sizeof(m_tileGarbage) / sizeof(m_tileGarbage[0]))
    {
        printf("\n  PreallocationTile error \n");
        return false;
    }
    for (int i = 0; i < count; ++i)
    {
        CHmsNavSvsTile* pTile = new CHmsNavSvsTile();
        pTile->ResetLoadState();

        vec.push_back(pTile);
        m_tileGarbage[m_nTileGarbageCount++] = pTile;
    }
    return true;
}

CHmsNavSvsTile * CHmsNavSvsTilePool::GetOrCreateTile(int nLayer, int nx, int ny)
{
    CHmsNavSvsTile *pRet = NULL;
    if (nLayer < SVS_MGR_LAYER_MIN || nLayer > SVS_MGR_LAYER_MAX)
    {
        return pRet;
    }
    SvsMgrLayerModelStu *pStu = &m_layerModel[nLayer - SVS_MGR_LAYER_MIN];
    if (nx >= pStu->xmin && nx <= pStu->xmax && ny >= pStu->ymin && ny <= pStu->ymax)
    {
        pRet = pStu->model[nx - pStu->xmin][ny - pStu->ymin].pSvsTile;
        pStu->model[nx - pStu->xmin][ny - pStu->ymin].pSvsTile = NULL;
    }
    if (!pRet)
    {
        if (0 == m_nTileGarbageCount)
        {
            pRet = new CHmsNavSvsTile();

            printf("\n CHmsNavSvsTilePool create tile(should not create) \n");

            pRet->ResetLoadState();
        }
        else
        {
            pRet = m_tileGarbage[m_nTileGarbageCount - 1];
            pRet->ResetLoadState();
            m_tileGarbage[m_nTileGarbageCount - 1] = NULL;
            --m_nTileGarbageCount;
        }
    }
    return pRet;
}

void CHmsNavSvsTilePool::PushToPool(CHmsNavSvsTile *pTile, int nLayer, int nx, int ny)
{
    if (nLayer >= SVS_MGR_LAYER_MIN && nLayer <= SVS_MGR_LAYER_MAX)
    {
        SvsMgrLayerModelStu *pStu = &m_layerModel[nLayer - SVS_MGR_LAYER_MIN];
        if (nx >= pStu->xmin && nx <= pStu->xmax && ny >= pStu->ymin && ny <= pStu->ymax)
        {
            pStu->model[nx - pStu->xmin][ny - pStu->ymin].pSvsTile = pTile;
        }
        else
        {
            printf("\n PushToPool error \n");
        }
    }
    else
    {
        printf("\n PushToPool error \n");
    }
}

void CHmsNavSvsTilePool::MoveUnusedTileToGarbage(const SvsMgrLayerModelStu *pModelStu)
{
    SvsMgrLayerModelStu layerModel[SVS_MGR_LAYER_MAX - SVS_MGR_LAYER_MIN + 1];
    memcpy(&layerModel, pModelStu, sizeof(layerModel));
    for (int nLayer = SVS_MGR_LAYER_MIN; nLayer <= SVS_MGR_LAYER_MAX; ++nLayer)
    {
        SvsMgrLayerModelStu *pStu = &m_layerModel[nLayer - SVS_MGR_LAYER_MIN];
        SvsMgrLayerModelStu *pNewStu = &layerModel[nLayer - SVS_MGR_LAYER_MIN];
        for (int nx = pStu->xmin; nx <= pStu->xmax; ++nx)
        {
            for (int ny = pStu->ymin; ny <= pStu->ymax; ++ny)
            {
                CHmsNavSvsTile *pTile = pStu->model[nx - pStu->xmin][ny - pStu->ymin].pSvsTile;
                if (pTile)
                {
                    if (nx >= pNewStu->xmin && nx <= pNewStu->xmax && ny >= pNewStu->ymin && ny <= pNewStu->ymax)
                    {
                        pNewStu->model[nx - pNewStu->xmin][ny - pNewStu->ymin].pSvsTile = pTile;
                    }
                    else
                    {
                        m_tileGarbage[m_nTileGarbageCount++] = pTile;
                    }
                }
            }
        }
    }
    memcpy(&m_layerModel, layerModel, sizeof(layerModel));
}