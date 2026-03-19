#include "HVTDataSql.h"
#include <assert.h>
#include "base/HmsFileUtils.h"
#include "HVTMgr.h"
#include "AltManager.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "FileManager.h"

#ifdef __cplusplus
}
#endif

//extern "C" const char *GetTawsDataPath(void);

HVTDataSql* HVTDataSql::_instance = nullptr;

HVTDataSql::HVTDataSql()
	: _pDb(nullptr)
{
	char *pErrMsg = 0;
	int ret = 0;
	char buff[256] = { 0 };
	//std::string dbPath = FileUtils::getInstance()->fullPathForFilename("hvtData/testdb.db");
	std::string dbPath = std::string(HVTMgr::getInstance()->getHvtDataPath()+ "HvtData.db");
	auto exist = HmsAviPf::CHmsFileUtils::getInstance()->isFileExist(dbPath);
	if (!exist)
	{
		std::cout << "\nHVT database file not existed!" << std::endl;
		return;
	}
	ret = sqlite3_open(dbPath.c_str(), &_pDb);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "open hvtdb fail: %s\n", sqlite3_errmsg(_pDb));
		sqlite3_close(_pDb);
	}

	printf("open hvtDb success\n");
}

HVTDataSql* HVTDataSql::getInstance()
{
	if (nullptr == _instance)
	{
		_instance = new HVTDataSql;		
	}
	return _instance;
}

std::vector<Tower> HVTDataSql::GetTowersData(const CVec3d& centerLLA, const double& LonRange, const double& LatRange)
{
	std::vector<Tower> ts;

	//1、塔的信息
	std::string strSql =
		"select id,lineid,towerIdx,Lat,Lon,Alt,Heigh "
		"from tower "
		"where abs(tower.Lon-?)<? and abs(tower.Lat-?)<? "
		"order by lineid,towerIdx asc";

	sqlite3_stmt * pStmt = nullptr;
	if (_pDb == nullptr)
	{
		printf("tower database null!\n");
		return ts;
	}
	sqlite3_prepare(_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return ts;
	}

	int tempIndex = 1;
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[0]);
	sqlite3_bind_double(pStmt, tempIndex++, LonRange);
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[1]);
	sqlite3_bind_double(pStmt, tempIndex++, LatRange);

	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		int tempIndex = 0;
		towerinfo tmptif;
		tmptif.towerIdx = sqlite3_column_int(pStmt, tempIndex++);
		tmptif.lineid = sqlite3_column_int(pStmt, tempIndex++);
		tmptif.towerid = sqlite3_column_int(pStmt, tempIndex++);
		tmptif.lonlatalt.lat = sqlite3_column_double(pStmt, tempIndex++);
		tmptif.lonlatalt.lon = sqlite3_column_double(pStmt, tempIndex++);
		tmptif.lonlatalt.alt = sqlite3_column_double(pStmt, tempIndex++);
		double topalt = sqlite3_column_double(pStmt, tempIndex++);
		tmptif.lonlatalt.height = topalt - tmptif.lonlatalt.alt;

		ts.push_back(Tower(tmptif, nullptr, 0));
	}
	sqlite3_finalize(pStmt);

	//如果此区域未查找到任何塔数据，则直接返回。
	if (ts.size()<=0)
	{
		return ts;
	}

	//2、每个塔的端子数量
	strSql =
		"select count(*) "
		"from endp inner join ("
		"select towerIdx,lineid "
		"from tower "
		"where abs(tower.Lon-?)<? and abs(tower.Lat-?)<?"
		") as tmpTowers "
		"on tmpTowers.lineid==endp.lineid and tmpTowers.towerIdx==endp.towerIdx "
		"group by endp.lineid,endp.towerIdx "
		"order by endp.lineid,endp.towerIdx asc";

	pStmt = nullptr;
	sqlite3_prepare(_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return ts;
	}

	tempIndex = 1;
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[0]);
	sqlite3_bind_double(pStmt, tempIndex++, LonRange);
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[1]);
	sqlite3_bind_double(pStmt, tempIndex++, LatRange);

	std::vector<int> endpCounts;
	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		int tempIndex = 0;
		int endpCount = 0;
		endpCount = sqlite3_column_int(pStmt, tempIndex++);

		endpCounts.push_back(endpCount);
	}
	sqlite3_finalize(pStmt);


	//3、塔上端子的信息
	strSql =
		"select Lon,Lat,Alt "
		"from endp inner join ("
		"select towerIdx,lineid "
		"from tower "
		"where abs(tower.Lon-?)<? and abs(tower.Lat-?)<?"
		") as tmpTowers "
		"on (endp.towerIdx==tmpTowers.towerIdx and endp.lineid==tmpTowers.lineid) "
		"order by endp.lineid,endp.towerIdx,endp.idx asc";

	sqlite3_prepare(_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return ts;
	}

	tempIndex = 1;
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[0]);
	sqlite3_bind_double(pStmt, tempIndex++, LonRange);
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[1]);
	sqlite3_bind_double(pStmt, tempIndex++, LatRange);

	auto itT = ts.begin();
	auto itC = endpCounts.begin();
	int endcount = *itC;

	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		int tempIndex = 0;
		CVec3d EndLLA;

		if (endcount <= 0)
		{
			itT++;
			itC++;
			endcount = *itC;
		}
		endcount--;

		EndLLA._v[0] = sqlite3_column_double(pStmt, tempIndex++);
		EndLLA._v[1] = sqlite3_column_double(pStmt, tempIndex++);
		EndLLA._v[2] = sqlite3_column_double(pStmt, tempIndex++);
		itT->_endps.push_back(EndLLA);
	}

	sqlite3_finalize(pStmt);

	return ts;
}

TowerEndData HVTDataSql::GetTowerEndData(const CVec3d& centerLLA, const double& LonRange, const double& LatRange)
{
	TowerEndData ted;

	//1、塔的信息
	std::string strSql =
		"select id,lineid,towerIdx,Lat,Lon,Alt,Heigh "
		"from tower "
		"where abs(tower.Lon-?)<? and abs(tower.Lat-?)<? "
		"order by lineid,towerIdx asc";

	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return ted;
	}

	int tempIndex = 1;
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[0]);
	sqlite3_bind_double(pStmt, tempIndex++, LonRange);
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[1]);
	sqlite3_bind_double(pStmt, tempIndex++, LatRange);

	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		int tempIndex = 0;
		towerinfo tmptif;
		tmptif.towerIdx = sqlite3_column_int(pStmt, tempIndex++);
		tmptif.lineid = sqlite3_column_int(pStmt, tempIndex++);
		tmptif.towerid = sqlite3_column_int(pStmt, tempIndex++);
		tmptif.lonlatalt.lat = sqlite3_column_double(pStmt, tempIndex++);
		tmptif.lonlatalt.lon = sqlite3_column_double(pStmt, tempIndex++);
		tmptif.lonlatalt.alt = sqlite3_column_double(pStmt, tempIndex++);
		double topalt = sqlite3_column_double(pStmt, tempIndex++);
		tmptif.lonlatalt.height = topalt - tmptif.lonlatalt.alt;

		ted.tinfos.push_back(tmptif);
	}
	sqlite3_finalize(pStmt);

	//2、每个塔的端子数量
	strSql =
		"select count(*) "
		"from endp inner join ("
		"select towerIdx,lineid "
		"from tower "
		"where abs(tower.Lon-?)<? and abs(tower.Lat-?)<?"
		") as tmpTowers "
		"on tmpTowers.lineid==endp.lineid and tmpTowers.towerIdx==endp.towerIdx "
		"group by endp.lineid,endp.towerIdx "
		"order by endp.lineid,endp.towerIdx asc";

	pStmt = nullptr;
	sqlite3_prepare(_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		//return myTowers;
		return ted;
	}

	tempIndex = 1;
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[0]);
	sqlite3_bind_double(pStmt, tempIndex++, LonRange);
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[1]);
	sqlite3_bind_double(pStmt, tempIndex++, LatRange);

	std::vector<int> endpCounts;
	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		int tempIndex = 0;
		int endpCount = 0;
		endpCount = sqlite3_column_int(pStmt, tempIndex++);

		endpCounts.push_back(endpCount);
	}
	sqlite3_finalize(pStmt);


	//3、塔上端子的信息
	strSql =
		"select Lon,Lat,Alt "
		"from endp inner join ("
		"select towerIdx,lineid "
		"from tower "
		"where abs(tower.Lon-?)<? and abs(tower.Lat-?)<?"
		") as tmpTowers "
		"on (endp.towerIdx==tmpTowers.towerIdx and endp.lineid==tmpTowers.lineid) "
		"order by endp.lineid,endp.towerIdx,endp.idx asc";
	
	sqlite3_prepare(_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return ted;
	}

	tempIndex = 1;
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[0]);
	sqlite3_bind_double(pStmt, tempIndex++, LonRange);
	sqlite3_bind_double(pStmt, tempIndex++, centerLLA._v[1]);
	sqlite3_bind_double(pStmt, tempIndex++, LatRange);

	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		int tempIndex = 0;
		CVec3d EndLLA;

		EndLLA._v[0] = sqlite3_column_double(pStmt, tempIndex++);
		EndLLA._v[1] = sqlite3_column_double(pStmt, tempIndex++);
		EndLLA._v[2] = sqlite3_column_double(pStmt, tempIndex++);				

		ted.einfos.push_back(EndLLA);
	}

	sqlite3_finalize(pStmt);

	return ted;
}

HVTDataSql::~HVTDataSql()
{

}
