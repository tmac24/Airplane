//#include "base\ccMacros.h"


#include "../Calculate/CalculateZs.h"
#include <vector>
#include "base/HmsFileUtils.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTexture2D.h"
#include "render/HmsTextureCache.h"
#include <iostream>
#include <mutex>
#include "HVTMgr.h"
#include "MatrixNode.h"


#define HVT_ALARM_DISTANCE 100.0

extern bool g_supportVAO;

static std::mutex res_mutex1;
HVTMgr* HVTMgr::_pInstance = nullptr;
std::string HVTMgr::_databasePath = "";

HVTMgr::HVTMgr()
{
    _rootWires = CNode_CreateNode(EN_NodeType::Node);
    _rootTowers = CNode_CreateNode(EN_NodeType::Node);
    CVec3d initlla = { 103.0, 30.0, 700.0 };
    _lastLonLatAlt = initlla;
    _alarmDistance = HVT_ALARM_DISTANCE;
}

HVTMgr* HVTMgr::getInstance()
{
    if (nullptr == _pInstance)
    {
        std::unique_lock<std::mutex> mymutex1(res_mutex1);
        if (nullptr == _pInstance)
        {
            _pInstance = new HVTMgr;
        }
    }
    return _pInstance;
}

HVTMgr::~HVTMgr()
{
}

#define AAA_THRESHOLD_DISTANCE 100//meter

void HVTMgr::Update(CVec3d LonLatAlt)
{
    double deltaDistance = CalculateDistanceM(LonLatAlt._v[0], LonLatAlt._v[1],
        _lastLonLatAlt._v[0], _lastLonLatAlt._v[1]);

    if (deltaDistance > 50000)//50km
    {//移动距离超过一定阈值后，更新电塔数据
        CVec3d initPos = { 116.306352, 40.249058, 400 };//for test
        //clear node

        //for test
        HVTDataMgr::getInstance()->UpdateData(LonLatAlt);
        auto wires = HVTDataMgr::getInstance()->Wires();
        for (auto it = wires.begin(); it != wires.end(); it++)
        {
            auto pnodeWires = CreateHVTWiresNode(*(it->second));
            CNode_AddChild(_rootWires, pnodeWires);

            auto pnodeTowers = CreateHVTTowersNode(*(it->second));
            CNode_AddChild(_rootTowers, pnodeTowers);
        }

        //for test
        //HmsAviPf::HVTDataMgr::getInstance()->UpdateData(LonLatAlt);
        //std::cout << "77" << std::endl;

        _lastLonLatAlt = LonLatAlt;
    }

    CVec3d aircraftPos = Coordinate_Vec3dDegreeLonlatAltToVec3d(LonLatAlt);
    //判断电线与飞机的距离，进行告警判定
    for (auto phwo : _vecNearbyHvWireObj)
    {
        phwo->updateAircraftPos(aircraftPos);
        phwo->pDwa->alarm_distance = _alarmDistance;
    }

    //判断电塔与飞机的距离，进行告警判定   
    int alarm = 0;
    for (auto pto : _vecNearbyTowerObj)
    {
        auto distance = Vec_Vec3dDistance(&pto->pos, &aircraftPos);
        distance < _alarmDistance ? alarm = 1 : alarm = 0;

        for (auto dwa : pto->vecDwa)
        {
            dwa->alarm_state = alarm;
        }
    }

}

void HVTMgr::SetHvtDataPath(const std::string & strPath)
{
    _databasePath = strPath;
}

std::string HVTMgr::getHvtDataPath() const
{
    return _databasePath;
}

//************************************
// Method:    CreateHVTNode
// FullName:  HVTMgr::CreateHVTWiresNode
// Access:    private 
// Desc:      节点包含高压线
// Returns:   pCNode
// Qualifier: //pCNode CreateHVTNode(const CVec3d& wd)
// Parameter: const HmsAviPf::WireData & wd_in
// Author:    yan.jiang
//************************************
pCNode HVTMgr::CreateHVTWiresNode(const WireData& wd)
{
    auto pnodegroup = CNode_CreateNode(EN_NodeType::Group);//wires
#if 0//data for test
    int twrCnt = wd.TowerCnt();//tower count of the line
    int endpCnt = wd.ThreadCnt();//endpoint count per tower
    TowerLLAH* pLonlatalthgtTowers = wd.PlonlataltTowers();
    pCVec3d pLonlataltEnds = wd.PlonlataltEnds();

    HmsAviPf::TowerLLAH lonlatalthgtTowers_test[] = {
        116.306352, 40.24905793, 359.236, 53.1405,
        116.3097487, 40.24515794, 311.866 , 30.6803,
        116.312017, 40.24255535,  240.3251, 48.4431,
        116.3144166, 40.23980192, 159.9263, 50.1491,
        116.3169077, 40.23694008, 137.3325, 39.5487,
    };

    CVec3d lonlataltEnds_test[] = {
        116.3062293, 40.24899242, 410.8773,
        116.3064744, 40.24912064, 411.0912,
        116.3062121, 40.24898346, 402.7489,
        116.3063523, 40.24905681, 402.428,
        116.3064905, 40.24912906, 402.7489,
        116.3096439, 40.24510351, 341.3796,
        116.3098512, 40.24521181, 341.5307,
        116.3096274, 40.2450949, 334.6144,
        116.3097481, 40.24515796, 334.5472,
        116.3098688, 40.24522101, 334.6144,
        116.311912, 40.24250125, 288.3046,
        116.3121186, 40.24260844, 288.2305,
        116.3118938, 40.24249183, 281.2788,
        116.3120155, 40.24255494, 281.106,
        116.3121364, 40.24261766, 281.3035,
        116.3142904, 40.23973921, 209.3542,
        116.3145388, 40.23986444, 209.3674,
        116.3142745, 40.23973118, 201.1393,
        116.3144147, 40.23980188, 201.0331,
        116.3145542, 40.23987219, 201.1659,
        116.3168023, 40.23688715, 175.7987,
        116.3170125, 40.23699395, 175.8197,
        116.3167844, 40.23687805, 169.3751,
        116.3169074, 40.23694055, 169.543,
        116.3170292, 40.2370024, 169.3961,
    };

    pLonlatalthgtTowers = lonlatalthgtTowers_test;
    pLonlataltEnds = lonlataltEnds_test;
    twrCnt = 5;
    endpCnt = 5;

    HmsAviPf::WireData wd(1, twrCnt, endpCnt, pLonlatalthgtTowers, pLonlataltEnds);
#endif	

    //pCNode pnodeDraw = CreateHVWireDrawableNode2(wd);
    pCNode pnodeDraw = CreateHVWireDrawableNode3(wd);
    pCDrawable pd = (pCDrawable)pnodeDraw->pNodeData;
    auto pnodeMat = CreateHVTMatrixNode(pd->centerPos);
    CNode_AddChild(pnodeMat, pnodeDraw);
    CNode_AddChild(pnodegroup, pnodeMat);

    //记录附近的高压线相对飞机实时位置
    pd->alarm_distance = _alarmDistance;
    HvWireObj* phwo = new HvWireObj(pd);
    CVec3d tmp = Vec_CreatVec3d(0, 0, 0);//初始值任意给，地心
    phwo->updateAircraftPos(tmp);
    _vecNearbyHvWireObj.push_back(phwo);

    return pnodegroup;
}

//************************************
// Method:    CreateHVTTowersNode
// FullName:  HVTMgr::CreateHVTTowersNode
// Access:    private 
// Desc:      节点包含高压电塔
// Returns:   pCNode
// Qualifier:
// Parameter: const WireData & wd_in
// Author:    yan.jiang
//************************************
pCNode HVTMgr::CreateHVTTowersNode(const WireData& wd)
{
    auto pnodegroup = CNode_CreateNode(EN_NodeType::Group);//towers

    TowerLLAH* pLonlatalthgtTowers = wd.PlonlataltTowers();
    pCVec3d pLonlataltEnds = wd.PlonlataltEnds();
    int twrCnt = wd.TowerCnt();//tower count on the line

    double towerDirection = 0.0;
    TowerLLAH* p0 = pLonlatalthgtTowers;

    for (int i = 0; i < twrCnt; i++, p0++)
    {
        if (i == 0)
        {//第一个塔
            towerDirection = CalculateAngle(p0->lon, p0->lat, pLonlatalthgtTowers[1].lon, pLonlatalthgtTowers[1].lat);
        }
        else
        {
            towerDirection = CalculateAngle(pLonlatalthgtTowers[i - 1].lon, pLonlatalthgtTowers[i - 1].lat, p0->lon, p0->lat);
        }

        auto pnodeMatTower = CreateTowerMatrixNode(*p0, towerDirection);

        TowerObj* pto = new TowerObj(*p0);

        CreateTowerDrawableNode(pnodeMatTower, *p0, pto);

        _vecNearbyTowerObj.push_back(pto);

        CNode_AddChild(pnodegroup, pnodeMatTower);
    }

    return pnodegroup;
}


pCNode HVTMgr::CreateHVTMatrixNode(CVec3d& lonlatalt)
{
    pCNode pnodemat = CNode_CreateNode(EN_NodeType::MatrixTransform);
    pCMatrixTransform pnodeMatTrans = (pCMatrixTransform)(pnodemat->pNodeData);
    CMatrix m1 = CMatrix_TranslateXYZ(lonlatalt._v[0], lonlatalt._v[1], lonlatalt._v[2]);
    auto matscale = CMatrix_ScaleXYZ(1, 1, 1);
    CMatrix mOut = CMatrix_MatrixMultMatrix(&m1, &matscale);

    pnodeMatTrans->matrix = mOut;
    return pnodemat;
}

//************************************
// Method:    CreateTowerMatrixNode
// FullName:  HVTMgr::CreateTowerMatrixNode
// Access:    private 
// Desc:      
// Returns:   pCNode
// Qualifier:
// Parameter: const TowerLLAH & llah
// Parameter: const double & towerDir 塔的朝向(方位角），弧度
// Author:    yan.jiang
//************************************
pCNode HVTMgr::CreateTowerMatrixNode(const TowerLLAH& llah, const double& towerDir)
{
    pCNode pnodemat = CNode_CreateNode(EN_NodeType::MatrixTransform);
    pCMatrixTransform pnodeMatTrans = (pCMatrixTransform)(pnodemat->pNodeData);

    double deepReserved = 5.0;//地基高度，位置往下深5米. 避免塔模型的地基露出来
    CVec3d LLA2 = { llah.lon, llah.lat, llah.alt - deepReserved };

    CVec3d xyzpos = Coordinate_Vec3dDegreeLonlatAltToVec3d(LLA2);

    CMatrix trans = CMatrix_TranslateXYZ(xyzpos._v[0], xyzpos._v[1], xyzpos._v[2]);

    //塔的模型坐标为：塔底平面位于xz平面，从塔底到塔顶的方向指向y轴正方向。
    double modelY = 3900;//3d模型的最大高度值（厘米）
    double realXZ = 60;//假定的实际底边长度（米）
    double yScale = llah.height / modelY;
    double xzScale = realXZ / modelY;//模型坐标的zx

    auto matscale = CMatrix_ScaleXYZ(xzScale, yScale, xzScale);

    const double pi = 3.1415926;
    CVec3d AxisZ = { 0.0, 0.0, 1.0 };
    CVec3d AxisY = { 0.0, 1.0, 0.0 };
    CVec3d AxisX = { 1.0, 0.0, 0.0 };
    double tmpAngle = llah.lat * pi / 180.0;
    CMatrix rot_1 = CMatrix_RotateAXYZ(tmpAngle, 1, 0, 0);

    tmpAngle = (llah.lon - 90.0) * pi / 180.0;
    CMatrix rot_2 = CMatrix_RotateAXYZ(tmpAngle, 0, 0, 1);

    CMatrix rot_3 = CMatrix_RotateAXYZ(-towerDir, 0, 1, 0);

    CMatrix tmpMatrix;

    tmpMatrix = CMatrix_MatrixMultMatrix(&matscale, &rot_3);//缩放与塔的朝向旋转
    tmpMatrix = CMatrix_MatrixMultMatrix(&tmpMatrix, &rot_1);//按纬度旋转
    tmpMatrix = CMatrix_MatrixMultMatrix(&tmpMatrix, &rot_2);//按经度旋转
    tmpMatrix = CMatrix_MatrixMultMatrix(&tmpMatrix, &trans);//平移

    pnodeMatTrans->matrix = tmpMatrix;
    return pnodemat;
}

//************************************
// Method:    CreateTowerDrawableNode
// FullName:  HVTMgr::CreateTowerDrawableNode
// Access:    private 
// Desc:      
// Returns:   void
// Qualifier:
// Parameter: pCNode fatherNode
// Parameter: const CVec3d & pos0
// Author:    yan.jiang
//************************************
void HVTMgr::CreateTowerDrawableNode(pCNode fatherNode, const TowerLLAH& T_LLAH, TowerObj* pto)
{
    if (fatherNode == nullptr)
    {
        return;
    }

    SpriteSvs3DData* psp3dd = LoadTowerModel();

    if (psp3dd->shapes.size() <= 0)
    {
        return;
    }
    auto sss = psp3dd->shapes.size();
    for (int idx = 0; idx < (int)psp3dd->shapes.size(); ++idx)
    {
        auto pnodeDraw = CNode_CreateNode(EN_NodeType::NT_Drawable);
        pCDrawable pdraw = (pCDrawable)pnodeDraw->pNodeData;
        pto->vecDwa.push_back(pdraw);

        float* ppos = psp3dd->shapes[idx].mesh.positions.data();
        auto szpos = psp3dd->shapes[idx].mesh.positions.size();
        float* ptx = psp3dd->shapes[idx].mesh.texcoords.data();
        auto sztx = psp3dd->shapes[idx].mesh.texcoords.size();

        int vertixCnt = szpos / 3;
        float* pclr = new float[vertixCnt * 4];
        float* palt = new float[vertixCnt];

        if (g_supportVAO)
        {
            glGenVertexArrays(1, &pdraw->vao);
            glBindVertexArray(pdraw->vao);
        }

        glGenBuffers(1, &pdraw->vtx_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, pdraw->vtx_buffer);
        glBufferData(GL_ARRAY_BUFFER, (szpos)*sizeof(float), ppos, GL_STATIC_DRAW);

        glGenBuffers(1, &pdraw->clr_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, pdraw->clr_buffer);
        glBufferData(GL_ARRAY_BUFFER, (vertixCnt * 4)*sizeof(float), pclr, GL_STATIC_DRAW);
        delete[] pclr;

        glGenBuffers(1, &pdraw->alt_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, pdraw->alt_buffer);
        glBufferData(GL_ARRAY_BUFFER, (vertixCnt)*sizeof(float), palt, GL_STATIC_DRAW);
        delete[] palt;

        glGenBuffers(1, &pdraw->tcd_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, pdraw->tcd_buffer);
        glBufferData(GL_ARRAY_BUFFER, (sztx)*sizeof(float), ptx, GL_STATIC_DRAW);

        glGenBuffers(1, &pdraw->idx_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pdraw->idx_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, psp3dd->shapes[idx].mesh.indices.size() * sizeof(short),
            psp3dd->shapes[idx].mesh.indices.data(), GL_STATIC_DRAW);

        if (g_supportVAO)
        {
            setGeometyVbo(pdraw);
            glBindVertexArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        std::string fullPathTx = SpriteSvs3DCache::getInstance()->getModelFullPath("res/3dModel/ElectroTower/boss.png");
        HmsAviPf::Texture2D *texture = HmsAviPf::CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(fullPathTx);
        if (texture)
        {
            pdraw->texture_image = new CImage;
            pdraw->texture_image->m_ID = texture->getName();
        }
        else
        {
            pdraw->texture_image = nullptr;
        }

        pdraw->centerPos = Coordinate_Vec3dDegreeLonlatAltToVec3d(T_LLAH.LLA);
        pdraw->drawType = CDrawableType_HvTower;
        CPrimitiveSet ps;
        ps.beg = 0;
        ps.count = psp3dd->shapes[idx].mesh.indices.size();
        ps.type = GL_TRIANGLES;
        //CVector_Pushback(pdraw->_primitiveSets, &ps);
        pdraw->_primitiveSets2.push_back(ps);

        CNode_AddChild(fatherNode, pnodeDraw);
    }
}


SpriteSvs3DData* HVTMgr::LoadTowerModel() const
{
    std::string fileName("res/3dModel/ElectroTower/dianta01.obj");

    SpriteSvs3DData* psp3dd = SpriteSvs3DCache::getInstance()->getSprite3DData(fileName);
    if (psp3dd == nullptr)
    {
        psp3dd = new SpriteSvs3DData;
        if (SpriteSvs3DCache::getInstance()->Load3DObj(fileName, nullptr, psp3dd))
        {
            SpriteSvs3DCache::getInstance()->AddSprite3DData(fileName, psp3dd);
        }
        else
        {
            delete psp3dd;
            return nullptr;
        }
    }
    return psp3dd;
}

//************************************
// Method:    CreateHVWireDrawableNode2
// FullName:  HVTMgr::CreateHVWireDrawableNode2
// Access:    private 
// Desc:      第二种方式创建线vbo，兼容Drawable中的默认vbo渲染
// Returns:   pCNode
// Qualifier:
// Parameter: const WireData & wd
// Author:    yan.jiang
//************************************
pCNode HVTMgr::CreateHVWireDrawableNode2(const WireData& wd)
{
    auto pnodeDraw = CNode_CreateNode(EN_NodeType::NT_Drawable);
    pCDrawable geometry = (pCDrawable)pnodeDraw->pNodeData;

    int towerCnt = wd.TowerCnt();
    int endpCntPerTower = wd.ThreadCnt();
    int endpCnt = towerCnt * endpCntPerTower;

    CVec3d centerxyz = { 0, 0, 0 };

    CVec3d *posxyz = new CVec3d[endpCnt];
    for (int i = 0; i < endpCnt; i++)
    {
        posxyz[i] = Coordinate_pVec3dDegreeLonlatAltToVec3d(&(wd.PlonlataltEnds()[i]));
        centerxyz = Vec_Vec3dAddVec3d(&centerxyz, &posxyz[i]);
    }
    centerxyz = Vec_Vec3dDivDouble(&centerxyz, endpCnt);

    int j = 0;
    int endNameId = 0;
    bool directionOfTraversal = true;//true:forward, false:backward
    CVec3d tmp;
    CVec3f vertexTempf;
    CVec2f texcoord = Vec_CreatVec2f(0.0f, 0.0f);
    CVec4f color = Vec_CreatVec4f(1, 1, 1, 1);
    float alt = 0.0;
    unsigned short pos2idx = 0;

    for (int endNameId = 0; endNameId < endpCntPerTower; endNameId++)
    {
        if (directionOfTraversal)
        {
            for (int towerId = 0; towerId < towerCnt; towerId++)
            {
                tmp = Vec_Vec3dSubVec3d(&posxyz[endNameId + towerId * endpCntPerTower], &centerxyz);

                //CVector_Pushback(geometry->_vertexs, &tmp);
                geometry->_vertexs2.push_back(tmp);
                vertexTempf = Vec_pVec3dToVec3f(&tmp);
                //CVector_Pushback(geometry->_vertexsF, &vertexTempf);
                geometry->_vertexsF2.push_back(vertexTempf);
                //CVector_Pushback(geometry->_texcoords, &texcoord);
                //CVector_Pushback(geometry->_colors, &color);
                //CVector_Pushback(geometry->_altitudes, &alt);
                //CVector_Pushback(geometry->_indices, &pos2idx);
                geometry->_texcoords2.push_back(texcoord);
                geometry->_colors2.push_back(color);
                geometry->_altitudes2.push_back(alt);
                geometry->_indices2.push_back(pos2idx);
                pos2idx++;
            }
        }
        else
        {
            for (int towerId = towerCnt - 1; towerId >= 0; towerId--)
            {
                tmp = Vec_Vec3dSubVec3d(&posxyz[endNameId + towerId * endpCntPerTower], &centerxyz);
                //CVector_Pushback(geometry->_vertexs, &tmp);
                geometry->_vertexs2.push_back(tmp);
                vertexTempf = Vec_pVec3dToVec3f(&tmp);
                //CVector_Pushback(geometry->_vertexsF, &vertexTempf);
                geometry->_vertexsF2.push_back(vertexTempf);
                //CVector_Pushback(geometry->_texcoords, &texcoord);
                //CVector_Pushback(geometry->_colors, &color);
                //CVector_Pushback(geometry->_altitudes, &alt);
                //CVector_Pushback(geometry->_indices, &pos2idx);
                geometry->_texcoords2.push_back(texcoord);
                geometry->_colors2.push_back(color);
                geometry->_altitudes2.push_back(alt);
                geometry->_indices2.push_back(pos2idx);
                pos2idx++;
            }
        }

        directionOfTraversal = !directionOfTraversal;
    }
    delete[] posxyz;

    DrawableNodeCreateVBO(geometry);

    geometry->centerPos = centerxyz;
    geometry->texture_image = nullptr;
    geometry->drawType = CDrawableType_HvWire;

    CPrimitiveSet ps;
    ps.beg = 0;
    ps.count = endpCnt;
    ps.type = GL_LINE_STRIP;

    //CVector_Pushback(geometry->_primitiveSets, &ps);
    geometry->_primitiveSets2.push_back(ps);

    return pnodeDraw;
}



//************************************
// Method:    CreateHVWireDrawableNode3
// FullName:  HVTMgr::CreateHVWireDrawableNode3
// Access:    private 
// Desc:      第三种方式，兼容默认vbo渲染方法
// Returns:   pCNode
// Qualifier:
// Parameter: const WireData & wd
// Author:    yan.jiang
//************************************
pCNode HVTMgr::CreateHVWireDrawableNode3(const WireData& wd)
{
    auto pnodeDraw = CNode_CreateNode(EN_NodeType::NT_Drawable);
    pCDrawable pdraw = (pCDrawable)pnodeDraw->pNodeData;

    int towerCnt = wd.TowerCnt();
    int endpCntPerTower = wd.ThreadCnt();
    int endpCnt = towerCnt * endpCntPerTower;

    CVec3d centerxyz = { 0, 0, 0 };

    float* pPos = new float[3 * endpCnt]{0.0};
    float* pColor = new float[4 * endpCnt]{0.0};
    float* pTexcoord = new float[2 * endpCnt]{0.0};
    float* pAlt = new float[1 * endpCnt]{0.0};
    unsigned short *pIndices = new unsigned short[endpCnt];

    for (int i = 0; i < endpCnt; i++)
    {
        pIndices[i] = i;
    }

    CVec3d *posxyz = new CVec3d[endpCnt];
    pCVec3d pWireDataEndLLA = wd.PlonlataltEnds();
    for (int i = 0; i < endpCnt; i++)
    {
        posxyz[i] = Coordinate_pVec3dDegreeLonlatAltToVec3d(&(pWireDataEndLLA[i]));
        centerxyz = Vec_Vec3dAddVec3d(&centerxyz, &posxyz[i]);
    }
    centerxyz = Vec_Vec3dDivDouble(&centerxyz, endpCnt);
    pdraw->centerPos = centerxyz;

    int j = 0;
    int endNameId = 0;
    bool directionOfTraversal = true;//true:forward, false:backward
    CVec3d tmp;
    int idx = 0;//endidx
    int inum = 0;
    int itmp = 0;
    for (int endNameId = 0; endNameId < endpCntPerTower; endNameId++)
    {
        if (directionOfTraversal)
        {
            for (int towerId = 0; towerId < towerCnt; towerId++)
            {
                inum = endNameId + towerId * endpCntPerTower;
                tmp = Vec_Vec3dSubVec3d(&posxyz[inum], &pdraw->centerPos);
                itmp = idx * 3;
                pPos[itmp + 0] = tmp._v[0];
                pPos[itmp + 1] = tmp._v[1];
                pPos[itmp + 2] = tmp._v[2];
                pAlt[idx] = pWireDataEndLLA[inum]._v[2];//altitude

                idx++;
            }
        }
        else
        {
            for (int towerId = towerCnt - 1; towerId >= 0; towerId--)
            {
                inum = endNameId + towerId * endpCntPerTower;
                tmp = Vec_Vec3dSubVec3d(&posxyz[inum], &pdraw->centerPos);
                itmp = idx * 3;
                pPos[itmp + 0] = tmp._v[0];
                pPos[itmp + 1] = tmp._v[1];
                pPos[itmp + 2] = tmp._v[2];
                pAlt[idx] = pWireDataEndLLA[inum]._v[2];//altitude

                idx++;
            }
        }

        directionOfTraversal = !directionOfTraversal;
    }
    delete[] posxyz;

    if (g_supportVAO)
    {
        glGenVertexArrays(1, &pdraw->vao);
        glBindVertexArray(pdraw->vao);
    }

    glGenBuffers(1, &pdraw->vtx_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, pdraw->vtx_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 3 * endpCnt, pPos, GL_STATIC_DRAW);
    delete[] pPos;

    glGenBuffers(1, &pdraw->clr_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, pdraw->clr_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 4 * endpCnt, pColor, GL_STATIC_DRAW);
    delete[] pColor;

    glGenBuffers(1, &pdraw->tcd_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, pdraw->tcd_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 2 * endpCnt, nullptr, GL_STATIC_DRAW);
    delete[] pTexcoord;

    glGenBuffers(1, &pdraw->alt_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, pdraw->alt_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* endpCnt, pAlt, GL_STATIC_DRAW);
    delete[] pAlt;

    glGenBuffers(1, &pdraw->idx_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pdraw->idx_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*endpCnt, pIndices, GL_STATIC_DRAW);
    delete[] pIndices;

    if (g_supportVAO)
    {
        setGeometyVbo(pdraw);
        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::string fullPathTx = SpriteSvs3DCache::getInstance()->getModelFullPath("res/3dModel/ElectroTower/wire.png");
    HmsAviPf::Texture2D *texture = HmsAviPf::CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(fullPathTx);

    if (texture)
    {
        pdraw->texture_image = new CImage;
        pdraw->texture_image->m_ID = texture->getName();
    }
    else
    {
        pdraw->texture_image = nullptr;
    }

    pdraw->drawType = CDrawableType_HvWire;

    CPrimitiveSet ps;
    ps.beg = 0;
    ps.count = endpCnt;
    ps.type = GL_LINE_STRIP;

    //CVector_Pushback(pdraw->_primitiveSets, &ps);
    pdraw->_primitiveSets2.push_back(ps);

    return pnodeDraw;
}
