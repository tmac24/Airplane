#pragma once

#include <vector>
#include <map>
#include "HmsDatabase.h"

#include "../Nav/RoutePlanData/mcduDef.h"

class CHmsSSADatabase : public CHmsDatabase
{
public:
	static CHmsSSADatabase* GetInstance();

	void GetMcduProgramInfo(std::string &airport, ROUTE_TYPE proType, std::map<std::string, McduProgramInfo> &mapInfo);

	void CharToMcduProgramInfo(char *pData, char *pTransData, char *pRunwayData, McduProgramInfo &mcduFixSize);
private:
	CHmsSSADatabase();
	~CHmsSSADatabase();

	static CHmsSSADatabase *m_pInstance;
};

