
#include "Drawable.h"
#include "HmsLog.h"
#include "3DAirportMgr.h"
#include "MatrixNode.h"
#include "Global.h"
#include "Coordinate.h"


static pC3DAirportMgr p2DbarrierMgr = NULL;

pC3DAirportMgr C3DAirportMgr_Create()
{
    //pC3DAirportMgr mgr = (pC3DAirportMgr)malloc(sizeof(C3DAirportMgr));
    //if (!mgr)
    //{
    //    LogWrite(LOG_LEVEL_FATAL,
    //        "严重错误 pCTerrainMgr chunk =  (pCTerrainMgr)malloc(sizeof(CTerrainMgr)) ==null ");
    //    //system("pause");//xyh 直接返回0,避免假死.
    //    return 0;
    //}
    //memset(mgr, 0, sizeof(C3DAirportMgr));

    C3DAirportMgr* mgr = NULL;
    try
    {
        mgr = new C3DAirportMgr;
    }
    catch (std::bad_alloc& e1)
    {
        HMSLOG(" pCTerrainMgr chunk =  (pCTerrainMgr)malloc(sizeof(CTerrainMgr)) ==null %s\n", e1.what());
        return NULL;
    }

    mgr->m_root = CNode_CreateNode(Node);
    CNode_SetName(mgr->m_root, "Airport root node");

    //mgr->addChunk = &(mgr->m_v_addChunk);
    //mgr->delChunk = &(mgr->m_v_delChunk);
    //mgr->renderChunk = &(mgr->m_v_renderChunk);
    //mgr->tmpChunk = &(mgr->m_v_tmpChunk);

    //c_vector_create(mgr->addChunk, 0);
    //c_vector_create(mgr->delChunk, 0);
    //c_vector_create(mgr->renderChunk, 0);
    //c_vector_create(mgr->tmpChunk, 0);

    return mgr;
}

pC3DAirportMgr GetC3DAirportMgr()
{
    if (!p2DbarrierMgr)
    {
        p2DbarrierMgr = C3DAirportMgr_Create();
    }
    return p2DbarrierMgr;
}

//unsigned char C3DAirportMgrIsContain(c_pvector chunkLonLatVector, pC3DAirportBlock chunk)
//{
//    if (chunkLonLatVector && chunk)
//    {
//        unsigned int index = 0, num = c_vector_size(chunkLonLatVector);
//        c_iterator iter, beg = c_vector_begin(chunkLonLatVector), end = c_vector_end(chunkLonLatVector);
//        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
//        {
//            pC3DAirportBlock ch = (pC3DAirportBlock)(ITER_REF(iter));
//            if (ch && (int)ch->m_lonlat._v[0] == (int)(chunk->m_lonlat._v[0]) && (int)(ch->m_lonlat._v[1]) == (int)(chunk->m_lonlat._v[1]))
//            {
//                return 1;
//            }
//        }
//    }
//    return 0;
//}

bool C3DAirportMgrIsContain(std::vector<pC3DAirportBlock>& chunkLonLatVector, pC3DAirportBlock chunk)
{
    if (!chunk)
    {
        return false;
    }

    auto num = chunkLonLatVector.size();
    for (unsigned int i = 0; i < num; ++i)
    {
        auto ch = chunkLonLatVector[i];
        if (ch
            && (int)floorf(ch->m_lonlat._v[0]) == (int)floorf(chunk->m_lonlat._v[0])
            && (int)floorf(ch->m_lonlat._v[1]) == (int)floorf(chunk->m_lonlat._v[1]))
        {
            return true;
        }
    }
    return false;
}

void C2DC3DAirportRangeCalc(void)
{
    /*判断是够为空*/
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (!ptrmgr)
    {
        return;
    }

    /*计算更新区域*/
    int centerLon;
    int centerLat;

    CVec3d lonlat = ptrmgr->m_aircraftLonlat;

    auto lonF = floor(lonlat._v[0]);
    auto latF = floor(lonlat._v[1]);

    auto perx = lonlat._v[0] - lonF;
    auto pery = lonlat._v[1] - latF;

    if (perx >= 0.5)
    {
        centerLon = lonF + 1;
    }
    else
    {
        centerLon = lonF;
    }

    if (latF >= 0.5)
    {
        centerLat = latF + 1;
    }
    else
    {
        centerLat = latF;
    }

    // 		printf("解析到经纬度%lf,%lf\n",lonlat._v[0]+(-180.0),lonlat._v[1]+(-90.0));
    // 		printf("%d,%d,   %d,%d\n",centerLon-1+(-180),centerLat+(-90),centerLon+(-180),centerLat+(-90));
    // 		printf("%d,%d,   %d,%d\n",centerLon-1+(-180),centerLat-1+(-90),centerLon+(-180),centerLat-1+(-90));
    // 		printf("over\n"); 
    ptrmgr->m_startLon = centerLon - 1;
    ptrmgr->m_startLat = centerLat - 1;
    ptrmgr->m_lonCount = 2;
    ptrmgr->m_latCount = 2;

    /*写入应该显示和需要添加的区域*/
    int curLon = 0;
    int curLat = 0;
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

            pC3DAirportBlock terrmp = C3DAirportBlock_Create();
            if (NULL == terrmp)
            {
                continue;
            }

            terrmp->m_lonlat._v[0] = curLon;
            terrmp->m_lonlat._v[1] = curLat;
            terrmp->m_lonlat._v[2] = 0.0;

            //c_vector_push_back(ptrmgr->tmpChunk, terrmp);
            ptrmgr->tmpChunk2.push_back(terrmp);

            /*检查渲染的节点是否包含，若不包含怎需要添加*/
            if (!C3DAirportMgrIsContain(ptrmgr->renderChunk2, terrmp))
            {
                pC3DAirportBlock nchunk = C3DAirportBlock_Create();
                if (nchunk)
                {
                    memcpy_op(&(nchunk->m_lonlat), &(terrmp->m_lonlat), sizeof(CVec3d));
                    //c_vector_push_back(ptrmgr->addChunk, nchunk);
                    ptrmgr->addChunk2.push_back(nchunk);
                }
            }
        }
    }

    /*计算需要删除的块*/
    //unsigned int index = 0, num = c_vector_size(ptrmgr->renderChunk);
    //c_iterator iter, beg = c_vector_begin(ptrmgr->renderChunk), end = c_vector_end(
    //    ptrmgr->renderChunk);
    //iter = c_vector_begin(ptrmgr->renderChunk);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    //{
    //    pC3DAirportBlock chunk = (pC3DAirportBlock)(ITER_REF(iter));
    //    if (chunk && !C3DAirportMgrIsContain(ptrmgr->tmpChunk2, chunk))
    //    {
    //        pC3DAirportBlock nchunk = C3DAirportBlock_Creat();
    //        if (nchunk)
    //        {
    //            memcpy_op(&(nchunk->m_lonlat), &(chunk->m_lonlat), sizeof(CVec3d));
    //            c_vector_push_back(ptrmgr->delChunk, nchunk);
    //        }
    //    }
    //}
    //////////////////////////////////////////////////////////////////////////
    for (unsigned int i = 0; i < ptrmgr->renderChunk2.size(); ++i)
    {
        pC3DAirportBlock chunk = ptrmgr->renderChunk2[i];
        if (chunk && !C3DAirportMgrIsContain(ptrmgr->tmpChunk2, chunk))
        {
            pC3DAirportBlock nchunk = C3DAirportBlock_Create();
            if (nchunk)
            {
                memcpy_op(&(nchunk->m_lonlat), &(chunk->m_lonlat), sizeof(CVec3d));
                //c_vector_push_back(ptrmgr->delChunk, nchunk);
                ptrmgr->delChunk2.push_back(nchunk);

            }
        }
    }
    //////////////////////////////////////////////////////////////////////////    
}

//void C3DAirportClearChunkVector(c_pvector chunkVector)
//{
//    if (0 == chunkVector){ return; }

//    unsigned int index = 0, num = c_vector_size(chunkVector);
//    c_iterator iter, beg = c_vector_begin(chunkVector), end = c_vector_end(chunkVector);
//    for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
//    {
//        pC3DAirportBlock chunk = (pC3DAirportBlock)(ITER_REF(iter));
//        C3DAirportBlock_Destory(&chunk);
//        chunk = 0;
//    }
//    c_vector_clear(chunkVector);
//}

#if 0
void C3DAirportMgrVectorRemove(c_pvector chunkVector, pC3DAirportBlock blk)
{
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (0 == chunkVector || 0 == blk || 0 == ptrmgr){ return; }

    unsigned int index = 0, num = c_vector_size(chunkVector);
    c_iterator iter, beg = c_vector_begin(chunkVector), end = c_vector_end(chunkVector);
    for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    {
        pC3DAirportBlock chunk = (pC3DAirportBlock)(ITER_REF(iter));
        if ((int)chunk->m_lonlat._v[0] == (int)blk->m_lonlat._v[0] &&
            (int)chunk->m_lonlat._v[1] == (int)blk->m_lonlat._v[1])
        {
            CNode_RemoveChild(ptrmgr->m_root, chunk->m_root);
            CNode_FreeNode(chunk->m_root);
            chunk->m_root = 0;
            C3DAirportBlock_Destory(&chunk);
            c_vector_erase(chunkVector, iter);
        }
    }
}
#endif

void C3DAirportClearChunkVector(std::vector<pC3DAirportBlock>& chunkVector)
{
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (NULL == ptrmgr)
    {
        return;
    }

    unsigned int num = chunkVector.size();
    for (unsigned int i = 0; i < num; ++i)
    {
        auto chunk = chunkVector[i];
        C3DAirportBlock_Destory(&chunk);
    }

    chunkVector.clear();
}

void C3DAirportMgrVectorRemove(std::vector<pC3DAirportBlock>& chunkVector, pC3DAirportBlock blk)
{
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (NULL == blk || NULL == ptrmgr)
    {
        return;
    }

    for (unsigned int i = 0; i < chunkVector.size();)
    {
        auto chunk = chunkVector[i];
        if ((int)chunk->m_lonlat._v[0] == (int)blk->m_lonlat._v[0] &&
            (int)chunk->m_lonlat._v[1] == (int)blk->m_lonlat._v[1])
        {
            CNode_DetachChild(ptrmgr->m_root, chunk->m_root);
            CNode_FreeNode(chunk->m_root);
            chunk->m_root = NULL;
            C3DAirportBlock_Destory(&chunk);
            chunkVector.erase(chunkVector.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}

void C3DAirportMgrUpdateBlockDelete()
{
    /*判断是够为空*/
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (0 == ptrmgr)
    {
        return;
    }

    unsigned int index = 0;
    //unsigned int num = c_vector_size(ptrmgr->delChunk);
    //c_iterator iter, beg = c_vector_begin(ptrmgr->delChunk), end = c_vector_end(ptrmgr->delChunk);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    //{
    //    pC3DAirportBlock chunk = (pC3DAirportBlock)(ITER_REF(iter));
    //    //Terrain2DChunkVectorRemove(ptrmgr->renderChunk,chunk->m_lonlat._v[0],chunk->m_lonlat._v[1]); 
    //    C3DAirportMgrVectorRemove(ptrmgr->renderChunk2, chunk);
    //}

    auto num = ptrmgr->delChunk2.size();
    for (unsigned int i = 0; i < num; ++i)
    {
        auto chunk = ptrmgr->delChunk2[i];
        C3DAirportMgrVectorRemove(ptrmgr->renderChunk2, chunk);
    }
}

void C3DAirportUpdateChunkAdd()
{
    /*判断是够为空*/
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (0 == ptrmgr)
    {
        return;
    }

    unsigned int index = 0;
    //unsigned int num = c_vector_size(ptrmgr->addChunk);
    //c_iterator iter, beg = c_vector_begin(ptrmgr->addChunk), end = c_vector_end(ptrmgr->addChunk);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    //{
    //    pC3DAirportBlock block = (pC3DAirportBlock)(ITER_REF(iter));
    //    pC3DAirportBlock newBlock = C3DAirportBlock_Creat();
    //    if (0 == newBlock) { continue; }

    //    C3DAirportBlockSetLonlat(newBlock, block->m_lonlat, ptrmgr->m_hpr);
    //    C3DAirportBlockReadData(newBlock);
    //    C3DAirportBlockCreatNode(newBlock);
    //    //	printf("添加block：%d，%d\n",(int)newBlock->m_lonlat._v[0],(int)newBlock->m_lonlat._v[1]);
    //    CNode_AddChild(ptrmgr->m_root, newBlock->m_root);
    //    //c_vector_push_back(ptrmgr->renderChunk, newBlock);
    //    ptrmgr->renderChunk2.push_back(newBlock);
    //}
    auto num = ptrmgr->addChunk2.size();
    for (unsigned int i = 0; i < num; ++i)
    {
        auto block = ptrmgr->addChunk2[i];
        pC3DAirportBlock newBlock = C3DAirportBlock_Create();
        if (0 == newBlock)
        {
            continue;
        }

        C3DAirportBlockSetLonlat(newBlock, block->m_lonlat, ptrmgr->m_hpr);
        C3DAirportBlockReadData(newBlock);
        C3DAirportBlockCreatNode(newBlock);
        //	printf("添加block：%d，%d\n",(int)newBlock->m_lonlat._v[0],(int)newBlock->m_lonlat._v[1]);
        CNode_AddChild(ptrmgr->m_root, newBlock->m_root);
        //c_vector_push_back(ptrmgr->renderChunk, newBlock);
        ptrmgr->renderChunk2.push_back(newBlock);
    }
}

//************************************
// Method:    IsThereRunwayNearby
// FullName:  IsThereRunwayNearby
// Access:    public 
// Desc:      within the range of any runway
// Returns:   bool, true: There's a runway within range. 
//                 false: There is no runway within range
// Qualifier:
// Parameter: const CVec3d lonlatalt
// Parameter: const double range
// Author:    yan.jiang
//************************************
bool IsThereRunwayNearby(const CVec3d lonlatalt, const double range)
{
    CVec3d pos = Coordinate_DegreeVec3dLonLatAltToVec3d(lonlatalt);

    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (0 == ptrmgr)
    {
        return false;
    }

    unsigned int index = 0;
#if 0
    unsigned int num = c_vector_size(ptrmgr->renderChunk);
    c_iterator iter;
    c_iterator beg = c_vector_begin(ptrmgr->renderChunk);
    c_iterator end = c_vector_end(ptrmgr->renderChunk);
    for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    {
        pC3DAirportBlock blk = (pC3DAirportBlock)(ITER_REF(iter));

        c_pvector vector = blk->m_drawRunwayInfo;
        unsigned int index = 0, num = c_vector_size(vector);
        c_iterator iter, beg = c_vector_begin(vector), end = c_vector_end(vector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pCDrawRunway ruw = (pCDrawRunway)(ITER_REF(iter));
            CVec3d posRunwayCenter = Coordinate_DegreeLonLatAltToVec3d(
                ruw->runway->lon, ruw->runway->lat, ruw->runway->altitude);
            CVec3d vecDistance = Vec_Vec3dSubVec3d(&pos, &posRunwayCenter);
            double distance = Vec_LengthVec3d(&vecDistance);
            if (distance < range)
            {
                return true;
            }
        }
    }
#endif
    auto num = ptrmgr->renderChunk2.size();
    for (unsigned int i = 0; i < num; ++i)
    {
        pC3DAirportBlock blk = ptrmgr->renderChunk2[i];
#if 0
        c_pvector vector = blk->m_drawRunwayInfo;
        unsigned int index = 0, num = c_vector_size(vector);
        c_iterator iter, beg = c_vector_begin(vector), end = c_vector_end(vector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pCDrawRunway ruw = (pCDrawRunway)(ITER_REF(iter));
            CVec3d posRunwayCenter = Coordinate_DegreeLonLatAltToVec3d(
                ruw->runway->lon, ruw->runway->lat, ruw->runway->altitude);
            CVec3d vecDistance = Vec_Vec3dSubVec3d(&pos, &posRunwayCenter);
            double distance = Vec_LengthVec3d(&vecDistance);
            if (distance < range)
            {
                return true;
            }
        }
#endif
        auto& vec = blk->m_drawRunwayInfo2;
        unsigned int index = 0;
        auto num = vec.size();
        for (unsigned int i = 0; i < num; ++i)
        {
            //pCDrawRunway ruw = (pCDrawRunway)(ITER_REF(iter));
            auto& ruw = vec[i];
            CVec3d posRunwayCenter = Coordinate_DegreeLonLatAltToVec3d(ruw->runway->lon, ruw->runway->lat, ruw->runway->altitude);
            CVec3d vecDistance = Vec_Vec3dSubVec3d(&pos, &posRunwayCenter);
            double distance = Vec_LengthVec3d(&vecDistance);
            if (distance < range)
            {
                return true;
            }
        }
    }

    return false;
}

void C3DAirportUpdateChunk()
{
    /*判断是够为空*/
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
}


void C3DAirportUpdateRender()
{
    /*判断是够为空*/
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (0 == ptrmgr)
    {
        return;
    }

    unsigned int index = 0;
    //unsigned int num = c_vector_size(ptrmgr->renderChunk);
    //c_iterator iter, beg = c_vector_begin(ptrmgr->renderChunk), end = c_vector_end(ptrmgr->renderChunk);
    ////	printf("渲染经纬度：%d块\n",num);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    //{
    //}
    auto num = ptrmgr->renderChunk2.size();
    for (unsigned int i = 0; i < num; ++i)
    {

    }
}

void C3DAirportMgrSetLonLat(CVec3d lonlat, CVec3d hpr)
{
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (ptrmgr)
    {
        ptrmgr->m_aircraftLonlat = lonlat;
        ptrmgr->m_hpr = hpr;
        ptrmgr->updateNum = 0;
    }
}

const double distanceThreshold = 7000.0;
void C3DAirportMgrUpdate()
{
    /*判断是够为空*/
    pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
    if (!ptrmgr)
    {
        return;
    }

    /*进入逻辑计算*/
    C2DC3DAirportRangeCalc();
    C3DAirportUpdateChunkAdd();
    C3DAirportMgrUpdateBlockDelete();

    C3DAirportClearChunkVector(ptrmgr->tmpChunk2);
    C3DAirportClearChunkVector(ptrmgr->addChunk2);
    C3DAirportClearChunkVector(ptrmgr->delChunk2);
    C3DAirportUpdateRender();

    IsThereRunwayNearby(ptrmgr->m_aircraftLonlat, distanceThreshold) ?
        GetGlobal()->bIsAnyRunwayNearby = 1
        :
        GetGlobal()->bIsAnyRunwayNearby = 0
        ;
}

