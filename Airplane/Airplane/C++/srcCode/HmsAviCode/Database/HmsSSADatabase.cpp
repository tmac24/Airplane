#include "HmsSSADatabase.h"
#include "../HmsStringUtils.h"
#include <sstream>

CHmsSSADatabase *CHmsSSADatabase::m_pInstance = nullptr;
CHmsSSADatabase::CHmsSSADatabase()
{
}

CHmsSSADatabase::~CHmsSSADatabase()
{
	Close();
}

CHmsSSADatabase* CHmsSSADatabase::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CHmsSSADatabase();
		m_pInstance->InitDatabase("navData/SSAInfo.db");
	}

	return m_pInstance;
}

void CHmsSSADatabase::GetMcduProgramInfo(std::string &airport, ROUTE_TYPE proType, std::map<std::string, McduProgramInfo> &mapInfo)
{
	std::stringstream sql;
	sql << "select AreaCode,AirportIdent,IcaoCode,SSAIdent,Data,TransData,RunwayData from ";
	if (proType == ROUTE_TYPE_DEP_SID)
	{
		sql << " SID ";
	}
	else if (proType == ROUTE_TYPE_ARR_STAR)
	{
		sql << " STAR ";
	}
	else
	{
		sql << " APP ";
	}
	sql << " where AirportIdent='" << airport << "' ";
	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);
	if (!pStmt)
	{
		return;
	}
	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		McduProgramInfo mcduFixSize;
		mcduFixSize.eRouteType = proType;

		char *pName = (char *)sqlite3_column_text(pStmt, 3);
		if (pName && strlen(pName) < 16)
		{
			strcpy(mcduFixSize.name, pName);
		}
		char *pData = (char *)sqlite3_column_blob(pStmt, 4);
		char *pTransData = (char *)sqlite3_column_blob(pStmt, 5);
		char *pRunwayData = (char *)sqlite3_column_blob(pStmt, 6);

		if (pData && pTransData && pRunwayData)
		{
			CharToMcduProgramInfo(pData, pTransData, pRunwayData, mcduFixSize);
		}		

		//进近程序从名字里提取跑道名称
		if (proType == ROUTE_TYPE_ARR_APPR)
		{
			std::string strName = pName;
			std::string strRunwayName;
			if (strName.size() >= 3)
			{
				strRunwayName.push_back(strName[1]);
				strRunwayName.push_back(strName[2]);
				if (strName.size() >= 4)
				{
					if (strName[3] == 'L' || strName[3] == 'R')
					{
						strRunwayName.push_back(strName[3]);
					}
				}
			}
			if (strRunwayName.size() > 0)
			{
				strRunwayName = "RW" + strRunwayName;
				mcduFixSize.mapRunway[strRunwayName].SetName(strRunwayName);
			}
		}

		mapInfo.insert(std::make_pair(pName, mcduFixSize));
	}
	ReleaseSqlObject(pStmt);
}

void CHmsSSADatabase::CharToMcduProgramInfo(char *pData, char *pTransData, char *pRunwayData, McduProgramInfo &mcduFixSize)
{
	if (pData && pTransData && pRunwayData)
	{
		{
			int pos = 0;
			memcpy((char *)&mcduFixSize.eRouteType, pData + pos, sizeof(mcduFixSize.eRouteType));
			pos += sizeof(mcduFixSize.eRouteType);
			memcpy(mcduFixSize.name, pData + pos, sizeof(mcduFixSize.name));
			pos += sizeof(mcduFixSize.name);

			int vecSize = 0;
			memcpy((char *)&vecSize, pData + pos, sizeof(vecSize));
			pos += sizeof(vecSize);
			mcduFixSize.vWayPoint.resize(vecSize);
			memcpy(mcduFixSize.vWayPoint.data(), pData + pos, vecSize * sizeof(McduWayCtrlPoint));
			pos += vecSize * sizeof(McduWayCtrlPoint);
		}
		{
			int pos = 0;
			int mapSize = 0;
			memcpy((char *)&mapSize, pTransData + pos, sizeof(mapSize));
			pos += sizeof(mapSize);
			//
			for (int i = 0; i < mapSize; ++i)
			{
				char name[16];
				memcpy(name, pTransData + pos, sizeof(name));
				name[15] = 0;
				pos += sizeof(name);

				int vecSize = 0;
				memcpy((char *)&vecSize, pTransData + pos, sizeof(vecSize));
				pos += sizeof(vecSize);

				strcpy(mcduFixSize.mapTrans[name].transName, name);
				mcduFixSize.mapTrans[name].vWayPoint.resize(vecSize);
				memcpy(mcduFixSize.mapTrans[name].vWayPoint.data(), pTransData + pos, vecSize * sizeof(McduWayCtrlPoint));
				pos += vecSize * sizeof(McduWayCtrlPoint);
			}
		}
		{
			int pos = 0;
			int mapSize = 0;
			memcpy((char *)&mapSize, pRunwayData + pos, sizeof(mapSize));
			pos += sizeof(mapSize);
			//
			for (int i = 0; i < mapSize; ++i)
			{
				char name[16];
				memcpy(name, pRunwayData + pos, sizeof(name));
				name[15] = 0;
				pos += sizeof(name);

				int vecSize = 0;
				memcpy((char *)&vecSize, pRunwayData + pos, sizeof(vecSize));
				pos += sizeof(vecSize);

				strcpy(mcduFixSize.mapRunway[name].runwayName, name);
				mcduFixSize.mapRunway[name].vWayPoint.resize(vecSize);
				memcpy(mcduFixSize.mapRunway[name].vWayPoint.data(), pRunwayData + pos, vecSize * sizeof(McduWayCtrlPoint));
				pos += vecSize * sizeof(McduWayCtrlPoint);
			}
		}
	}
}