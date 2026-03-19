#include <string>
#include "external/wmm/HmsMagneticDecl.h"

extern bool g_supportVAO;

#include "Drawable.h"
#include "HmsLog.h"
#include "MathCommon.h"
#include "3DAirportBlock.h"
//#include "TAWS.h"
#include "FileManager.h"
#include "TextureMgr.h"
#include "Coordinate.h"
#include "Matrix.h"


#define threshold_length    (20.0)
#define left_length         (10.0)
#define num_length          (10.0)

#define LINE_COLOR          (Vec_CreatVec4f(0.8f,0.8f,0.8f,1.f))

pC3DAirportBlock C3DAirportBlock_Create()
{
#if 0
    pC3DAirportBlock blk = (pC3DAirportBlock)malloc(sizeof(C3DAirportBlock));
    if (blk)
    {
        memset(blk, 0, sizeof(C3DAirportBlock));
        blk->m_root = CNode_CreatNode(Node);
        blk->m_mat = (pCMatrixTransform)(CNode_CreatNode(MatrixTransform)->pNodeData);
        CNode_SetName(blk->m_mat->node, "__airportblock_mat");
        CNode_AddChild(blk->m_root, blk->m_mat->node);

        /*创建障碍物信息容器*/
        //blk->m_airportInfo = &(blk->m_v_airportInfo);
        //blk->m_runWayInfo = &(blk->m_v_runWayInfo);
        //blk->m_drawRunwayInfo = &(blk->m_v_drawRunwayInfo);
        //c_vector_create(blk->m_airportInfo, NULL);
        //c_vector_create(blk->m_runWayInfo, NULL);
        //c_vector_create(blk->m_drawRunwayInfo, NULL);
    }
#endif
    pC3DAirportBlock blk = NULL;
    try
    {
        blk = new C3DAirportBlock;
    }
    catch (std::bad_alloc& e1)
    {
        HMSLOG(" C3DAirportBlock create error,  %s\n", e1.what());
        return NULL;
    }

    blk->m_root = CNode_CreateNode(Node);
    blk->m_mat = (pCMatrixTransform)(CNode_CreateNode(MatrixTransform)->pNodeData);
    CNode_SetName(blk->m_mat->node, "__airportblock_mat");
    CNode_AddChild(blk->m_root, blk->m_mat->node);
    return blk;
}

void C3DAirportBlock_Destory(pC3DAirportBlock *blk)
{
    if (0 == blk)
    {
        return;
    }

    CNode_RemoveFromParentsAndFreeNode((*blk)->m_root);

    /*卸载机场数据集*/
    {
#if 0
        c_pvector vector = (*blk)->m_airportInfo;
        unsigned int index = 0, num = c_vector_size(vector);
        c_iterator iter, beg = c_vector_begin(vector), end = c_vector_end(vector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pCAirport airport = (pCAirport)(ITER_REF(iter));
            free(airport);
            airport = 0;
        }
        c_vector_clear((*blk)->m_airportInfo);
        c_vector_destroy((*blk)->m_airportInfo);
#endif  
        auto& vec = (*blk)->m_airportInfo2;
        auto num = vec.size();
        for (unsigned int i = 0; i < num; ++i)
        {
            auto airport = vec[i];
            free(airport);
            airport = NULL;
        }
        vec.clear();
    }
    /*卸载跑道数据集*/
    {
#if 0
        c_pvector vector = (*blk)->m_runWayInfo;
        unsigned int index = 0, num = c_vector_size(vector);
        c_iterator iter, beg = c_vector_begin(vector), end = c_vector_end(vector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pCRunway ruw = (pCRunway)(ITER_REF(iter));
            free(ruw);
            ruw = 0;
        }

        c_vector_clear((*blk)->m_runWayInfo);
        c_vector_destroy((*blk)->m_runWayInfo);
#endif
        auto& vec = (*blk)->m_runWayInfo2;
        unsigned int index = 0;
        auto num = vec.size();
        for (unsigned int i = 0; i < num; ++i)
        {
            auto ruw = vec[i];
            free(ruw);
            ruw = NULL;
        }
        vec.clear();
    }
    /*卸载绘制跑道数据集*/
    {
#if 0
        c_pvector vector = (*blk)->m_drawRunwayInfo;
        unsigned int index = 0, num = c_vector_size(vector);
        c_iterator iter, beg = c_vector_begin(vector), end = c_vector_end(vector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pCRunway ruw = (pCRunway)(ITER_REF(iter));
            free(ruw);
            ruw = 0;
        }

        c_vector_clear((*blk)->m_drawRunwayInfo);
        c_vector_destroy((*blk)->m_drawRunwayInfo);
#endif
        auto& vec = (*blk)->m_drawRunwayInfo2;
        unsigned int index = 0;
        auto num = vec.size();
        for (unsigned int i = 0; i < num; ++i)
        {
            auto ruw = vec[i];
            free(ruw);
            ruw = NULL;
        }
        vec.clear();
    }

    //free((*blk));        
    delete (*blk);
    (*blk) = 0;
}

void C3DAirportBlockSetLonlat(pC3DAirportBlock blk, CVec3d lonlatalt, CVec3d hpr)
{
    if (blk)
    {
        /*先按照0海拔计算经纬度、XYZ值处理*/
        CVec3d pos = Vec_CreatVec3d(lonlatalt._v[0], lonlatalt._v[1], 0.0);
        blk->m_hpr = hpr;
        blk->m_lonlat = pos;
        blk->m_m_lonlatPos = pos = Coordinate_DegreeVec3dLonLatAltToVec3d(pos);
        blk->m_mat->matrix = CMatrix_TranslateVec3d(pos);
        blk->m_mat->coord = WGS84;
    }
}

void C3DAirportBlockReadAirportData(pC3DAirportBlock blk)
{
    if (0 == blk){ return; }

    char  filepath[256] = { 0 };
    unsigned int len = 0;
    unsigned char* airportData = NULL;

    char buff[256] = { 0 }, name[64] = { 0 };
    const char* taws_data_path = GetTawsDataPath();
    int lon = (int)(blk->m_lonlat._v[0]), lat = (int)(blk->m_lonlat._v[1]);

    sprintf(name, "export_%03d_%03d.dat", abs(lon), abs(lat));
    sprintf(buff, "%s/Airport/%sx%sy/%s", taws_data_path, blk->m_lonlat._v[0] >= 0 ? "+" : "-", blk->m_lonlat._v[1] >= 0 ? "+" : "-", name);
    memcpy_op(filepath, buff, strlen(buff));

    len = FileManager_LoadBinaryFile(filepath, &airportData);

    if (NULL == airportData)
    {
        return;
    }
    else
    {
        const unsigned int sizeOfPerData = sizeof(CAirport);
        int i = 0;
        int recordNumber = len / sizeOfPerData;
        for (i = 0; i < recordNumber; i++)
        {
            pCAirport record = (pCAirport)malloc(sizeOfPerData);
            if (record)
            {
                memcpy_op(record, airportData + sizeOfPerData*i, sizeOfPerData);
                //c_vector_push_back(blk->m_airportInfo, record);
                blk->m_airportInfo2.push_back(record);
            }
        }
        if (NULL != airportData)
        {
            free(airportData);
            airportData = NULL;
        }
    }
}

void C3DAirportBlockReadRunWayData(pC3DAirportBlock blk)
{
    if (NULL == blk)
    {
        return;
    }

    char filepath[256] = { 0 };
    unsigned char *binaryData = NULL;

    char buff[256] = { 0 };
    char name[64] = { 0 };
    const char *taws_data_path = GetTawsDataPath();
    int lon = (int)(blk->m_lonlat._v[0]);
    int lat = (int)(blk->m_lonlat._v[1]);

    sprintf(name, "export_%03d_%03d.dat", abs(lon), abs(lat));
    sprintf(buff, "%s/Runway/%sx%sy/%s",
        taws_data_path,
        blk->m_lonlat._v[0] >= 0 ? "+" : "-",
        blk->m_lonlat._v[1] >= 0 ? "+" : "-",
        name);
    memcpy_op(filepath, buff, strlen(buff));

    unsigned int len = FileManager_LoadBinaryFile(filepath, &binaryData);

    printf("read airport %s\n", buff);
    if (NULL == binaryData)
    {
        printf("failed\n");
        return;
    }
    else
    {
        const unsigned int sizeOfPerData = sizeof(CRunway);
        int i = 0;
        int recordNumber = len / sizeOfPerData;
        for (i = 0; i < recordNumber; i++)
        {
            pCRunway record = (pCRunway)malloc(sizeOfPerData);
            if (record)
            {
                memcpy_op(record, binaryData + sizeOfPerData * i, sizeOfPerData);
                //c_vector_push_back(blk->m_runWayInfo, record);
                blk->m_runWayInfo2.push_back(record);
            }
        }
        if (NULL != binaryData)
        {
            free(binaryData);
            binaryData = NULL;
        }
    }

    C3DAirportAnysysRunwayData(blk);
}

#if 0
void C3DAirportAnysysRunwayData(pC3DAirportBlock blk)
{
    if (0 == blk){ return; }
    c_pvector runwayInfo = blk->m_runWayInfo;
    unsigned int i = 0, j = 0;
Next:
    for (i = 0; i < c_vector_size(runwayInfo); i++)
    {
        pCRunway runway = (pCRunway)c_vector_at(runwayInfo, i);
        if (0 == runway){ continue; }

        /*获取方向*/
        char dir[3] = { 0 };
        int runDir0 = 0;
        unsigned char isBoth = 0;

        dir[0] = runway->runway_id[2];
        dir[1] = runway->runway_id[3];
        dir[2] = 0;

        runDir0 = atoi(dir);

        c_vector_erase(runwayInfo, c_vector_begin(runwayInfo));
        i = i - 1;
        for (j = 0; j < c_vector_size(runwayInfo); j++)
        {
            pCRunway runway2 = (pCRunway)c_vector_at(runwayInfo, j);
            if (0 == runway2){ continue; }

            int runDir1;
            memcpy_op(dir, runway2->runway_id + 2, 2);
            runDir1 = atoi(dir);

            if (!strcmp(runway->ICAO_id, runway2->ICAO_id) &&
                runway->len == runway2->len &&
                abs(runDir1 - runDir0) == 18)
            {
                if (((runway->runway_id[4] == 'L') && (runway2->runway_id[4] == 'R')) ||
                    ((runway->runway_id[4] == 'R') && (runway2->runway_id[4] == 'L')) ||
                    (runway->runway_id[4] == '\0') && (runway2->runway_id[4] == '\0'))
                {
                    //小航向 L端为正方向
                    double angle = (runway->direction >
                        runway2->direction) ? runway2->direction : runway->direction;
                    unsigned char runwayIslittle = (runway->direction < runway2->direction) ? 1 : 0;
                    //	angle = (runway->direction + runway2->direction-180.0)/2.0 ;
                    if (runwayIslittle)
                    {
                        runway->direction = angle;
                        runway2->direction = angle + 180.0;
                        runway->lon = (runway2->lon + runway->lon) / 2.0;
                        runway->lat = (runway2->lat + runway->lat) / 2.0;
                        {
                            pCDrawRunway drw = (pCDrawRunway)malloc(sizeof(CDrawRunway));
                            if (drw)
                            {
                                memset(drw, 0, sizeof(CDrawRunway));
                                drw->runway = runway;
                                drw->runway->altitude = (runway->altitude + runway2->altitude) / 2.0;
                                drw->isDoth = 1;
                                memcpy_op(drw->runway_id, runway2->runway_id, 6);
                                c_vector_push_back(blk->m_drawRunwayInfo, drw);
                            }

                            free(runway2);
                            runway2 = 0;
                        }
                    }
                    else
                    {
                        runway2->lon = (runway2->lon + runway->lon) / 2.0;
                        runway2->lat = (runway2->lat + runway->lat) / 2.0;
                        {
                            pCDrawRunway drw = (pCDrawRunway)malloc(sizeof(CDrawRunway));
                            if (drw)
                            {
                                memset(drw, 0, sizeof(CDrawRunway));
                                drw->runway = runway2;
                                drw->runway->altitude = (runway->altitude + runway2->altitude) / 2.0;
                                drw->isDoth = true;
                                memcpy_op(drw->runway_id, runway->runway_id, 6);
                                c_vector_push_back(blk->m_drawRunwayInfo, drw);
                            }

                            free(runway);
                            runway = 0;
                        }
                    }
                    c_vector_erase(runwayInfo, c_vector_iter_at(runwayInfo, j));
                    j = j - 1;
                    isBoth = 1;
                    goto Next;
                }
            }
        }

        if (!isBoth)
        {
            pCDrawRunway drw = (pCDrawRunway)malloc(sizeof(CDrawRunway));
            if (drw)
            {
                memset(drw, 0, sizeof(CDrawRunway));
                drw->runway = runway;
                drw->isDoth = 0;
                c_vector_push_back(blk->m_drawRunwayInfo, drw);
            }
        }
    }
}
#endif

void C3DAirportAnysysRunwayData(pC3DAirportBlock blk)
{
    if (0 == blk)
    {
        return;
    }
    //c_pvector runwayInfo = blk->m_runWayInfo;
    auto& runwayInfo = blk->m_runWayInfo2;
    unsigned int i = 0;
    unsigned int j = 0;
Next:
    for (i = 0; i < runwayInfo.size(); i++)
    {
        auto runway = runwayInfo[i];
        if (0 == runway)
        {
            continue;
        }

        /*获取方向*/
        char dir[3] = { 0 };
        int runDir0 = 0;
        unsigned char isBoth = 0;

        dir[0] = runway->runway_id[2];
        dir[1] = runway->runway_id[3];
        dir[2] = 0;

        runDir0 = atoi(dir);

        runwayInfo.erase(runwayInfo.begin());
        i = i - 1;
        for (j = 0; j < runwayInfo.size(); j++)
        {
            //pCRunway runway2 = (pCRunway)c_vector_at(runwayInfo, j);
            auto runway2 = runwayInfo[j];
            if (0 == runway2)
            {
                continue;
            }

            int runDir1;
            memcpy_op(dir, runway2->runway_id + 2, 2);
            runDir1 = atoi(dir);

            if (!strcmp(runway->ICAO_id, runway2->ICAO_id) &&
                runway->len == runway2->len &&
                abs(runDir1 - runDir0) == 18)
            {
                if (((runway->runway_id[4] == 'L') && (runway2->runway_id[4] == 'R')) ||
                    ((runway->runway_id[4] == 'R') && (runway2->runway_id[4] == 'L')) ||
                    (runway->runway_id[4] == '\0') && (runway2->runway_id[4] == '\0'))
                {
                    //小航向 L端为正方向
                    double angle = (runway->direction > runway2->direction) ? runway2->direction : runway->direction;
                    unsigned char runwayIslittle = (runway->direction < runway2->direction) ? 1 : 0;
                    //	angle = (runway->direction + runway2->direction-180.0)/2.0 ;
                    if (runwayIslittle)
                    {
                        runway->direction = angle;
                        runway2->direction = angle + 180.0;
                        runway->lon = (runway2->lon + runway->lon) / 2.0;
                        runway->lat = (runway2->lat + runway->lat) / 2.0;
                        {
                            pCDrawRunway drw = (pCDrawRunway)malloc(sizeof(CDrawRunway));
                            if (drw)
                            {
                                memset(drw, 0, sizeof(CDrawRunway));
                                drw->runway = runway;
                                drw->runway->altitude = (runway->altitude + runway2->altitude) / 2.0;
                                drw->isDoth = 1;
                                memcpy_op(drw->runway_id, runway2->runway_id, 6);
                                //c_vector_push_back(blk->m_drawRunwayInfo, drw);
                                blk->m_drawRunwayInfo2.push_back(drw);
                            }

                            free(runway2);
                            runway2 = 0;
                        }
                    }
                    else
                    {
                        runway2->lon = (runway2->lon + runway->lon) / 2.0;
                        runway2->lat = (runway2->lat + runway->lat) / 2.0;
                        {
                            pCDrawRunway drw = (pCDrawRunway)malloc(sizeof(CDrawRunway));
                            if (drw)
                            {
                                memset(drw, 0, sizeof(CDrawRunway));
                                drw->runway = runway2;
                                drw->runway->altitude = (runway->altitude + runway2->altitude) / 2.0;
                                drw->isDoth = true;
                                memcpy_op(drw->runway_id, runway->runway_id, 6);
                                //c_vector_push_back(blk->m_drawRunwayInfo, drw);
                                blk->m_drawRunwayInfo2.push_back(drw);
                            }

                            free(runway);
                            runway = 0;
                        }
                    }
                    //c_vector_erase(runwayInfo, c_vector_iter_at(runwayInfo, j));
                    runwayInfo.erase(runwayInfo.begin() + j);
                    j = j - 1;
                    isBoth = 1;
                    goto Next;
                }
            }
        }

        if (!isBoth)
        {
            pCDrawRunway drw = (pCDrawRunway)malloc(sizeof(CDrawRunway));
            if (drw)
            {
                memset(drw, 0, sizeof(CDrawRunway));
                drw->runway = runway;
                drw->isDoth = 0;
                //c_vector_push_back(blk->m_drawRunwayInfo, drw);
                blk->m_drawRunwayInfo2.push_back(drw);
            }
        }
    }
}


void C3DAirportBlockReadData(pC3DAirportBlock blk)
{
    //C3DAirportBlockReadAirportData(blk);
    C3DAirportBlockReadRunWayData(blk);
}

void C3DAirportBlockCreatNode(pC3DAirportBlock blk)
{
    if (0 == blk) { return; }
    /*创建机场*/
    {
#if 0
        c_pvector vector = blk->m_airportInfo;
        unsigned int index = 0, num = c_vector_size(vector);
        c_iterator iter, beg = c_vector_begin(vector), end = c_vector_end(vector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pCAirport airport = (pCAirport)(ITER_REF(iter));
            pCMatrixTransform matnode = (pCMatrixTransform)(CNode_CreatNode(
                MatrixTransform)->pNodeData);
            pCNode geode = C3DAirportBlockCreatAirportDrawable(airport);
            CVec3d pos;
            //matnode->usList = 1;

            CNode_AddChild(matnode->node, geode);
            pos = Vec_CreatVec3d(airport->lon, airport->lat, airport->altitude);
            pos = Coordinate_DegreeVec3dLonLatAltToVec3d(pos);
            pos = Vec_Vec3dSubVec3d(&pos, &(blk->m_m_lonlatPos));

            matnode->matrix = CMatrix_TranslateVec3d(
                Vec_CreatVec3d(pos._v[0], pos._v[1], Layer2D_Airport));
            {

            }
            matnode->coord = WGS84;
            CNode_SetName(blk->m_mat->node, "blockMat");
            CNode_SetName(blk->m_mat->node, "blockMat__barrmat");
            CNode_AddChild(blk->m_mat->node, matnode->node);
        }
#endif
        auto &vec = blk->m_airportInfo2;
        unsigned int index = 0;
        auto num = vec.size();
        for (unsigned int i = 0; i < num; ++i)
        {
            auto airport = vec[i];
            pCMatrixTransform matnode = (pCMatrixTransform)(CNode_CreateNode(MatrixTransform)->pNodeData);
            pCNode geode = C3DAirportBlockCreatAirportDrawable(airport);
            CVec3d pos;
            //matnode->usList = 1;
            CNode_AddChild(matnode->node, geode);
            pos = Vec_CreatVec3d(airport->lon, airport->lat, airport->altitude);
            pos = Coordinate_DegreeVec3dLonLatAltToVec3d(pos);
            pos = Vec_Vec3dSubVec3d(&pos, &(blk->m_m_lonlatPos));

            matnode->matrix = CMatrix_TranslateVec3d(Vec_CreatVec3d(pos._v[0], pos._v[1], Layer2D_Airport));
            matnode->coord = WGS84;
            CNode_SetName(blk->m_mat->node, "blockMat");
            CNode_SetName(blk->m_mat->node, "blockMat__barrmat");
            CNode_AddChild(blk->m_mat->node, matnode->node);
        }
    }

    /*创建跑道*/
    {
#if 0
        c_pvector vector = blk->m_drawRunwayInfo;
        unsigned int index = 0, num = c_vector_size(vector);
        c_iterator iter, beg = c_vector_begin(vector), end = c_vector_end(vector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pCDrawRunway ruw = (pCDrawRunway)(ITER_REF(iter));

            pCMatrixTransform matnode = (pCMatrixTransform)(CNode_CreatNode(
                MatrixTransform)->pNodeData);
            CNode_SetName(matnode->node, "__runway_mat");
            pCNode geode = C3DAirportBlockCreatRunway(ruw);
            CNode_SetName(geode, "__runway_node");
            pCNode runwayframe = C3DAirportBlockCreatRunwayFrame(ruw);
            CNode_SetName(runwayframe, "__runwayframe");

            CMatrix rotate;

            CNode_AddChild(matnode->node, geode);
            CNode_AddChild(matnode->node, runwayframe);

#if 1//jiangyan:
            rotate = CMatrix_RotateAVec3d(ruw->runway->direction * DegreesToRadians,
                Vec_CreatVec3d(0, 0, -1));
            CMatrix lonlatMat = Coordinate_DegreeLonlatToMatrix(ruw->runway->lon, ruw->runway->lat,
                ruw->runway->altitude * 0.3048);

            matnode->matrix = CMatrix_MatrixMultMatrix(&rotate, &lonlatMat);
            matnode->coord = WGS84;
            CNode_SetName(blk->m_mat->node, "blockRunwayMat__runwaymat");
            CNode_AddChild(blk->m_mat->node, matnode->node);
#else
            pos = Vec_CreatVec3d(ruw->runway->lon, ruw->runway->lat, ruw->runway->altitude*0.3048 + 0.0);
            pos = Coordinate_DegreeVec3dLonLatAltToVec3d(pos);
            pos = Vec_Vec3dSubVec3d(&pos, &(blk->m_m_lonlatPos));


            rotate = CMatrix_RotateAVec3d(ruw->runway->direction*DegreesToRadians, Vec_CreatVec3d(0, 0, -1));
            {
                CMatrix lonlatMat = Coordinate_DegreeLonlatToMatrix(ruw->runway->lon, ruw->runway->lat, ruw->runway->altitude*0.3048);
                lonlatMat = CMatrix_GetRotateMatrix(&lonlatMat);
                rotate = Coordinate_YawPitchRollDegreeToMatrix(ruw->runway->direction, 0.0, 0.0);
                rotate = CMatrix_MatrixMultMatrix(&rotate, &lonlatMat);
                //	rotate = Coordinate_DegreeVec3dLonLatAltToVec3d(ruw->runway->direction*DegreesToRadians,Vec_CreatVec3d(0,0,-1));
            }
            trans = CMatrix_TranslateVec3d(pos);
            trans = CMatrix_MatrixMultMatrix(&rotate, &trans);
            //matnode->matrix = CMatrix_TranslateVec3d(Vec_CreatVec3d(pos._v[0],pos._v[1],Layer2D_Airport+1)); 
            matnode->matrix = trans;
            matnode->coord = WGS84;
            CNode_SetName(blk->m_mat->node, "blockRunwayMat");
            CNode_SetName(blk->m_mat->node, "blockRunwayMat__runwaymat");
            CNode_AddChild(blk->m_mat->node, matnode->node);
#endif
        }
#endif
        //c_pvector vec = blk->m_drawRunwayInfo;
        auto& vec = blk->m_drawRunwayInfo2;
        unsigned int index = 0;
        auto num = vec.size();
        for (unsigned int i = 0; i < num; ++i)
        {
            auto ruw = vec[i];

            pCMatrixTransform matnode = (pCMatrixTransform)(CNode_CreateNode(
                MatrixTransform)->pNodeData);
            CNode_SetName(matnode->node, "__runway_mat");
            pCNode geode = C3DAirportBlockCreatRunway(ruw);
            CNode_SetName(geode, "__runway_node");
            pCNode runwayframe = C3DAirportBlockCreatRunwayFrame(ruw);
            CNode_SetName(runwayframe, "__runwayframe");

            CMatrix rotate;

            CNode_AddChild(matnode->node, geode);
            CNode_AddChild(matnode->node, runwayframe);

#if 1//jiangyan:
            rotate = CMatrix_RotateAVec3d(ruw->runway->direction * DegreesToRadians,
                Vec_CreatVec3d(0, 0, -1));
            CMatrix lonlatMat = Coordinate_DegreeLonlatToMatrix(ruw->runway->lon, ruw->runway->lat, ruw->runway->altitude * 0.3048);

            matnode->matrix = CMatrix_MatrixMultMatrix(&rotate, &lonlatMat);
            matnode->coord = WGS84;
            CNode_SetName(blk->m_mat->node, "blockRunwayMat__runwaymat");
            CNode_AddChild(blk->m_mat->node, matnode->node);
#else
            pos = Vec_CreatVec3d(ruw->runway->lon, ruw->runway->lat, ruw->runway->altitude*0.3048 + 0.0);
            pos = Coordinate_DegreeVec3dLonLatAltToVec3d(pos);
            pos = Vec_Vec3dSubVec3d(&pos, &(blk->m_m_lonlatPos));

            rotate = CMatrix_RotateAVec3d(ruw->runway->direction*DegreesToRadians, Vec_CreatVec3d(0, 0, -1));
            {
                CMatrix lonlatMat = Coordinate_DegreeLonlatToMatrix(ruw->runway->lon, ruw->runway->lat, ruw->runway->altitude*0.3048);
                lonlatMat = CMatrix_GetRotateMatrix(&lonlatMat);
                rotate = Coordinate_YawPitchRollDegreeToMatrix(ruw->runway->direction, 0.0, 0.0);
                rotate = CMatrix_MatrixMultMatrix(&rotate, &lonlatMat);
                //	rotate = Coordinate_DegreeVec3dLonLatAltToVec3d(ruw->runway->direction*DegreesToRadians,Vec_CreatVec3d(0,0,-1));
            }
            trans = CMatrix_TranslateVec3d(pos);
            trans = CMatrix_MatrixMultMatrix(&rotate, &trans);
            //matnode->matrix = CMatrix_TranslateVec3d(Vec_CreatVec3d(pos._v[0],pos._v[1],Layer2D_Airport+1)); 
            matnode->matrix = trans;
            matnode->coord = WGS84;
            CNode_SetName(blk->m_mat->node, "blockRunwayMat");
            CNode_SetName(blk->m_mat->node, "blockRunwayMat__runwaymat");
            CNode_AddChild(blk->m_mat->node, matnode->node);
#endif
        }
    }
}

//************************************
// Method:    DrawableNodeRunwayCreateVBO
// FullName:  DrawableNodeRunwayCreateVBO
// Access:    public 
// Desc:      prepair a rectangle surface data for pCDrawable , create VBO for it
// Returns:   void
// Qualifier:
// Parameter: pCDrawable geometry
// Author:    yan.jiang
//************************************
void DrawableNodeRunwayCreateVBO_old(pCDrawable geometry)
{
    if (0 == geometry){ return; }
    if (g_supportVAO)
    {
        glGenVertexArrays(1, &geometry->vao);
        glBindVertexArray(geometry->vao);
    }

    //create vbo
    glGenBuffers(1, &geometry->vtx_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vtx_buffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(CVec3f)* geometry->_vertexsF->size, geometry->_vertexsF->data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CVec3f)* geometry->_vertexsF2.size(), geometry->_vertexsF2.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->clr_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->clr_buffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(CVec4f)*geometry->_colors->size, geometry->_colors->data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CVec4f)*geometry->_colors2.size(), geometry->_colors2.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->tcd_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->tcd_buffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(CVec2f)*geometry->_texcoords->size, geometry->_texcoords->data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CVec2f)*geometry->_texcoords2.size(), geometry->_texcoords2.data(), GL_STATIC_DRAW);

    if (g_supportVAO)
    {
        setGeometyVbo(geometry);
        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


pCNode C3DAirportBlockCreatRunwayMid(pCDrawRunway ruw)
{
    pCNode group = CNode_CreateNode(Group);
    //pCNode lines = CNode_CreatNode(Drawable);
    pCNode _geode = CNode_CreateNode(NT_Drawable);

    if (0 == ruw || 0 == group || 0 == _geode || 0 == ruw->runway){
        return group;
    }

    pCDrawable geometry = (pCDrawable)_geode->pNodeData;
    geometry->drawType = CDrawableType_RunWay;

    CVec3d tmpPos[4];
    CVec2f tmpTexcoord[4];
    CVec4f tmpColors[4];

    double _w = ruw->runway->width * 0.3048;
    double _h = ruw->runway->len * 0.3048;
    double _h_left, _h_right;

    _h_left = _h_right = _h / 2.0;

    if (ruw->runway->runway_id[4] == 'L' || ruw->runway->runway_id[4] == 'R')
    {
        //减掉L和R占据的位置
        _h_left = _h_left - left_length;
        if (ruw->isDoth)
        {
            _h_right = _h_right - left_length;
        }
    }

    //threshold(zebra marking)
    _h_left = _h_left - threshold_length;
    //减掉数字端长度
    _h_left = _h_left - num_length;
    if (ruw->isDoth)
    {
        _h_right = _h_right - threshold_length;
        _h_right = _h_right - num_length;
    }

    tmpPos[0] = Vec_CreatVec3d(-_w / 2.0, -_h_left, 0.0);
    tmpPos[1] = Vec_CreatVec3d(_w / 2.0, -_h_left, 0.0);
    tmpPos[2] = Vec_CreatVec3d(_w / 2.0, _h_right, 0.0);
    tmpPos[3] = Vec_CreatVec3d(-_w / 2.0, _h_right, 0.0);

    tmpTexcoord[0] = Vec_CreatVec2f(0.0f, 100.0f);
    tmpTexcoord[1] = Vec_CreatVec2f(1, 100.0f);
    tmpTexcoord[2] = Vec_CreatVec2f(1.f, 0);
    tmpTexcoord[3] = Vec_CreatVec2f(0, 0);

    tmpColors[0] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[1] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[2] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[3] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);

    drawRectangleToDrawableNode(geometry, tmpPos, tmpColors, tmpTexcoord);

    DrawableNodeCreateVBO(geometry);

    geometry->_normalBind = BIND_OVERALL;//jy: to be checked out
    geometry->_colorBind = BIND_OVERALL;

    /*设置文理贴图*/
    geometry->texture_image = TextureMgr_GetImage(TextureType_RunWayLine);

    CNode_SetName(group, "blockMat__Mid_runway___group");
    CNode_SetName(_geode, "blockMat__Mid_runway___drawable");
    CNode_AddChild(group, _geode);

    return group;
}

pCNode C3DAirportBlockCreatRunwayThreshold(pCDrawRunway ruw, unsigned char isLittle)
{
    pCNode group = CNode_CreateNode(Group);
    pCNode _geode = CNode_CreateNode(NT_Drawable);

    if (0 == ruw || 0 == group || 0 == _geode || 0 == ruw->runway){
        return group;
    }

    pCDrawable geometry = (pCDrawable)_geode->pNodeData;
    geometry->drawType = CDrawableType_RunWay;

    CVec3d tmpPos[4];
    CVec2f tmpTexcoord[4];
    CVec4f tmpColors[4];

    double _w = ruw->runway->width * 0.3048;
    double _h = ruw->runway->len * 0.3048;
    double _h_left, _h_right;

    _h_left = _h_right = _h / 2.0;

    double left = _h_left;
    double right = _h_left - threshold_length;
    //减掉入口占据的位置 

    tmpPos[0] = Vec_CreatVec3d(-_w / 2.0, -left, 0.0);
    tmpPos[1] = Vec_CreatVec3d(_w / 2.0, -left, 0.0);
    tmpPos[2] = Vec_CreatVec3d(_w / 2.0, -right, 0.0);
    tmpPos[3] = Vec_CreatVec3d(-_w / 2.0, -right, 0.0);

    tmpTexcoord[0] = Vec_CreatVec2f(0.0f, 0.0f);
    tmpTexcoord[1] = Vec_CreatVec2f(1.0f, 0.0f);
    tmpTexcoord[2] = Vec_CreatVec2f(1.0f, 1.0f);
    tmpTexcoord[3] = Vec_CreatVec2f(0.0f, 1.0f);

    tmpColors[0] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[1] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[2] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[3] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);

    drawRectangleToDrawableNode(geometry, tmpPos, tmpColors, tmpTexcoord);

    if (ruw->isDoth)
    {
        //jy: just modify the vertex position
        tmpPos[0] = Vec_CreatVec3d(_w / 2.0, left, 0.0);
        tmpPos[1] = Vec_CreatVec3d(-_w / 2.0, left, 0.0);
        tmpPos[2] = Vec_CreatVec3d(-_w / 2.0, right, 0.0);
        tmpPos[3] = Vec_CreatVec3d(_w / 2.0, right, 0.0);

        drawRectangleToDrawableNode(geometry, tmpPos, tmpColors, tmpTexcoord);
    }

    DrawableNodeCreateVBO(geometry);

    geometry->_normalBind = BIND_OVERALL;
    geometry->_colorBind = BIND_OVERALL;

    /*设置文理贴图*/
    geometry->texture_image = TextureMgr_GetImage(TextureType_RunWayThr);

    CNode_SetName(_geode, "blockMat__runway___drawable");
    CNode_AddChild(group, _geode);

    return group;
}

pCNode C3DAirportBlockCreatRunwayLeft(pCDrawRunway ruw, unsigned char isLittle)
{
    pCNode node = CNode_CreateNode(Node);
    if (0 == ruw || 0 == node || 0 == ruw->runway){
        return node;
    }

    CVec3d tmpPos[4];
    CVec2f tmpTexcoord[4];
    CVec4f tmpColors[4];

    pCImage tmpImage = NULL;

    double _w = ruw->runway->width * 0.3048;
    double _h = ruw->runway->len * 0.3048;
    double _h_left, _h_right;
    _h_left = _h_right = _h / 2.0;

    //左侧的
    _h_left = _h_left - threshold_length;
    _h_right = _h_left - left_length;

    tmpTexcoord[0] = Vec_CreatVec2f(0.0f, 0.0f);
    tmpTexcoord[1] = Vec_CreatVec2f(1.0f, 0.0f);
    tmpTexcoord[2] = Vec_CreatVec2f(1.0f, 1.0f);
    tmpTexcoord[3] = Vec_CreatVec2f(0.0f, 1.0f);

    tmpColors[0] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[1] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[2] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[3] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);

    if (ruw->runway->runway_id[4] == 'L' || (ruw->runway->runway_id[4] == 'R'))
    {
        pCNode _geode = CNode_CreateNode(NT_Drawable);
        pCDrawable geometry = (pCDrawable)_geode->pNodeData;
        geometry->drawType = CDrawableType_RunWay;

        tmpPos[0] = Vec_CreatVec3d(-_w / 2.0, -_h_left, 0.0);
        tmpPos[1] = Vec_CreatVec3d(_w / 2.0, -_h_left, 0.0);
        tmpPos[2] = Vec_CreatVec3d(_w / 2.0, -_h_right, 0.0);
        tmpPos[3] = Vec_CreatVec3d(-_w / 2.0, -_h_right, 0.0);

        drawRectangleToDrawableNode(geometry, tmpPos, tmpColors, tmpTexcoord);
        DrawableNodeCreateVBO(geometry);

        /*设置文理贴图*/
        if (ruw->runway->runway_id[4] == 'L')
        {
            tmpImage = TextureMgr_GetImage(TextureType_RunWayLeft);
        }
        if (ruw->runway->runway_id[4] == 'R')
        {
            tmpImage = TextureMgr_GetImage(TextureType_RunWayRight);
        }
        geometry->texture_image = tmpImage;

        CNode_SetName(_geode, "blockMat__runwayNum___drawable");
        CNode_AddChild(node, _geode);
    }

    //right
    if (ruw->isDoth && (ruw->runway_id[4] == 'L' || (ruw->runway_id[4] == 'R')))
    {
        pCNode _geode2 = CNode_CreateNode(NT_Drawable);
        pCDrawable geometry2 = (pCDrawable)_geode2->pNodeData;
        geometry2->drawType = CDrawableType_RunWay;

        tmpPos[0] = Vec_CreatVec3d(_w / 2.0, _h_left, 0.0);
        tmpPos[1] = Vec_CreatVec3d(-_w / 2.0, _h_left, 0.0);
        tmpPos[2] = Vec_CreatVec3d(-_w / 2.0, _h_right, 0.0);
        tmpPos[3] = Vec_CreatVec3d(_w / 2.0, _h_right, 0.0);

        drawRectangleToDrawableNode(geometry2, tmpPos, tmpColors, tmpTexcoord);
        DrawableNodeCreateVBO(geometry2);

        /*设置文理贴图*/
        if (ruw->runway_id[4] == 'L')
        {
            tmpImage = TextureMgr_GetImage(TextureType_RunWayLeft);
        }
        if (ruw->runway_id[4] == 'R')
        {
            tmpImage = TextureMgr_GetImage(TextureType_RunWayRight);
        }
        geometry2->texture_image = tmpImage;

        geometry2->_normalBind = BIND_OVERALL;
        geometry2->_colorBind = BIND_OVERALL;

        CNode_SetName(_geode2, "blockMat__runway___drawable");
        CNode_AddChild(node, _geode2);
    }

    return node;
}

pCNode C3DAirportBlockCreatRunwayNum(pCDrawRunway ruw, unsigned char isLittle)
{
    char dirLeft[3] = { 0 };
    char dirRight[3] = { 0 };
    int runLeft = 0;
    int runRight = 0;
    pCNode node = CNode_CreateNode(Group);
    if (0 == ruw || 0 == node || 0 == ruw->runway){
        return node;
    }

    dirLeft[0] = ruw->runway->runway_id[2];
    dirLeft[1] = ruw->runway->runway_id[3];
    dirLeft[2] = 0;
    runLeft = atoi(dirLeft);
    if (ruw->isDoth)
    {
        dirRight[0] = ruw->runway_id[2];
        dirRight[1] = ruw->runway_id[3];
        dirRight[2] = 0;
        runRight = atoi(dirRight);
    }

    CVec3d tmpPos[4];
    CVec2f tmpTexcoord[4];
    CVec4f tmpColors[4];

    pCImage tmpImage = NULL;

    double _w = ruw->runway->width * 0.3048;
    double _h = ruw->runway->len * 0.3048;
    double _h_left, _h_right;
    _h_left = _h_right = _h / 2.0;

    //左侧的
    tmpTexcoord[0] = Vec_CreatVec2f(0.0f, 0.0f);
    tmpTexcoord[1] = Vec_CreatVec2f(1.0f, 0.0f);
    tmpTexcoord[2] = Vec_CreatVec2f(1.0f, 1.0f);
    tmpTexcoord[3] = Vec_CreatVec2f(0.0f, 1.0f);

    tmpColors[0] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[1] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[2] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    tmpColors[3] = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);

    _h_left = _h_left - threshold_length;
    if (ruw->runway->runway_id[4] == 'L' || ruw->runway->runway_id[4] == 'R')
    {
        _h_left = _h_left - left_length;
    }
    _h_right = _h_left - num_length;

    pCNode _geode = CNode_CreateNode(NT_Drawable);
    pCDrawable geometry = (pCDrawable)_geode->pNodeData;
    geometry->drawType = CDrawableType_RunWay;

    tmpPos[0] = Vec_CreatVec3d(-_w / 2.0, -_h_left, 0.0);
    tmpPos[1] = Vec_CreatVec3d(_w / 2.0, -_h_left, 0.0);
    tmpPos[2] = Vec_CreatVec3d(_w / 2.0, -_h_right, 0.0);
    tmpPos[3] = Vec_CreatVec3d(-_w / 2.0, -_h_right, 0.0);

    drawRectangleToDrawableNode(geometry, tmpPos, tmpColors, tmpTexcoord);
    DrawableNodeCreateVBO(geometry);

    /*设置文理贴图*/
    geometry->texture_image = TextureMgr_GetImage(static_cast<_TextureType>(runLeft));

    CNode_SetName(_geode, "blockMat__runwayNum___drawable");
    CNode_AddChild(node, _geode);

    //right
    if (ruw->isDoth)
    {
        pCNode _geode2 = CNode_CreateNode(NT_Drawable);
        pCDrawable geometry2 = (pCDrawable)_geode2->pNodeData;
        geometry2->drawType = CDrawableType_RunWay;

        tmpPos[0] = Vec_CreatVec3d(_w / 2.0, _h_left, 0.0);
        tmpPos[1] = Vec_CreatVec3d(-_w / 2.0, _h_left, 0.0);
        tmpPos[2] = Vec_CreatVec3d(-_w / 2.0, _h_right, 0.0);
        tmpPos[3] = Vec_CreatVec3d(_w / 2.0, _h_right, 0.0);

        drawRectangleToDrawableNode(geometry2, tmpPos, tmpColors, tmpTexcoord);
        DrawableNodeCreateVBO(geometry2);

        /*设置文理贴图*/
        geometry2->texture_image = TextureMgr_GetImage(static_cast<_TextureType>(runRight));

        CNode_SetName(_geode2, "blockMat__runway___drawable");
        CNode_AddChild(node, _geode2);
    }

    return node;
}

pCNode C3DAirportBlockCreatRunway(pCDrawRunway ruw)
{
    pCNode _node = CNode_CreateNode(Node);
    pCNode _geode_mid = C3DAirportBlockCreatRunwayMid(ruw);
    pCNode _geode_rway_thr_little = C3DAirportBlockCreatRunwayThreshold(ruw, 1);
    pCNode _geode_rway_left_little = C3DAirportBlockCreatRunwayLeft(ruw, 1);
    pCNode _geode_rway_num_little = C3DAirportBlockCreatRunwayNum(ruw, 1);
    CNode_AddChild(_node, _geode_mid);
    CNode_AddChild(_node, _geode_rway_thr_little);
    CNode_AddChild(_node, _geode_rway_left_little);
    CNode_AddChild(_node, _geode_rway_num_little);
    return _node;
}

pCNode C3DAirportBlockCreatRunwayFrame(pCDrawRunway ruw)
{
    pCNode _geode = CNode_CreateNode(NT_Drawable);
    pCDrawable geometry = (pCDrawable)_geode->pNodeData;
    //pCVector _vertexs = geometry->_vertexs;
    //pCVector _coords = geometry->_texcoords;
    //pCVector _colors = geometry->_colors;
    //pCVector _normals = geometry->_normals;
    //pCVector primitiveSets = geometry->_primitiveSets;

    CPrimitiveSet primitiveSet;
    CVec3d pos[4];
    CVec4f tmpColor;
    float _x = 0, _y = 0, _z = 0, _w = ruw->runway->width*0.3048, _h = ruw->runway->len*0.3048, _s = 1, _t = 1;

    geometry->drawType = CDrawableType_RunWayFrame;
    pos[0] = Vec_CreatVec3d(-_w / 2.0, -_h / 2.0, 0.0);
    pos[1] = Vec_CreatVec3d(_w / 2.0, -_h / 2.0, 0.0);
    pos[2] = Vec_CreatVec3d(_w / 2.0, _h / 2.0, 0.0);
    pos[3] = Vec_CreatVec3d(-_w / 2.0, _h / 2.0, 0.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[1]);
    //CVector_Pushback(_vertexs, &pos[2]);
    //CVector_Pushback(_vertexs, &pos[3]);

    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[1]);
    geometry->_vertexs2.push_back(pos[2]);
    geometry->_vertexs2.push_back(pos[3]);

    tmpColor = Vec_CreatVec4f(0.0, 1.0, 0.0, 1.0);
    //CVector_Pushback(_colors, &tmpColor);
    geometry->_colors2.push_back(tmpColor);

    primitiveSet.beg = 0;
    primitiveSet.count = 4;
    primitiveSet.type = GL_LINE_LOOP;
    //CVector_Pushback(primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);

    _t = 0;
    for (_s = 0; _s < 120; _s += 2)
    {
        pos[0] = Vec_CreatVec3d(0.0, -_h*0.6 + _s*(1.2*_h / 120), 0.0);
        pos[1] = Vec_CreatVec3d(0.0, -_h*0.6 + (_s + 1)*(1.2* _h / 120), 0.0);
        //CVector_Pushback(_vertexs, &pos[0]);
        //CVector_Pushback(_vertexs, &pos[1]);
        geometry->_vertexs2.push_back(pos[0]);
        geometry->_vertexs2.push_back(pos[1]);
        _t += 2;
    }

    tmpColor = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    //CVector_Pushback(_colors, &tmpColor);
    geometry->_colors2.push_back(tmpColor);

    primitiveSet.beg = 4;
    primitiveSet.count = _t;
    primitiveSet.type = GL_LINES;
    //CVector_Pushback(primitiveSets, &primitiveSet);        
    geometry->_primitiveSets2.push_back(primitiveSet);

    geometry->_normalBind = BIND_OFF;
    geometry->_colorBind = BIND_PER_PRIMITIVE_SET;

    CNode_SetName(_geode, "blockMat__runway___drawable");
    return _geode;
}

pCNode  C3DAirportBlockCreatAirportDrawable(pCAirport airport)
{
    pCNode _geode = CNode_CreateNode(NT_Drawable);
    pCDrawable _geometry = (pCDrawable)_geode->pNodeData;
    //pCVector _vertexs = _geometry->_vertexs;
    //pCVector _coords = _geometry->_texcoords;
    //pCVector _colors = _geometry->_colors;
    //pCVector _normals = _geometry->_normals;
    //pCVector primitiveSets = _geometry->_primitiveSets;
    pCImage image = _geometry->texture_image;
    CPrimitiveSet primitiveSet;

    float _x = 0, _y = 0, _z = 0, _w = 2000, _h = 2000, _s = 1, _t = 1;

    CVec3d pos[4];
    CVec2f _coord[4];

    CVec3f normal;
    CVec4f tmpColor;
    pos[0] = Vec_CreatVec3d(-_w / 2.0, -_h / 2.0, 0.0);
    pos[1] = Vec_CreatVec3d(_w / 2.0, -_h / 2.0, 0.0);
    pos[2] = Vec_CreatVec3d(_w / 2.0, _h / 2.0, 0.0);
    pos[3] = Vec_CreatVec3d(-_w / 2.0, _h / 2.0, 0.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[1]);
    //CVector_Pushback(_vertexs, &pos[2]);
    //CVector_Pushback(_vertexs, &pos[3]);
    _geometry->_vertexs2.push_back(pos[0]);
    _geometry->_vertexs2.push_back(pos[1]);
    _geometry->_vertexs2.push_back(pos[2]);
    _geometry->_vertexs2.push_back(pos[3]);

    _coord[0] = Vec_CreatVec2f(0.0f, 1.0f);
    _coord[1] = Vec_CreatVec2f(1, 1.0f);
    _coord[2] = Vec_CreatVec2f(1, 0);
    _coord[3] = Vec_CreatVec2f(0, 0);

    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[1]);
    //CVector_Pushback(_coords, &_coord[2]);
    //CVector_Pushback(_coords, &_coord[3]);
    _geometry->_texcoords2.push_back(_coord[0]);
    _geometry->_texcoords2.push_back(_coord[1]);
    _geometry->_texcoords2.push_back(_coord[2]);
    _geometry->_texcoords2.push_back(_coord[3]);

    normal = Vec_CreatVec3f(0.0f, 0.0f, 1.0f);
    //CVector_Pushback(_normals, &normal);
    _geometry->_normals2.push_back(normal);

    tmpColor = Vec_CreatVec4f(1.0, 1.0, 1.0, 1.0);
    //CVector_Pushback(_colors, &tmpColor);
    _geometry->_colors2.push_back(tmpColor);

    primitiveSet.beg = 0;
    primitiveSet.count = 4;
#if 0//jiangyan: opengl es2 不支持
    primitiveSet.type = GL_QUADS;
#endif
    //CVector_Pushback(primitiveSets, &primitiveSet);
    _geometry->_primitiveSets2.push_back(primitiveSet);

    _geometry->_normalBind = BIND_OVERALL;
    _geometry->_colorBind = BIND_OVERALL;

    CNode_SetName(_geode, "blockMat__barrmat___drawable");
    return _geode;
}

