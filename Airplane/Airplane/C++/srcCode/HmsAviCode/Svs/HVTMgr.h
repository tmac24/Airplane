#pragma once

#include "HVTDataMgr.h"
#include "SpriteSvs3DCache.h"
#include "Drawable.h"

class TowerObj//高压塔。记录此时与飞机的实时告警状态
{
public:
    TowerObj(const TowerLLAH& llah){ 
        tllah = llah; 
        CVec3d towerCenter = Vec_CreatVec3d(tllah.lon, tllah.lat, tllah.alt + tllah.height / 2);
        pos = Coordinate_Vec3dDegreeLonlatAltToVec3d(towerCenter);
    }

    TowerLLAH tllah;
    CVec3d pos;
    bool bAlarmState;
    std::vector<pCDrawable> vecDwa;
};

class HvWireObj//高压线（line为整体）。记录与此时飞机的实时位置关系
{
public:
    HvWireObj(pCDrawable pDraw){
        pDwa = pDraw;
    }
    void updateAircraftPos(const CVec3d& aircraftPos){
        CVec3d aircraftPosRelCenterPos = Vec_Vec3dSubVec3d(&aircraftPos, &pDwa->centerPos);
        _aircraftPosRelCenterPos = Vec_pVec3dToVec3f(&aircraftPosRelCenterPos);
        pDwa->aftPosRel2CenterPos = _aircraftPosRelCenterPos;
    }
    
    pCDrawable pDwa;//此高压线的drawable 节点
    CVec3f _aircraftPosRelCenterPos;//此时飞机相对于此高压线centerPos的位置    
};

class HVTMgr
{
public:
	static HVTMgr* getInstance();
	~HVTMgr();

	void Update(CVec3d LonLatAlt);

	pCNode RootWires() const { return _rootWires; }
    pCNode RootTowers() const { return _rootTowers; }
	void RootWires(pCNode val) { _rootWires = val; }
    void RootTowers(pCNode val) { _rootTowers = val; }
    void SetAlarmDistance(float alarmRange){ _alarmDistance = alarmRange < 1.0f ? 1.0f : alarmRange; }//单位 米
	static void SetHvtDataPath(const std::string & strPath);
	std::string getHvtDataPath() const;
private:
	HVTMgr();
	
	pCNode CreateHVTWiresNode(const WireData& wd_in);
    pCNode CreateHVTTowersNode(const WireData& wd_in);
    pCNode CreateHVTMatrixNode(CVec3d& lonlatalt);

	pCNode CreateTowerMatrixNode(const TowerLLAH& llah, const double& towerDir);	
    void CreateTowerDrawableNode(pCNode fatherNode, const TowerLLAH& T_LLAH, TowerObj* pto);

	SpriteSvs3DData* LoadTowerModel() const;

	pCNode CreateHVWireDrawableNode2(const WireData& wd);
	pCNode CreateHVWireDrawableNode3(const WireData& wd);
	static HVTMgr* _pInstance;
	pCNode _rootWires;
    pCNode _rootTowers;
	CVec3d _lastLonLatAlt;
	static std::string _databasePath;
    std::vector<TowerObj*> _vecNearbyTowerObj;
    std::vector<HvWireObj*> _vecNearbyHvWireObj;

    float _alarmDistance;//meter
};
