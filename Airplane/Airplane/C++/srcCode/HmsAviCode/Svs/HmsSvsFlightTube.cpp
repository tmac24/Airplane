#include "HmsSvsFlightTube.h"
#include <mutex>
#include "NavSvs/mathd/Vec3d.h"
#include "Calculate/CalculateZs.h"
#include "SpriteSvs3DCache.h"
#include "render/HmsTexture2D.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "Nav/HmsNavMathHelper.h"
#include "math/Quaternion.h"
#include "AltManager.h"
#include "Node.h"
#include "Global.h"
#include "Coordinate.h"


static std::mutex res_mutex2;

/*math helper 中的地球坐标系为：
x向东，从地心指向+90°经度与赤道相交处。
y向南，从地心指向南极点。
z从地心指向本初子午线 0°和赤道相交处。

g_transCoord 将此坐标系转换为如下坐标系来与原来的C代码匹配
x从地心指向本初子午线0°和赤道相交处。
y向东，从地心指向+90°经度与赤道相交处。
z向北，从地心指向北极点。
*/
HmsAviPf::Mat4d g_transCoord = HmsAviPf::Mat4d(
    0, 0, 1, 0,
    1, 0, 0, 0,
    0, -1, 0, 0,
    0, 0, 0, 1
    );

HmsSvsFlightTubeMgr* HmsSvsFlightTubeMgr::_pInstance = nullptr;

HmsSvsFlightTubeMgr::~HmsSvsFlightTubeMgr()
{
    _mtx_vecTubeSegRef.lock();
    for (auto pseg : _vecTubeSegRef)
    {
        delete pseg;
    }
    _vecTubeSegRef.clear();
    _mtx_vecTubeSegRef.unlock();
}

//************************************
// Method:    CreateWPMatrixNode
// FullName:  HmsSvsFlightTubeMgr::CreateWPMatrixNode
// Access:    private 
// Desc:      
// Returns:   pCNode
// Qualifier:
// Parameter: const HmsAviPf::Vec3d & lla
// Parameter: const float & dir , The direction of the wp node facing to (0~360 degree)
// Author:    yan.jiang
//************************************
pCNode HmsSvsFlightTubeMgr::CreateWPMatrixNode(const HmsAviPf::Vec3d& lla, const float& dir)
{
    pCNode pnodemat = CNode_CreateNode(EN_NodeType::MatrixTransform);
    pCMatrixTransform pnodeMatTrans = (pCMatrixTransform)(pnodemat->pNodeData);
    CMatrix tmpMatrix;
    CVec3d LLA2 = { lla.x, lla.y, lla.z };

    SetMatrixByLlaAndDir(tmpMatrix, LLA2, dir);

    pnodeMatTrans->matrix = tmpMatrix;
    return pnodemat;
}

void HmsSvsFlightTubeMgr::SetMatrixByLlaAndDir(CMatrix& outMatrix, const CVec3d& LngLatAlt, const float& dir)
{
    outMatrix = CMatrix_RotateAVec3d(dir / 180 * SIM_Pi, Vec_CreatVec3d(0, 0, -1));
    CMatrix tmptransferM = Coordinate_Vec3dDegreeLonlatToMatrix(LngLatAlt);
    outMatrix = CMatrix_MatrixMultMatrix(&outMatrix, &tmptransferM);
}

//************************************
// Method:    CreateWPDrawableNode
// FullName:  HmsSvsFlightTubeMgr::CreateWPDrawableNode
// Access:    private 
// Desc:      Way Point Sign 
// Returns:   pCNode
// Qualifier:
// Parameter: const SvsWayPointInfo & wpi
// Author:    yan.jiang
//************************************
pCNode HmsSvsFlightTubeMgr::CreateWPDrawableNode(const SvsWayPointInfo& wpi)
{
    pCImage image = NULL;
    float _w = 200, _h = 200;

    float heightFromGround = wpi._lngLatAlt.z - wpi._terrainAlt;

    int TextureMultiple = heightFromGround / _h;

    CVec3d pos[4];
    CVec2f texcoord[4];
    CVec4f colors[4];
    float alts[4];

    CVec4f defaultColor = Vec_CreatVec4f(0.1, 1.0, 0.2, 1.0);

    GLfloat altUseless = 0.0F;

    pCNode _pNodeDrawable = CNode_CreateNode(NT_Drawable);
    CNode_SetName(_pNodeDrawable, "__WayPointBillboard");
    pCDrawable geometry = (pCDrawable)_pNodeDrawable->pNodeData;

    pos[0] = Vec_CreatVec3d(-_w / 2.0, 0.0, 0);
    pos[1] = Vec_CreatVec3d(_w / 2.0, 0.0, 0);
    pos[2] = Vec_CreatVec3d(_w / 2.0, 0.0, _h);
    pos[3] = Vec_CreatVec3d(-_w / 2.0, 0.0, _h);

    texcoord[0] = Vec_CreatVec2f(0.0f, 1.0f);
    texcoord[1] = Vec_CreatVec2f(0.5f, 1.0f);
    texcoord[2] = Vec_CreatVec2f(0.5f, 0.0f);
    texcoord[3] = Vec_CreatVec2f(0.0f, 0.0f);

    colors[0] = defaultColor;
    colors[1] = defaultColor;
    colors[2] = defaultColor;
    colors[3] = defaultColor;

    alts[0] = altUseless;
    alts[1] = altUseless;
    alts[2] = altUseless;
    alts[3] = altUseless;

    //vertex texcoord color normal altitude
    DrawRectanglePlane(geometry, pos, texcoord, colors, alts);

    if (TextureMultiple > 0)//draw the arrows band below the waypoint
    {
        int coef = 2;
        auto _ww = _w / coef;

        pos[0] = Vec_CreatVec3d(-_ww / 2.0, 0.0, -heightFromGround);
        pos[1] = Vec_CreatVec3d(_ww / 2.0, 0.0, -heightFromGround);
        pos[2] = Vec_CreatVec3d(_ww / 2.0, 0.0, 0.0);
        pos[3] = Vec_CreatVec3d(-_ww / 2.0, 0.0, 0.0);

        texcoord[0] = Vec_CreatVec2f(0.5f, TextureMultiple * coef);
        texcoord[1] = Vec_CreatVec2f(1.0f, TextureMultiple * coef);
        texcoord[2] = Vec_CreatVec2f(1.0f, 0.0f);
        texcoord[3] = Vec_CreatVec2f(0.5f, 0.0f);

        colors[0] = defaultColor;
        colors[1] = defaultColor;
        colors[2] = defaultColor;
        colors[3] = defaultColor;

        alts[0] = altUseless;
        alts[1] = altUseless;
        alts[2] = altUseless;
        alts[3] = altUseless;

        //vertex texcoord color normal altitude
        DrawRectanglePlane(geometry, pos, texcoord, colors, alts);
    }

    //create vbo
    DrawableNodeCreateVBO(geometry);

    //texture
    //std::string fullPathTx = SpriteSvs3DCache::getInstance()->getModelFullPath("res/3dModel/FlightTube/waypoint2.png");
    std::string fullPathTx = SpriteSvs3DCache::getInstance()->getModelFullPath("res/3dModel/FlightTube/waypointband2.png");
    HmsAviPf::Texture2D *texture = HmsAviPf::CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(fullPathTx);

    geometry->color[0] = 255.0f / 255;
    geometry->color[1] = 146.0f / 255;
    geometry->color[2] = 18.0f / 255;
    geometry->color[3] = 1.0f;

    Texture2D::TexParams texP = {
        GL_LINEAR,
        GL_LINEAR,
        GL_REPEAT,
        GL_REPEAT
    };
    texture->setTexParameters(texP);

    if (texture)
    {
        geometry->texture_image = new CImage;
        geometry->texture_image->m_iType = GL_RGBA;//todo: texture_image 不完备，但有了m_iType和m_ID就可渲染，以后再废弃C版本的CImage
        geometry->texture_image->m_ID = texture->getName();
    }
    else
    {
        geometry->texture_image = nullptr;
    }

    geometry->drawType = CDrawableType_FlightTubeWaypoint;

    //primitiveSet
    CPrimitiveSet primitiveSet = CPrimitiveSet_CreatDefaultVisible(GL_TRIANGLES, 0, geometry->_indices2.size());
    //CVector_Pushback(geometry->_primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);

    return _pNodeDrawable;
}

//************************************
// Method:    GetCharTextureCoord
// FullName:  HmsSvsFlightTubeMgr::GetCharTextureCoord
// Access:    private 
// Desc:      获取字符在 ASCIIorg.png(896x552) 上的纹理坐标
// Returns:   HmsAviPf::Vec4
// Qualifier:
// Parameter: const HmsAviPf::Size & textureSize
// Parameter: const char * ch
// Author:    yan.jiang
//************************************
HmsAviPf::Rect HmsSvsFlightTubeMgr::GetCharTextureCoord(const char& ch, const HmsAviPf::Size& textureSize /*= HmsAviPf::Size(896, 552)*/)
{
    HmsAviPf::Rect tc;
    auto chWidth = textureSize.width / 16;
    auto chHeight = textureSize.height / 6;
    tc.origin.x = (ch % 16) * chWidth / textureSize.width;
    tc.origin.y = (ch - 0x20) / 16 * chHeight / textureSize.height;
    tc.size.width = chWidth / textureSize.width;
    tc.size.height = chHeight / textureSize.height;
    return tc;
}

//************************************
// Method:    CreateWPDrawableNodeIdx
// FullName:  HmsSvsFlightTubeMgr::CreateWPDrawableNodeIdx
// Access:    private 
// Desc:      航路点的序号显示节点（信息显示节点）
// Returns:   pCNode
// Qualifier:
// Parameter: const SvsWayPointInfo & wpi
// Author:    yan.jiang
//************************************
pCNode HmsSvsFlightTubeMgr::CreateWPDrawableNodeIdx(const SvsWayPointInfo& wpi)
{
    pCImage image = NULL;
    float _w = 400, _h = 600;
    float _hSpace = 250;//下方预留空间

    auto idxLen = HmsSvsFlightTubeMgr::getNumLength(wpi._index);

    float heightFromGround = wpi._lngLatAlt.z - wpi._terrainAlt;

    int TextureMultiple = heightFromGround / _h;

    CVec3d pos[4];
    CVec2f texcoord[4];
    CVec4f colors[4];
    float alts[4];

    CVec4f defaultColor = Vec_CreatVec4f(0.1, 1.0, 0.2, 0.7);

    GLfloat altUseless = 0.0F;

    pCNode _pNodeDrawable = CNode_CreateNode(NT_Drawable);
    CNode_SetName(_pNodeDrawable, "__WayPointIdxMsgBillboard");
    pCDrawable geometry = (pCDrawable)_pNodeDrawable->pNodeData;

    auto tempNum = wpi._index;
    for (unsigned int i = 0; i < idxLen; ++i)
    {
        pos[0] = Vec_CreatVec3d(-_w / 2.0 - i * _w, 0.0, _hSpace);//每多一位就往左多移动_w的距离
        pos[1] = Vec_CreatVec3d(_w / 2.0 - i * _w, 0.0, _hSpace);
        pos[2] = Vec_CreatVec3d(_w / 2.0 - i * _w, 0.0, _hSpace + _h);
        pos[3] = Vec_CreatVec3d(-_w / 2.0 - i * _w, 0.0, _hSpace + _h);

        auto txcod = GetCharTextureCoord(tempNum % 10 + '0');//获取字符对应的纹理坐标
        tempNum /= 10;

        texcoord[0] = Vec_CreatVec2f(txcod.origin.x, txcod.origin.y + txcod.size.height);
        texcoord[1] = Vec_CreatVec2f(txcod.origin.x + txcod.size.width, txcod.origin.y + txcod.size.height);
        texcoord[2] = Vec_CreatVec2f(txcod.origin.x + txcod.size.width, txcod.origin.y);
        texcoord[3] = Vec_CreatVec2f(txcod.origin.x, txcod.origin.y);

        colors[0] = defaultColor;
        colors[1] = defaultColor;
        colors[2] = defaultColor;
        colors[3] = defaultColor;

        alts[0] = altUseless;
        alts[1] = altUseless;
        alts[2] = altUseless;
        alts[3] = altUseless;

        //vertex texcoord color normal altitude
        DrawRectanglePlane(geometry, pos, texcoord, colors, alts);
    }

    //create vbo
    DrawableNodeCreateVBO(geometry);

    //texture
    //std::string fullPathTx = SpriteSvs3DCache::getInstance()->getModelFullPath("res/3dModel/FlightTube/waypoint2.png");
    std::string fullPathTx = SpriteSvs3DCache::getInstance()->getModelFullPath("res/ASCIIorg_white.png");
    HmsAviPf::Texture2D *texture = HmsAviPf::CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(fullPathTx);

    geometry->color[0] = 255.0f / 255;
    geometry->color[1] = 146.0f / 255;
    geometry->color[2] = 18.0f / 255;
    geometry->color[3] = 1.0f;

    Texture2D::TexParams texP = {
        GL_LINEAR,
        GL_LINEAR,
        GL_REPEAT,
        GL_REPEAT
    };
    texture->setTexParameters(texP);

    if (texture)
    {
        geometry->texture_image = new CImage;
        geometry->texture_image->m_iType = GL_RGBA;//todo: texture_image 不完备，但有了m_iType和m_ID就可渲染，以后再废弃C版本的CImage
        geometry->texture_image->m_ID = texture->getName();
    }
    else
    {
        geometry->texture_image = nullptr;
    }

    geometry->drawType = CDrawableType_FlightTubeWaypoint;

    //primitiveSet
    CPrimitiveSet primitiveSet = CPrimitiveSet_CreatDefaultVisible(GL_TRIANGLES, 0, geometry->_indices2.size());
    //CVector_Pushback(geometry->_primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);

    return _pNodeDrawable;
}


pCNode HmsSvsFlightTubeMgr::CreateWayPointsRootNode(void)
{
    if (_vecSvsWayPointInfos.size() <= 0)
    {
        return NULL;
    }

    auto pNodeWpRoot = CNode_CreateNode(EN_NodeType::Group);//waypoint root node

    for (auto &wpi : _vecSvsWayPointInfos)
    {
        auto pnodeMatWp = CreateWPMatrixNode(wpi._lngLatAlt, 0.0f);
        auto pnodeDrawableWp = CreateWPDrawableNode(wpi);
        auto pnodeDrawableWpIdx = CreateWPDrawableNodeIdx(wpi);//航路点的序号信息显示
        CNode_AddChild(pnodeMatWp, pnodeDrawableWp);
        CNode_AddChild(pnodeMatWp, pnodeDrawableWpIdx);

        _vecpWpMatrixNodeWpInfo.push_back(std::make_pair(pnodeMatWp, &wpi));//record the referrence of WP matrix and info

        CNode_AddChild(pNodeWpRoot, pnodeMatWp);
    }

    return pNodeWpRoot;
}

bool HmsSvsFlightTubeMgr::UpdateWayPointNodes(const HmsAviPf::Vec3d& planeLngLatAlt)
{
    Vec3d aftLLA = planeLngLatAlt;

    for (auto & wp : _vecpWpMatrixNodeWpInfo)
    {
        if (NULL == wp.first || NULL == wp.second)
        {
            return false;
        }
        pCMatrixTransform pnodeMatTrans = (pCMatrixTransform)(wp.first->pNodeData);

        CVec3d wplla = { wp.second->_lngLatAlt.x, wp.second->_lngLatAlt.y, wp.second->_lngLatAlt.z };
        auto ang = CalculateAngle(aftLLA.x, aftLLA.y, wplla._v[0], wplla._v[1]);
        auto angDeg = ang*180.0f / SIM_Pi;
        SetMatrixByLlaAndDir(pnodeMatTrans->matrix, wplla, angDeg);//update the matrix of Way point node
    }

    return true;
}

pCNode HmsSvsFlightTubeMgr::CreateFligthTubesRootNode(void)
{
    if (_vecSvsWayPointInfos.size() <= 1)
    {
        return NULL;
    }

    auto pNodeFTRoot = CNode_CreateNode(EN_NodeType::Group);//flight tube root node

    for (unsigned int segidx = 1; segidx < _vecSvsWayPointInfos.size(); segidx++)
    {
        auto pSegment = new HmsSvsFlightTubeSegment(_vecSvsWayPointInfos[segidx - 1]._lngLatAlt, _vecSvsWayPointInfos[segidx]._lngLatAlt, 1000);
        auto pNode = pSegment->GenerateCNode();

        _mtx_vecTubeSegRef.lock();
        _vecTubeSegRef.push_back(pSegment);
        _mtx_vecTubeSegRef.unlock();
        CNode_AddChild(pNodeFTRoot, pNode);
    }

    return pNodeFTRoot;
}


HmsSvsFlightTubeMgr* HmsSvsFlightTubeMgr::getInstance()
{
    if (nullptr == _pInstance)
    {
        std::unique_lock<std::mutex> mymutex1(res_mutex2);
        if (nullptr == _pInstance)
        {
            _pInstance = new HmsSvsFlightTubeMgr;
        }
    }
    return _pInstance;
}

void HmsSvsFlightTubeMgr::Update(float delta)
{
    //从队列获取新的
    if (_queue_WpiVec.size())
    {
        _mtx_wpi.lock();
        auto pwpivec = _queue_WpiVec.front();
        _queue_WpiVec.pop();
        _mtx_wpi.unlock();

        setFlightPlanData(&pwpivec, 10);
    }

    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
    auto aftData = dataBus->GetData();
    auto gnssData_Alt = dataBus->GetMslAltitudeMeter();
    Vec3d aftLLA = Vec3d(aftData->longitude, aftData->latitude, gnssData_Alt);

    UpdateWayPointNodes(aftLLA);

    _mtx_vecTubeSegRef.lock();
    for (auto& trubeSeg : _vecTubeSegRef)
    {
        trubeSeg->update(delta, aftLLA);
    }
    _mtx_vecTubeSegRef.unlock();
}

void HmsSvsFlightTubeMgr::setFlightPlanData(const std::vector<SvsWayPointInfo>* vecWPI, int rnp)
{
    //清除旧数据    
    _vecSvsWayPointInfos.clear();
    _vecpWpMatrixNodeWpInfo.clear();

    _mtx_vecTubeSegRef.lock();
    for (auto pseg : _vecTubeSegRef)
    {
        delete pseg;
    }
    _vecTubeSegRef.clear();
    _mtx_vecTubeSegRef.unlock();

    //清除旧显示节点
    CNode_RemoveFromParentsAndFreeNode(_pRootFlightTubes);
    CNode_RemoveFromParentsAndFreeNode(_pRootWaypoints);
    _pRootFlightTubes = NULL;
    _pRootWaypoints = NULL;

    //更新航路数据	
    //todo: test data
    if (vecWPI == nullptr)
    {
        //test mode
        auto wp0 = HmsAviPf::Vec3d(103.941778, 30.567674, 501);
        double alt_ground_surface = CAltManager_GetAltByLonLat(wp0.x, wp0.y);
        wp0.z = HMS_MAX(wp0.z, alt_ground_surface);
        _vecSvsWayPointInfos.push_back(SvsWayPointInfo(wp0, "zuuu", alt_ground_surface));

        wp0 = HmsAviPf::Vec3d(103.803580, 30.873737, 1300);
        alt_ground_surface = CAltManager_GetAltByLonLat(wp0.x, wp0.y);
        wp0.z = HMS_MAX(wp0.z, alt_ground_surface);
        _vecSvsWayPointInfos.push_back(SvsWayPointInfo(wp0, "ande", alt_ground_surface));

        wp0 = HmsAviPf::Vec3d(103.614053, 30.999155, 1500);
        alt_ground_surface = CAltManager_GetAltByLonLat(wp0.x, wp0.y);
        wp0.z = HMS_MAX(wp0.z, alt_ground_surface);
        _vecSvsWayPointInfos.push_back(SvsWayPointInfo(wp0, "nanqiao", alt_ground_surface));

        wp0 = HmsAviPf::Vec3d(103.573994, 31.036607, 950);
        alt_ground_surface = CAltManager_GetAltByLonLat(wp0.x, wp0.y);
        wp0.z = HMS_MAX(wp0.z, alt_ground_surface);
        _vecSvsWayPointInfos.push_back(SvsWayPointInfo(wp0, "zipinpu", alt_ground_surface));

        wp0 = HmsAviPf::Vec3d(103.471700, 30.996726, 900);
        alt_ground_surface = CAltManager_GetAltByLonLat(wp0.x, wp0.y);
        wp0.z = HMS_MAX(wp0.z, alt_ground_surface);
        _vecSvsWayPointInfos.push_back(SvsWayPointInfo(wp0, "xuankou", alt_ground_surface));

        wp0 = HmsAviPf::Vec3d(103.489516, 31.058921, 2060);
        alt_ground_surface = CAltManager_GetAltByLonLat(wp0.x, wp0.y);
        wp0.z = HMS_MAX(wp0.z, alt_ground_surface);
        _vecSvsWayPointInfos.push_back(SvsWayPointInfo(wp0, "yinxiu", alt_ground_surface));
    }
    else
    {
        //real data
        for (auto& wpi : *vecWPI)
        {
            SvsWayPointInfo _wpi = wpi;
            auto alt_ground_surface = CAltManager_GetAltByLonLat(_wpi._lngLatAlt.x, _wpi._lngLatAlt.y);
            _wpi._lngLatAlt.z = HMS_MAX(_wpi._lngLatAlt.z, alt_ground_surface);
            _vecSvsWayPointInfos.push_back(_wpi);
        }
    }

    //创建航路点
    _pRootWaypoints = CreateWayPointsRootNode();

    //创建飞行管道
    _pRootFlightTubes = CreateFligthTubesRootNode();

}

pCNode HmsSvsFlightTubeMgr::getRootWaypoints(void)
{
    return _pRootWaypoints;
}

pCNode HmsSvsFlightTubeMgr::getRootFlightTubes(void)
{
    return _pRootFlightTubes;
}

HmsSvsFlightTubeMgr::HmsSvsFlightTubeMgr()
: _pRootWaypoints(NULL)
, _pRootFlightTubes(NULL)
{
    _pRootWaypoints = CNode_CreateNode(EN_NodeType::Node);
    _pRootFlightTubes = CNode_CreateNode(EN_NodeType::Node);
}

HmsSvsFlightTubeSegment::HmsSvsFlightTubeSegment(const HmsAviPf::Vec3d& startLLA, const HmsAviPf::Vec3d& endLLA, const double& interval /*= 2000*/)
: _node(NULL)
, _nodeMatrix(NULL)
, _nodeDrawable(NULL)
, _startLLA(startLLA)
, _endLLA(endLLA)
, _interval(interval)
, _ShowHide(true)
, _InOut(false)
{
    _w = 600;//管道环的宽度
    _h = 350;//管道环的高度
    _t1 = 25;//管道环的环宽度
    _t2 = 30;//管道环的厚度（侧面边框的宽度）

    _tupeLen = CalculateDistanceM(_startLLA.x, _startLLA.y, _endLLA.x, _endLLA.y);

    auto llS = HmsAviPf::Vec2d(_startLLA.x, _startLLA.y);
    auto llE = HmsAviPf::Vec2d(_endLLA.x, _endLLA.y);

    /*MathHelper中的地球坐标系为左手坐标系*/
    _startPos = EarthRadiusM * CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&llS, _startLLA.z);
    _endPos = EarthRadiusM * CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&llE, _endLLA.z);

    _startPosf = HmsAviPf::Vec3(_startPos.x, _startPos.y, _startPos.z);
    _endPosf = HmsAviPf::Vec3(_endPos.x, _endPos.y, _endPos.z);

    auto tmpMidPos = (_startPos + _endPos) / 2;

    /*_diff_range为粗略碰撞测试包围胶囊体的半径，
    粗略碰撞测试的包围胶囊体为：底球心为本管道起点，顶球心为本管道终点，半径为此半径
    */
    _diff_range = EarthRadiusM + (_startLLA.z + _endLLA.z) / 2 - tmpMidPos.length() + _h * 1.2;

    interpolation(startLLA, endLLA, interval);
}

HmsSvsFlightTubeSegment::~HmsSvsFlightTubeSegment()
{
    //just reset the reference, do not release them
    _nodeDrawable = NULL;
    _nodeMatrix = NULL;
    _node = NULL;
}

//************************************
// Method:    intersectTestLineSegmentAndBall
// FullName:  HmsSvsFlightTubeMgr::intersectTestLineSegmentAndBall
// Access:    public static 
// Desc:      Determine if the line segment intersects the ball, 注意入参用相同的单位
// Returns:   bool
// Qualifier:
// Parameter: const HmsAviPf::Vec3 & lineEnd1
// Parameter: const HmsAviPf::Vec3 & lineEnd2
// Parameter: const HmsAviPf::Vec3 & ballCenter
// Parameter: const double & ballRadius
// Author:    yan.jiang
//************************************
bool HmsSvsFlightTubeMgr::intersectTestLineSegmentAndBall(
    const HmsAviPf::Vec3& lineEnd1,
    const HmsAviPf::Vec3& lineEnd2,
    const HmsAviPf::Vec3& ballCenter,
    const double& ballRadius
    )
{
    auto e = ballCenter - lineEnd1;
    auto dis = lineEnd2 - lineEnd1;
    auto lineSegLen = dis.length();
    dis.normalize();
    auto d = dis;
    auto a = e.dot(d);

    auto r = ballRadius;

    auto tmp = r*r - e.dot(e);
    if (tmp > 0)
    {
        //起点在求内
        //线段和球相交
        return true;
    }

    auto ff = tmp + a*a;

    if (ff < 0)
    {
        //与直线无相交
        return false;
    }
    else
    {
        //和线有相交，再进一步判断是否与线段相交
        auto f = sqrt(ff);
        auto t = a - f;
        if (t <= 0 || t >= lineSegLen)
        {
            //与线段无相交
            return false;
        }
        else
        {
            //与线段相交
            return true;
        }
    }
}

unsigned int HmsSvsFlightTubeMgr::getNumLength(const int& val)
{
    auto x = val;
    int len = 0;

    if (0 == x)
    {
        len = 1;
    }
    else
    {
        while (x)
        {
            x /= 10;
            len++;
        }
    }

    return len;
}

void HmsSvsFlightTubeSegment::update(float delta, const HmsAviPf::Vec3d& planeLngLatAlt)
{
    if (!_node || !_nodeMatrix || !_nodeDrawable)
    {
        std::cout << "_node not existed" << std::endl;
        return;
    }

    pCDrawable geometry = (pCDrawable)_nodeDrawable->pNodeData;

    if (_ShowHide)
    {
        geometry->is_display = 1;
    }
    else
    {
        geometry->is_display = 0;
    }

    if (_InOut)
    {
        //在管道内 绿色
        geometry->color[0] = 0.0f;
        geometry->color[1] = 1.0f;
        geometry->color[2] = 0.0f;
        geometry->color[3] = 1.0f;
    }
    else
    {
        //在管道外 橙色
        geometry->color[0] = 255.0f / 255;
        geometry->color[1] = 174.0f / 255;
        geometry->color[2] = 69.0f / 255;
        geometry->color[3] = 1.0f;
    }
}

#define SHOW_TUBE_MAX_DISTANCE 50000    //50km
void HmsSvsFlightTubeSegment::updateShowHide(float delta, const HmsAviPf::Vec3d& planeLngLatAlt)
{
    if (!_node || !_nodeMatrix || !_nodeDrawable)
    {
        std::cout << "_node not existed" << std::endl;
        return;
    }

    pCDrawable geometry = (pCDrawable)_nodeDrawable->pNodeData;

    auto planeLngLat = HmsAviPf::Vec2d(planeLngLatAlt.x, planeLngLatAlt.y);
    auto PPos_normal = CHmsNavSvsMathHelper::LonLatToEarthPoint_Double(&planeLngLat);
    auto PPos = PPos_normal * (EarthRadiusM + planeLngLatAlt.z);
    auto PPosf = HmsAviPf::Vec3(PPos.x, PPos.y, PPos.z);

    _ShowHide = HmsSvsFlightTubeMgr::intersectTestLineSegmentAndBall(_startPosf, _endPosf, PPosf, SHOW_TUBE_MAX_DISTANCE);//超出此距离使其不可见
}

void HmsSvsFlightTubeSegment::updateInOut(float delta, const HmsAviPf::Vec3d& planeLngLatAlt)
{
    if (!_node || !_nodeMatrix || !_nodeDrawable)
    {
        std::cout << "_node not existed" << std::endl;
        return;
    }

    auto planeLngLat = HmsAviPf::Vec2d(planeLngLatAlt.x, planeLngLatAlt.y);
    auto PPos_normal = CHmsNavSvsMathHelper::LonLatToEarthPoint_Double(&planeLngLat);
    auto PPos = PPos_normal * (EarthRadiusM + planeLngLatAlt.z);
    auto PPosf = HmsAviPf::Vec3(PPos.x, PPos.y, PPos.z);

    _InOut = false;//init val false: out

    if (_ShowHide)//only for the trub which would be shown
    {
        if (HmsSvsFlightTubeMgr::intersectTestLineSegmentAndBall(_startPosf, _endPosf, PPosf, _diff_range))
        {
            //当飞机在本管道的胶囊体范围内，才进一步精确判断是否在本管道内
            auto sz = _vecPos.size();
            for (unsigned int i = 0; i + 1 < sz; ++i)
            {
                auto p1 = _vecPos[i];
                auto p2 = _vecPos[i + 1];
                auto pp1 = HmsAviPf::Vec3(p1.x, p1.y, p1.z);
                auto pp2 = HmsAviPf::Vec3(p2.x, p2.y, p2.z);
                bool colli3 = HmsSvsFlightTubeMgr::intersectTestLineSegmentAndBall(pp1, pp2, PPosf, _w / 2);//管道宽度的一半作为半径
                if (colli3)
                {
                    _InOut = true;// in   
                    break;
                }
            }
        }
    }
}

pCNode HmsSvsFlightTubeSegment::getNode(void)
{
    return _node;
}

pCNode HmsSvsFlightTubeSegment::GenerateCNode(void)
{
    if (_node || _nodeMatrix || _nodeDrawable)
    {
        std::cout << "_node existed" << std::endl;
        return NULL;
    }

    _node = CNode_CreateNode(EN_NodeType::Group);
    _nodeMatrix = CreateTubeMatrixNode();
    _nodeDrawable = CreateTubeDrawableNode();

    CNode_AddChild(_nodeMatrix, _nodeDrawable);
    CNode_AddChild(_node, _nodeMatrix);
    return _node;
}


//************************************
// Method:    interpolation
// FullName:  HmsSvsFlightTubeSegment::interpolation
// Access:    private 
// Desc:      插值生成中间的点
// Returns:   void
// Qualifier:
// Parameter: const HmsAviPf::Vec3d & startLLA
// Parameter: const HmsAviPf::Vec3d & endLLA
// Parameter: const double & interval
// Author:    yan.jiang
//************************************
void HmsSvsFlightTubeSegment::interpolation(const HmsAviPf::Vec3d& startLLA, const HmsAviPf::Vec3d& endLLA, const double& interval /*= 2000*/)
{
    /*
    地心到start点的矢量为 _OS
    地心到end点的矢量为 _OE
    矢量 _OS 旋转到矢量 _OE 的旋转轴为过地心的矢量 OA
    */
    auto llS = HmsAviPf::Vec2d(startLLA.x, startLLA.y);//lng lat start 
    auto llE = HmsAviPf::Vec2d(endLLA.x, endLLA.y);//lng lat end 

    _OS_normal = CHmsNavSvsMathHelper::LonLatToEarthPoint_Double(&llS) - Vec3d(0, 0, 0);
    _OS = _OS_normal * (EarthRadiusM + startLLA.z);
    _OE_normal = CHmsNavSvsMathHelper::LonLatToEarthPoint_Double(&llE) - Vec3d(0, 0, 0);
    _OE = _OE_normal * (EarthRadiusM + endLLA.z);

    HmsAviPf::Vec3d::cross(_OS_normal, _OE_normal, &_OA_normal);//都是左手定则
    _OA_normal.normalize();

    //通过绕OA旋转生成其余的管道环的顶点，直到结束点
    auto angle_S2E = HmsAviPf::Vec3d::angle(_OS_normal, _OE_normal);
    auto disttance_S2E = CalculateDistanceM(startLLA.x, startLLA.y, endLLA.x, endLLA.y);//海平面上的距离
    auto cnt_ring = disttance_S2E / interval;
    auto angel_step = angle_S2E / cnt_ring;
    auto height_step = (endLLA.z - startLLA.z) / cnt_ring;

    _vecPos.clear();
    for (int i = 0; i < cnt_ring; ++i)
    {
        HmsAviPf::Quaterniond q(_OA_normal, i * angel_step);
        auto OCurr_normal = q * _OS_normal;
        auto height_diff = OCurr_normal * height_step * i;
        auto OCurr = q * _OS;
        OCurr += height_diff;
        _vecPos.push_back(OCurr);
    }
}

pCNode HmsSvsFlightTubeSegment::CreateTubeMatrixNode()
{
    pCNode pnodemat = CNode_CreateNode(EN_NodeType::MatrixTransform);
    pCMatrixTransform pnodeMatTrans = (pCMatrixTransform)(pnodemat->pNodeData);
    CMatrix tmpMatrix;

    auto tmppos = g_transCoord * _startPos;

    CVec3d XYZ = Vec_CreatVec3d(tmppos.x, tmppos.y, tmppos.z);
    CMatrix_SetMakeTranslateVec3d(&tmpMatrix, &XYZ);

    pnodeMatTrans->matrix = tmpMatrix;
    return pnodemat;
}

pCNode HmsSvsFlightTubeSegment::CreateTubeDrawableNode()
{
    /*
    矩形管道环
    level 1:
    3-----------------2
    | \             / |
    |  7-----------6  |
    |  |           |  |
    |  |           |  |
    |  |           |  |
    |  4-----------5  |
    | /             \ |
    0-----------------1

    level 2:
    11---------------10
    |                 |
    |                 |
    |                 |
    |                 |
    |                 |
    |                 |
    |                 |
    8-----------------9
    */

    pCNode pdraw = CNode_CreateNode(NT_Drawable);
    CNode_SetName(pdraw, "__flightTubeDrawable");
    pCDrawable geometry = (pCDrawable)pdraw->pNodeData;

    float ww = _w;// 管道环的宽度
    float hh = _h;// 管道环的高度
    float tt1 = _t1;// 管道环的环宽度
    float tt2 = _t2;// 管道环的厚度（侧面边框的宽度）

    auto ww2 = ww / 2.0;
    auto hh2 = hh / 2.0;
    auto ww2in = ww2 - tt1;
    auto hh2in = hh2 - tt1;

    HmsAviPf::Vec3d ring_Pos[12];//当前环的顶点
    CVec2f texcoord[12];
    CVec4f colors[12];
    float alts[12];

    //纹理坐标、color、altitude
    CVec4f defaultColor = Vec_CreatVec4f(1.0, 1.0, 1.0, 0.5);
    GLfloat altUseless = 0.0F;
    texcoord[0] = Vec_CreatVec2f(0.0f, 1.0f); colors[0] = defaultColor; alts[0] = altUseless;
    texcoord[1] = Vec_CreatVec2f(1.0f, 1.0f); colors[1] = defaultColor; alts[1] = altUseless;
    texcoord[2] = Vec_CreatVec2f(1.0f, 0.0f); colors[2] = defaultColor; alts[2] = altUseless;
    texcoord[3] = Vec_CreatVec2f(0.0f, 0.0f); colors[3] = defaultColor; alts[3] = altUseless;
    texcoord[4] = Vec_CreatVec2f(0.1f, 0.9f); colors[4] = defaultColor; alts[4] = altUseless;
    texcoord[5] = Vec_CreatVec2f(0.9f, 0.9f); colors[5] = defaultColor; alts[5] = altUseless;
    texcoord[6] = Vec_CreatVec2f(0.9f, 0.1f); colors[6] = defaultColor; alts[6] = altUseless;
    texcoord[7] = Vec_CreatVec2f(0.1f, 0.1f); colors[7] = defaultColor; alts[7] = altUseless;
    texcoord[8] = Vec_CreatVec2f(0.1f, 0.9f); colors[8] = defaultColor; alts[8] = altUseless;
    texcoord[9] = Vec_CreatVec2f(0.9f, 0.9f); colors[9] = defaultColor; alts[9] = altUseless;
    texcoord[10] = Vec_CreatVec2f(0.9f, 0.1f); colors[10] = defaultColor; alts[10] = altUseless;
    texcoord[11] = Vec_CreatVec2f(0.1f, 0.1f); colors[11] = defaultColor; alts[11] = altUseless;

    CVec3d pos[12];
    HmsAviPf::Vec3d ring_Pos2[12];

    for (auto& OR : _vecPos)//OR （O Ring）地心到当前环位置(环心)的矢量
    {
        auto OR_normal = OR;
        OR_normal.normalize();
        HmsAviPf::Vec3d OP_normal;
        HmsAviPf::Vec3d::cross(_OA_normal, OR_normal, &OP_normal);//都是左手定则
        OP_normal.normalize();//当前环指向下一个环的方向矢量

        //计算本环的12个顶点
        ring_Pos[0] = OR + OR_normal * (-hh2) + _OA_normal * (-ww2);
        ring_Pos[1] = OR + OR_normal * (-hh2) + _OA_normal * (+ww2);
        ring_Pos[2] = OR + OR_normal * (+hh2) + _OA_normal * (+ww2);
        ring_Pos[3] = OR + OR_normal * (+hh2) + _OA_normal * (-ww2);
        ring_Pos[4] = OR + OR_normal * (-hh2in) + _OA_normal * (-ww2in);
        ring_Pos[5] = OR + OR_normal * (-hh2in) + _OA_normal * (+ww2in);
        ring_Pos[6] = OR + OR_normal * (+hh2in) + _OA_normal * (+ww2in);
        ring_Pos[7] = OR + OR_normal * (+hh2in) + _OA_normal * (-ww2in);
        ring_Pos[8] = ring_Pos[0] + OP_normal * tt2;
        ring_Pos[9] = ring_Pos[1] + OP_normal * tt2;
        ring_Pos[10] = ring_Pos[2] + OP_normal * tt2;
        ring_Pos[11] = ring_Pos[3] + OP_normal * tt2;

        for (int j = 0; j < 12; ++j)
        {
            ring_Pos2[j] = ring_Pos[j] - _startPos;//所有顶点都以_startPos为模型矩阵的原点
            ring_Pos2[j] = g_transCoord * ring_Pos2[j];//cpp mathhelper的坐标转换 成c的那套库的坐标系
            pos[j] = Vec_CreatVec3d(ring_Pos2[j].x, ring_Pos2[j].y, ring_Pos2[j].z);
        }
        DrawRectangleRingPlaneWithSideFrame(geometry, pos, texcoord, colors, alts);
    }

    //create vbo
    DrawableNodeCreateVBO(geometry);

    //texture
    //std::string fullPathTx = SpriteSvs3DCache::getInstance()->getModelFullPath("res/3dModel/FlightTube/waypoint2.png");
    std::string fullPathTx = SpriteSvs3DCache::getInstance()->getModelFullPath("res/3dModel/FlightTube/flightTupeRing.png");
    HmsAviPf::Texture2D *texture = HmsAviPf::CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(fullPathTx);

    //init color
    geometry->color[0] = 255.0f / 255;
    geometry->color[1] = 174.0f / 255;
    geometry->color[2] = 69.0f / 255;
    geometry->color[3] = 1.0f;

    Texture2D::TexParams texP = {
        GL_LINEAR,
        GL_LINEAR,
        GL_REPEAT,
        GL_REPEAT
    };
    texture->setTexParameters(texP);

    if (texture)
    {
        geometry->texture_image = new CImage;
        geometry->texture_image->m_iType = GL_RGBA;//todo: texture_image 不完备，但有了m_iType和m_ID就可渲染，以后再废弃C版本的CImage
        geometry->texture_image->m_ID = texture->getName();
    }
    else
    {
        geometry->texture_image = nullptr;
    }

    geometry->drawType = CDrawableType_FlightTubeWaypoint;

    return pdraw;
}
