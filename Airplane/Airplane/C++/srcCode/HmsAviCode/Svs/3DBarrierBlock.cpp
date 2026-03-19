
#include <string>
#include "MathCommon.h"

extern bool g_supportVAO;
extern double CalculateAngle(double dLong1, double dLat1, double dLong2, double dLat2);
#include "Drawable.h"
#include "AltManager.h"
#include "FPPManipulator.h"
#include "3DBarrierBlock.h"
#include "Group.h"
//#include "TAWS.h"
#include "Global.h"
#include "FileManager.h"
#include "TextureMgr.h"
#include "Coordinate.h"
#include "Matrix.h"



#define KM           (1000)
#define VISIBLE_DS   20*KM
static int plane_index[5][4] = { { 1, 5, 4, 0 }, { 0, 4, 6, 2 }, { 2, 6, 7, 3 }, { 3, 7, 5, 1 }, { 1, 0, 2, 3 } };

pC3DBarrierBlock C3DBarrierBlock_Create()
{
    //pC3DBarrierBlock blk = (pC3DBarrierBlock)malloc(sizeof(C3DBarrierBlock));
    auto blk = new C3DBarrierBlock;
#if 0
    if (blk)
    {
        memset(blk, 0, sizeof(C3DBarrierBlock));
        blk->m_root = CNode_CreatNode(Node);
        CNode_SetName(blk->m_root, "__BarrBlock");
        // 	blk->m_mat = (pCMatrixTransform)(CNode_CreatNode(MatrixTransform)->pNodeData);
        // 	CNode_AddChild(blk->m_root,blk->m_mat->node);
        /*创建障碍物信息容器*/

        //blk->m_Barriernfo = &(blk->m_v_Barriernfo);
        //blk->m_pBarrierRecord = &(blk->m_v_pBarrierRecord);
        //c_vector_create(blk->m_Barriernfo, NULL);
        //c_vector_create(blk->m_pBarrierRecord, NULL);
    }
#endif
    blk->m_root = CNode_CreateNode(EN_NodeType::Node);
    CNode_SetName(blk->m_root, "__BarrBlock");

    return blk;
}

void C3DBarrierBlock_Destory(pC3DBarrierBlock blk)
{
    if (NULL == blk)
    {
        return;
    }

    CNode_RemoveFromParentsAndFreeNode(blk->m_root);
    /*释放数据集*/
    {
#if 0
        c_pvector barrierVector = blk->m_Barriernfo;
        unsigned int index = 0, num = c_vector_size(barrierVector);
        c_iterator iter, beg = c_vector_begin(barrierVector), end = c_vector_end(barrierVector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pCBarrierParameters ruw = (pCBarrierParameters)(ITER_REF(iter));
            free(ruw);
            ruw = 0;
        }

        c_vector_clear(blk->m_Barriernfo);
        c_vector_destroy(blk->m_Barriernfo);
#endif
        for (unsigned int i = 0; i < blk->m_Barriernfo2.size(); ++i)
        {
            auto ruw = blk->m_Barriernfo2[i];
            free(ruw);
        }
        blk->m_Barriernfo2.clear();
    }
    /*释放关联数据*/
    {
#if 0
        c_pvector barrierVector = blk->m_pBarrierRecord;
        unsigned int index = 0, num = c_vector_size(barrierVector);
        c_iterator iter, beg = c_vector_begin(barrierVector), end = c_vector_end(barrierVector);
        for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
        {
            pC3DBarrierRecord data = (pC3DBarrierRecord)(ITER_REF(iter));
            free(data);
            data = 0;
            //iter  =c_vector_begin(barrierVector);
        }

        c_vector_clear(blk->m_pBarrierRecord);
        c_vector_destroy(blk->m_pBarrierRecord);
#endif
        for (unsigned int i = 0; i < blk->m_pBarrierRecord2.size(); ++i)
        {
            auto data = blk->m_pBarrierRecord2[i];
            free(data);
            data = NULL;
        }
        blk->m_pBarrierRecord2.clear();
    }
    //free(blk);
    delete blk;
}

void C3DBarrierBlockSetLonlat(pC3DBarrierBlock blk, CVec3d lon)
{
    if (blk)
    {
        //CVec3d pos=Vec_CreatVec3d((int)lon._v[0]+0.5, (int)lon._v[1]+0.5, 0.0);
        CVec3d pos = Vec_CreatVec3d((int)lon._v[0], (int)lon._v[1], 0.0);
        blk->m_lonlat = pos;
        blk->m_lonlatPos = Coordinate_DegreeVec3dLonLatAltToVec3d(pos);
    }
}

void C3DBarrierBlockReadBarrierData(pC3DBarrierBlock blk)
{
    if (0 == blk)
    {
        return;
    }

    char filepath[256] = { 0 };
    unsigned int len;
    unsigned char* barrierData = NULL;

    char buff[256] = { 0 };
    char name[64] = { 0 };
    const char* taws_data_path = GetTawsDataPath();
    int lon = (int)(blk->m_lonlat._v[0]);
    int lat = (int)(blk->m_lonlat._v[1]);
    if (lon < 0)
    {
        lon += 1;
    }
    if (lat < 0)
    {
        lat += 1;
    }

    sprintf(name, "export_%03d_%03d.dat", abs(lon), abs(lat));
    sprintf(buff, "%s/Obstacle/%sx%sy/%s", taws_data_path, blk->m_lonlat._v[0] >= 0 ? "+" : "-", blk->m_lonlat._v[1] >= 0 ? "+" : "-", name);
    memcpy_op(filepath, buff, strlen(buff));

    len = FileManager_LoadBinaryFile(filepath, &barrierData);

    if (NULL == barrierData)
    {
        return;
    }
    else
    {
        auto sizeofBarrier = sizeof(BarrierParameters);
        int recordNumber = len / sizeofBarrier;
        printf("num:%d\n", recordNumber);
        for (int i = 0; i < recordNumber; i++)
        {
            pCBarrierParameters barrie = (pCBarrierParameters)malloc(sizeof(BarrierParameters));
            memcpy_op(barrie, barrierData + sizeofBarrier*i, sizeofBarrier);
            barrie->height = barrie->height *0.3048;
            barrie->altitude = barrie->altitude *0.3048;
            if (barrie->height == 0)
            {
                int jepAlt = barrie->altitude;
                int demAlt = CAltManager_GetAltByLonLat(barrie->longitude, barrie->latitude);
                barrie->height = jepAlt > demAlt ? (jepAlt - demAlt) : (demAlt - jepAlt);
                barrie->altitude = jepAlt > demAlt ? (demAlt) : (jepAlt);
                printf("warning barr height error,height:%d,alt:%d\n", barrie->height, barrie->altitude);
            }
            //c_vector_push_back(blk->m_Barriernfo, barrie);
            blk->m_Barriernfo2.push_back(barrie);
        }

        if (NULL != barrierData)
        {
            free(barrierData);
            barrierData = NULL;
        }
        return;
    }
}

void C3DBarrierBlockCreatBarrierNode(pC3DBarrierBlock blk)
{
    if (0 == blk){ return; }
    //c_pvector barrierVector = blk->m_Barriernfo;
    unsigned int index = 0;
    //unsigned int num = c_vector_size(barrierVector);
    unsigned int num = blk->m_Barriernfo2.size();
    //c_iterator iter;
    //c_iterator beg = c_vector_begin(barrierVector);
    //c_iterator end = c_vector_end(barrierVector);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < num; ++i)
    {
        //pCBarrierParameters dataptr = (pCBarrierParameters)(ITER_REF(iter));
        auto dataptr = blk->m_Barriernfo2[i];
        pCMatrixTransform matnode = (pCMatrixTransform)(CNode_CreateNode(MatrixTransform)->pNodeData);
        CNode_SetName(matnode->node, "__BarrBillboardMatrix");
        pCNode geode = C3DBarrierBlockCreatBarrierDrawableBillboard(dataptr);
        CVec3d pos;
        CMatrix rotate;
        float barrierAlt;
        if (dataptr->height <= 0)
        {
            dataptr->height = 20.0;
        }

        barrierAlt = CAltManager_GetAltByLonLat(dataptr->longitude, dataptr->latitude);
        barrierAlt += dataptr->height / 2.0;
        dataptr->altitude = barrierAlt;

        CNode_AddChild(matnode->node, geode);
        pos = Vec_CreatVec3d(dataptr->longitude, dataptr->latitude, barrierAlt);
        rotate = Coordinate_Vec3dDegreeLonlatToMatrix(pos);
        matnode->matrix = rotate;
        CNode_AddChild(blk->m_root, matnode->node);

        pC3DBarrierRecord record = (pC3DBarrierRecord)malloc(
            sizeof(C3DBarrierRecord));
        memset(record, 0, sizeof(C3DBarrierRecord));
        record->barrier = dataptr;
        record->m_mat = matnode->node;
        record->m_drawable = geode;
        //c_vector_push_back(blk->m_pBarrierRecord, record);
        blk->m_pBarrierRecord2.push_back(record);
    }
}

#if 0
pCNode  C3DBarrierBlockCreatBarrierDrawable_V(pCBarrierParameters ruw)
{
    pCNode _geode = CNode_CreateNode(NT_Drawable);
    if (0 == ruw || 0 == _geode){
        return 0;
    }

    pCDrawable geometry = (pCDrawable)_geode->pNodeData;
    if (0 == geometry){
        return 0;
    }

    //pCVector _vertexs = geometry->_vertexs;
    //pCVector _coords = geometry->_texcoords;
    //pCVector _colors = geometry->_colors;
    //pCVector _normals = geometry->_normals;
    //pCVector primitiveSets = geometry->_primitiveSets;
    pCImage image = geometry->texture_image;
    CPrimitiveSet primitiveSet;

    float _x = 0, _y = 0, _z = 0, _w = ruw->height, _h = ruw->height, _s = 1, _t = 1;

    CVec3d pos[9];
    CVec2f _coord[4];

    CVec3f normal;
    CVec4f tmpColor;
    if (_h <= 0.0)
    {
        _h = _w;
    }
    pos[0] = Vec_CreatVec3d(0.0, 0.0, 0.0);
    pos[1] = Vec_CreatVec3d(-_w / 2.0, 0.0, -_h / 2.0);
    pos[2] = Vec_CreatVec3d(0.0, 0.0, -_h / 2.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[1]);
    //CVector_Pushback(_vertexs, &pos[2]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[1]);
    geometry->_vertexs2.push_back(pos[2]);

    _coord[0] = Vec_CreatVec2f(0.5f, 0.5f);
    _coord[1] = Vec_CreatVec2f(0.0, 0.0f);
    _coord[2] = Vec_CreatVec2f(0.5, 0);
    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[1]);
    //CVector_Pushback(_coords, &_coord[2]);
    geometry->_texcoords2.push_back(_coord[0]);
    geometry->_texcoords2.push_back(_coord[1]);
    geometry->_texcoords2.push_back(_coord[2]);

    pos[0] = Vec_CreatVec3d(0.0, 0.0, 0.0);
    pos[1] = Vec_CreatVec3d(_w / 2.0, 0.0, -_h / 2.0);
    pos[2] = Vec_CreatVec3d(0.0, 0.0, -_h / 2.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[2]);
    //CVector_Pushback(_vertexs, &pos[1]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[2]);
    geometry->_vertexs2.push_back(pos[1]);


    _coord[0] = Vec_CreatVec2f(0.5f, 0.5f);
    _coord[1] = Vec_CreatVec2f(1.0, 0.0f);
    _coord[2] = Vec_CreatVec2f(0.5, 0);
    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[2]);
    //CVector_Pushback(_coords, &_coord[1]);
    geometry->_texcoords2.push_back(_coord[0]);
    geometry->_texcoords2.push_back(_coord[2]);
    geometry->_texcoords2.push_back(_coord[1]);


    pos[0] = Vec_CreatVec3d(0.0, 0.0, 0.0);
    pos[1] = Vec_CreatVec3d(_w / 2.0, 0.0, -_h / 2.0);
    pos[2] = Vec_CreatVec3d(_w / 2.0, 0.0, 0.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[1]);
    //CVector_Pushback(_vertexs, &pos[2]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[1]);
    geometry->_vertexs2.push_back(pos[2]);


    _coord[0] = Vec_CreatVec2f(0.5f, 0.5f);
    _coord[1] = Vec_CreatVec2f(1.0, 0.0f);
    _coord[2] = Vec_CreatVec2f(1.0, 0.5);
    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[1]);
    //CVector_Pushback(_coords, &_coord[2]);
    geometry->_texcoords2.push_back(_coord[0]);
    geometry->_texcoords2.push_back(_coord[1]);
    geometry->_texcoords2.push_back(_coord[2]);


    pos[0] = Vec_CreatVec3d(0.0, 0.0, 0.0);
    pos[1] = Vec_CreatVec3d(_w / 2.0, 0.0, _h / 2.0);
    pos[2] = Vec_CreatVec3d(_w / 2.0, 0.0, 0.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[2]);
    //CVector_Pushback(_vertexs, &pos[1]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[2]);
    geometry->_vertexs2.push_back(pos[1]);


    _coord[0] = Vec_CreatVec2f(0.5f, 0.5f);
    _coord[1] = Vec_CreatVec2f(1.0, 1.0f);
    _coord[2] = Vec_CreatVec2f(1.0, 0.5);
    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[2]);
    //CVector_Pushback(_coords, &_coord[1]);
    geometry->_texcoords2.push_back(_coord[0]);
    geometry->_texcoords2.push_back(_coord[2]);
    geometry->_texcoords2.push_back(_coord[1]);
    //
    pos[0] = Vec_CreatVec3d(0.0, 0.0, 0.0);
    pos[1] = Vec_CreatVec3d(_w / 2.0, 0.0, _h / 2.0);
    pos[2] = Vec_CreatVec3d(0.0, 0.0, _h / 2.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[1]);
    //CVector_Pushback(_vertexs, &pos[2]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[1]);
    geometry->_vertexs2.push_back(pos[2]);


    _coord[0] = Vec_CreatVec2f(0.5f, 0.5f);
    _coord[1] = Vec_CreatVec2f(1.0, 1.0f);
    _coord[2] = Vec_CreatVec2f(0.5, 1.0);
    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[1]);
    //CVector_Pushback(_coords, &_coord[2]);
    geometry->_texcoords2.push_back(_coord[0]);
    geometry->_texcoords2.push_back(_coord[1]);
    geometry->_texcoords2.push_back(_coord[2]);

    //
    pos[0] = Vec_CreatVec3d(0.0, 0.0, 0.0);
    pos[1] = Vec_CreatVec3d(-_w / 2.0, 0.0, _h / 2.0);
    pos[2] = Vec_CreatVec3d(0.0, 0.0, _h / 2.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[2]);
    //CVector_Pushback(_vertexs, &pos[1]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[2]);
    geometry->_vertexs2.push_back(pos[1]);


    _coord[0] = Vec_CreatVec2f(0.5f, 0.5f);
    _coord[1] = Vec_CreatVec2f(0.0, 1.0f);
    _coord[2] = Vec_CreatVec2f(0.5, 1.0);
    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[2]);
    //CVector_Pushback(_coords, &_coord[1]);
    geometry->_texcoords2.push_back(_coord[0]);
    geometry->_texcoords2.push_back(_coord[2]);
    geometry->_texcoords2.push_back(_coord[1]);

    //
    pos[0] = Vec_CreatVec3d(0.0, 0.0, 0.0);
    pos[1] = Vec_CreatVec3d(-_w / 2.0, 0.0, _h / 2.0);
    pos[2] = Vec_CreatVec3d(-_w / 2.0, 0.0, 0.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[1]);
    //CVector_Pushback(_vertexs, &pos[2]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[1]);
    geometry->_vertexs2.push_back(pos[2]);


    _coord[0] = Vec_CreatVec2f(0.5f, 0.5f);
    _coord[1] = Vec_CreatVec2f(0.0, 1.0f);
    _coord[2] = Vec_CreatVec2f(0.0, 0.5);
    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[1]);
    //CVector_Pushback(_coords, &_coord[2]);
    geometry->_texcoords2.push_back(_coord[0]);
    geometry->_texcoords2.push_back(_coord[1]);
    geometry->_texcoords2.push_back(_coord[2]);

    //
    pos[0] = Vec_CreatVec3d(0.0, 0.0, 0.0);
    pos[1] = Vec_CreatVec3d(-_w / 2.0, 0.0, -_h / 2.0);
    pos[2] = Vec_CreatVec3d(-_w / 2.0, 0.0, 0.0);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[2]);
    //CVector_Pushback(_vertexs, &pos[1]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[2]);
    geometry->_vertexs2.push_back(pos[1]);


    _coord[0] = Vec_CreatVec2f(0.5f, 0.5f);
    _coord[1] = Vec_CreatVec2f(0.0, 0.0f);
    _coord[2] = Vec_CreatVec2f(0.0, 0.5);
    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[2]);
    //CVector_Pushback(_coords, &_coord[1]);
    geometry->_texcoords2.push_back(_coord[0]);
    geometry->_texcoords2.push_back(_coord[2]);
    geometry->_texcoords2.push_back(_coord[1]);

    normal = Vec_CreatVec3f(0.0f, 0.0f, 1.0f);
    //CVector_Pushback(_normals, &normal);
    geometry->_normals2.push_back(normal);

    tmpColor = Vec_CreatVec4f(0.0, 1.0, 0.0, 0.0);
    //CVector_Pushback(_colors, &tmpColor);
    geometry->_colors2.push_back(tmpColor);

    primitiveSet.beg = 0;
    primitiveSet.count = 24;
    primitiveSet.type = GL_TRIANGLES;
    //CVector_Pushback(primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);

    geometry->_normalBind = BIND_OVERALL;
    geometry->_colorBind = BIND_OVERALL;
    /*设置文理贴图*/
    if (1)
    {
        pCImage tmp = 0;
        if (ruw->lightingType[0] != 'N' && ruw->lightingType[0] != 'Q')
        {
            tmp = TextureMgr_GetImage(TextureType_Obstacle_with_Light);
        }
        else
        {
            tmp = TextureMgr_GetImage(TextureType_Obstacle);
        }
        if (tmp)
        {
            geometry->texture_image->m_ID = tmp->m_ID;
            {
                geometry->texture_image->min_filter = GL_LINEAR;
                geometry->texture_image->mag_filter = GL_LINEAR;
            }
        }
    }
    return _geode;
}

pCNode C3DBarrierBlockCreatBarrierDrawable3D(pCBarrierParameters bp)
{
    pCNode _geode = CNode_CreateNode(NT_Drawable);
    if (0 == bp || 0 == _geode){
        return 0;
    }

    pCDrawable geometry = (pCDrawable)_geode->pNodeData;
    if (0 == geometry){
        return 0;
    }
    //pCVector _vertexs = geometry->_vertexs;
    //pCVector _coords = geometry->_texcoords;
    //pCVector _colors = geometry->_colors;
    //pCVector _normals = geometry->_normals;
    //pCVector primitiveSets = geometry->_primitiveSets;
    pCImage image = geometry->texture_image;
    CPrimitiveSet primitiveSet;

    float _x = 0, _y = 0, _z = 0, w = bp->height, _h = bp->height, _s = 1, _t = 1;
    CVec3d pos[5];
    CVec3f normal;
    CVec4f tmpColor;
    if (_h <= 0.0)
    {
        _h = w;
    }

    pos[0] = Vec_CreatVec3d(0, 0, _h / 2);
    pos[1] = Vec_CreatVec3d(-w / 2, -w / 2, -_h / 2);
    pos[2] = Vec_CreatVec3d(w / 2, -w / 2, -_h / 2);
    pos[3] = Vec_CreatVec3d(-w / 2, w / 2, -_h / 2);
    pos[4] = Vec_CreatVec3d(w / 2, w / 2, -_h / 2);

    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[1]);
    //CVector_Pushback(_vertexs, &pos[2]);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[2]);
    //CVector_Pushback(_vertexs, &pos[4]);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[4]);
    //CVector_Pushback(_vertexs, &pos[3]);
    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[3]);
    //CVector_Pushback(_vertexs, &pos[1]);

    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[1]);
    geometry->_vertexs2.push_back(pos[2]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[2]);
    geometry->_vertexs2.push_back(pos[4]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[4]);
    geometry->_vertexs2.push_back(pos[3]);
    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[3]);
    geometry->_vertexs2.push_back(pos[1]);


    normal = Vec_CreatVec3f(0.0f, 0.0f, 1.0f);
    //CVector_Pushback(_normals, &normal);
    geometry->_normals2.push_back(normal);

    tmpColor = Vec_CreatVec4f(1.0, 0.0, 0.0, 1.0);
    //CVector_Pushback(_colors, &tmpColor);
    geometry->_colors2.push_back(tmpColor);


    primitiveSet.beg = 0;
    primitiveSet.count = 12;
    primitiveSet.type = GL_TRIANGLES;
    //CVector_Pushback(primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);

    geometry->_normalBind = BIND_OVERALL;
    geometry->_colorBind = BIND_OVERALL;

    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[1]);
    //CVector_Pushback(_vertexs, &pos[2]);

    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[1]);
    geometry->_vertexs2.push_back(pos[2]);

    return _geode;
}
#endif

#if 0
pCNode  C3DBarrierBlockCreatBarrierDrawable(pCBarrierParameters ruw)
{
    pCNode _geode = CNode_CreateNode(NT_Drawable);
    if (0 == ruw || 0 == _geode){
        return 0;
    }

    pCDrawable geometry = (pCDrawable)_geode->pNodeData;
    if (0 == geometry){
        return 0;
    }
    //pCVector _vertexs = geometry->_vertexs;
    //pCVector _coords = geometry->_texcoords;
    //pCVector _colors = geometry->_colors;
    //pCVector _normals = geometry->_normals;
    //pCVector primitiveSets = geometry->_primitiveSets;
    pCImage image = geometry->texture_image;
    CPrimitiveSet primitiveSet;

    float _x = 0, _y = 0, _z = 0, _w = ruw->height, _h = ruw->height, _s = 1, _t = 1;

    CVec3d pos[4];
    CVec2f _coord[4];

    CVec3f normal;
    CVec4f tmpColor;
    if (_h <= 0.0)
    {
        _h = _w;
    }

    pos[0] = Vec_CreatVec3d(-_w / 2.0, 0.0, -_h / 2.0);
    pos[1] = Vec_CreatVec3d(_w / 2.0, 0.0, -_h / 2.0);
    pos[2] = Vec_CreatVec3d(_w / 2.0, 0.0, _h / 2.0);
    pos[3] = Vec_CreatVec3d(-_w / 2.0, 0.0, _h / 2.0);

    //CVector_Pushback(_vertexs, &pos[0]);
    //CVector_Pushback(_vertexs, &pos[1]);
    //CVector_Pushback(_vertexs, &pos[2]);
    //CVector_Pushback(_vertexs, &pos[3]);

    geometry->_vertexs2.push_back(pos[0]);
    geometry->_vertexs2.push_back(pos[1]);
    geometry->_vertexs2.push_back(pos[2]);
    geometry->_vertexs2.push_back(pos[3]);

    /*png倒置*/
    _coord[0] = Vec_CreatVec2f(0.0f, 0.0f);
    _coord[1] = Vec_CreatVec2f(_s, 0.0f);
    _coord[2] = Vec_CreatVec2f(_s, _t);
    _coord[3] = Vec_CreatVec2f(0.0f, _t);


    //CVector_Pushback(_coords, &_coord[0]);
    //CVector_Pushback(_coords, &_coord[1]);
    //CVector_Pushback(_coords, &_coord[2]);
    //CVector_Pushback(_coords, &_coord[3]);
    geometry->_texcoords2.push_back(_coord[0]);
    geometry->_texcoords2.push_back(_coord[1]);
    geometry->_texcoords2.push_back(_coord[2]);
    geometry->_texcoords2.push_back(_coord[3]);

    normal = Vec_CreatVec3f(0.0f, 0.0f, 1.0f);
    //CVector_Pushback(_normals, &normal);
    geometry->_normals2.push_back(normal);

    tmpColor = Vec_CreatVec4f(1.0, 0.0, 0.0, 1.0);
    //CVector_Pushback(_colors, &tmpColor);
    geometry->_colors2.push_back(tmpColor);

    primitiveSet.beg = 0;
    primitiveSet.count = 4;
    //CVector_Pushback(primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);

    geometry->_normalBind = BIND_OVERALL;
    geometry->_colorBind = BIND_OVERALL;
    /*设置文理贴图*/
    if (1)
    {
        pCImage tmp = 0;
        if (ruw->lightingType[0] != 'N' && ruw->lightingType[0] != 'Q')
        {
            tmp = TextureMgr_GetImage(TextureType_Obstacle_with_Light);
        }
        else
        {
            tmp = TextureMgr_GetImage(TextureType_Obstacle);
        }
        if (tmp)
        {
            geometry->texture_image->m_ID = tmp->m_ID;
            {
#if 0//jiangyan: opengl es2 不支持
                geometry->texture_image->TextureEnvMode = GL_TEXTURE_ENV_MODE;
                geometry->texture_image->TextureEnvParameter = GL_MODULATE;
#endif
                geometry->texture_image->min_filter = GL_LINEAR;
            }
        }
    }
    return _geode;
}
#endif

#if 0
void DrawLine(pCDrawable pdraw, int* index, pCVec3d pos)
{
    if (0 == pdraw || 0 == index){ return; }
    CVec3f vertexTempf;
    CVec4f tmpColor = Vec_CreatVec4f(0.3, 0.3, 0.3, 0.6);
    CVec3f tmpNormal = Vec_CreatVec3f(0.0f, 0.0f, 1.0f);
    GLfloat tmpAltitude = 0.0f;

    CVec2f tmpTexcoord[4];
    tmpTexcoord[0] = Vec_CreatVec2f(0.f, 0.f);
    tmpTexcoord[1] = Vec_CreatVec2f(1.0f, 1.0f);

    //CVector_Pushback(pdraw->_vertexs, &pos[index[0]]); 
    pdraw->_vertexs2.push_back(pos[index[0]]);
    vertexTempf = Vec_pVec3dToVec3f(&pos[index[0]]);
    //CVector_Pushback(pdraw->_vertexsF, &vertexTempf);
    pdraw->_vertexsF2.push_back(vertexTempf);

    //CVector_Pushback(pdraw->_vertexs, &pos[index[1]]); 
    pdraw->_vertexs2.push_back(pos[index[1]]);
    vertexTempf = Vec_pVec3dToVec3f(&pos[index[1]]);
    //CVector_Pushback(pdraw->_vertexsF, &vertexTempf);
    pdraw->_vertexsF2.push_back(vertexTempf);

    //CVector_Pushback(pdraw->_colors, &tmpColor);
    //CVector_Pushback(pdraw->_colors, &tmpColor);
    pdraw->_colors2.push_back(tmpColor);
    pdraw->_colors2.push_back(tmpColor);

    //CVector_Pushback(pdraw->_texcoords, &tmpTexcoord[0]);
    //CVector_Pushback(pdraw->_texcoords, &tmpTexcoord[1]);
    pdraw->_texcoords2.push_back(tmpTexcoord[0]);
    pdraw->_texcoords2.push_back(tmpTexcoord[1]);

    //CVector_Pushback(pdraw->_normals, &tmpNormal);
    //CVector_Pushback(pdraw->_normals, &tmpNormal);
    pdraw->_normals2.push_back(tmpNormal);
    pdraw->_normals2.push_back(tmpNormal);

    //CVector_Pushback(pdraw->_altitudes, &tmpAltitude);
    //CVector_Pushback(pdraw->_altitudes, &tmpAltitude);
    pdraw->_altitudes2.push_back(tmpAltitude);
    pdraw->_altitudes2.push_back(tmpAltitude);
}
#endif


//************************************
// Method:    C3DBarrierBlockCreatBarrierDrawableBillboard
// FullName:  C3DBarrierBlockCreatBarrierDrawableBillboard
// Access:    public 
// Desc:      obstacle in the form of Billboard
// Returns:   pCNode
// Qualifier:
// Parameter: pCBarrierParameters bp
// Author:    yan.jiang
//************************************
pCNode C3DBarrierBlockCreatBarrierDrawableBillboard(pCBarrierParameters bp)
{
    if (0 == bp)
    {
        return 0;
    }
    pCImage image = NULL;
    CPrimitiveSet primitiveSet;
    float _w = bp->height, _h = bp->height;

    CVec3d pos[4];
    CVec2f texcoord[4];
    CVec4f colors[4];
    float alts[4];

    CVec4f defaultColor = Vec_CreatVec4f(1.0, 1.0, 0.0, 1.0);

    GLfloat obstaclePeakAltitude = bp->altitude + bp->height;

    if (_h <= 0.0)
    {
        _h = _w;
    }

    pCNode _pNodeDrawable = CNode_CreateNode(NT_Drawable);
    if (0 == _pNodeDrawable){ return 0; }

    CNode_SetName(_pNodeDrawable, "__BarrDrawBillboard");
    pCDrawable geometry = (pCDrawable)_pNodeDrawable->pNodeData;

    pos[0] = Vec_CreatVec3d(-_w / 2.0, 0.0, -_h / 2.0);
    pos[1] = Vec_CreatVec3d(_w / 2.0, 0.0, -_h / 2.0);
    pos[2] = Vec_CreatVec3d(_w / 2.0, 0.0, _h / 2.0);
    pos[3] = Vec_CreatVec3d(-_w / 2.0, 0.0, _h / 2.0);

    texcoord[0] = Vec_CreatVec2f(0.0f, 0.0f);
    texcoord[1] = Vec_CreatVec2f(1.0f, 0.0f);
    texcoord[2] = Vec_CreatVec2f(1.0f, 1.0f);
    texcoord[3] = Vec_CreatVec2f(0.0f, 1.0f);

    colors[0] = defaultColor;
    colors[1] = defaultColor;
    colors[2] = defaultColor;
    colors[3] = defaultColor;

    alts[0] = obstaclePeakAltitude;
    alts[1] = obstaclePeakAltitude;
    alts[2] = obstaclePeakAltitude;
    alts[3] = obstaclePeakAltitude;

    //vertex texcoord color normal altitude
    DrawRectanglePlane(geometry, pos, texcoord, colors, alts);

    //create vbo
    DrawableNodeCreateVBO(geometry);

    //texture
    if (bp->lightingType[0] != 'N' && bp->lightingType[0] != 'Q')
    {
        image = TextureMgr_GetImage(TextureType_Obstacle_with_Light);
    }
    else
    {
        image = TextureMgr_GetImage(TextureType_Obstacle);
    }
    geometry->texture_image = image;
    geometry->drawType = CDrawableType_3DBarrier;

    //primitiveSet
    primitiveSet.beg = 0;
    primitiveSet.count = geometry->_indices2.size();
    primitiveSet.type = GL_TRIANGLES;
    //CVector_Pushback(geometry->_primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);

    return _pNodeDrawable;
}

void  C3DBarrierBlockUpdate(pC3DBarrierBlock blk)
{
    int display;
    double distance;
    CVec3d eyes;
    CVec3d locate;
    //c_pvector barrierVector = blk->m_pBarrierRecord;
    //unsigned int index = 0, num = c_vector_size(barrierVector);
    //c_iterator iter, beg = c_vector_begin(barrierVector), end = c_vector_end(barrierVector);
    eyes = Coordinate_DegreeLonLatAltToVec3d(GetGlobal()->vLonLatAlt._v[0], GetGlobal()->vLonLatAlt._v[1], GetGlobal()->vLonLatAlt._v[2]);
    double planeLon = GetGlobal()->vLonLatAlt._v[0];
    double planeLat = GetGlobal()->vLonLatAlt._v[1];
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < blk->m_pBarrierRecord2.size(); ++i)
    {
        //pC3DBarrierRecord data = (pC3DBarrierRecord)(ITER_REF(iter));
        auto data = blk->m_pBarrierRecord2[i];
        pCNode group = data->m_drawable;
        if (group == nullptr)
        {
            continue;
        }

        pCDrawable geometry = (pCDrawable)(group->pNodeData);
        //pCDrawable geometry1 = (pCDrawable)((CNode_GetChildByIndex(group,1))->pNodeData);//jy:
        pCMatrixTransform mattransform = (pCMatrixTransform)(data->m_mat->pNodeData);
        CVec3d lonlatalt = Vec_CreatVec3d(data->barrier->longitude, data->barrier->latitude, data->barrier->altitude);
        double angleOfB2E = CalculateAngle(lonlatalt._v[0], lonlatalt._v[1], planeLon, planeLat);
        angleOfB2E += PI;
        CMatrix  transferM = CMatrix_RotateAVec3d(angleOfB2E, Vec_CreatVec3d(0, 0, -1));
        CMatrix  tmptransferM = Coordinate_Vec3dDegreeLonlatToMatrix(lonlatalt);
        transferM = CMatrix_MatrixMultMatrix(&transferM, &tmptransferM);
        locate = CMatrix_GetTrans(&transferM);
        locate = Vec_Vec3dSubVec3d(&eyes, &locate);
        distance = Vec_LengthVec3d(&locate);
        display = distance < VISIBLE_DS;
        geometry->is_display = display;
        //geometry1->is_display = display;//jy:
        mattransform->matrix = transferM;
    }
}
