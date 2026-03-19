
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <string>
#include <cassert>
#include <vector>
#include <HmsGE/HmsLog.h>
#include "MathCommon.h"
#include "Drawable.h"
#include "AltManager.h"
#include "TerrainThread.h" 
#include "Node.h"
#include "FPPManipulator.h"
#include "FPPCamera.h"
#include "Global.h"
#include "Log.h"
#include "TextureMgr.h"
#include "Coordinate.h"
#include "CoordinateSystemMgr.h"
#include "NavSvs/mathd/Vec3d.h"

extern int svs_running;

static bool terrainTileNeedUpdateOnce = true;//true: terrain need Once Update,  false: no need update now
static int numberOfJobs = 0; //The amount of job at this point.

const TileLayer LayerMap[23] = {
    1, 32, 96000,
    0.5, 16, 58000,
    0.25, 16, 34000,
    0.125, 16, 15000,
    0.0625, 16, 8000,
    0.03125, 16, 4000,
    0.015625, 16, 2000,
    0.0078125, 8, 1000,
    0.00390625, 8, 500,
    0.001953125, 8, 400,
    0.0009765625, 4, 300,
    0.00048828125, 4, 26.9,
    0.000244140625, 4, 13.4,
    0.0001220703125, 4, 6.7,
    0.00006103515625, 4, 0,
};

const TileLayer LayerMap1[23] = {
    1, 32, 96000,
    0.5, 16, 58000,
    0.25, 16, 34000,
    0.125, 16, 15000,
    0.0625, 16, 8000,
    0.03125, 16, 4000,
    0.015625, 16, 2000,
    0.0078125, 8, 1000,
    0.00390625, 8, 500,
    0.001953125, 8, 400,
    0.0009765625, 8, 300,
    0.00048828125, 8, 26.9,
    0.000244140625, 8, 13.4,
    0.0001220703125, 8, 6.7,
    0.00006103515625, 8, 0,
};

static const int TERRAIN_TILE = 32;

static pCTerrainThread terrainMgr3D = nullptr;

static CVec3f sunlight_dir = { { -1, 1, 1 } };

#if 0
void CTerrainManager_DrawTerrain()
{
    pCImage image = TextureMgr_GetImage(TextureType_3DTerrain_NEW);
}
#endif

enum enTerrEventType
{
    enAddTopTile,//top terrain tile add
    enDelTopTile,//top terrain tile delete
    enDivideTile,//one terrain tile divided into 4 child terrain tiles
    enMergeTile//4 terrain tiles merge into one parent tile
};

struct struTileJob
{
    int id;//useless now
    char msg[64];
    enTerrEventType type;
    pCTerrainTile pTerrTile;
    pToChildNode pTochild;
    pToParentNode pToparent;
};

std::mutex mutex_main2sub;
std::queue<struTileJob> queue_main2sub;

std::mutex mutex_sub2main;
std::queue<struTileJob> queue_sub2main;

pCTerrainThread C3DTerrainManager_GetTerrainMgr()
{
    if (terrainMgr3D == nullptr)
    {
        terrainMgr3D = C3DTerrainManager_Create();
    }
    return terrainMgr3D;
}

#if 0
int PointBetweenTwoLines(pCVec2d point, pline2d line1, pline2d line2)
{
    double value1, value2;
    value1 = Line_DotProduce2d(line1, point);
    value2 = Line_DotProduce2d(line2, point);
    return value1*value2 < 0.0;
}

void C3DTerrainManager_CreateVeiwProjection()
{
    pCTerrainThread ptmgr = C3DTerrainManager_GetTerrainMgr();
    pGLOBAL g = GetGlobal();
    CVec3d ypr = g->vYawPitchRoll, l;
    CVec2d v0, v1, s, e;
    float yaw, fov, thea0, thea1;
    double len = 3.0;//长度为4度
    yaw = ypr._v[0];
    fov = g->fovX;
    thea0 = yaw + fov;
    thea1 = yaw - fov;
    v0 = Vec_CreatVec2d(cos(thea0*DegreesToRadians), sin(thea0*DegreesToRadians));
    v1 = Vec_CreatVec2d(cos(thea1*DegreesToRadians), sin(thea1*DegreesToRadians));
    l = g->vLonLatAlt;

    s = Vec_CreatVec2d(l._v[0], l._v[1]);
    e = Vec_CreatVec2d(l._v[0] + v0._v[0] * len, l._v[1] + v0._v[1] * len);
    ptmgr->viewprojection[0] = Line_CreateLine2d(&s, &e);
    e = Vec_CreatVec2d(l._v[0] + v1._v[0] * len, l._v[1] + v1._v[1] * len);
    ptmgr->viewprojection[1] = Line_CreateLine2d(&s, &e);
}

CVec2d GetRectPoint(pCVec2d start, double w, double h)
{
    CVec2d s;
    s = Vec_CreatVec2d(start->_v[0] + w, start->_v[1] + h);
    return s;
}

void C3DTerrainManager_RegisitTerrain()
{
    pCTerrainThread ptmgr = C3DTerrainManager_GetTerrainMgr();
    int a = 0, b = 0;
    if (ptmgr)
    {
        int i, j;

        pCNode root = ptmgr->m_root;
        CVec2d r, st;
        int terrain_num = CNode_GetNumberChildren(root);
        int tile_num, v1, v2, v3, v4, w, h;//display, 

        C3DTerrainManager_CreateVeiwProjection();

        for (i = 0; i < terrain_num; i++)
        {
            pCNode n = CNode_GetChildByIndex(root, i);
            pCTerrainTile tile = (pCTerrainTile)(n->pNodeData);
            tile_num = CNode_GetNumberChildren(tile->node);
            for (j = 0; j < tile_num; j++)
            {
                pCMatrixTransform mt = (pCMatrixTransform)(CNode_GetChildByIndex(n, j)->pNodeData);
                st = Vec_CreatVec2d(mt->rect._v[0], mt->rect._v[1]);
                w = mt->rect._v[2] - mt->rect._v[0];
                h = mt->rect._v[3] - mt->rect._v[1];
                v1 = PointBetweenTwoLines(&st, &(ptmgr->viewprojection[0]), &(ptmgr->viewprojection[1]));
                r = GetRectPoint(&st, w, 0);
                v2 = PointBetweenTwoLines(&r, &(ptmgr->viewprojection[0]), &(ptmgr->viewprojection[1]));
                r = GetRectPoint(&st, 0, h);
                v3 = PointBetweenTwoLines(&r, &(ptmgr->viewprojection[0]), &(ptmgr->viewprojection[1]));
                r = GetRectPoint(&st, w, h);
                v4 = PointBetweenTwoLines(&r, &(ptmgr->viewprojection[0]), &(ptmgr->viewprojection[1]));

                mt->display = !(v1 || v2 || v3 || v4);
            }
        }
    }
}
#endif

pCTerrainThread C3DTerrainManager_Create()
{
    auto p = new CTerrainThread;

    return p;
}

void C3DTerrainManager_SetLonLatAlt(const CVec3d  lla)
{
    pCTerrainThread p = C3DTerrainManager_GetTerrainMgr();
    if (p)
    {
        /*CLock lock(m_mutex);*/
        p->m_lla = lla;
        p->m_eyePos = Coordinate_DegreeVec3dLonLatAltToVec3d(lla);
    }
}


//************************************
// Method:    C3DTerrainManager_FindChildTer
// FullName:  C3DTerrainManager_FindChildTer
// Access:    public 
// Desc:      
// Returns:   bool，  true 眼睛与 此tile的父tile的中心距离大于上一级的阈值， false 不大于
// Qualifier:
// Parameter: pCTerrainThread p
// Parameter: pCTerrainTile root
// Parameter: unsigned char layer
//************************************
bool C3DTerrainManager_FindChildTer(pCTerrainThread pTerrainMgr, pCTerrainTile curTerrainTile, unsigned char layer)
{
    CVec3d lenVec3d;
    pCNode pnode = nullptr;
    pCTerrainTile ptile = nullptr;
    unsigned int i;

    if (CTerrainTile_HasChildTer(curTerrainTile))
    {
        bool needToParent = 0;
        //c_pvector childNodeArrary = curTerrainTile->node->childrenArrary;
        //unsigned int sizeNum = c_vector_size(childNodeArrary);
        unsigned int sizeNum = curTerrainTile->node ? curTerrainTile->node->childrenArrary2.size() : 0;
        for (i = 0; i < sizeNum; i++)
        {
            //pnode = (CNode_GetChildByIndex(curTerrainTile->node, i));
            pnode = curTerrainTile->node->childrenArrary2[i];
            if (pnode && pnode->type == TerrainTile)
            {
                ptile = (pCTerrainTile)pnode->pNodeData;
                if (C3DTerrainManager_FindChildTer(pTerrainMgr, ptile, (unsigned char)(layer + 1)))
                {
                    needToParent = true;
                }
            }
            else
            {
                printf("error node type in C3DTerrainManager_FindChildTer\n");
            }
        }
        if (needToParent)
        {
            pTerrainMgr->m_needCreatedP2.push_back(curTerrainTile);
        }
    }
    else
    {
        //对于叶子节点，根据与眼睛的距离，判断是向下发展出4个孩子节点还是向上退化成父节点
        if (layer < 8)
        {
            CVec3d curTileCenterPos = CTerrainTile_GetCenter(curTerrainTile);
            pTerrainMgr->m_curLayer = TAWS_MAX(pTerrainMgr->m_curLayer, layer);
            lenVec3d = Vec_Vec3dSubVec3d(&pTerrainMgr->m_eyePos, &curTileCenterPos);

            double len = Vec_LengthVec3d(&lenVec3d);

            if (len < LayerMap[layer].minDistance)
            {//jy距离低于阈值，则生长出子节点
                auto terNode = (pToChildNode)malloc(sizeof(ToChildNode));
                unsigned long mask = curTerrainTile->m_mask;/* root->getMask();*/

                terNode->parent = curTerrainTile;
                for (i = 0; i < 4; i++)
                {
                    terNode->child[i] = (pCTerrainTile)(CNode_CreateNode(TerrainTile)->pNodeData);

                    CTerrainTile_SetLonLat(terNode->child[i], curTerrainTile->m_lon, curTerrainTile->m_lat);

                    terNode->child[i]->m_parentsId = curTerrainTile->m_UniqueId;
                    CTerrainTile_SetParentIdString(terNode->child[i], curTerrainTile->m_IdString);
                    CTerrainTile_SetLodAndId(terNode->child[i], layer + 1, i);

                    CTerrainTile_SetLayer(&mask, (unsigned char)(layer + 1));
                    CTerrainTile_SetCount(&mask, (unsigned char)i, (unsigned char)(layer + 1));
                    terNode->child[i]->m_mask = mask;
                    terNode->child[i]->m_layer = layer + 1;
#if 0
                    CVec3d lonlat = CTerrainTile_GetLonLat(curTerrainTile);
                    if (lonlat._v[0] >= 180.0 || lonlat._v[1] >= 90.0)
                    {
                        if (CNode_FreeNode(terNode->child[i]->node))
                        {
                            terNode->child[i] = NULL;
                        }
                    }
#endif
                }
                //	printf("\r Creat Layer：%d\n", layer); 
                //c_vector_push_back(pTerrainMgr->m_needCreatedC, terNode);
                pTerrainMgr->m_needCreatedC2.push_back(terNode);
            }
            else
            {//jy距离未低于阈值,或不在视野范围内，则判断距离是否大于上一级的阈值。若大于则加载父节点，若不大于则不作变化 return 0

                //c_iterator ite = c_vector_idfind(curTerrainTile->node->parentsArrary, 0);
                //pnode = (pCNode)(ITER_REF(ite));
                //if (pnode && pnode->type == TerrainTile)
                //{
                //    ptile = (pCTerrainTile)(pnode->pNodeData);
                //    if (ptile)
                //    {
                //        lenVec3d = CTerrainTile_GetCenter(ptile);
                //        lenVec3d = Vec_Vec3dSubVec3d(&pTerrainMgr->m_eyePos, &lenVec3d);
                //        len = Vec_LengthVec3d(&lenVec3d);
                //        if (len >= LayerMap[layer - 1].minDistance)//判断距离是否大于上一级的阈值
                //        {
                //            return 1;
                //        }
                //    }
                //}
                if (!curTerrainTile->node->parentsArray2.empty())
                {
                    pnode = curTerrainTile->node->parentsArray2[0];
                    if (pnode && pnode->type == TerrainTile)
                    {
                        ptile = (pCTerrainTile)(pnode->pNodeData);
                        if (ptile)
                        {
                            lenVec3d = CTerrainTile_GetCenter(ptile);
                            lenVec3d = Vec_Vec3dSubVec3d(&pTerrainMgr->m_eyePos, &lenVec3d);
                            len = Vec_LengthVec3d(&lenVec3d);
                            if (len >= LayerMap[layer - 1].minDistance)//判断距离是否大于上一级的阈值
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

//************************************
// Method:    C3DTerrainManager_updateMount
// FullName:  C3DTerrainManager_updateMount
// Access:    public 
// Desc:      update the Terrain Render Tree
// Returns:   void
// Qualifier:
// Author:    
//************************************
void C3DTerrainManager_updateMount()
{
    pCTerrainThread pTerrainMgr = C3DTerrainManager_GetTerrainMgr();
    if (nullptr == pTerrainMgr)
    {
        return;
    }

    //c_iterator iter;
    //c_iterator beg;
    //c_iterator end;
    unsigned int num;

    /* 1. 删除需要删除的 top tile */
    //num = c_vector_size(pTerrainMgr->m_needDeleted);
    num = pTerrainMgr->m_needDeleted2.size();
    if (num > 0)
    {
        //beg = c_vector_begin(pTerrainMgr->m_needDeleted);
        //end = c_vector_end(pTerrainMgr->m_needDeleted);
        //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        //{
        //    pCTerrainTile pTile = (pCTerrainTile)(ITER_REF(iter));

        //    assert(pTile);

        //    CNode_RemoveChild(pTerrainMgr->m_root, pTile->node);
        //    CNode_FreeNode(pTile->node);
        //    pTile = NULL;
        //}
        //c_vector_clear(pTerrainMgr->m_needDeleted);

        for (unsigned int i = 0; i < num; ++i)
        {
            auto pTile = pTerrainMgr->m_needDeleted2[i];
            assert(pTile);

            CNode_DetachChild(pTerrainMgr->m_root, pTile->node);
            CNode_FreeNode(pTile->node);
            pTile = nullptr;
        }
        pTerrainMgr->m_needDeleted2.clear();
    }

    /* 2. 增加要显示的 top tile */
    //num = c_vector_size(pTerrainMgr->m_hasAdded);
    num = pTerrainMgr->m_hasAdded2.size();
    if (num > 0)
    {
        //beg = c_vector_begin(pTerrainMgr->m_hasAdded);
        //end = c_vector_end(pTerrainMgr->m_hasAdded);
        //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        //{
        //    pCTerrainTile pTile = (pCTerrainTile)(ITER_REF(iter));

        //    assert(pTile);

        //    CNode_AddChild(pTerrainMgr->m_root, pTile->node);
        //}
        //c_vector_clear(pTerrainMgr->m_hasAdded);

        for (unsigned int i = 0; i < num; i++)
        {
            auto pTile = pTerrainMgr->m_hasAdded2[i];

            assert(pTile);

            CNode_AddChild(pTerrainMgr->m_root, pTile->node);
        }
        pTerrainMgr->m_hasAdded2.clear();
    }

    /* 3. terrain tile split. one parent tile splits into four smaller children tile */
    //num = c_vector_size(pTerrainMgr->m_hasCreatedC);
    num = pTerrainMgr->m_hasCreatedC2.size();
    if (num > 0)
    {
        //beg = c_vector_begin(pTerrainMgr->m_hasCreatedC);
        //end = c_vector_end(pTerrainMgr->m_hasCreatedC);
        //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        //{
        //    pToChildNode pToChild = (pToChildNode)(ITER_REF(iter));
        //    /*printf("SVS: Add terrain of layer %d\n",CTerranTile_getla toChild->parent-> );*/
        //    //printf("\n[SVS] Add terrain of layer %d", pTerrainMgr->m_curLayer);

        //    assert(pToChild);

        //    CTerrainTile_RemoveTerrain(pToChild->parent);

        //    CNode_AddChild(pToChild->parent->node, pToChild->child[0]->node);
        //    CNode_AddChild(pToChild->parent->node, pToChild->child[1]->node);
        //    CNode_AddChild(pToChild->parent->node, pToChild->child[2]->node);
        //    CNode_AddChild(pToChild->parent->node, pToChild->child[3]->node);

        //    free(pToChild);
        //}
        //c_vector_clear(pTerrainMgr->m_hasCreatedC);

        for (unsigned int i = 0; i < num; i++)
        {
            auto pToChild = pTerrainMgr->m_hasCreatedC2[i];

            assert(pToChild);

            CTerrainTile_RemoveTerrain(pToChild->parent);

            CNode_AddChild(pToChild->parent->node, pToChild->child[0]->node);
            CNode_AddChild(pToChild->parent->node, pToChild->child[1]->node);
            CNode_AddChild(pToChild->parent->node, pToChild->child[2]->node);
            CNode_AddChild(pToChild->parent->node, pToChild->child[3]->node);

            free(pToChild);
        }
        pTerrainMgr->m_hasCreatedC2.clear();
    }

    /* 4. four children tile merge into one lager parent tile */
    //num = c_vector_size(pTerrainMgr->m_hasCreatedP);
    num = pTerrainMgr->m_hasCreatedP2.size();
    if (num > 0)
    {
        //beg = c_vector_begin(pTerrainMgr->m_hasCreatedP);
        //end = c_vector_end(pTerrainMgr->m_hasCreatedP);
        //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        //{
        //    pToParentNode pToParent = (pToParentNode)(ITER_REF(iter));

        //    assert(pToParent);

        //    CTerrainTile_SetTerrain(pToParent->parent, (pCMatrixTransform)(pToParent->pNodeMat->pNodeData));
        //    /*toChild->parent->setTerrain(toChild->matT);*/
        //    free(pToParent);
        //}
        //c_vector_clear(pTerrainMgr->m_hasCreatedP);

        for (unsigned int i = 0; i < num; i++)
        {
            auto pToParent = pTerrainMgr->m_hasCreatedP2[i];
            assert(pToParent);

            CTerrainTile_SetTerrain(pToParent->parent, (pCMatrixTransform)(pToParent->pNodeMat->pNodeData));
            /*toChild->parent->setTerrain(toChild->matT);*/
            free(pToParent);
        }
        pTerrainMgr->m_hasCreatedP2.clear();
    }
}


void RequestAddTopTile(pCTerrainTile ptile)
{
    static int id = 0;
    id++;
    numberOfJobs++;
    struct struTileJob job = { id, "add top tile", enAddTopTile, ptile, nullptr, nullptr };
    mutex_main2sub.lock();
    queue_main2sub.push(job);
    mutex_main2sub.unlock();
}

void RequestDelTopTile(pCTerrainTile ptile)
{
    static int id = 0;
    id++;
    numberOfJobs++;
    struct struTileJob job = { id, "del top tile", enDelTopTile, ptile, nullptr, nullptr };
    mutex_main2sub.lock();
    queue_main2sub.push(job);
    mutex_main2sub.unlock();
}

void RequestDivideTile(pToChildNode ptochild)
{
    static int id = 0;
    id++;
    numberOfJobs++;
    struct struTileJob job = { id, "divide tile", enDivideTile, nullptr, ptochild, nullptr };
    mutex_main2sub.lock();
    queue_main2sub.push(job);
    mutex_main2sub.unlock();
}

void RequestMergeTile(pCTerrainTile pTile)
{
    static int id = 0;
    id++;
    numberOfJobs++;
    struct struTileJob job = { id, "merge tile", enMergeTile, pTile, nullptr, nullptr };
    mutex_main2sub.lock();
    queue_main2sub.push(job);
    mutex_main2sub.unlock();
}


//void SendLoadTerrainDataRequest(int id, std::string msg)
//{
//	mutex_main2sub.lock();
//	queue_main2sub.push({ id, msg, enAddTopTile, nullptr, nullptr, nullptr });
//	mutex_main2sub.unlock();
//}

struTileJob AddTopTile(struTileJob jobMain2Sub)
{
    pCTerrainTile ptile = jobMain2Sub.pTerrTile;
    CVec3d lonlat = CTerrainTile_GetLonLat(ptile);
    pCMatrixTransform pmatrixtransform = C3DTerrainManager_CreateTile(ptile, lonlat._v[0], lonlat._v[1], LayerMap[0].unitCellCount, LayerMap[0].tileSpan, 0);
    if (pmatrixtransform)
    {
        CNode_SetName(pmatrixtransform->node, "TitleRoot");
    }
    else
    {
        HMSLOG("AddTopTile C3DTerrainManager_CreateTile get NULL\n");
    }
    CTerrainTile_SetTerrain(ptile, pmatrixtransform);

    struTileJob jobSub2Main = jobMain2Sub;

    return jobSub2Main;
}

struTileJob DelTopTile(struTileJob jobMain2Sub)
{
    struTileJob jobSub2Main = jobMain2Sub;
    return jobSub2Main;
}

struTileJob DivideTile(struTileJob jobMain2Sub)
{
    pToChildNode pToChild = jobMain2Sub.pTochild;

    for (auto pChildTile : pToChild->child)
    {
        if (pChildTile != nullptr)
        {
            CVec3d lonlat;
            unsigned char layer;
            CTerrainTile_GetLonLatLayerByMask(pChildTile, pChildTile->m_mask, &lonlat, &layer);
            pCMatrixTransform pMatrixtransform = C3DTerrainManager_CreateTile(pChildTile, lonlat._v[0], lonlat._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);

            char namebuff[32] = { 0 };
            sprintf(namebuff, "%d,%d,layer:%d", (int)lonlat._v[0], (int)lonlat._v[1], pChildTile->m_layer);
            if (pMatrixtransform)
            {
                CNode_SetName(pMatrixtransform->node, namebuff);
            }
            else
            {
                HMSLOG("DivideTile C3DTerrainManager_CreateTile get NULL\n");
            }

            CTerrainTile_SetTerrain(pChildTile, pMatrixtransform);
        }
    }

    struTileJob jobSub2Main = jobMain2Sub;
    return jobSub2Main;
}

struTileJob MergeTile(struTileJob jobMain2Sub)
{
    pCTerrainTile pTile = jobMain2Sub.pTerrTile;

    auto pToParentNew = (pToParentNode)malloc(sizeof(ToParentNode));
    CVec3d lonlat;
    pCMatrixTransform pMatrixtransform = nullptr;
    unsigned char layer;
    pToParentNew->parent = pTile;

    CTerrainTile_GetLonLatLayerByMask(pTile, pTile->m_mask, &lonlat, &layer);

    pMatrixtransform = C3DTerrainManager_CreateTile(pTile, lonlat._v[0], lonlat._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);
    if (pMatrixtransform)
    {
        pToParentNew->pNodeMat = pMatrixtransform->node;
    }
    else
    {
        HMSLOG("MergeTile C3DTerrainManager_CreateTile get NULL\n");
    }

    struTileJob jobSub2Main = jobMain2Sub;
    jobSub2Main.pTerrTile = nullptr;
    jobSub2Main.pToparent = pToParentNew;
    return jobSub2Main;
}

//************************************
// Method:    SvsTerrainDataService
// FullName:  SvsTerrainDataService
// Access:    public 
// Desc:      
// Returns:   void
// Qualifier:
// Parameter: void
// Author:    yan.jiang
//************************************
void SvsTerrainDataService()
{
    while (svs_running)
    {
        if (!queue_main2sub.empty())
        {
            mutex_main2sub.lock();

            struTileJob jobMain2Sub = queue_main2sub.front();
            queue_main2sub.pop();
            mutex_main2sub.unlock();

            //std::cout << "[sub rcv]" << "qs" << queue_main2sub.size()<<",id" << jobMain2Sub.id << ",msg=" << jobMain2Sub.msg << std::endl;
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));//simulate a long time job			

            struTileJob jobSub2Main = {};
            //process tile action
            switch (jobMain2Sub.type)
            {
            case enAddTopTile:
                jobSub2Main = AddTopTile(jobMain2Sub);
                break;
            case enDelTopTile:
                jobSub2Main = DelTopTile(jobMain2Sub);
                break;
            case enDivideTile:
                jobSub2Main = DivideTile(jobMain2Sub);
                break;
            case enMergeTile:
                jobSub2Main = MergeTile(jobMain2Sub);
                break;
            default:
                std::cout << "error tile action type." << std::endl;
                break;
            }

            //memcpy(jobMain2Sub.msg, "DONE", sizeof(jobMain2Sub.msg) - 1);
            //job done, send job reault
            mutex_sub2main.lock();
            queue_sub2main.push(jobSub2Main);
            mutex_sub2main.unlock();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //ATTENTION: If there is no delay, this sub thread can not quit in linux, but perfectly normal in widows.
        //The root course should be found out. 
    }
}

//************************************
// Method:    CreateVboForMatrixTransformNode
// FullName:  CreateVboForMatrixTransformNode
// Access:    public 
// Desc:      The type of pNodeMat must be MatrixTransform. This function only for terrain's MatrixTransform node.
// Returns:   void
// Qualifier:
// Parameter: pCNode pNodeMat
// Author:    yan.jiang
//************************************
void CreateVboForMatrixTransformNode(pCNode pNodeMat)
{
    if (pNodeMat == nullptr || pNodeMat->pNodeData == nullptr || pNodeMat->type != MatrixTransform)
    {
        std::cout << "MatrixTransform node error." << std::endl;
        return;
    }

    unsigned int index = 0;
    //unsigned int num = c_vector_size(pNodeMat->childrenArrary);
    //c_iterator iter, beg = c_vector_begin(pNodeMat->childrenArrary), end = c_vector_end(pNodeMat->childrenArrary);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (auto pDrawableNode : pNodeMat->childrenArrary2)
    {
        //pCNode pDrawableNode = (pCNode)(ITER_REF(iter));
        if (pDrawableNode->type == NT_Drawable)
        {
            pCDrawable pdrawable = (pCDrawable)pDrawableNode->pNodeData;
            CDrawable_CreateGLBuffers(pdrawable);
        }
        else
        {
            std::cout << "TerrainTile node structure error." << std::endl;
        }
        index++;
    }
}

//************************************
// Method:    CreateVboForTerrainTile
// FullName:  CreateVboForTerrainTile
// Desc:	  
//
//			TerrainTile
//				|
//		  MatrixTransform
//			|		|
//		Drawable	Drawable
//
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: pCTerrainTile pTile
// Author:    jy
//************************************
void CreateVboForTerrainTile(pCTerrainTile pTile)
{
    if (pTile == nullptr)
    {
        std::cout << "CreateVboForTerrainTile Failed." << std::endl;
        return;
    }

    CreateVboForMatrixTransformNode(pTile->m_terrain);
}

void PostProcForAddTopTile(struTileJob job)
{
    pCTerrainTile pTile = job.pTerrTile;

    assert(pTile);

    pCTerrainThread pTerrainMgr = C3DTerrainManager_GetTerrainMgr();

    //create VBO
    CreateVboForTerrainTile(pTile);

    //c_vector_push_back(pTerrainMgr->m_hasAdded, pTile);
    pTerrainMgr->m_hasAdded2.push_back(pTile);
}

void PostProcForDelTopTile(struTileJob job)
{
    pCTerrainTile pTile = job.pTerrTile;

    assert(pTile);

    pCTerrainThread pTerrainMgr = C3DTerrainManager_GetTerrainMgr();

    //c_vector_push_back(pTerrainMgr->m_needDeleted, pTile);
    pTerrainMgr->m_needDeleted2.push_back(pTile);
}

void PostProcForDivideTile(struTileJob job)
{
    pToChildNode pToChild = job.pTochild;

    assert(pToChild);

    pCTerrainThread pTerrainMgr = C3DTerrainManager_GetTerrainMgr();

    //create VBO		
    CreateVboForTerrainTile(pToChild->child[0]);
    CreateVboForTerrainTile(pToChild->child[1]);
    CreateVboForTerrainTile(pToChild->child[2]);
    CreateVboForTerrainTile(pToChild->child[3]);

    //printf("\n[SVS] Add terrain of layer %d", pTerrainMgr->m_curLayer);
    //c_vector_push_back(pTerrainMgr->m_hasCreatedC, pToChild);
    pTerrainMgr->m_hasCreatedC2.push_back(pToChild);
}

void PostProcForMergeTile(struTileJob job)
{
    pToParentNode pToParent = job.pToparent;

    assert(pToParent);

    pCTerrainThread pTerrainMgr = C3DTerrainManager_GetTerrainMgr();

    //create VBO	
    CreateVboForMatrixTransformNode(pToParent->pNodeMat);

    //c_vector_push_back(pTerrainMgr->m_hasCreatedP, pToParent);
    pTerrainMgr->m_hasCreatedP2.push_back(pToParent);
}

void ReceiveTileJobResult()
{
    while (!queue_sub2main.empty())
    {
        mutex_sub2main.lock();
        struTileJob job = queue_sub2main.front();
        queue_sub2main.pop();
        mutex_sub2main.unlock();
        //std::cout << "[main rcv]qs=" << queue_sub2main.size() << "id=" << job.id << ",msg=" << job.msg << std::endl;

        switch (job.type)
        {
        case enAddTopTile:
            PostProcForAddTopTile(job);
            break;
        case enDelTopTile:
            PostProcForDelTopTile(job);
            break;
        case enDivideTile:
            PostProcForDivideTile(job);
            break;
        case enMergeTile:
            PostProcForMergeTile(job);
            break;
        default:
            std::cout << "error tile action type." << std::endl;
            break;
        }
        numberOfJobs--;
    }

    if (numberOfJobs <= 0)
    {
        //LogWrite(LOG_LEVEL_ALWAYS,"update t tree\n");
        //synchronous update terrain render tree
        C3DTerrainManager_updateMount();
        terrainTileNeedUpdateOnce = true;
    }
}

//************************************
// Method:    CalcTerrainTile
// FullName:  CalcTerrainTile
// Desc:	  Calculate the changes in the terrain. such as, terrain is added, removed, refined, and coarsened.
//			The output is four vectors. They are: m_needAdded, m_needDeleted, m_needCreatedC, m_needCreatedP.
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: void
// Author:    jy
//************************************
bool CalcTerrainTile()
{
    pCTerrainThread pTerrainMgr = C3DTerrainManager_GetTerrainMgr();

    if (!pTerrainMgr->m_hasCreatedC2.empty() || !pTerrainMgr->m_hasCreatedP2.empty() || !pTerrainMgr->m_hasAdded2.empty())
    {
        return false;
    }

    /*计算需要被添加的地形块*/
    CVec3d lonlatdir = Vec_CreatVec3d(1, 1, 0);
    CVec3d lla = Vec_Vec3dSubVec3d(&(pTerrainMgr->m_lla), &lonlatdir);


    CVec3d pos = Coordinate_DegreeVec3dLonLatAltToVec3d(lla);
    auto tmplon = (int)floorf(pTerrainMgr->m_lla._v[0]);
    if (pTerrainMgr->m_lon != tmplon)
    {
        //更新虚拟阳光方向，使当前地表始终被照亮
        auto o_tile = HmsAviPf::Vec3d(pos._v[0], pos._v[1], pos._v[2]);
        o_tile.normalize();
        auto o_northPole = HmsAviPf::Vec3d(0, 0, 1);
        HmsAviPf::Vec3d o_alpha;
        HmsAviPf::Vec3d::cross(o_northPole, o_tile, &o_alpha);//右手定则
        auto o_sun = o_tile + o_alpha;
        o_sun.normalize();
        sunlight_dir._v[0] = o_sun.x;
        sunlight_dir._v[1] = o_sun.y;
        sunlight_dir._v[2] = o_sun.z;
    }

    pTerrainMgr->m_lon = tmplon;
    pTerrainMgr->m_lat = (int)floorf(pTerrainMgr->m_lla._v[1]);

    CVec3d dir1 = Vec_Vec3dSubVec3d(&pos, &pTerrainMgr->m_eyePos);//jy：这样算出来的dir1，垂直方向上指向海拔更低的方向，因此后面算出来的pos1海拔低于pos2海拔
    Vec_NormalizeVec3d(&dir1);

    CVec3d maxPos = dir1;

    Vec_Vec3dMulDouble(&maxPos, MAXTERRAIN);

    CVec3d pos1 = Vec_Vec3dAddVec3d(&pTerrainMgr->m_eyePos, &maxPos);

    CVec3d ll = Coordinate_Vec3dToDegreeLonlatAltVec3d(pos1);
    int sLon = (int)floorf((float)ll._v[0]);
    int sLat = (int)floorf((float)ll._v[1]);
    int eLon = (int)floorf((float)ll._v[0]) + 2;
    int eLat = (int)floorf((float)ll._v[1]) + 2;
    unsigned int lastTopTileCount = CNode_GetNumberChildren(pTerrainMgr->m_root);
    if (lastTopTileCount > 9)
    {
        HMSLOG("Top Terrain Tile count error, bigger than 9\n");
        return false;
    }

    /*初始化*/
    for (unsigned int i = 0; i < lastTopTileCount; i++)
    {
        pTerrainMgr->lastTopTerrainTiles[i].pTerTile = nullptr;
        pTerrainMgr->lastTopTerrainTiles[i].needKeep = false;

        pCNode node = CNode_GetChildByIndex(pTerrainMgr->m_root, i);
        if (!node)
        {
            continue;
        }
        pTerrainMgr->lastTopTerrainTiles[i].pTerTile = (pCTerrainTile)(node->pNodeData);//装载上一帧的tile
    }

    for (int s = sLon; s <= eLon; s++)
    {
        for (int t = sLat; t <= eLat; t++)
        {
            auto _lon = s;
            auto _lat = t;

            if (_lat < -90 || _lat >= 90)
            {
                continue;
            }
            if (_lon < -180)
            {
                _lon += 360;
            }
            if (_lon >= 180)
            {
                _lon -= 360;
            }

            unsigned int i = 0;
            while (i < lastTopTileCount)
            {
                if (pTerrainMgr->lastTopTerrainTiles[i].pTerTile)
                {
                    int tLon = pTerrainMgr->lastTopTerrainTiles[i].pTerTile->m_lon;
                    int tLat = pTerrainMgr->lastTopTerrainTiles[i].pTerTile->m_lat;
                    if (tLon == _lon && tLat == _lat)
                    {
                        pTerrainMgr->lastTopTerrainTiles[i].needKeep = true;//若本帧需要的tile存在于上一帧的tile中，则置needKeep
                        break;
                    }
                }
                i++;
            }

            if (i >= lastTopTileCount)//本帧的此地块不存在于上帧最大地块子节点中，需要添加到needAdded中
            {
                unsigned int m_needAddedSize = 0;
                auto ter = (pCTerrainTile)(CNode_CreateNode(TerrainTile)->pNodeData);
                CTerrainTile_SetLonLat(ter, _lon, _lat);
                CTerrainTile_SetMask(ter, 0);

                ter->m_parentsId = 0;
                ter->m_Lod = 0;
                ter->m_UniqueId = 0;

                /*需要创建这些0级别地块*/
                pTerrainMgr->m_needAdded2.push_back(ter);
                m_needAddedSize = pTerrainMgr->m_needAdded2.size();
                printf("m_needAddedSize:%d\n", m_needAddedSize);
            }
        }
    }

    pTerrainMgr->m_curLayer = 0;

    /*遍历每一个地块，自顶向下遍历*/
    for (unsigned int i = 0; i < lastTopTileCount; i++)//遍历最大地块
    {
        if (pTerrainMgr->lastTopTerrainTiles[i].pTerTile)
        {
            //若上一帧中的此tile被置成needKeep，表示需要进一步对此tile进行更新，比如显示更深的tile level
            if (pTerrainMgr->lastTopTerrainTiles[i].needKeep)
            {
                C3DTerrainManager_FindChildTer(pTerrainMgr, pTerrainMgr->lastTopTerrainTiles[i].pTerTile, 0);
            }
            else
            {
                //若上一帧中的此tile没有被置成 needKeep，表示此tile不在本帧显示范围内，需要删除掉
                RequestDelTopTile(pTerrainMgr->lastTopTerrainTiles[i].pTerTile);
            }
        }
    }

    /*重置*/
    for (unsigned int i = 0; i < lastTopTileCount; i++)
    {
        pTerrainMgr->lastTopTerrainTiles[i].pTerTile = nullptr;
        pTerrainMgr->lastTopTerrainTiles[i].needKeep = false;
    }

    //得到虚拟太阳方向，始终在飞机上方，始终为白天


    return true;
}

bool InitiateTerrainTileUpdateRequest()
{
    if (!CalcTerrainTile())
    {
        return false;
    }

    pCTerrainThread pTerrainMgr = C3DTerrainManager_GetTerrainMgr();

    for (auto ptile : pTerrainMgr->m_needAdded2)
    {
        RequestAddTopTile(ptile);
    }
    pTerrainMgr->m_needAdded2.clear();

    for (auto pToChild : pTerrainMgr->m_needCreatedC2)
    {
        RequestDivideTile(pToChild);
    }
    pTerrainMgr->m_needCreatedC2.clear();

    for (auto pTile : pTerrainMgr->m_needCreatedP2)
    {
        RequestMergeTile(pTile);
    }
    pTerrainMgr->m_needCreatedP2.clear();

    return true;
}

void C3DTerrainManager_UpdateTerrain_multiThread()
{
    if (terrainTileNeedUpdateOnce)
    {
        terrainTileNeedUpdateOnce = false;

        if (!InitiateTerrainTileUpdateRequest())
        {
            std::cout << "InitiateTerrainTileUpdateRequest Failed." << std::endl;
        }
    }

    ReceiveTileJobResult();
}

#if 0//useless
void C3DTerrainManager_UpdateTerrain()
{
    pCTerrainThread pTerrainMgr = C3DTerrainManager_GetTerrainMgr();
    //int c = 0, p = 0, a = 0;

    //if (!c_vector_empty((pTerrainMgr->m_hasCreatedC)) || !c_vector_empty((pTerrainMgr->m_hasCreatedP)) || !c_vector_empty((pTerrainMgr->m_hasAdded)))
    if (0 == pTerrainMgr || !pTerrainMgr->m_hasCreatedC2.empty() || !pTerrainMgr->m_hasCreatedP2.empty() || !pTerrainMgr->m_hasAdded2.empty())
    {
        return;
    }

#if 0//for test
    static int i = 0;
    i++;
    std::string str1 = std::string("hello") + std::to_string(i);
    SendLoadTerrainDataRequest(i, str1);

    //main thread did manythings ...
    std::this_thread::sleep_for(std::chrono::milliseconds(20));//simulate main thread do something

    ReceiveTileJobResult();
#endif

    //LogWrite(LOG_LEVEL_ALWAYS, "遍历并创建新节点 start : %s,%d\n", __FILE__, __LINE__);
    /*遍历并创建新节点*/
    {
        /*	CLock lock(m_mutex);*/
        /*计算需要被添加的地形块*/
        CVec3d lonlatdir, lla, pos, pos1, dir1;//pos2；
        CVec3d maxPos, maxPos1;
        CVec3d ll;

        int sLon, sLat;
        int eLon, eLat;

        unsigned int nowTerCount;

        unsigned int i = 0;
        pCTerrainTile  title = NULL;

        int s;
        lonlatdir = Vec_CreatVec3d(1, 1, 0);
        lla = Vec_Vec3dSubVec3d(&(pTerrainMgr->m_lla), &lonlatdir);
        pos = Coordinate_DegreeVec3dLonLatAltToVec3d(lla);
        dir1 = Vec_Vec3dSubVec3d(&pos, &pTerrainMgr->m_eyePos);//jy：这样算出来的dir1，垂直方向上指向海拔更低的方向，因此后面算出来的pos1海拔低于pos2海拔
        Vec_NormalizeVec3d(&dir1);

        maxPos = dir1;
        maxPos1 = dir1;

        Vec_Vec3dMulDouble(&maxPos, MAXTERRAIN);
        Vec_Vec3dMulDouble(&maxPos1, -MAXTERRAIN);


        pos1 = Vec_Vec3dAddVec3d(&pTerrainMgr->m_eyePos, &maxPos);
        //pos2 = Vec_Vec3dAddVec3d(&pTerrainMgr->m_eyePos, &maxPos1);

        ll = Coordinate_Vec3dToDegreeLonlatAltVec3d(pos1);
        sLon = ll._v[0];
        sLat = ll._v[1];
        //ll = Coordinate_Vec3dToDegreeLonlatAltVec3d(pos2);
        eLon = ll._v[0] + 2;
        eLat = ll._v[1] + 2;
        //LogWrite(LOG_LEVEL_ALWAYS, "CNode_GetNumberChildren start : %s,%d\n", __FILE__, __LINE__);
        nowTerCount = CNode_GetNumberChildren(pTerrainMgr->m_root);
        /*初始化*/
        for (i = 0; i < nowTerCount; i++)
        {
            memset(&(pTerrainMgr->lastTopTerrainTiles[i]), 0, sizeof(TTerrain));
        }

        for (i = 0; i < nowTerCount; i++)
        {
            pCNode  tmnode = CNode_GetChildByIndex(pTerrainMgr->m_root, i);
            if (!tmnode)
            {
                continue;
            }
            title = (pCTerrainTile)(tmnode->pNodeData);
            if (title)
            {
                pTerrainMgr->lastTopTerrainTiles[i].pTerTile = title;//装载上一帧的tile
                pTerrainMgr->lastTopTerrainTiles[i].needKeep = 0;//上一帧的tile 全部默认不 needkeep
            }
            else
                pTerrainMgr->lastTopTerrainTiles[i].pTerTile = NULL;
        }
        //LogWrite(LOG_LEVEL_ALWAYS , "		for (s = sLon; s <= eLon; s++) start : %s,%d\n", __FILE__, __LINE__);
        for (s = sLon; s <= eLon; s++)
        {
            int t;
            for (t = sLat; t <= eLat; t++)
            {
                for (i = 0; i < nowTerCount; i++)
                {
                    if (pTerrainMgr->lastTopTerrainTiles[i].pTerTile && !pTerrainMgr->lastTopTerrainTiles[i].needKeep)
                    {
                        CVec3d tLL = CTerrainTile_GetLonLat(pTerrainMgr->lastTopTerrainTiles[i].pTerTile);//jy：既然terrains[]里面都是放的最大的1度跨度的地块，那么就不必这样获取地块经纬，可能是为了统一。
                        int tLon = tLL._v[0], tLat = tLL._v[1];
                        if (tLon == s && tLat == t)
                        {
                            pTerrainMgr->lastTopTerrainTiles[i].needKeep = true;//若本帧需要的tile存在于上一帧的tile中，则置needKeep
                            break;
                        }
                    }
                }
                //LogWrite(LOG_LEVEL_ALWAYS, "				if (i == nowTerCount)start : %s,%d\n", __FILE__, __LINE__);
                if (i == nowTerCount)//本帧的此地块不存在于上帧最大地块子节点中，需要添加到needAdded中
                {
                    unsigned int m_needAddedSize = 0;
                    pCTerrainTile  ter = (pCTerrainTile)(CNode_CreateNode(TerrainTile)->pNodeData);
                    //double lat = GetGlobal()->vLonLatAlt._v[0];
                    //double lon = GetGlobal()->vLonLatAlt._v[1];
                    CTerrainTile_SetLonLat(ter, s, t);
                    CTerrainTile_SetMask(ter, 0);

                    ter->m_parentsId = 0;
                    ter->m_Lod = 0;
                    ter->m_UniqueId = 0;

                    //CVec3d terCenterPos = CTerrainTile_GetCenter(ter);
                    //if (CTerrainManager_IsTileInTheFrontOfView(terCenterPos))

                    /*需要创建这些0级别地块*/
                    //c_vector_push_back((pTerrainMgr->m_needAdded), ter);
                    pTerrainMgr->m_needAdded2.push_back(ter);
                    //m_needAddedSize = c_vector_size(pTerrainMgr->m_needAdded);
                    m_needAddedSize = pTerrainMgr->m_needAdded2.size();
                    printf("m_needAddedSize:%d\n", m_needAddedSize);
                }
            }
        }
        pTerrainMgr->m_curLayer = 0;
        /*遍历每一个地块，自顶向下遍历*/
        //LogWrite(LOG_LEVEL_ALWAYS, "	/*遍历每一个地块，自顶向下遍历*/	 start : %s,%d\n", __FILE__, __LINE__);
        for (i = 0; i < nowTerCount; i++)//遍历最大地块
        {
            if (pTerrainMgr->lastTopTerrainTiles[i].pTerTile)
            {//若上一帧中的此tile被置成needKeep，表示需要进一步对此tile进行更新，比如显示更深的tile level
                /*需要保留的向下遍历*/
                if (pTerrainMgr->lastTopTerrainTiles[i].needKeep)
                {
                    C3DTerrainManager_FindChildTer(pTerrainMgr, pTerrainMgr->lastTopTerrainTiles[i].pTerTile, 0);
                }
                else
                {//若上一帧中的此tile没有被置成 needKeep，表示此tile不在本帧显示范围内，需要删除掉
                    /*否则删除这个地块*/
                    //c_vector_push_back(pTerrainMgr->m_needDeleted, pTerrainMgr->terrains[i].terrain);
                    pTerrainMgr->m_needDeleted2.push_back(pTerrainMgr->lastTopTerrainTiles[i].pTerTile);
                }
            }
        }

        //LogWrite(LOG_LEVEL_FATAL+1,"/*重置*/	 start : %s,%d\n", __FILE__, __LINE__);
        /*重置*/
        for (i = 0; i < nowTerCount; i++)
        {
            memset(&(pTerrainMgr->lastTopTerrainTiles[i]), 0, sizeof(TTerrain));
        }
    }

    //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*创建最大的地形块*/ start : %s,%d\n", __FILE__, __LINE__);
    /*创建最大的地形块*/
    {
        unsigned int index = 0;
        //unsigned int num = c_vector_size(pTerrainMgr->m_needAdded);
        //c_iterator iter, beg = c_vector_begin(pTerrainMgr->m_needAdded), end = c_vector_end(pTerrainMgr->m_needAdded);
        //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        //{
        //    pCTerrainTile tile = (pCTerrainTile)(ITER_REF(iter));
        //    CVec3d  ll = CTerrainTile_GetLonLat(tile);
        //    pCMatrixTransform ter = C3DTerrainManager_CreateTile(tile, ll._v[0], ll._v[1], LayerMap[0].unitCellCount, LayerMap[0].tileSpan, 0, 0);
        //    CNode_SetName(ter->node, "TitleRoot");
        //    tile->m_layer = 0;
        //    CTerrainTile_SetTerrain(tile, ter);
        //    {
        //        //c_vector_push_back(pTerrainMgr->m_hasAdded, tile);
        //        pTerrainMgr->m_hasAdded2.push_back(tile);
        //        index++;
        //    }
        //}
        //c_vector_clear(pTerrainMgr->m_needAdded);

        unsigned int num = pTerrainMgr->m_needAdded2.size();
        for (unsigned int i = 0; i < num; i++)
        {
            auto tile = pTerrainMgr->m_needAdded2[i];
            CVec3d  ll = CTerrainTile_GetLonLat(tile);
            pCMatrixTransform ter = C3DTerrainManager_CreateTile(tile, ll._v[0], ll._v[1], LayerMap[0].unitCellCount, LayerMap[0].tileSpan, 0);
            CNode_SetName(ter->node, "TitleRoot");
            tile->m_layer = 0;
            CTerrainTile_SetTerrain(tile, ter);
            {
                //c_vector_push_back(pTerrainMgr->m_hasAdded, tile);
                pTerrainMgr->m_hasAdded2.push_back(tile);
                index++;
            }
        }
        pTerrainMgr->m_needAdded2.clear();
    }
    //LogWrite(LOG_LEVEL_DEBUG_FP, "		/*创建地形*/start : %s,%d\n", __FILE__, __LINE__);
    /*创建地形*/
    {
        unsigned int index = 0;
        //unsigned int num = c_vector_size(pTerrainMgr->m_needCreatedC);
        //c_iterator iter, beg = c_vector_begin(pTerrainMgr->m_needCreatedC), end = c_vector_end(pTerrainMgr->m_needCreatedC);
        //{
        //    for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        //    {
        //        pToChildNode tcn = (pToChildNode)malloc(sizeof(ToChildNode));
        //        unsigned char i;
        //        pToChildNode iterToChild = (pToChildNode)(ITER_REF(iter));
        //        tcn->parent = iterToChild->parent;
        //        for (i = 0; i < 4; i++)
        //        {
        //            pCTerrainTile  tmp = iterToChild->child[i];
        //            if (tmp != NULL)
        //            {
        //                CVec3d  ll;
        //                unsigned char layer;
        //                pCMatrixTransform  ter = NULL;
        //                unsigned long m_mask = CTerrainTile_GetMask(tmp);
        //                CTerrainTile_GetLonLatLayerByMask(tmp, m_mask, &ll, &layer);
        //                //	printf("创建孩子节点:%lf,%lf,格子:%d,tileSpan%lf,Layer:%d\n",ll._v[0],ll._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);
        //                ter = C3DTerrainManager_CreateTile(tmp, ll._v[0], ll._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer, 0);
        //                tmp->m_layer = layer;

        //                {
        //                    char buff[32] = { 0 };
        //                    sprintf(buff, "%d,%d,layer:%d", (int)ll._v[0], (int)ll._v[1], tmp->m_layer);
        //                    CNode_SetName(ter->node, buff);
        //                }
        //                CTerrainTile_SetTerrain(tmp, ter);
        //            }
        //            tcn->child[i] = tmp;
        //        }
        //        {
        //            /*	CLock lock(m_mutex);*/
        //            index++;
        //            c_vector_push_back(pTerrainMgr->m_hasCreatedC, tcn);
        //            /*m_hasCreatedC.push_back(tcn);*/
        //        }
        //    }
        //    c_vector_clear(pTerrainMgr->m_needCreatedC);
        //}

        unsigned int num = pTerrainMgr->m_needCreatedC2.size();
        for (unsigned int j = 0; j < num; j++)
        {
            auto iterToChild = pTerrainMgr->m_needCreatedC2[j];
            pToChildNode tcn = (pToChildNode)malloc(sizeof(ToChildNode));
            unsigned char i;
            tcn->parent = iterToChild->parent;
            for (i = 0; i < 4; i++)
            {
                pCTerrainTile  tmp = iterToChild->child[i];
                if (tmp != NULL)
                {
                    CVec3d  ll;
                    unsigned char layer;
                    pCMatrixTransform  ter = NULL;
                    unsigned long m_mask = CTerrainTile_GetMask(tmp);
                    CTerrainTile_GetLonLatLayerByMask(tmp, m_mask, &ll, &layer);
                    //	printf("创建孩子节点:%lf,%lf,格子:%d,tileSpan%lf,Layer:%d\n",ll._v[0],ll._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);
                    ter = C3DTerrainManager_CreateTile(tmp, ll._v[0], ll._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);
                    tmp->m_layer = layer;

                    {
                        char buff[32] = { 0 };
                        sprintf(buff, "%d,%d,layer:%d", (int)ll._v[0], (int)ll._v[1], tmp->m_layer);
                        CNode_SetName(ter->node, buff);
                    }
                    CTerrainTile_SetTerrain(tmp, ter);
                }
                tcn->child[i] = tmp;
            }
            {
                /*	CLock lock(m_mutex);*/
                index++;
                //c_vector_push_back(pTerrainMgr->m_hasCreatedC, tcn);
                pTerrainMgr->m_hasCreatedC2.push_back(tcn);
                /*m_hasCreatedC.push_back(tcn);*/
            }
        }
        pTerrainMgr->m_needCreatedC2.clear();

        //LogWrite(LOG_LEVEL_DEBUG_FP, "num=c_vector_size(pTerrainMgr->m_needCreatedP)  start : %s,%d\n", __FILE__, __LINE__);

        index = 0;
        //num = c_vector_size(pTerrainMgr->m_needCreatedP);
        //beg = c_vector_begin(pTerrainMgr->m_needCreatedP), end = c_vector_end(pTerrainMgr->m_needCreatedP);
        //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        //{
        //    pToParentNode tpn = (pToParentNode)malloc(sizeof(ToParentNode));
        //    pCTerrainTile iterTitle = (pCTerrainTile)(ITER_REF(iter));
        //    CVec3d ll;
        //    pCMatrixTransform  ter = NULL;
        //    unsigned char layer;
        //    tpn->parent = iterTitle;

        //    CTerrainTile_GetLonLatLayerByMask(iterTitle, CTerrainTile_GetMask(iterTitle), &ll, &layer);
        //    /*	(*iter)->getLonLatLayerByMask((*iter)->getMask(), ll, layer);
        //    createTitle(ll, LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);*/
        //    //	printf("创建父节点:%lf,%lf,格子:%d,tileSpan%lf,Layer:%d\n",ll._v[0],ll._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);

        //    ter = C3DTerrainManager_CreateTile(iterTitle, ll._v[0], ll._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer, 0);
        //    tpn->pNodeMat = ter->node;
        //    {
        //        index++;
        //        c_vector_push_back(pTerrainMgr->m_hasCreatedP, tpn);
        //    }
        //}
        //c_vector_clear(pTerrainMgr->m_needCreatedP);

        num = pTerrainMgr->m_needCreatedP2.size();
        for (unsigned int i = 0; i < num; i++)
        {
            auto iterTitle = pTerrainMgr->m_needCreatedP2[i];
            pToParentNode tpn = (pToParentNode)malloc(sizeof(ToParentNode));
            CVec3d ll;
            pCMatrixTransform  ter = NULL;
            unsigned char layer;
            tpn->parent = iterTitle;

            CTerrainTile_GetLonLatLayerByMask(iterTitle, CTerrainTile_GetMask(iterTitle), &ll, &layer);
            /*	(*iter)->getLonLatLayerByMask((*iter)->getMask(), ll, layer);
            createTitle(ll, LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);*/
            //	printf("创建父节点:%lf,%lf,格子:%d,tileSpan%lf,Layer:%d\n",ll._v[0],ll._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);

            ter = C3DTerrainManager_CreateTile(iterTitle, ll._v[0], ll._v[1], LayerMap[layer].unitCellCount, LayerMap[layer].tileSpan, layer);
            tpn->pNodeMat = ter->node;
            {
                index++;
                //c_vector_push_back(pTerrainMgr->m_hasCreatedP, tpn);
                pTerrainMgr->m_hasCreatedP2.push_back(tpn);
            }
        }
        pTerrainMgr->m_needCreatedP2.clear();
    }

    C3DTerrainManager_updateMount();

    //C3DTerrainManager_FillRenderBuffer();
}
#endif

double minValue = 0.5;

pCMatrixTransform C3DTerrainManager_CreateTile(pCTerrainTile pTerTile, double lon, double lat, unsigned int count, double titleSpan_degree, unsigned char layer)
{
    if (lon < -180 || lat < -90 || lon >= 180 || lat >= 90)
    {
        return nullptr;
    }

    double eLon = (lon + titleSpan_degree) >= 180.0 ? 180.0 : (lon + titleSpan_degree);
    double eLat = (lat + titleSpan_degree) >= 90.0 ? 90.0 : (lat + titleSpan_degree);

    pTerTile->m_longlatalt._v[0] = (lon + eLon)*0.5;
    pTerTile->m_longlatalt._v[1] = (lat + eLat)*0.5;
    pTerTile->m_longlatalt._v[2] = 0;
    pTerTile->m_layer = layer;

    double unit_degree = titleSpan_degree / count;
    CVec3d centerPos = Coordinate_DegreeLonLatAltToVec3d((lon + eLon)*0.5, (lat + eLat)*0.5, 0.0);

    CVec3d startLLA = Vec_CreatVec3d(lon, lat, 0.0);
    CVec3d endLLA = Vec_CreatVec3d(eLon, eLat, 0.0);

    pCNode matNode = CNode_CreateNode(MatrixTransform);
    //pCNode geodeNode = CNode_CreatNode(Node);
    pCNode drawableNode = CNode_CreateNode(NT_Drawable);

    auto matT = (pCMatrixTransform)matNode->pNodeData;
    auto geometry = (pCDrawable)drawableNode->pNodeData;

    /*读取海拔*/
    int w = (int)((eLon - lon) / unit_degree + 1);
    int h = (int)((eLat - lat) / unit_degree + 1);
    int t;
    int s;

    geometry->drawType = CDrawableType_Terrain;
    geometry->count = count;
    geometry->centerPos = centerPos;

    geometry->_colorBind = BIND_PER_VERTEX;
    geometry->_normalBind = BIND_PER_VERTEX;

    matT->matrix = CMatrix_TranslateXYZ(centerPos._v[0], centerPos._v[1], centerPos._v[2]);
    matT->rect = Vec_CreatVec4d(startLLA._v[0], startLLA._v[1], endLLA._v[0], endLLA._v[1]);

    auto totalVetexCount = (unsigned int)(h*w);

    std::vector<CVec3d> temp_vertexes(totalVetexCount);
    std::vector<CVec2f> temp_texcoords(totalVetexCount);
    std::vector<CVec4f> temp_colors(totalVetexCount);
    std::vector<CVec3f> temp_normals(totalVetexCount);
    std::vector<GLfloat> temp_altitudes(totalVetexCount);

    //calculate the position,texcoord,altitude, except normal, and set a default color
    for (t = 0; t < h; t++)
    {
        for (s = 0; s < w; s++)
        {
            int currentIndex = t*w + s;
            double tlon1 = TAWS_MIN(lon + s*unit_degree, eLon);
            double tlat1 = TAWS_MIN(lat + t*unit_degree, eLat);
            CVec3d vpos;
            vpos._v[0] = lon + s*unit_degree;
            vpos._v[1] = lat + t*unit_degree;

            double alt = CAltManager_GetAltByLonLat(vpos._v[0], vpos._v[1]);

            vpos._v[2] = alt;
            //convert LonLatAlt to XYZ
            vpos = Coordinate_DegreeLonLatAltToVec3d(vpos._v[0], vpos._v[1], vpos._v[2]);
            //convert to tile center based position
            vpos = Vec_Vec3dSubVec3d(&vpos, &centerPos);
            temp_vertexes[currentIndex] = vpos;
            //temp_colors[currentIndex] = C2DWarningColor_Get3DHeightColorByAlt(alt);//default color
            temp_colors[currentIndex] = Vec_CreatVec4f(1.0f, 1.0f, 1.0f, 1.0f);//white
            CVec2f coordf;
            coordf._v[0] = ((float)tlon1 - (float)pTerTile->m_lon)*TERRAIN_TILE;
            coordf._v[1] = ((float)tlat1 - (float)pTerTile->m_lat)*TERRAIN_TILE;
            temp_texcoords[currentIndex] = coordf;
            temp_altitudes[currentIndex] = (GLfloat)alt;
        }
    }

    //now let's caculate normal by position
    //	p4	p1  p5
    //  p0--| --p2
    //	p7  p3  p6
    CVec3d norPos[4];
    for (t = 0; t < h; t++)
    {
        for (s = 0; s < w; s++)
        {
            int index = t*w + s;
            CVec3d curPos = temp_vertexes[index];

            //first, get the positions of p0,p1,p2,p3

            if (s == 0)//left border
            {
                double tlon1 = TAWS_MIN(lon + (s - 1)*unit_degree, eLon);
                double tlat1 = TAWS_MIN(lat + t*unit_degree, eLat);
                double alt1 = CAltManager_GetAltByLonLat(tlon1, tlat1);
                //转换为地心坐标系
                norPos[0] = Coordinate_DegreeLonLatAltToVec3d(tlon1, tlat1, alt1);
                //获取局部坐标
                norPos[0] = Vec_Vec3dSubVec3d(&norPos[0], &centerPos);
            }
            else
            {
                norPos[0] = temp_vertexes[index - 1];
            }

            if (t == 0)//bottom boundary
            {
                double tlon1 = TAWS_MIN(lon + s*unit_degree, 180.0);
                double tlat1 = TAWS_MIN(lat + (t - 1)*unit_degree, 90.0);
                double alt1 = CAltManager_GetAltByLonLat(tlon1, tlat1);
                //转换为地心坐标系
                norPos[3] = Coordinate_DegreeLonLatAltToVec3d(tlon1, tlat1, alt1);
                //获取局部坐标
                norPos[3] = Vec_Vec3dSubVec3d(&norPos[3], &centerPos);
            }
            else
            {
                norPos[3] = temp_vertexes[index - w];
            }

            if (s == w - 1)//right boundary
            {
                double tlon1 = TAWS_MIN(lon + (s + 1)*unit_degree, 180.0);
                double tlat1 = TAWS_MIN(lat + t*unit_degree, 90.0);
                double alt1 = CAltManager_GetAltByLonLat(tlon1, tlat1);
                //转换为地心坐标系
                norPos[2] = Coordinate_DegreeLonLatAltToVec3d(tlon1, tlat1, alt1);
                //获取局部坐标
                norPos[2] = Vec_Vec3dSubVec3d(&norPos[2], &centerPos);
            }
            else
            {
                norPos[2] = temp_vertexes[index + 1];
            }

            if (t == h - 1)//top boundary
            {
                double tlon1 = TAWS_MIN(lon + s*unit_degree, eLon);
                double tlat1 = TAWS_MIN(lat + (t + 1)*unit_degree, 90.0);
                double alt1 = CAltManager_GetAltByLonLat(tlon1, tlat1);
                //转换为地心坐标系
                norPos[1] = Coordinate_DegreeLonLatAltToVec3d(tlon1, tlat1, alt1);
                //获取局部坐标
                norPos[1] = Vec_Vec3dSubVec3d(&norPos[1], &centerPos);
            }
            else
            {
                norPos[1] = temp_vertexes[index + w];
            }

            norPos[0] = Vec_Vec3dSubVec3d(&norPos[0], &curPos);
            norPos[1] = Vec_Vec3dSubVec3d(&norPos[1], &curPos);
            norPos[2] = Vec_Vec3dSubVec3d(&norPos[2], &curPos);
            norPos[3] = Vec_Vec3dSubVec3d(&norPos[3], &curPos);

            CVec3d normal[4];
            CVec3f normal_f;
            Vec_NormalizeVec3d(&norPos[0]);
            Vec_NormalizeVec3d(&norPos[1]);
            Vec_NormalizeVec3d(&norPos[2]);
            Vec_NormalizeVec3d(&norPos[3]);
            normal[0] = Vec_CrossProductVec3d(&norPos[0], &norPos[1]);//jy:按右手定侧，得出来的是朝下的
            normal[1] = Vec_CrossProductVec3d(&norPos[1], &norPos[2]);
            normal[2] = Vec_CrossProductVec3d(&norPos[2], &norPos[3]);
            normal[3] = Vec_CrossProductVec3d(&norPos[3], &norPos[0]);

            normal[0] = Vec_Vec3dAddVec3d(&normal[0], &normal[1]);
            normal[0] = Vec_Vec3dAddVec3d(&normal[0], &normal[2]);
            normal[0] = Vec_Vec3dAddVec3d(&normal[0], &normal[3]);
            normal[0] = Vec_Vec3dDivDouble(&normal[0], 4.0);//jy:4个累加后取平均

            Vec_NormalizeVec3d(&normal[0]);

            normal_f = Vec_Vec3dToVec3f(normal[0]);
            Vec_Vec3MulDouble(&normal_f, -1.0f);//jy:朝下的，乘以-1 反个向

            //got the normal
            temp_normals[index] = normal_f;

            //now caculate the color by normal
            CVec4f tmpColor = temp_colors[index];
            auto cosValue = Vec_CosValueVec3f(&temp_normals[index], &sunlight_dir);
            cosValue = (cosValue < minValue) ? minValue : (cosValue > 1.0 ? 1.0 : cosValue);
            tmpColor._v[0] = tmpColor._v[0] * (float)cosValue;
            tmpColor._v[1] = tmpColor._v[1] * (float)cosValue;
            tmpColor._v[2] = tmpColor._v[2] * (float)cosValue;
            tmpColor._v[3] = tmpColor._v[3] * (float)cosValue;
            temp_colors[index] = tmpColor;

            //All ready, now, save all data to geometry
            CVec3d tmppos;
            CVec3f tmpposF;
            CVec2f tmptex;
            CVec3f tmpnorm;
            CVec4f tmpcolor;
            GLfloat tmpalt;
            tmppos = temp_vertexes[index]; //CVector_Pushback(geometry->_vertexs, &tmppos); 
            geometry->_vertexs2.push_back(tmppos);
            tmpposF = Vec_pVec3dToVec3f(&tmppos); //CVector_Pushback(geometry->_vertexsF, &tmpposF);
            geometry->_vertexsF2.push_back(tmpposF);
            tmptex = temp_texcoords[index]; //CVector_Pushback(geometry->_texcoords, &tmptex);
            geometry->_texcoords2.push_back(tmptex);
            tmpnorm = temp_normals[index]; //CVector_Pushback(geometry->_normals, &tmpnorm);
            geometry->_normals2.push_back(tmpnorm);
            tmpcolor = temp_colors[index]; //CVector_Pushback(geometry->_colors, &tmpcolor);
            geometry->_colors2.push_back(tmpcolor);
            tmpalt = temp_altitudes[index]; //CVector_Pushback(geometry->_altitudes, &tmpalt);
            geometry->_altitudes2.push_back(tmpalt);
        }
    }

    GLushort tmpIdx = 0;
    //generate the elements indices data for glDrawElements
    unsigned int IndicesCount = 0;
    for (t = 0; t < h - 1; t++)
    {
        for (s = 0; s < w - 1; s++)
        {
            int index = t*w + s;

            /*
            triangle strip order, ex. a 33x33 mesh :
            .
            .
            .
            34--36--38--40    ---66
            | / | / | / | ...  / |
            35--37--39--41    ---67(same line)
            0---2---4---6     ---32(same line)
            | / | / | / | ...  / |
            1---3---5---7     ---33
            */
            if (s == 0)
            {
                tmpIdx = (GLushort)(index + w);
                //CVector_Pushback(geometry->_indices, &tmpIdx);
                geometry->_indices2.push_back(tmpIdx);
                IndicesCount++;
                tmpIdx = (GLushort)index;
                //CVector_Pushback(geometry->_indices, &tmpIdx);
                geometry->_indices2.push_back(tmpIdx);
                IndicesCount++;
            }

            tmpIdx = (GLushort)(index + w + 1);
            //CVector_Pushback(geometry->_indices, &tmpIdx);
            geometry->_indices2.push_back(tmpIdx);
            IndicesCount++;
            tmpIdx = (GLushort)(index + 1);
            //CVector_Pushback(geometry->_indices, &tmpIdx);
            geometry->_indices2.push_back(tmpIdx);
            IndicesCount++;
            if (s == w - 1 - 1)//the last vertex at the line
            {
                //jy:unfortunately OpenGL ES 2.0 not support Primitive Restart(Require ES 3.0). so...use degenerate triangles 
                //CVector_Pushback(geometry->_indices, &tmpIdx);//jy: add index for building degenerate triangle
                geometry->_indices2.push_back(tmpIdx);
                IndicesCount++;
                tmpIdx = (GLushort)(index + w + 1 + 1);
                if (tmpIdx < w * h)
                {
                    //CVector_Pushback(geometry->_indices, &tmpIdx);//jy: add index for building degenerate triangle
                    geometry->_indices2.push_back(tmpIdx);
                    IndicesCount++;
                }
            }
        }
    }

    CPrimitiveSet primitiveSet;
    primitiveSet.beg = 0;
    primitiveSet.count = IndicesCount;
    //primitiveSet.type = GL_TRIANGLES;
    primitiveSet.type = GL_TRIANGLE_STRIP;
    //CVector_Pushback(geometry->_primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);

    CNode_SetName(matNode, "MatrixTransform_TileMat");
    //CNode_SetName(geodeNode,"Node_TileGeode");
    CNode_SetName(drawableNode, "Drawable_TileDrawable");

    //CNode_AddChild(geodeNode,drawableNode);
    CNode_AddChild(matNode, drawableNode);

    //LogWrite(LOG_LEVEL_FATAL, "CTerrainManager_CreateBorder   start : %s,%d\n", __FILE__, __LINE__);
    drawableNode = CTerrainManager_CreateBorder2(pTerTile, lon, eLon, lat, eLat, w, h, unit_degree, centerPos)->node;
    CNode_SetName(drawableNode, "Drawable_TileDrawableBorder");
    CNode_AddChild(matNode, drawableNode);
    //LogWrite(LOG_LEVEL_FATAL, "CTerrainManager_CreateBorderCNode_AddChild   start : %s,%d\n", __FILE__, __LINE__);

    //pCTerrainThread ptd = C3DTerrainManager_GetTerrainMgr();
    //Chuck_Insert(ptd->v_buffer,(char*)geometry->_vertexs->data, geometry->_vertexs->size);

    return matT;
}

#if 0
void C3DTerrainManager_RenderFilledBuffer()
{
    pCTerrainThread ptd = C3DTerrainManager_GetTerrainMgr();
    if (ptd)
    {
        unsigned int beg = 0;// , count;
        /*顶点个数*/
        unsigned int _vertexsSize = ptd->v_buffer->size;
        unsigned int _texcoordsSize = ptd->t_buffer->size;

        pchuck primitiveSets = ptd->p_buffer;
        unsigned int textPloylon = 0;
    }
}

void _C3DTerrainManager_FillBuffer(pCTerrainThread ptd, pCNode node)
{
    if (node && ptd)
    {
        int i;
        int childrensize = CNode_GetNumberChildren(node);
        for (i = 0; i < childrensize; i++)
        {
            pCNode child = CNode_GetChildByIndex(node, i);
            if (child->type == NT_Drawable)
            {
                pCDrawable draw = (pCDrawable)(child->pNodeData);
                //Chuck_Insert(ptd->v_buffer, (char*)draw->_vertexs->data, draw->_vertexs->size);
                Chuck_Insert(ptd->v_buffer, (char*)draw->_vertexs2.data(), draw->_vertexs2.size());
                //Chuck_Insert(ptd->t_buffer, (char*)draw->_texcoords->data, draw->_texcoords->size);
                //Chuck_Insert(ptd->c_buffer, (char*)draw->_colors->data, draw->_colors->size);
                //Chuck_Insert(ptd->p_buffer, (char*)draw->_primitiveSets->data, draw->_primitiveSets->size);
                Chuck_Insert(ptd->t_buffer, (char*)draw->_texcoords2.data(), draw->_texcoords2.size());
                Chuck_Insert(ptd->c_buffer, (char*)draw->_colors2.data(), draw->_colors2.size());
                Chuck_Insert(ptd->p_buffer, (char*)draw->_primitiveSets2.data(), draw->_primitiveSets2.size());

            }

            _C3DTerrainManager_FillBuffer(ptd, child);
        }
    }
}

void C3DTerrainManager_FillRenderBuffer()
{
    pCTerrainThread ptd = C3DTerrainManager_GetTerrainMgr();
    if (ptd)
    {
        pCNode node = ptd->m_root;
        Chuck_Del(ptd->v_buffer);
        Chuck_Del(ptd->t_buffer);
        Chuck_Del(ptd->c_buffer);
        Chuck_Del(ptd->p_buffer);
        //fill all render vertex data to buffer block
        _C3DTerrainManager_FillBuffer(ptd, node);
    }
}
#endif

pCDrawable CTerrainManager_CreateBorder2(const pCTerrainTile tile, const double sLon, const double eLon, const double sLat, const double eLat,
    const unsigned int w, const unsigned int h, const double unit, const CVec3d centerPos)
{
    double len = unit * 200000;

    pCNode drawNode = CNode_CreateNode(NT_Drawable);
    auto geometry = (pCDrawable)drawNode->pNodeData;

    //index:
    int b0Start = 0;
    int b0End = (w - 1);
    int b1Start = b0End;
    int b1End = (w - 1) + (h - 1);
    int b2Start = b1End;
    int b2End = (w - 1) + (h - 1) + (w - 1);
    int b3Start = b2End;
    int b3End = (w - 1) + (h - 1) + (w - 1) + (h - 1) + 1;

    geometry->drawType = CDrawableType_Terrain_Border;
    geometry->count = 0;
    geometry->centerPos = centerPos;

    geometry->_colorBind = BIND_OVERALL;
    geometry->_normalBind = BIND_PER_VERTEX;

    CVec3d tmpSurfaceLLA;
    CVec3d tmpDowngroundLLA;
    CVec3d tmpSurfacePos;
    CVec3d tmpDowngroundPos;


    /*
                leg 2
                -----------------
                |       <       |
                |               |
                |v leg3        ^|  leg 1
                |               |
                |       >       |
                -----------------
                leg 0
                */
    GLushort IndicesCount = 0;
    for (int idx = 0; idx < b3End; idx++)
    {
        if (b0Start <= idx&& idx < b0End)
        {
            tmpSurfaceLLA._v[0] = sLon + (idx - b0Start)*unit;
            tmpSurfaceLLA._v[1] = sLat;
        }
        else if (b1Start <= idx&& idx < b1End)
        {
            tmpSurfaceLLA._v[0] = eLon;
            tmpSurfaceLLA._v[1] = sLat + (idx - b1Start)*unit;
        }
        else if (b2Start <= idx&& idx < b2End)
        {
            tmpSurfaceLLA._v[0] = eLon - (idx - b2Start)*unit;
            tmpSurfaceLLA._v[1] = eLat;
        }
        else if (b3Start <= idx&& idx < b3End)
        {
            tmpSurfaceLLA._v[0] = sLon;
            tmpSurfaceLLA._v[1] = eLat - (idx - b3Start)*unit;
        }
        else
        {
            assert(false);
        }

        tmpSurfaceLLA._v[2] = CAltManager_GetAltByLonLat(tmpSurfaceLLA._v[0], tmpSurfaceLLA._v[1]);
        tmpDowngroundLLA = tmpSurfaceLLA;
        tmpDowngroundLLA._v[2] = tmpDowngroundLLA._v[2] - len;

        tmpSurfacePos = Coordinate_Vec3dDegreeLonlatAltToVec3d(tmpSurfaceLLA);
        tmpDowngroundPos = Coordinate_Vec3dDegreeLonlatAltToVec3d(tmpDowngroundLLA);

        CVec3f tmpposF;

        auto clon = (float)(tmpSurfaceLLA._v[0] - tile->m_lon);
        auto clat = (float)(tmpSurfaceLLA._v[1] - tile->m_lat);
        CVec2f tmptex = Vec_CreatVec2f(clon, clat);

        //get the altitude
        auto tmpSurfaceAltidude = (GLfloat)tmpSurfaceLLA._v[2];
        auto tmpDowngroundAltitude = (GLfloat)tmpDowngroundLLA._v[2];

        //calculate the normal of this surface position	
        double tmpLon;
        double tmpLat;
        int alt1;
        int alt2;
        int alt3;
        int alt4;
        CVec3d p1;
        CVec3d p2;
        CVec3d p3;
        CVec3d p4;
        CVec3d tmpNor;

        tmpLon = tmpSurfaceLLA._v[0] - unit;
        if (tmpLon < -180.0)tmpLon += 360.0;
        alt1 = CAltManager_GetAltByLonLat(tmpLon, tmpSurfaceLLA._v[1]);
        if (alt1 < 0)alt1 = (int)tmpSurfaceLLA._v[2];
        p1 = Coordinate_DegreeLonLatAltToVec3d(tmpLon, tmpSurfaceLLA._v[1], alt1);

        tmpLon = tmpSurfaceLLA._v[0] + unit;
        if (tmpLon > 180.0)tmpLon -= 360.0;
        alt2 = CAltManager_GetAltByLonLat(tmpLon, tmpSurfaceLLA._v[1]);
        if (alt2 < 0)alt2 = (int)tmpSurfaceLLA._v[2];
        p2 = Coordinate_DegreeLonLatAltToVec3d(tmpLon, tmpSurfaceLLA._v[1], alt2);

        tmpLat = TAWS_MAX(-90.0, tmpSurfaceLLA._v[1] - unit);
        alt3 = CAltManager_GetAltByLonLat(tmpSurfaceLLA._v[0], tmpLat);
        if (alt3 < 0)alt3 = (int)tmpSurfaceLLA._v[2];
        p3 = Coordinate_DegreeLonLatAltToVec3d(tmpSurfaceLLA._v[0], tmpLat, alt3);

        tmpLat = TAWS_MIN(90.0, tmpSurfaceLLA._v[1] + unit);
        alt4 = CAltManager_GetAltByLonLat(tmpSurfaceLLA._v[0], tmpLat);
        if (alt4 < 0)alt4 = (int)tmpSurfaceLLA._v[2];
        p4 = Coordinate_DegreeLonLatAltToVec3d(tmpSurfaceLLA._v[0], tmpLat, alt4);

        p2 = Vec_Vec3dSubVec3d(&p2, &p1);
        p4 = Vec_Vec3dSubVec3d(&p4, &p3);
        tmpNor = Vec_CrossProductVec3d(&p2, &p4);
        Vec_NormalizeVec3d(&tmpNor);

        CVec3f tmpNormal = Vec_pVec3dToVec3f(&tmpNor);

        //calculate color
        //CVec4f tmpColor = C2DWarningColor_Get3DHeightColorByAlt(tmpSurfaceLLA._v[2]);
        CVec4f tmpColor = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);//white
        auto cosValue = Vec_CosValueVec3f(&tmpNormal, &sunlight_dir);
        cosValue = (cosValue < minValue) ? minValue : (cosValue > 1.0 ? 1.0 : cosValue);
        tmpColor._v[0] = tmpColor._v[0] * (float)cosValue;
        tmpColor._v[1] = tmpColor._v[1] * (float)cosValue;
        tmpColor._v[2] = tmpColor._v[2] * (float)cosValue;
        tmpColor._v[3] = tmpColor._v[3] * (float)cosValue;

        //surface vertex
        tmpSurfacePos = Vec_Vec3dSubVec3d(&tmpSurfacePos, &centerPos);//based on the center position of this tile
        //CVector_Pushback(geometry->_vertexs, &tmpSurfacePos);
        geometry->_vertexs2.push_back(tmpSurfacePos);
        tmpposF = Vec_pVec3dToVec3f(&tmpSurfacePos);
        //CVector_Pushback(geometry->_vertexsF, &tmpposF);
        geometry->_vertexsF2.push_back(tmpposF);
        //CVector_Pushback(geometry->_texcoords, &tmptex);
        geometry->_texcoords2.push_back(tmptex);
        //CVector_Pushback(geometry->_normals, &tmpNormal);
        geometry->_normals2.push_back(tmpNormal);
        //CVector_Pushback(geometry->_colors, &tmpColor);
        geometry->_colors2.push_back(tmpColor);
        //CVector_Pushback(geometry->_altitudes, &tmpSurfaceAltidude);
        geometry->_altitudes2.push_back(tmpSurfaceAltidude);
        //CVector_Pushback(geometry->_indices, &IndicesCount);//idx*2 + 0
        geometry->_indices2.push_back(IndicesCount);
        IndicesCount++;

        //downground vertex
        tmpDowngroundPos = Vec_Vec3dSubVec3d(&tmpDowngroundPos, &centerPos);//based on the center position of this tile
        //CVector_Pushback(geometry->_vertexs, &tmpDowngroundPos);
        geometry->_vertexs2.push_back(tmpDowngroundPos);
        tmpposF = Vec_pVec3dToVec3f(&tmpDowngroundPos);
        //CVector_Pushback(geometry->_vertexsF, &tmpposF);
        geometry->_vertexsF2.push_back(tmpposF);
        //CVector_Pushback(geometry->_texcoords, &tmptex);//just set the same value as the corresponding pos on the surface
        geometry->_texcoords2.push_back(tmptex);
        //CVector_Pushback(geometry->_normals, &tmpNormal);//just set the same value as the corresponding pos on the surface
        geometry->_normals2.push_back(tmpNormal);
        //CVector_Pushback(geometry->_colors, &tmpColor);//just set the same value as the corresponding pos on the surface
        //CVector_Pushback(geometry->_altitudes, &tmpDowngroundAltitude);
        //CVector_Pushback(geometry->_indices, &IndicesCount);//idx*2 + 1
        geometry->_colors2.push_back(tmpColor);
        geometry->_altitudes2.push_back(tmpDowngroundAltitude);
        geometry->_indices2.push_back(IndicesCount);
        IndicesCount++;
    }

    CPrimitiveSet primitiveSet;
    primitiveSet.beg = 0;
    primitiveSet.count = IndicesCount;
    primitiveSet.type = GL_TRIANGLE_STRIP;
    //CVector_Pushback(geometry->_primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);

    return geometry;
}

#if 0
void CTerrainManager_RegistRenderList()
{
    int i;
    double lon, lat, tlon, tlat;
    pCTerrainThread pTerrainMgr = C3DTerrainManager_GetTerrainMgr();
    int nowTerCount = CNode_GetNumberChildren(pTerrainMgr->m_root);
    lon = GetGlobal()->vLonLatAlt._v[0];
    lat = GetGlobal()->vLonLatAlt._v[1];

    for (i = 0; i < nowTerCount; i++)
    {
        CVec3d lonlatalt = CTerrainTile_GetLonLat(pTerrainMgr->terrains[i].terrain);
        tlon = lonlatalt._v[0];
        tlat = lonlatalt._v[1];
        if (((tlat + 1) > lat && (tlat - 1) < lat) && ((tlon + 1) > lon && (tlon - 1) < lon))
        {
            //pTerrainMgr->m_dl_renderiId = i;
            pTerrainMgr->terrains[i].terrain->intile = 1;
            break;
        }
    }
}
#endif

