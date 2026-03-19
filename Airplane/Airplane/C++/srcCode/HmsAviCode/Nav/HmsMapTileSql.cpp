#include "HmsMapTileSql.h"
#include "HmsStandardDef.h"
CHmsMapTileSql::CHmsMapTileSql()
: m_pDb(nullptr)
, m_pDbMap(nullptr)
, m_pStrDbPath(nullptr)
{
	char *pErrMsg = 0;
	int ret = 0;

	auto config = CHmsConfigXMLReader::GetSingleConfig();

    if (config)
    {
        config->GetMapConfigInfo(m_mapInfo);
        m_strDefaultDB = config->GetConfigInfo("FunctionConfig/NavMapDefaultWithType", "vfr");
    }
}

CHmsMapTileSql::CHmsMapTileSql(const CHmsMapTileSql&)
{

}


CHmsMapTileSql::~CHmsMapTileSql()
{
}

CHmsMapTileSql& CHmsMapTileSql::GetInstance()
{
	static CHmsMapTileSql sql;
	return sql;
}

CHmsMapTileSql & CHmsMapTileSql::GetInstanceBackup()
{
	static CHmsMapTileSql sql_backup;
	return sql_backup;
}

CHmsMapTileSql* CHmsMapTileSql::CreateInstance()
{
    return new CHmsMapTileSql;
}

bool CHmsMapTileSql::ChangeMapTypeName(std::string mapTypeName)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_pDbMap)
	{
		if (mapTypeName == m_pDbMap->strMapType)
		{
			return false;
		}
	}

	auto iter = m_mapInfo.find(mapTypeName);
	if (iter != m_mapInfo.end())
	{
		m_pDbMap = &iter->second;
	}
    else if (m_mapInfo.size() > 0)
    {
        iter = m_mapInfo.begin();
        m_pDbMap = &iter->second;
    }

	return true;
}

std::string CHmsMapTileSql::GetMapTypeName()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_pDbMap)
	{
		return m_pDbMap->strMapType;
	}
	else
	{
		return "";
	}
}

bool CHmsMapTileSql::GetImageFromDB(int nLayer, int nX, int nY, HmsAviPf::Image* image)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	bool bRet = false;

	auto pDbPath = GetDataBaseName(nLayer, nX, nY);
	if (pDbPath)
	{
		if (!m_pStrDbPath || (m_pStrDbPath != pDbPath && *m_pStrDbPath != *pDbPath))
		{
			if (!OpenDataBase(pDbPath))
			{
				return bRet;
			}
			m_pStrDbPath = pDbPath;
		}

		int id = nX << nLayer | nY;
		std::stringstream s;
		s << "select Tile from Layer" << nLayer << " where ID=" << id;
		sqlite3_stmt * pStmt = nullptr;
		sqlite3_prepare(m_pDb, s.str().c_str(), -1, &pStmt, nullptr);
		if (pStmt)
		{
			if (SQLITE_ROW == sqlite3_step(pStmt))
			{
				unsigned char * pData = (unsigned char *)sqlite3_column_blob(pStmt, 0);
				int dataLen = sqlite3_column_bytes(pStmt, 0);

				bRet = image->initWithImageData(pData, dataLen);
			}
			sqlite3_finalize(pStmt);
		}	
	}
	return bRet;
}

std::string * CHmsMapTileSql::GetDataBaseName(int nLayer, int nX, int nY)
{
	if (m_pDbMap)
	{
		auto iter = m_pDbMap->mapLayer.find(nLayer);
		if (iter != m_pDbMap->mapLayer.end())
		{
			if (iter->second.isSubdivision)
			{
				for (auto &stu : iter->second.vDbPath)
				{
					if (nX >= stu.xMin && nX <= stu.xMax && nY >= stu.yMin && nY <= stu.yMax)
					{
						return &stu.strDbPath;
					}
				}
			}
			else
			{
				return &iter->second.strDbPath;
			}
		}
	}
	return nullptr;
}

bool CHmsMapTileSql::OpenDataBase(const std::string *pDbPath)
{
	if (m_pDb)
	{
		sqlite3_close(m_pDb);
		m_pDb = nullptr;
	}
	int ret = sqlite3_open((*pDbPath).c_str(), &m_pDb);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "open earth tile fail：%s\n", sqlite3_errmsg(m_pDb));
		sqlite3_close(m_pDb);
		m_pDb = nullptr;

		return false;
	}
	return true;
}

int CHmsMapTileSql::DetectLayerEdge(int layer)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_pDbMap)
	{
		if (layer < m_pDbMap->layerMin)
		{
			layer = m_pDbMap->layerMin;
		}
		if (layer > m_pDbMap->layerMax)
		{
			layer = m_pDbMap->layerMax;
		}
	}
	return layer;
}

std::vector<MapTempItems> CHmsMapTileSql::GetMapItems()
{
    std::vector<MapTempItems> items;
    for (auto & c:m_mapInfo)
    {
        MapTempItems item;
        item.strMapType = c.second.strMapType;
        item.strNameDisplay = c.second.strDisplayName;
        items.push_back(item);
    }
    return items;
}


