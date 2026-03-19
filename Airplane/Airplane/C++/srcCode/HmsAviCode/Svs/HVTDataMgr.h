/*!
 * \file HVTDataMgr.h
 *
 * \author yan.jiang
 * \date 一月 2019
 *
 * 提供渲染所需要的高压线以及电塔数据
 * 飞机附近一定距离范围内的高压线以及电塔才显示
 * 数据刷新，按照一定的规则，
 * 规则可以为：飞机距离上一次所在的位置超过一定的距离阈值后就触发数据刷新。
 * 本类只负责数据刷新（不包含vbo刷新）
 */
#pragma once
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include "AltManager.h"


class TowerLLAH
{
public:
	union
	{
		CVec3d LLA;
		struct
		{
			double lon;
			double lat;
			double alt;//塔底的海拔
		};
	};
	double height;//塔高
};

class towerinfo{
public:
	int towerid;//唯一的
	int lineid;
	int towerIdx;
	TowerLLAH lonlatalt;
};

class Tower
{
public:	
	Tower(const towerinfo& ti, const CVec3d *endps, const int& endpCount);
	int _towerIdx;//tower 在线中的序号
	int _lineId;
	TowerLLAH _towerLLAH;//longitude latitude altitude of tower
	std::vector<CVec3d> _endps;//all endpoints the tower owned
};

class WireData
{
public:
	WireData(const int& lineId, std::vector<Tower*>& towers);
	WireData(const int& lineId, const int& towerCnt, const int& endpCntPerTower, const TowerLLAH* pTowersLLAH, const pCVec3d& pEndsLLA);
	~WireData();
	int WireId() const { return _wireId; }
	int TowerCnt() const { return _towerCnt; }
	TowerLLAH* PlonlataltTowers() const { return _plonlatTowers; }
	int ThreadCnt() const { return _threadCnt; }
	pCVec3d PlonlataltEnds() const { return _plonlataltEnds; }

private:
	int _wireId = -1;		
	int _towerCnt = 0;//tower 数量		
	int _threadCnt = 0; //线的股数，即塔的EndPoint数量		
	TowerLLAH* _plonlatTowers = nullptr;
	pCVec3d _plonlataltEnds = nullptr;
};

class TowerHash{
	size_t operator()(const Tower& t)
	{
		return std::hash<double>()(t._towerLLAH.lon) ^ std::hash<double>()(t._towerLLAH.lat) ^ std::hash<double>()(t._towerLLAH.alt);
	}
};
//const double ESP = 10E-12;// 0.000000000001;
#define ESP (10E-12)
class TowerCmp{
	bool operator()(const Tower& t1, const Tower& t2)
	{
		auto delta = Vec_Vec3dSubVec3d(&t1._towerLLAH.LLA, &t2._towerLLAH.LLA);
		return (delta._v[0] < ESP && delta._v[1] < ESP && delta._v[2] < ESP);
	}
};

class  HVTDataMgr
{
public:		 
	virtual ~ HVTDataMgr();
	static HVTDataMgr* getInstance();
	void UpdateData(const CVec3d& centerLLA);
	const std::unordered_map<int, WireData *>& Wires() const { return _wires; }	
private:
	HVTDataMgr();
	static HVTDataMgr* _instance;	
	std::unordered_map<int, Tower*> _mapTowerInfo;//the key is tower id
	std::unordered_map<int, std::vector<Tower*>> _mapLineTowers;//the key is line id
	std::unordered_map<int, WireData*> _wires;//the key is line id
	std::vector<Tower> _towers;
	void ClearData();
};

