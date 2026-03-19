#pragma once

#include "NavSvs/mathd/Vec3d.h"
#include "HmsAviMacros.h"
#include "math/HmsMath.h"
#include <vector>
#include <string>
#include <utility>
#include <queue>
#include <mutex>
#include "math/HmsGeometry.h"
#include "Drawable.h"
#include "MatrixNode.h"

extern HmsAviPf::Mat4d g_transCoord;

class HmsSvsFlightTubeSegment;

class SvsWayPointInfo{
public:
    HmsAviPf::Vec3d _lngLatAlt;
    std::string _name;
    unsigned int _index;
    double _terrainAlt;
    SvsWayPointInfo(){
        _lngLatAlt = HmsAviPf::Vec3d(0.0, 0.0, 0.0);
        _name = std::string("");
        _index = 0;
        _terrainAlt = 0.0;
    };
    SvsWayPointInfo(const HmsAviPf::Vec3d& lla, const std::string& name, const unsigned int& index, const double& terrainAlt = 0.0)
        : _lngLatAlt(lla), _name(name), _index(index), _terrainAlt(terrainAlt){};
    ~SvsWayPointInfo(){

    };
};

class HmsSvsFlightTubeMgr
{
public:
    HmsSvsFlightTubeMgr();

    ~HmsSvsFlightTubeMgr();

    static HmsSvsFlightTubeMgr* getInstance();
    void Update(float delta);
    void setFlightPlanData(const std::vector<SvsWayPointInfo>* vecWPI, int rnp);
    pCNode getRootWaypoints(void);
    pCNode getRootFlightTubes(void);
    static void SetMatrixByLlaAndDir(CMatrix& outMatrix, const CVec3d& LngLatAlt, const float& dir);
    static bool intersectTestLineSegmentAndBall(const HmsAviPf::Vec3& lineEnd1, const HmsAviPf::Vec3& lineEnd2, const HmsAviPf::Vec3& ballCenter, const double& ballRadius);
    static unsigned int getNumLength(const int& val);
    std::mutex _mtx_wpi;
    std::mutex _mtx_vecTubeSegRef;
    std::queue<std::vector<SvsWayPointInfo>> _queue_WpiVec;
    std::vector<HmsSvsFlightTubeSegment *>& VecTubeSegRef() { return _vecTubeSegRef; }
private:
    pCNode CreateWayPointsRootNode(void);
    bool UpdateWayPointNodes(const HmsAviPf::Vec3d& planeLngLatAlt);
    pCNode CreateFligthTubesRootNode(void);
    pCNode CreateWPMatrixNode(const HmsAviPf::Vec3d& lla, const float& dir);

    pCNode CreateWPDrawableNode(const SvsWayPointInfo& wpi);
    HmsAviPf::Rect GetCharTextureCoord(const char& ch, const HmsAviPf::Size& textureSize = HmsAviPf::Size(896, 552));
    pCNode CreateWPDrawableNodeIdx(const SvsWayPointInfo& wpi);
    pCNode _pRootWaypoints;
    pCNode _pRootFlightTubes;
    std::vector<HmsSvsFlightTubeSegment*> _vecTubeSegRef;

    std::vector<std::pair<pCNode, SvsWayPointInfo*>> _vecpWpMatrixNodeWpInfo;//for billboard facing rotation angle calc
    static HmsSvsFlightTubeMgr* _pInstance;
    std::vector<SvsWayPointInfo> _vecSvsWayPointInfos;

};

class HmsSvsFlightTubeSegment
{
public:
    HmsSvsFlightTubeSegment(const HmsAviPf::Vec3d& startLLA, const HmsAviPf::Vec3d& endLLA, const double& interval = 2000);
    ~HmsSvsFlightTubeSegment();
    void update(float delta, const HmsAviPf::Vec3d& planeLngLatAlt);
    void updateShowHide(float delta, const HmsAviPf::Vec3d& planeLngLatAlt);
    void updateInOut(float delta, const HmsAviPf::Vec3d& planeLngLatAlt);
    pCNode getNode(void);
    pCNode GenerateCNode(void);
private:
    void interpolation(const HmsAviPf::Vec3d& startLLA, const HmsAviPf::Vec3d& endLLA, const double& interval = 2000);
    pCNode CreateTubeMatrixNode();
    pCNode CreateTubeDrawableNode();
    pCNode _node;
    pCNode _nodeMatrix;
    pCNode _nodeDrawable;
    HmsAviPf::Vec3d _startLLA;  //lon lat alt
    HmsAviPf::Vec3d _endLLA;    //lon lat alt
    HmsAviPf::Vec3d _startPos;   //xyz
    HmsAviPf::Vec3d _endPos;     //xyz
    HmsAviPf::Vec3 _startPosf;   //xyz
    HmsAviPf::Vec3 _endPosf;     //xyz
    bool _ShowHide;//�ܵ��Ƿ���ʾ
    bool _InOut;//�ڹܵ���or��
    float _w;//���ιܵ����Ŀ��
    float _h;//���ιܵ����ĸ߶�
    float _t1;//�ܵ����Ļ����
    float _t2;//�ܵ����ĺ�ȣ�����߿�Ŀ�ȣ�    
    double _diff_range;//������ײ���԰�Χ����İ뾶
    std::vector<HmsAviPf::Vec3d> _vecPos;//�������д�Բ������ֵ��xyz��
    double _interval;
    double _tupeLen;//segment length (meter)
    HmsAviPf::Vec3d _OS;
    HmsAviPf::Vec3d _OS_normal;
    HmsAviPf::Vec3d _OE;
    HmsAviPf::Vec3d _OE_normal;
    HmsAviPf::Vec3d _OA;
    HmsAviPf::Vec3d _OA_normal;
};

