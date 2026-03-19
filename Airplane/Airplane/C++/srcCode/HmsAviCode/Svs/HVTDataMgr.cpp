#include "HVTDataMgr.h"
#include "FileManager.h"
#include <assert.h>
#include "HVTDataSql.h"
#include "HmsAviMacros.h"


HVTDataMgr* HVTDataMgr::_instance = nullptr;

void HVTDataMgr::ClearData()
{
	//清空线
	for (auto it = _wires.begin(); it != _wires.end();)
	{
		HMS_SAFE_DELETE(it->second);
		_wires.erase(it++);
	}
	_wires.clear();

	//清空 _mapLineTowers
	for (auto it = _mapLineTowers.begin(); it != _mapLineTowers.end(); it++)
	{
		it->second.clear();
	}
	_mapLineTowers.clear();

	//清空 _mapTowerInfo
	for (auto it = _mapTowerInfo.begin(); it != _mapTowerInfo.end(); it++)
	{
		HMS_SAFE_DELETE(it->second);
	}
	_mapTowerInfo.clear();

	//清空 _towers
	_towers.clear();
}

HVTDataMgr::HVTDataMgr()
{

}

HVTDataMgr* HVTDataMgr::getInstance()
{
	if (nullptr == _instance)
	{
		_instance = new HVTDataMgr;		
	}
	return _instance;
}



//************************************
// Method:    UpdateData
// FullName:  HmsAviPf::HVTDataMgr::UpdateData
// Access:    public 
// Desc:      
// Returns:   void
// Qualifier:
// Parameter: const CVec3d & centerLLA //数据范围的中心位置
// Author:    yan.jiang
//************************************
void HVTDataMgr::UpdateData(const CVec3d& centerLLA)
{
	//清空Wires缓存
	ClearData();

	//1、根据经纬度从数据库查询数据
	_towers = HVTDataSql::getInstance()->GetTowersData(centerLLA, 0.3, 0.3);

#if 0//测试数据	
	const int towercount = 5;//tower count of the line
	const int endpointcnt = 5;//endpoint count per tower

	towerinfo lonlataltTowers[] = {
		1, 1, 1, 116.306352, 40.24905793, 359.236, 53.1405,
		2, 1, 2, 116.3097487, 40.24515794, 301.866, 40.6803,
		3, 1, 3, 116.312017, 40.24255535, 240.3251, 48.4431,
		4, 1, 4, 116.3144166, 40.23980192, 159.9263, 50.1491,
		5, 1, 5, 116.3169077, 40.23694008, 137.3325, 39.5487,
	};

	CVec3d lonlataltEnds[towercount * endpointcnt] = {
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
	//test data

	//2、将查询到的数据进行封装
	//构造塔
	CVec3d* ptmp = lonlataltEnds;
	
	for (auto t: lonlataltTowers)
	{
		_mapTowerInfo[t.towerid] = new Tower(t, ptmp, endpointcnt);
		ptmp += endpointcnt;
	}

	
	//3、将塔组合成线
	//归并到线
	for (auto &LT:_mapTowerInfo)
	{
		_mapLineTowers[LT.second->_lineId].push_back(LT.second);
	}
#else
	//2、将塔组合成线
	//归并到线
	for (auto &t : _towers)
	{
		_mapLineTowers[t._lineId].push_back(&t);
	}
#endif
	//3、去掉只有一个塔的线
	for (auto LT = _mapLineTowers.begin(); LT != _mapLineTowers.end();)
	{
		if (LT->second.size()<=1)
		{//只有一个塔的线
			_mapLineTowers.erase(LT++);
		}
		else
		{
			LT++;
		}
	}

	//4、组合成线
	for (auto LT = _mapLineTowers.begin(); LT != _mapLineTowers.end();LT++)
	{
		//每条线中的塔按照塔序号排序
		std::sort(LT->second.begin(), LT->second.end(),
			[](const Tower* t1, const Tower* t2){
			return t1->_towerIdx < t2->_towerIdx;
		});

		//生成线
		WireData* pwd = new WireData(LT->first, LT->second);
		_wires[LT->first] = pwd;
	}
}

HVTDataMgr::~HVTDataMgr()
{

}


//************************************
// Method:    Tower
// FullName:  Tower::Tower
// Access:    public 
// Desc:      
// Returns:   
// Qualifier:
// Parameter: const towerinfo & ti
// Parameter: const CVec3d * endps
// Parameter: const int & endpCount 塔上接线端子的数量
// Author:    yan.jiang
//************************************
Tower::Tower(const towerinfo& ti, const CVec3d *endps, const int& endpCount)
{
	_towerLLAH = ti.lonlatalt;
	_towerIdx = ti.towerIdx;
	_lineId = ti.lineid;
	for (int i = 0; i < endpCount; i++)
	{
		_endps.push_back(*(endps + i));
	}
}

WireData::WireData(const int& lineId, std::vector<Tower*>& towers)
{	
	_wireId = lineId;	
	_towerCnt = towers.size();

	if (_towerCnt == 0 && towers[0] == nullptr)
	{
		std::cout << "towers empty." << std::endl;
		return;
	}

	_threadCnt = towers[0]->_endps.size();

	_plonlatTowers = new TowerLLAH[_towerCnt];	
	_plonlataltEnds = new CVec3d[_towerCnt*_threadCnt];

	TowerLLAH* ptmp1 = _plonlatTowers;
	pCVec3d ptmp2 = _plonlataltEnds;//塔上的端子
	for (auto& t:towers)
	{
		//塔
		*ptmp1 = t->_towerLLAH;
		ptmp1++;

		//端子
		for (auto& e:t->_endps)
		{
			*ptmp2 = e;
			ptmp2++;
		}
	}
}

WireData::WireData(const int& lineid, const int& towerCnt, const int& endpCntPerTower, const TowerLLAH* pTowersLLAH, const pCVec3d& pEndsLLA)
:_threadCnt(endpCntPerTower)
{
	_towerCnt = towerCnt;
	_plonlatTowers = new TowerLLAH[_towerCnt];
	_plonlataltEnds = new CVec3d[_towerCnt*_threadCnt];

	memcpy(_plonlatTowers, pTowersLLAH, _towerCnt*sizeof(TowerLLAH));
	memcpy(_plonlataltEnds, pEndsLLA, _towerCnt*_threadCnt*sizeof(CVec3d));
	_wireId = lineid;
}

WireData::~WireData()
{
	HMS_SAFE_DELETE_ARRAY(_plonlatTowers);
	HMS_SAFE_DELETE_ARRAY(_plonlataltEnds);
	_wireId = -1;
	_towerCnt = 0;
	_threadCnt = 0;
}
