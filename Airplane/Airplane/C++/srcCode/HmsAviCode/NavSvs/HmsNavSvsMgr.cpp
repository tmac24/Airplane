#include "HmsNavSvsMgr.h"
#include "HmsNavSvsTilePool.h"
#include "render/HmsRenderer.h"
#include "HmsNavSvsMathHelper.h"
#include "HmsNavSvsLoadDataThread.h"
#include "HmsNavSvsDataFactory.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderState.h"
#include "NavSvsObject/HmsNavSvsObjectMgr.h"
#include "HmsNavSvsLoadCountMgr.h"
#include "HmsNavSvsModuleMgr.h"
#include "HmsLog.h"

CHmsNavSvsMgr * CHmsNavSvsMgr::GetInstance()
{
    static CHmsNavSvsMgr *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsMgr;
    }
    return s_pIns;
}

CHmsNavSvsMgr::CHmsNavSvsMgr()
{
    GetMapDataRange();
}

CHmsNavSvsMgr::~CHmsNavSvsMgr()
{

}

void CHmsNavSvsMgr::InitContent(Size size)
{
    m_pSvsObjectMgr = CHmsNavSvsObjectMgr::GetInstance();

    m_layerMin = SVS_MGR_LAYER_MIN;
    m_layerMax = SVS_MGR_LAYER_MAX;

    memset(&m_layerModel, 0, sizeof(m_layerModel));

    CHmsNavSvsObjectMgr::GetInstance()->SetNavSvsObjectEnable(NavSvsObjectType::OBJECTTYPE_PLANE, false);
    CHmsNavSvsObjectMgr::GetInstance()->SetNavSvsObjectEnable(NavSvsObjectType::OBJECTTYPE_RUNWAY, false);
    CHmsNavSvsObjectMgr::GetInstance()->SetNavSvsObjectEnable(NavSvsObjectType::OBJECTTYPE_FLYPIPE, false);
    CHmsNavSvsObjectMgr::GetInstance()->SetNavSvsObjectEnable(NavSvsObjectType::OBJECTTYPE_ADSB, false);

    m_bEnableSatelliteModel = true;
    m_bEnableTerrainAlarm = false;
    m_bEnableLightColor = false;

    m_mat4dTrans = Mat4d::IDENTITY;
    m_mat4dP = Mat4d::IDENTITY;
    m_mat4dVRelative = Mat4d::IDENTITY;
    m_mat4dPVRelative = Mat4d::IDENTITY;
    m_mat4dVWorld = Mat4d::IDENTITY;
    m_mat4dPVWorld = Mat4d::IDENTITY;

    m_relativeOrigin = Vec3d::ZERO;

    Image *pImage = new Image;
    pImage->initWithImageFile("res/NavSvs/detailmap.jpg");
    pImage->GenerateMipmaps();
    m_pTextureDetail = new Texture2D;
    m_pTextureDetail->initWithImage(pImage);
    HMS_SAFE_RELEASE(pImage);

    m_nVBO = 0;
    m_nVBOIndex = 0;

    GLProgram *glProgram = GLProgram::createWithFilenames("Shaders/zs_navsvs.vsh", "Shaders/zs_navsvs.fsh");
    if (0 == glProgram)
    {
        return;
    }
    SetGLProgram(glProgram);
    glUseProgram(GetGLProgram()->getProgram());

    m_locAttrColorLight = glProgram->getAttribLocation("a_colorLight");
    m_locAttrAltitude = glProgram->getAttribLocation("a_altitude");
    m_locUniformEnableTerrainAlarm = glProgram->getUniformLocation("u_enableTerrainAlarm");
    m_locUniformEnableLightColor = glProgram->getUniformLocation("u_enableLightColor");
    m_locUniformPlaneAltitude = glProgram->getUniformLocation("u_eyeAltitude");
    m_locUniformRelativeOrigin = glProgram->getUniformLocation("u_relativeOrigin");

    CHmsNavSvsTilePool::GetInstance();
    int tileCount = 145 + (SVS_TEN_LAYER_OFFSET * 2 + 1) * (SVS_TEN_LAYER_OFFSET * 2 + 1);
    std::vector<CHmsNavSvsTile*> vec;
    CHmsNavSvsTilePool::GetInstance()->PreallocationTile(tileCount, vec);
    GLuint *pIndex = (GLuint*)malloc(sizeof(GLuint) * tileCount * c_CHmsNavSvsTile_indexCount);
    for (int tileIndex = 0; tileIndex < tileCount; ++tileIndex)
    {
        vec[tileIndex]->SetVBOOffset(tileIndex * c_CHmsNavSvsTile_vertexCount * sizeof(SvsTileVertexStu)
            , tileIndex * c_CHmsNavSvsTile_indexCount * sizeof(GLuint));

        int vertexStart = tileIndex * c_CHmsNavSvsTile_vertexCount;
        int nPos = tileIndex * c_CHmsNavSvsTile_indexCount;
        int nCurStart = 0;
        for (int i = 0; i < (c_CHmsNavSvsTile_row - 1); i++)
        {
            for (int j = 0; j < (c_CHmsNavSvsTile_row - 1); j++)
            {
                nCurStart = i * c_CHmsNavSvsTile_row + j;

                pIndex[nPos++] = vertexStart + nCurStart + 1;
                pIndex[nPos++] = vertexStart + nCurStart;
                pIndex[nPos++] = vertexStart + nCurStart + c_CHmsNavSvsTile_row;
                pIndex[nPos++] = vertexStart + nCurStart + 1;
                pIndex[nPos++] = vertexStart + nCurStart + c_CHmsNavSvsTile_row;
                pIndex[nPos++] = vertexStart + nCurStart + c_CHmsNavSvsTile_row + 1;
            }
        }
    }
    glGenBuffers(1, &m_nVBO);
    glGenBuffers(1, &m_nVBOIndex);

    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SvsTileVertexStu)*tileCount*c_CHmsNavSvsTile_vertexCount, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVBOIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* tileCount * c_CHmsNavSvsTile_indexCount, pIndex, GL_STATIC_DRAW);

    free(pIndex);
}

void CHmsNavSvsMgr::OnDraw(const Mat4 &transform, uint32_t flags)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    //glClearColor(24 / 255.0f, 64 / 255.0f, 152 / 255.0f, 1.0f);
    glClearColor(51.0f / 255.0f, 102.0f / 255.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    GLProgram *glProgram = GetGLProgram();
    glProgram->use();

    Mat4 mat;
    CHmsNavSvsMathHelper::GetMatFromMat4d(&mat, &m_mat4dPVRelative);
    glProgram->setUniformLocationWithMatrix4fv(glProgram->getUniformLocation("HMS_MVPMatrix"), mat.m, 1);

    glProgram->setUniformLocationWith1i(m_locUniformEnableTerrainAlarm, m_bEnableTerrainAlarm);
    glProgram->setUniformLocationWith1i(m_locUniformEnableLightColor, m_bEnableLightColor);
    glProgram->setUniformLocationWith1f(m_locUniformPlaneAltitude, m_attitudeStu.Height);
    glProgram->setUniformLocationWith3f(m_locUniformRelativeOrigin, m_relativeOrigin.x, m_relativeOrigin.y, m_relativeOrigin.z);

    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(SvsTileVertexStu), (GLvoid *)offsetof(SvsTileVertexStu, vertices));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(SvsTileVertexStu), (GLvoid *)offsetof(SvsTileVertexStu, texCoords));
    glVertexAttribPointer(m_locAttrColorLight, 4, GL_FLOAT, GL_FALSE, sizeof(SvsTileVertexStu), (GLvoid *)offsetof(SvsTileVertexStu, color));
    glVertexAttribPointer(m_locAttrAltitude, 1, GL_FLOAT, GL_FALSE, sizeof(SvsTileVertexStu), (GLvoid *)offsetof(SvsTileVertexStu, altitude));
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
    glEnableVertexAttribArray(m_locAttrColorLight);
    glEnableVertexAttribArray(m_locAttrAltitude);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVBOIndex);

    m_pVecPrepareDraw.clear();
    int len = m_pVecBackgroundTiles.size();
    for (int i = 0; i < len; ++i)
    {
        CHmsNavSvsTile *pTile = m_pVecBackgroundTiles[i];
        pTile->GetDrawTile(m_pVecPrepareDraw);
    }

    std::sort(m_pVecPrepareDraw.begin(), m_pVecPrepareDraw.end(), 
        [=](const CHmsNavSvsTile* stuA, const CHmsNavSvsTile* stuB){
        return stuA->m_indexOffset < stuB->m_indexOffset;
    });

    len = m_pVecPrepareDraw.size();
    if (m_bEnableSatelliteModel)
    {
        for (int i = 0; i < len; ++i)
        {
            CHmsNavSvsTile *pTile = m_pVecPrepareDraw[i];
            pTile->DirectDrawTileWithTexture();
        }
    }
    else
    {
        GL::bindTexture2D(m_pTextureDetail->getName());
        for (int i = 0; i < len; ++i)
        {
            CHmsNavSvsTile *pTile = m_pVecPrepareDraw[i];
            pTile->DirectDrawTileNoTexture();
        }
    }

#if 0
    /*use for test*/
    static int svsDrawCount = 0;
    ++svsDrawCount;
    if (svsDrawCount % 200 == 0)
    {
        CHmsNavSvsLoadCountMgr *pIns = CHmsNavSvsLoadCountMgrGetInstance();
        printf("\n svsDrawTileCount %d\n", len);
        printf("senddem:%lld, recvdem:%lld, sendtex:%lld, recvtex:%lld, max:%d, percent:%f, finished:%d\n",
            pIns->m_sendDemCount, pIns->m_recvDemCount, pIns->m_sendTexCount, 
            pIns->m_recvTexCount, pIns->m_maxDiff, pIns->m_percent, pIns->m_bIsLoadFinished);
    }
#endif

    CHmsNavSvsLoadDataThread *pLoadIns = CHmsNavSvsLoadDataThread::GetInstance();
    pLoadIns->SetPlaneInfo(&m_attitudeStu);
    pLoadIns->FilterSortDemTask();
    pLoadIns->FilterSortImageTask();

    CHmsNavSvsDataFactory::GetInstance()->Run();  

    m_pSvsObjectMgr->OnDraw();

    GL::bindTexture2D(0);
    RenderState::StateBlock::_defaultState->restore(0);
}

bool CHmsNavSvsMgr::IsLonLatInValidRange(double lon, double lat)
{
    if (lon >= m_mapDataMinLon && lon <= m_mapDataMaxLon && lat >= m_mapDataMinLat && lat <= m_mapDataMaxLat)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CHmsNavSvsMgr::GetMapDataRange()
{
    m_mapDataMinLon = 0;
    m_mapDataMaxLon = 0;
    m_mapDataMinLat = 0;
    m_mapDataMaxLat = 0;

    std::map<std::string, HmsDbMap> mapInfo;
    auto config = CHmsConfigXMLReader::GetSingleConfig();
    config->GetMapConfigInfo(mapInfo);
    auto iter = mapInfo.find("satellite");
    if (iter != mapInfo.end())
    {
        HmsDbMap &dbMap = iter->second;
        auto iter2 = dbMap.mapLayer.find(dbMap.layerMax);
        if (iter2 != dbMap.mapLayer.end())
        {
            HmsDbMapLayer &dbMapLayer = iter2->second;
            if (dbMapLayer.vDbPath.size() > 0)
            {
                int xMin = dbMapLayer.vDbPath.front().xMin;
                int xMax = dbMapLayer.vDbPath.back().xMax;
                int yMin = dbMapLayer.vDbPath.front().yMin + 1;
                int yMax = dbMapLayer.vDbPath.back().yMax;

                auto lonlat = CHmsNavSvsMathHelper::GetTileLonLatByXY(dbMap.layerMax, Vec2(xMin, yMax));
                m_mapDataMinLon = lonlat.x;
                m_mapDataMinLat = lonlat.y;

                lonlat = CHmsNavSvsMathHelper::GetTileLonLatByXY(dbMap.layerMax, Vec2(xMax, yMin));
                m_mapDataMaxLon = lonlat.x;
                m_mapDataMaxLat = lonlat.y;
            }   
        }
    }
    HMSLOG("\n m %f %f %f %f \n", m_mapDataMinLon, m_mapDataMaxLon, m_mapDataMinLat, m_mapDataMaxLat);
}

void CHmsNavSvsMgr::Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags)
{
    m_customCommand.init(m_globalZOrder, transform, CHmsNode::FLAGS_RENDER_AS_3D);
    m_customCommand.func = std::bind(&CHmsNavSvsMgr::OnDraw, this, transform, flags);
    renderer->addCommand(&m_customCommand);
}

#define PushModelInfoToMap(_pModelStu_, _nLayer_, _nX_, _nY_, _bHasChild_) \
    pInfo = &(_pModelStu_)->model[(_nX_) - (_pModelStu_)->xmin][(_nY_) - (_pModelStu_)->ymin]; \
    pInfo->nLayer = (_nLayer_); \
    pInfo->nX = (_nX_); \
    pInfo->nY = (_nY_); \
    pInfo->bHasChild = (_bHasChild_)

void CHmsNavSvsMgr::Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags)
{
    if (!IsLonLatInValidRange(m_attitudeStu.LngLat.x, m_attitudeStu.LngLat.y))
    {
        return;
    }

    int len = m_pVecBackgroundTiles.size();
    for (int i = 0; i < len; ++i)
    {
        CHmsNavSvsTile *pTile = m_pVecBackgroundTiles[i];
        pTile->ClearAllChild();
        CHmsNavSvsTilePool::GetInstance()->PushToPool(pTile,
            pTile->m_tileLoadState.demLayerInfo.nLayer,
            pTile->m_tileLoadState.demLayerInfo.nX,
            pTile->m_tileLoadState.demLayerInfo.nY);
    }
    m_pVecBackgroundTiles.clear();

    memset(&m_layerModel, 0, sizeof(m_layerModel));
    for (int i = m_layerMin; i < m_layerMax; ++i)
    {
        ObjectPoolLayerInfo *pInfo = NULL;

        Vec2 pos = CHmsNavSvsMathHelper::GetTileXYByLonLat(i, m_attitudeStu.LngLat);
        SvsMgrLayerModelStu *pCurLayerStu = &m_layerModel[i - m_layerMin];
        if (i == m_layerMin)
        {  
            pCurLayerStu->xmin = pos.x - SVS_TEN_LAYER_OFFSET;
            pCurLayerStu->xmax = pos.x + SVS_TEN_LAYER_OFFSET;
            pCurLayerStu->ymin = pos.y - SVS_TEN_LAYER_OFFSET;
            pCurLayerStu->ymax = pos.y + SVS_TEN_LAYER_OFFSET;
            for (int n = pos.x - SVS_TEN_LAYER_OFFSET; n <= pos.x + SVS_TEN_LAYER_OFFSET; ++n)
            {
                for (int m = pos.y - SVS_TEN_LAYER_OFFSET; m <= pos.y + SVS_TEN_LAYER_OFFSET; ++m)
                {
                    PushModelInfoToMap(pCurLayerStu, i, n, m, false);
                }
            }
        }
        SvsMgrLayerModelStu *pNextLayerStu = &m_layerModel[i + 1 - m_layerMin];
        pNextLayerStu->xmin = (pos.x - 1) * 2;
        pNextLayerStu->xmax = (pos.x + 1) * 2 + 1;
        pNextLayerStu->ymin = (pos.y - 1) * 2;
        pNextLayerStu->ymax = (pos.y + 1) * 2 + 1;
        for (int n = pos.x - 1; n <= pos.x + 1; ++n)
        {
            for (int m = pos.y - 1; m <= pos.y + 1; ++m)
            {
                PushModelInfoToMap(pCurLayerStu, i, n, m, true);

                int x = n << 1;
                int y = m << 1;

                PushModelInfoToMap(pNextLayerStu, i + 1, x, y, false);
                PushModelInfoToMap(pNextLayerStu, i + 1, x, y + 1, false);
                PushModelInfoToMap(pNextLayerStu, i + 1, x + 1, y, false);
                PushModelInfoToMap(pNextLayerStu, i + 1, x + 1, y + 1, false);
            }
        }  
    }
    CHmsNavSvsTilePool::GetInstance()->MoveUnusedTileToGarbage(m_layerModel);

    Mat4 mat;
    CHmsNavSvsMathHelper::GetMatFromMat4d(&mat, &m_mat4dPVWorld);
    m_frustum.initFrustum(&mat);

    SvsMgrLayerModelStu *pLayerStu = &m_layerModel[0];
    for (int x = pLayerStu->xmin; x <= pLayerStu->xmax; ++x)
    {
        for (int y = pLayerStu->ymin; y <= pLayerStu->ymax; ++y)
        {
            ObjectPoolLayerInfo *pInfo = &pLayerStu->model[x - pLayerStu->xmin][y - pLayerStu->ymin];

            CHmsNavSvsTile *pTile = CHmsNavSvsTilePool::GetInstance()->GetOrCreateTile(pInfo->nLayer, pInfo->nX, pInfo->nY);
            pTile->SendLoadRequest(pInfo->nLayer, pInfo->nX, pInfo->nY, false);
            pTile->UpdateDrawState(&m_frustum, m_layerModel);

            m_pVecBackgroundTiles.push_back(pTile);
        }
    }

    if (renderer)
    {
        CHmsNode::Visit(renderer, parentTransform, parentFlags);
    }
}

bool CHmsNavSvsMgr::IsEnableSatelliteModel()
{
    return m_bEnableSatelliteModel;
}

void CHmsNavSvsMgr::SetSatelliteModel(bool b)
{
    m_bEnableSatelliteModel = b;

    m_bEnableLightColor = m_bEnableSatelliteModel ? false : true;

    CHmsNavSvsObjectMgr::GetInstance()->SetNavSvsObjectEnable(NavSvsObjectType::OBJECTTYPE_RUNWAY, !b);
}

bool CHmsNavSvsMgr::IsEnableTerrainAlarm()
{
    return m_bEnableTerrainAlarm;
}

void CHmsNavSvsMgr::SetTerrainAlarm(bool b)
{
    m_bEnableTerrainAlarm = b;
}

void CHmsNavSvsMgr::MoveCameraDirectTarget(float speed)
{
    Vec2d ll;
    ll = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&m_attitudeStu.LngLat, speed, m_attitudeStu.yaw);
    m_attitudeStu.LngLat = ll;
}

void CHmsNavSvsMgr::MoveCameraUpOrDown(float step)
{
    m_attitudeStu.Height += step;
}

void CHmsNavSvsMgr::Update(float delta)
{
    auto pCurCameraCtrl = CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl();
    if (pCurCameraCtrl)
    {
        pCurCameraCtrl->Update(delta, &m_attitudeStu);

        m_mat4dVWorld = pCurCameraCtrl->m_mat4dV;
        m_mat4dPVWorld = m_mat4dP * m_mat4dVWorld;

        m_mat4dVRelative = m_mat4dVWorld.getInversed();
        m_mat4dVRelative = m_mat4dTrans * m_mat4dVRelative;
        m_mat4dVRelative.inverse();
        m_mat4dPVRelative = m_mat4dP * m_mat4dVRelative;
    }
    
    m_pSvsObjectMgr->Update(delta);

    CHmsNavSvsLoadCountMgr::GetInstance()->Update(delta);
}

const CalAttitudeStu* CHmsNavSvsMgr::GetAttitudeStu()
{
    return &m_attitudeStu;
}

const HmsAviPf::Mat4d* CHmsNavSvsMgr::GetMat4dPVRelative()
{
    return &m_mat4dPVRelative;
}

const HmsAviPf::Mat4d* CHmsNavSvsMgr::GetMat4dTrans()
{
    return &m_mat4dTrans;
}

const HmsAviPf::Mat4d* CHmsNavSvsMgr::GetPlaneModel()
{
    auto pCurCameraCtrl = CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl();
    if (pCurCameraCtrl)
    {
        return &pCurCameraCtrl->m_mat4dPlaneModel;
    }
    else
    {
        return nullptr;
    }
}
