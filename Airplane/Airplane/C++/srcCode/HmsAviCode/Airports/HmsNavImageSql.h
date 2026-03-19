#pragma once

#include <iostream>
#include <string>
#include <vector>
#ifdef _WIN32
#include "sqlite3/include/sqlite3.h"
#pragma comment(lib,"sqlite3.lib")  
#endif

#ifdef __vxworks
#include "sqlite3/include/sqlite3.h"
#endif

#ifdef __linux
#include "sqlite3.h"
#endif

#ifdef __APPLE__
#include "sqlite3.h"
#endif

#include "base/HmsFileUtils.h"
#include "HmsAirportsStuDef.h"

using namespace HmsAviPf;

class CHmsNavImageSql
{
public:
	~CHmsNavImageSql();

	static CHmsNavImageSql& GetInstance();

	//bNullToAll = false, icaoChar 为空，搜出来的结果为空
	//bNullToAll = true , icaoChar 为空，搜出来的结果为全部
	std::vector<std::string> GetAirportName(std::string icaoChar, bool isFavorite, bool bNullToAll = false);

	std::vector<StarSidStu> GetStarSidInfo(std::string icaoChar, bool isFavorite, int typeId);
    StarSidStu GetStarSidInfo(int id);
    std::vector<StarSidStu> GetFavoriteStarSidInfo();

	void ModProgramFavorite(const StarSidStu &stu);

	int GetFavoriteCount(const std::string &icaoChar);
	void SetAirportFavorite(const std::string &icaoChar, bool isFavorite);

    void AddFavoriteModifyCallback(std::function<void()> func);

private:
	CHmsNavImageSql();
	CHmsNavImageSql(const CHmsNavImageSql&);

	void GetRunwayInfo(StarSidStu &stu);

private:
	sqlite3 *m_pDb;
	sqlite3 *m_pJeppesenDb;

    std::vector<std::function<void()> > m_vecFavoriteModifyCalllback;
};

