
#include "Drawable.h"
#include "MatrixNode.h"
#include "3DBarrierBlock.h"
#include "3DBarrierMgr.h"
#include "Global.h" 
#include "Image.h"
#include "TextureMgr.h"
#include "Coordinate.h"

#ifdef __cplusplus
extern "C" {
#endif


#include "_Vec2.h"

#ifdef __cplusplus
}
#endif

static pC3DBarrierMgr  p2DbarrierMgr = NULL;

pC3DBarrierMgr C3DBarrierMgr_Create()
{
#if 0
    pC3DBarrierMgr mgr = (pC3DBarrierMgr)malloc(sizeof(C3DBarrierMgr));
    if (!mgr)
    {
        LogWrite(LOG_LEVEL_FATAL, "严重错误 pCTerrainMgr chunk =  (pCTerrainMgr)malloc(sizeof(CTerrainMgr)) ==null ");
        //system("pause");//xyh 直接返回0,避免假死.
        return 0;
    }
    memset(mgr, 0, sizeof(C3DBarrierMgr));
#endif
    auto mgr = new C3DBarrierMgr;

    mgr->m_root = CNode_CreateNode(Node);
    CNode_SetName(mgr->m_root, "障碍物_地形根节点");

    /*需要添加的BarrierLonLatUnit*/
    //mgr->addChunk = &(mgr->m_v_addChunk);
    /*需要删除的BarrierLonLatUnit*/
    //mgr->delChunk = &(mgr->m_v_delChunk);
    /*需要显示的BarrierLonLatUnit*/
    //mgr->renderChunk = &(mgr->m_v_renderChunk);
    /*需要显示的BarrierLonLatUnit*/
    //mgr->tmpChunk = &(mgr->m_v_tmpChunk);

    //c_vector_create(mgr->addChunk, NULL);
    //c_vector_create(mgr->delChunk, NULL);
    //c_vector_create(mgr->renderChunk, NULL);
    //c_vector_create(mgr->tmpChunk, NULL);

    return mgr;
}

pC3DBarrierMgr GetC3DBarrierMgr()
{
    if (!p2DbarrierMgr)
    {
        p2DbarrierMgr = C3DBarrierMgr_Create();
    }
    return p2DbarrierMgr;
}

#if 0
int C3DBarrierIsContain(c_pvector chunkLonLatVector, pC3DBarrierBlock chunk)
{
    if (chunkLonLatVector)
    {
        unsigned int index = 0, num = c_vector_size(chunkLonLatVector);
        c_iterator iter, beg = c_vector_begin(chunkLonLatVector), end = c_vector_end(chunkLonLatVector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pC3DBarrierBlock ch = (pC3DBarrierBlock)(ITER_REF(iter));
            if ((int)ch->m_lonlat._v[0] == (int)(chunk->m_lonlat._v[0]) && (int)(ch->m_lonlat._v[1]) == (int)(chunk->m_lonlat._v[1]))
            {
                return 1;
            }
        }
    }
    return 0;
}
#endif

bool C3DBarrierIsContain2(std::vector<pC3DBarrierBlock>& chunkLonLatVector, pC3DBarrierBlock chunk)
{
    if (!chunk)
    {
        return false;
    }

    for (unsigned int i = 0; i < chunkLonLatVector.size(); ++i)
    {
        auto ch = chunkLonLatVector[i];
        if ((int)floorf(ch->m_lonlat._v[0]) == (int)floorf(chunk->m_lonlat._v[0])
            && (int)floorf(ch->m_lonlat._v[1]) == (int)floorf(chunk->m_lonlat._v[1]))
        {
            return true;
        }
    }

    return false;
}

void C3DBarrierMgrRangeCalc(CVec3d cameraLonlat)
{
    /*判断是够为空*/
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
    if (!ptrmgr)
    {
        return;
    }

    int curLon = 0;
    int curLat = 0;
    /*写入应该显示和需要添加的区域*/
    for (int i = 0; i < ptrmgr->m_lonCount; i++)
    {
        for (int j = 0; j < ptrmgr->m_latCount; j++)
        {
            curLon = ptrmgr->m_startLon + i;
            curLat = ptrmgr->m_startLat + j;

            if (curLat < -90 || curLat >= 90)
            {
                continue;
            }

            if (curLon < -180)
            {
                curLon += 360;
            }
            if (curLon >= 180)
            {
                curLon -= 360;
            }

            pC3DBarrierBlock terrmp = C3DBarrierBlock_Create();
            terrmp->m_lonlat._v[0] = curLon;
            terrmp->m_lonlat._v[1] = curLat;
            terrmp->m_lonlat._v[2] = 0.0;

            //c_vector_push_back(ptrmgr->tmpChunk, terrmp);
            ptrmgr->tmpChunk2.push_back(terrmp);

            /*检查渲染的节点是否包含，若不包含怎需要添加*/
            //isCon = C3DBarrierIsContain(ptrmgr->renderChunk, terrmp);
            bool isCon = C3DBarrierIsContain2(ptrmgr->renderChunk2, terrmp);
            if (!isCon)
            {
                pC3DBarrierBlock nchunk = C3DBarrierBlock_Create();
                memcpy_op(&(nchunk->m_lonlat), &(terrmp->m_lonlat), sizeof(CVec3d));
                //c_vector_push_back(ptrmgr->addChunk, nchunk);
                ptrmgr->addChunk2.push_back(nchunk);
            }
            //	C3DBarrierBlock_Destory(terrmp);
        }
    }

    /*计算需要删除的块*/

    //unsigned int index = 0, num = c_vector_size(ptrmgr->renderChunk);
    //c_iterator iter, beg = c_vector_begin(ptrmgr->renderChunk), end = c_vector_end(ptrmgr->renderChunk);


    //iter = c_vector_begin(ptrmgr->renderChunk);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < ptrmgr->renderChunk2.size(); ++i)
    {
        //pC3DBarrierBlock chunk = (pC3DBarrierBlock)(ITER_REF(iter));
        auto chunk = ptrmgr->renderChunk2[i];
        //if (!C3DBarrierIsContain(ptrmgr->tmpChunk, chunk))
        if (!C3DBarrierIsContain2(ptrmgr->tmpChunk2, chunk))
        {
            pC3DBarrierBlock nchunk = C3DBarrierBlock_Create();
            memcpy_op(&(nchunk->m_lonlat), &(chunk->m_lonlat), sizeof(CVec3d));
            //c_vector_push_back(ptrmgr->delChunk, nchunk);
            ptrmgr->delChunk2.push_back(nchunk);
        }
    }
}

#if 0
void C3DBarrierClearChunkVector(c_pvector chunkVector)
{
    unsigned int index = 0, num = c_vector_size(chunkVector);
    c_iterator iter, beg = c_vector_begin(chunkVector), end = c_vector_end(chunkVector);
    for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    {
        pC3DBarrierBlock chunk = (pC3DBarrierBlock)(ITER_REF(iter));
        C3DBarrierBlock_Destory(chunk);
        chunk = 0;
    }
    c_vector_clear(chunkVector);
}
#endif

void C3DBarrierClearChunkVector2(std::vector<pC3DBarrierBlock>& chunkVector)
{
    //unsigned int index = 0, num = c_vector_size(chunkVector);
    //c_iterator iter, beg = c_vector_begin(chunkVector), end = c_vector_end(chunkVector);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < chunkVector.size(); ++i)
    {
        //pC3DBarrierBlock chunk = (pC3DBarrierBlock)(ITER_REF(iter));
        auto chunk = chunkVector[i];
        C3DBarrierBlock_Destory(chunk);
        chunk = 0;
    }
    //c_vector_clear(chunkVector);
    chunkVector.clear();
}

#if 0
void C3DBarrierMgrVectorRemove(c_pvector chunkVector, pC3DBarrierBlock blk)
{
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
    unsigned int index = 0;
    unsigned int num = c_vector_size(chunkVector);
    c_iterator iter, beg = c_vector_begin(chunkVector), end = c_vector_end(chunkVector);
    for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    {
        pC3DBarrierBlock chunk = (pC3DBarrierBlock)(ITER_REF(iter));
        if ((int)chunk->m_lonlat._v[0] == (int)blk->m_lonlat._v[0] &&
            (int)chunk->m_lonlat._v[1] == (int)blk->m_lonlat._v[1])
        {
            CNode_RemoveChild(ptrmgr->m_root, chunk->m_root);
            CNode_FreeNode(chunk->m_root);
            chunk->m_root = 0;
            C3DBarrierBlock_Destory(chunk);
            c_vector_erase(chunkVector, iter);
        }
    }
}
#endif

void C3DBarrierMgrVectorRemove2(std::vector<pC3DBarrierBlock>& chunkVector, pC3DBarrierBlock blk)
{
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
    for (unsigned int i = 0; i < chunkVector.size(); ++i)
    {
        auto chunk = chunkVector[i];
        if ((int)floorf(chunk->m_lonlat._v[0]) == (int)floorf(blk->m_lonlat._v[0])
            && (int)floorf(chunk->m_lonlat._v[1]) == (int)floorf(blk->m_lonlat._v[1])
            )
        {
            CNode_DetachChild(ptrmgr->m_root, chunk->m_root);
            CNode_FreeNode(chunk->m_root);
            chunk->m_root = 0;
            C3DBarrierBlock_Destory(chunk);
            chunkVector.erase(chunkVector.begin() + i);
        }
    }
}

void C3DBarrierMgrUpdateChunkDelete()
{
    /*判断是够为空*/
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
    //unsigned int index = 0, num = c_vector_size(ptrmgr->delChunk);
    //c_iterator iter, beg = c_vector_begin(ptrmgr->delChunk), end = c_vector_end(ptrmgr->delChunk);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < ptrmgr->delChunk2.size(); ++i)
    {
        //pC3DBarrierBlock chunk = (pC3DBarrierBlock)(ITER_REF(iter));
        auto chunk = ptrmgr->delChunk2[i];
        //Terrain2DChunkVectorRemove(ptrmgr->renderChunk,chunk->m_lonlat._v[0],chunk->m_lonlat._v[1]); 
        //C3DBarrierMgrVectorRemove(ptrmgr->renderChunk, chunk);
        C3DBarrierMgrVectorRemove2(ptrmgr->renderChunk2, chunk);
    }
    //c_vector_clear(ptrmgr->delChunk);
}


void C3DBarrierUpdateChunkAdd()
{
    /*判断是够为空*/
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
    if (0 == ptrmgr)
    {
        return;
    }

    //unsigned int index = 0, num = c_vector_size(ptrmgr->addChunk);
    //c_iterator iter, beg = c_vector_begin(ptrmgr->addChunk), end = c_vector_end(ptrmgr->addChunk);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < ptrmgr->addChunk2.size(); ++i)
    {
        //pC3DBarrierBlock block = (pC3DBarrierBlock)(ITER_REF(iter));
        auto block = ptrmgr->addChunk2[i];
        pC3DBarrierBlock chunk = C3DBarrierBlock_Create();
        if (0 == chunk)
        {
            continue;
        }

        //memcpy_op(&(chunk->m_lonlat), &(block->m_lonlat), sizeof(CVec3d));

        //C3DBarrierBlockSetLonlat(chunk, chunk->m_lonlat);
        C3DBarrierBlockSetLonlat(chunk, block->m_lonlat);
        printf(" set chunk lon lat: %lf, %lf", chunk->m_lonlat._v[0], chunk->m_lonlat._v[1]);
        C3DBarrierBlockReadBarrierData(chunk);
        C3DBarrierBlockCreatBarrierNode(chunk);
        //	printf("添加block：%d，%d\n",(int)chunk->m_lonlat._v[0],(int)chunk->m_lonlat._v[1]);
        CNode_AddChild(ptrmgr->m_root, chunk->m_root);
        //c_vector_push_back(ptrmgr->renderChunk, chunk);
        ptrmgr->renderChunk2.push_back(chunk);
    }
}

void C3DBarrierUpdateChunk()
{
    /*判断是够为空*/
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
}

void C3DBarrierUpdateRender()
{
    /*判断是够为空*/
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
    if (0 == ptrmgr){ return; }

    //unsigned int index = 0, num = c_vector_size(ptrmgr->renderChunk);
    //c_iterator iter, beg = c_vector_begin(ptrmgr->renderChunk), end = c_vector_end(ptrmgr->renderChunk);
    //	printf("渲染经纬度：%d块\n",num);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < ptrmgr->renderChunk2.size(); ++i)
    {
        //pC3DBarrierBlock block = (pC3DBarrierBlock)(ITER_REF(iter));
        auto block = ptrmgr->renderChunk2[i];
        C3DBarrierBlockUpdate(block);
    }
}

void C3DBarrierMgrProjectOrth(CVec4d vp)
{
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
    if (ptrmgr)
    {
        ptrmgr->m_2DProjectOrth = vp;
    }
}

void C3DBarrierMgrSetLonLat(CVec3d lonlat)
{
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
    auto lon1 = (int)floor(lonlat._v[0]);
    auto lat1 = (int)floor(lonlat._v[1]);
    auto lon2 = (int)floor(ptrmgr->m_aircraftLonlat._v[0]);
    auto lat2 = (int)floor(ptrmgr->m_aircraftLonlat._v[1]);

    if (ptrmgr
        && (lon1 != lon2 || lat1 != lat2)
        )
    {
        ptrmgr->m_aircraftLonlat = lonlat;
        /*计算更新区域*/
        double lonWidth = GetGlobal()->ucRange*1000.0;

        CVec3d pos = Coordinate_DegLonLatToMercator(Vec_CreatVec3d(ptrmgr->m_aircraftLonlat._v[0], ptrmgr->m_aircraftLonlat._v[1], 0.0));
        CVec3d pos2 = Vec_CreatVec3d(pos._v[0] + lonWidth, pos._v[1], 0.0);
        pos = Coordinate_MercatorToDegLonLat(pos);
        pos2 = Coordinate_MercatorToDegLonLat(pos2);
        double maxLon = pos2._v[0] - pos._v[0];

        ptrmgr->m_startLon = floor(ptrmgr->m_aircraftLonlat._v[0] - maxLon);
        ptrmgr->m_startLat = floor(ptrmgr->m_aircraftLonlat._v[1] - maxLon);
        ptrmgr->m_endLon = floor(ptrmgr->m_aircraftLonlat._v[0] + maxLon);
        ptrmgr->m_endLat = floor(ptrmgr->m_aircraftLonlat._v[1] + maxLon);

        ptrmgr->m_lonCount = ptrmgr->m_endLon - ptrmgr->m_startLon + 1;
        ptrmgr->m_latCount = ptrmgr->m_endLat - ptrmgr->m_startLat + 1;

        ptrmgr->updateNum = 1;
    }
}

void C3DBarrierMgrUpdate()
{
    /*判断是够为空*/
    pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
    CVec3d cameraLonlat = ptrmgr->m_aircraftLonlat;
    if (!ptrmgr)
    {
        return;
    }
    /*进入逻辑计算*/
    if (ptrmgr->updateNum == 1)
    {
        printf("C3DBarrierMgrUpdate");
        C3DBarrierMgrRangeCalc(cameraLonlat);
        C3DBarrierUpdateChunkAdd();
        C3DBarrierMgrUpdateChunkDelete();

        //C3DBarrierClearChunkVector(ptrmgr->tmpChunk);
        C3DBarrierClearChunkVector2(ptrmgr->tmpChunk2);
        //C3DBarrierClearChunkVector(ptrmgr->addChunk);            
        C3DBarrierClearChunkVector2(ptrmgr->addChunk2);
        //C3DBarrierClearChunkVector(ptrmgr->delChunk);            
        C3DBarrierClearChunkVector2(ptrmgr->delChunk2);
        ptrmgr->updateNum = 0;
    }

    C3DBarrierUpdateRender();

    //jy test
#if 0
    static int kk = 0;
    if (kk == 0x1)
    {
        BarrierParameters tempParam;
        pCNode pTmpGroupNode = C3DBarrierBlockCreatBarrierDrawableBillboard(&tempParam);
        CNode_FreeNode(pTmpGroupNode);
    }
    if (kk == 0x2)
    {
        int* pTemp;
        pTemp = (int*)malloc(8*1024*1024*sizeof(int));
        *pTemp = 999;
        free(pTemp);
    }
    if(kk == 0x4)
    {
        pCNode _pNodeDrawable = CNode_CreatNode(NT_Drawable);
        CNode_FreeNode(_pNodeDrawable);
    }
    if(kk == 0x3)
    {
        pCNode _pNodeGroup = CNode_CreatNode(Group);
        CNode_FreeNode(_pNodeGroup);
    }
    if(kk == 0x5)
    {
        pCNode _pNodeNode = CNode_CreatNode(Node);
        CNode_FreeNode(_pNodeNode);
    }
    if(kk == 0x6)
    {
        pCVector pv1 = CVector_Create(4096);
        CVector_Destroy(pv1);
    }
    if (kk == 0x7)
    {
        pCNode _pNodeTerrainTile = CNode_CreatNode(TerrainTile);
        CNode_FreeNode(_pNodeTerrainTile);
    }
    if (kk == 0x8)
    {
        pCNode _pNodeMatrix = CNode_CreatNode(MatrixTransform);
        CNode_FreeNode(_pNodeMatrix);
    }
#endif
}
