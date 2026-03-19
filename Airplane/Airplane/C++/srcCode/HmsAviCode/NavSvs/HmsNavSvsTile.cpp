#include "HmsNavSvsTile.h"
#include "HmsNavSvsLoadDataThread.h"
#include "HmsNavSvsDataFactory.h"
#include "HmsNavSvsTilePool.h"
#include "HmsNavSvsMgr.h"
#include "HmsNavSvsLoadCountMgr.h"

std::mutex CHmsNavSvsTile::s_demMutex;

/*CHmsNavSvsTile*/

int c_CHmsNavSvsTile_row = 0;
int c_CHmsNavSvsTile_column = 0;
int c_CHmsNavSvsTile_vertexCount = 0;
int c_CHmsNavSvsTile_indexCount = 0;

CHmsNavSvsTile::CHmsNavSvsTile()
{
    c_CHmsNavSvsTile_row = 17;
    c_CHmsNavSvsTile_column = 17;
    c_CHmsNavSvsTile_vertexCount = c_CHmsNavSvsTile_row * c_CHmsNavSvsTile_column;
    c_CHmsNavSvsTile_indexCount = 6 * (c_CHmsNavSvsTile_row - 1)* (c_CHmsNavSvsTile_column - 1);

    m_pData = new SvsTileVertexStu[c_CHmsNavSvsTile_vertexCount];
    m_pTexture = new Texture2D;
    m_aabb.reset();
    m_eDrawState = TILEDRAWSTATE_NODRAW;
    for (int i = 0; i < 4; ++i)
    {
        m_tileChild[i] = nullptr;
    }
    m_tileChildCount = 0;
    m_pFrustum = nullptr;
    m_vertexOffset = 0;
    m_indexOffset = 0;
}

CHmsNavSvsTile::~CHmsNavSvsTile()
{
    if (m_pData)
    {
        delete[] m_pData;
    }
    HMS_SAFE_RELEASE(m_pTexture);
}

bool CHmsNavSvsTile::IsVisitableByFrustum(Frustum * frustum)
{
#define CHECK_FRUSTUM(x) if (!frustum->isOutOfFrustum(x.vertices)) return true;

    if (m_tileLoadState.demState != TILELOADENUM_LOAD_FINISHED)
    {
        return true;
    }
    if (m_pData)
    {
#if 0
        CHECK_FRUSTUM(m_pData[c_CHmsNavSvsTile_column*c_CHmsNavSvsTile_row / 2]);//GetCentre
        CHECK_FRUSTUM(m_pData[0]);//GetNorthWest
        CHECK_FRUSTUM(m_pData[c_CHmsNavSvsTile_column - 1]);//GetNorthEast
        CHECK_FRUSTUM(m_pData[c_CHmsNavSvsTile_column*(c_CHmsNavSvsTile_row - 1)]);//GetSouthWest
        CHECK_FRUSTUM(m_pData[c_CHmsNavSvsTile_column*c_CHmsNavSvsTile_row - 1]);//GetSouthEast   
#endif     
        if (!frustum->isOutOfFrustum(m_aabb))
        {
            return true;
        }
    }
    return false;
}

void CHmsNavSvsTile::SetVBOOffset(int vertexOffset, int indexOffset)
{
    m_vertexOffset = vertexOffset;
    m_indexOffset = indexOffset;
}

void CHmsNavSvsTile::UpdateDrawState(Frustum * frustum, const SvsMgrLayerModelStu *pModelStu)
{
    m_pFrustum = frustum;
    m_eDrawState = TILEDRAWSTATE_DRAWSELF;

    int nLayer = m_tileLoadState.demLayerInfo.nLayer;
    int nX = m_tileLoadState.demLayerInfo.nX;
    int nY = m_tileLoadState.demLayerInfo.nY;
    if (nLayer < SVS_MGR_LAYER_MIN || nLayer > SVS_MGR_LAYER_MAX)
    {
        return;
    }
    const SvsMgrLayerModelStu *pStu = &pModelStu[nLayer - SVS_MGR_LAYER_MIN];
    if (nX >= pStu->xmin && nX <= pStu->xmax && nY >= pStu->ymin && nY <= pStu->ymax)
    {
        const ObjectPoolLayerInfo *pInfo = &pStu->model[nX - pStu->xmin][nY - pStu->ymin];
        if (pInfo->bHasChild)
        {
            m_tileChildCount = 4;
            int bChildState[4] = { false, false, false, false };
            for (int i = 0; i < 4; i++)
            {
                int nChildLayer = nLayer + 1;
                int nChildX = (nX << 1) + i % 2;
                int nChildY = (nY << 1) + i / 2;

                CHmsNavSvsTile *pTile = CHmsNavSvsTilePool::GetInstance()->GetOrCreateTile(nChildLayer, nChildX, nChildY);

                m_tileChild[i] = pTile;

                pTile->SendLoadRequest(nChildLayer, nChildX, nChildY, false);

                bChildState[i] = pTile->IsLoadFinished();
            }
            if (bChildState[0] && bChildState[1] && bChildState[2] && bChildState[3])
            {
                m_eDrawState = TILEDRAWSTATE_DRAWCHILD;
            }
            else
            {
                m_eDrawState = TILEDRAWSTATE_DRAWSELF;
            }
        }
    }

    for (int i = 0; i < m_tileChildCount; ++i)
    {
        m_tileChild[i]->UpdateDrawState(frustum, pModelStu);
    }
}

void CHmsNavSvsTile::SendLoadRequest(int nLayer, int nX, int nY, bool bAlwaysLoad)
{  
    if (!m_tileLoadState.demLayerInfo.IsEqual(nLayer, nX, nY) || m_tileLoadState.demState == SvsTileLoadEnum::TILELOADENUM_LOAD_NONE)
    {
    	SendLoadDemRequest(nLayer, nX, nY, bAlwaysLoad);
    }
    if (!m_tileLoadState.textureLayerInfo.IsEqual(nLayer, nX, nY) || m_tileLoadState.textureState == SvsTileLoadEnum::TILELOADENUM_LOAD_NONE)
    {
    	SendLoadImageRequest(nLayer, nX, nY, bAlwaysLoad);
    }
    if (m_tileLoadState.demState == TILELOADENUM_LOAD_FINISHED)
    {
    	m_tileLoadState.bInFrustum = IsVisitableByFrustum(m_pFrustum);
    }
}

void CHmsNavSvsTile::SendLoadDemRequest(int nLayer, int nX, int nY, bool bAlwaysLoad)
{
    SvsLoadDemTaskStu stu;
    stu.pObj = this;
    stu.idByLayer = CHmsNavSvsTilePool::GetIDByLayer(nLayer, nX, nY);
    stu.nLayer = nLayer;
    stu.nX = nX;
    stu.nY = nY;
    stu.verticalsCnt = c_CHmsNavSvsTile_vertexCount;
    stu.numRows = c_CHmsNavSvsTile_row;
    stu.numCols = c_CHmsNavSvsTile_column;
    stu.callbackFunc = HMS_CALLBACK_1(CHmsNavSvsTile::LoadDemCallback, this);
    CHmsNavSvsLoadDataThread::GetInstance()->AddLoadDemTask(&stu);

    m_tileLoadState.demState = TILELOADENUM_LOAD_SEND;
    m_tileLoadState.demLayerInfo.Set(nLayer, nX, nY);

    CHmsNavSvsLoadCountMgr::GetInstance()->AddSendDemCount();
}

void CHmsNavSvsTile::LoadDemCallback(SvsLoadDemCallbackStu callbackStu)
{
    CHmsNavSvsTile *pThis = (CHmsNavSvsTile*)callbackStu.pObj;
    if (m_tileLoadState.demLayerInfo.IsEqual(callbackStu.nLayer, callbackStu.nX, callbackStu.nY))
    {
        if (callbackStu.bSuccess)
        {
            s_demMutex.lock();
            m_aabb = callbackStu.aabb;
            memcpy(m_pData, callbackStu.pVertexData, sizeof(SvsTileVertexStu)* c_CHmsNavSvsTile_vertexCount);
            s_demMutex.unlock();

            FactoryDemStu stu;
            stu.pObj = pThis;
            stu.nLayer = callbackStu.nLayer;
            stu.nX = callbackStu.nX;
            stu.nY = callbackStu.nY;
            stu.func = HMS_CALLBACK_4(CHmsNavSvsTile::LoadDemToVBO, this);
            CHmsNavSvsDataFactory::GetInstance()->AddDemTask(&stu);
        }
        else
        {
            CHmsNavSvsLoadCountMgr::GetInstance()->AddRecvDemCount();
            m_tileLoadState.demState = SvsTileLoadEnum::TILELOADENUM_LOAD_NONE;

            printf("\n tile load dem fail %d %d %d \n"
                , m_tileLoadState.demLayerInfo.nLayer
                , m_tileLoadState.demLayerInfo.nX
                , m_tileLoadState.demLayerInfo.nY);
        }       
    }
    else
    {
        CHmsNavSvsLoadCountMgr::GetInstance()->AddRecvDemCount();
    }  
}

void CHmsNavSvsTile::LoadDemToVBO(void *pObj, int nLayer, int nX, int nY)
{
    CHmsNavSvsLoadCountMgr::GetInstance()->AddRecvDemCount();

    CHmsNavSvsTile *pThis = (CHmsNavSvsTile*)pObj;
    if (m_tileLoadState.demLayerInfo.IsEqual(nLayer, nX, nY))
    {
        s_demMutex.lock();
        glBufferSubData(GL_ARRAY_BUFFER, m_vertexOffset, sizeof(SvsTileVertexStu)*c_CHmsNavSvsTile_vertexCount, m_pData);
        m_tileLoadState.demState = TILELOADENUM_LOAD_FINISHED;
        s_demMutex.unlock();
    }
}

void CHmsNavSvsTile::SendLoadImageRequest(int nLayer, int nX, int nY, bool bAlwaysLoad)
{
    SvsLoadImageTaskStu stu;
    stu.pObj = this;
    stu.idByLayer = CHmsNavSvsTilePool::GetIDByLayer(nLayer, nX, nY);
    stu.nLayer = nLayer;
    stu.nX = nX;
    stu.nY = nY;
    stu.callbackFunc = HMS_CALLBACK_1(CHmsNavSvsTile::LoadImageCallback, this);
    CHmsNavSvsLoadDataThread::GetInstance()->AddLoadImageTask(&stu);

    m_tileLoadState.textureState = TILELOADENUM_LOAD_SEND;
    m_tileLoadState.textureLayerInfo.Set(nLayer, nX, nY);

    CHmsNavSvsLoadCountMgr::GetInstance()->AddSendTexCount();
}

void CHmsNavSvsTile::LoadImageCallback(SvsLoadImageCallbackStu callbackStu)
{
    CHmsNavSvsTile *pThis = (CHmsNavSvsTile*)callbackStu.pObj;
    if (m_tileLoadState.textureLayerInfo.IsEqual(callbackStu.nLayer, callbackStu.nX, callbackStu.nY))
    {
        if (callbackStu.pImage && callbackStu.pImage->getWidth() == 256 && callbackStu.pImage->getHeight() == 256)
        {
            callbackStu.pImage->GenerateMipmaps();

            FactoryTextureStu stu;
            stu.pObj = pThis;
            stu.nLayer = callbackStu.nLayer;
            stu.nX = callbackStu.nX;
            stu.nY = callbackStu.nY;
            stu.pImage = callbackStu.pImage;
            stu.pTexture = m_pTexture;
            stu.func = HMS_CALLBACK_5(CHmsNavSvsTile::LoadTexture, this);
            CHmsNavSvsDataFactory::GetInstance()->AddTextureTask(&stu);
        }
        else
        {
            CHmsNavSvsLoadCountMgr::GetInstance()->AddRecvTexCount();
            HMS_SAFE_RELEASE(callbackStu.pImage);
            m_tileLoadState.textureState = SvsTileLoadEnum::TILELOADENUM_LOAD_NONE;

            printf("\n tile load image fail %d %d %d \n"
                , m_tileLoadState.textureLayerInfo.nLayer
                , m_tileLoadState.textureLayerInfo.nX
                , m_tileLoadState.textureLayerInfo.nY);
        }
    }
    else
    {
        CHmsNavSvsLoadCountMgr::GetInstance()->AddRecvTexCount();

        HMS_SAFE_RELEASE(callbackStu.pImage);
    }
}

void CHmsNavSvsTile::LoadTexture(void *pObj, int nLayer, int nX, int nY, bool bSuccess)
{
    CHmsNavSvsLoadCountMgr::GetInstance()->AddRecvTexCount();

    CHmsNavSvsTile *pThis = (CHmsNavSvsTile*)pObj;
    if (bSuccess && m_tileLoadState.textureLayerInfo.IsEqual(nLayer, nX, nY))
    {
        m_tileLoadState.textureState = TILELOADENUM_LOAD_FINISHED;
    }
}

void CHmsNavSvsTile::ClearAllChild()
{
    for (int i = 0; i < m_tileChildCount; ++i)
    {
        CHmsNavSvsTile *pTile = m_tileChild[i];
        if (pTile->m_tileChildCount > 0)
        {
            pTile->ClearAllChild();
        }
        int nLayer = pTile->m_tileLoadState.demLayerInfo.nLayer;
        int nX = pTile->m_tileLoadState.demLayerInfo.nX;
        int nY = pTile->m_tileLoadState.demLayerInfo.nY;
        CHmsNavSvsTilePool::GetInstance()->PushToPool(pTile, nLayer, nX, nY);

        m_tileChild[i] = NULL;
    }
    m_tileChildCount = 0;
}

bool CHmsNavSvsTile::IsLoadFinished()
{
    return m_tileLoadState.textureState == TILELOADENUM_LOAD_FINISHED && m_tileLoadState.demState == TILELOADENUM_LOAD_FINISHED;
#if 0
    if (m_tileLoadState.bInFrustum)
    {
        return m_tileLoadState.textureState == TILELOADENUM_LOAD_FINISHED && m_tileLoadState.demState == TILELOADENUM_LOAD_FINISHED;
    }
    else
    {
        return m_tileLoadState.demState == TILELOADENUM_LOAD_FINISHED;
    }
#endif
}

void CHmsNavSvsTile::ResetLoadState()
{
    m_tileLoadState.Init();
}

void CHmsNavSvsTile::GetDrawTile(std::vector<CHmsNavSvsTile*> &vec)
{
    if (m_eDrawState == TILEDRAWSTATE_DRAWCHILD && m_tileChildCount == 4)
    {
        for (int i = 0; i < 4; ++i)
        {
            m_tileChild[i]->GetDrawTile(vec);
        }
    }
    else
    {
        if (!m_tileLoadState.bInFrustum)
        {
            return;
        }
        if (m_tileLoadState.textureState != TILELOADENUM_LOAD_FINISHED || m_tileLoadState.demState != TILELOADENUM_LOAD_FINISHED)
        {
            return;
        }

        vec.push_back(this);
    }
}

void CHmsNavSvsTile::DirectDrawTileWithTexture()
{
    glBindTexture(GL_TEXTURE_2D, m_pTexture->getName());
    glDrawElements(GL_TRIANGLES, c_CHmsNavSvsTile_indexCount, GL_UNSIGNED_INT, (GLvoid *)m_indexOffset);
}

void CHmsNavSvsTile::DirectDrawTileNoTexture()
{
    glDrawElements(GL_TRIANGLES, c_CHmsNavSvsTile_indexCount, GL_UNSIGNED_INT, (GLvoid *)m_indexOffset);
}
