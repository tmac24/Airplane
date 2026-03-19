#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <mutex>
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
#include "HmsConfigXMLReader.h"
#include "base/HmsImage.h"

using namespace HmsAviPf;

struct MapTempItems
{
    std::string strNameDisplay;
    std::string strMapType;
};

class CHmsMapTileSql
{
public:
	~CHmsMapTileSql();

	static CHmsMapTileSql& GetInstance();
	static CHmsMapTileSql & GetInstanceBackup();
    static CHmsMapTileSql* CreateInstance();

	bool ChangeMapTypeName(std::string mapTypeName);
	std::string GetMapTypeName();
    std::string GetDefaultMapDb(){ return m_strDefaultDB; }
	bool GetImageFromDB(int nLayer, int nX, int nY, HmsAviPf::Image* image);

	int DetectLayerEdge(int layer);

    std::vector<MapTempItems>   GetMapItems();

private:
	CHmsMapTileSql();
	CHmsMapTileSql(const CHmsMapTileSql&);

	bool OpenDataBase(const std::string *pDbPath);
	std::string * GetDataBaseName(int nLayer, int nX, int nY);

private:
	sqlite3 *m_pDb;
	const std::string *m_pStrDbPath;
	std::map<std::string, HmsDbMap> m_mapInfo;
	HmsDbMap *m_pDbMap;

	std::mutex m_mutex;
    std::string m_strDefaultDB;
};

