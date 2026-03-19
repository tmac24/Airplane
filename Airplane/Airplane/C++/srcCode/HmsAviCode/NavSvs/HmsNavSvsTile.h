#pragma once

#include "math/HmsMath.h"
#include "render/HmsTexture2D.h"
#include "base/HmsFrustum.h"
#include "HmsNavSvsMathHelper.h"
#include <vector>
#include <mutex>
#include "HmsNavSvsCommDef.h"

class CHmsNavSvsTile
{
public:
    CHmsNavSvsTile();
    ~CHmsNavSvsTile();
    bool IsVisitableByFrustum(Frustum * frustum);
    void SetVBOOffset(int vertexOffset, int indexOffset);
    void UpdateDrawState(Frustum * frustum, const SvsMgrLayerModelStu *pModelStu);
    void SendLoadRequest(int nLayer, int nX, int nY, bool bAlwaysLoad);
    void SendLoadDemRequest(int nLayer, int nX, int nY, bool bAlwaysLoad);
    void LoadDemCallback(SvsLoadDemCallbackStu callbackStu);
    void LoadDemToVBO(void *pObj, int nLayer, int nX, int nY);
    void SendLoadImageRequest(int nLayer, int nX, int nY, bool bAlwaysLoad);
    void LoadImageCallback(SvsLoadImageCallbackStu callbackStu);
    void LoadTexture(void *pObj, int nLayer, int nX, int nY, bool bSuccess);
    void ClearAllChild();
    bool IsLoadFinished();
    void ResetLoadState();
    void GetDrawTile(std::vector<CHmsNavSvsTile*> &vec);
    void DirectDrawTileWithTexture();
    void DirectDrawTileNoTexture();

private:
    static std::mutex s_demMutex;

    SvsTileVertexStu *m_pData;
    Texture2D *m_pTexture;
    AABB m_aabb;
    SvsTileDrawState m_eDrawState;
    CHmsNavSvsTile* m_tileChild[4];
    int m_tileChildCount;

    Frustum *m_pFrustum;
    SvsTileLoadStateStu m_tileLoadState;
    int m_vertexOffset;
    int m_indexOffset;

    friend class CHmsNavSvsMgr;
};

extern int c_CHmsNavSvsTile_row;
extern int c_CHmsNavSvsTile_column;
extern int c_CHmsNavSvsTile_vertexCount;
extern int c_CHmsNavSvsTile_indexCount;

