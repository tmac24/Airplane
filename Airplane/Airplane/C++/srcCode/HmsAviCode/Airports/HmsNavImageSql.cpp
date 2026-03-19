#include "HmsNavImageSql.h"


CHmsNavImageSql::CHmsNavImageSql()
{
	m_pDb = nullptr;
	m_pJeppesenDb = nullptr;

	char *pErrMsg = 0;
	int ret = 0;

	{
		std::string dbPath = CHmsFileUtils::getInstance()->fullPathForFilename("navData/NavImage.db");
		ret = sqlite3_open(dbPath.c_str(), &m_pDb);
		if (ret != SQLITE_OK)
		{
			fprintf(stderr, "open NavImage fail：%s\n", sqlite3_errmsg(m_pDb));
			sqlite3_close(m_pDb);
		}
	}
	{
// 		std::string dbPath = FileUtils::getInstance()->fullPathForFilename("navData/DataDb.db");
// 		ret = sqlite3_open(dbPath.c_str(), &m_pJeppesenDb);
// 		if (ret != SQLITE_OK)
// 		{
// 			fprintf(stderr, "open NavImage fail：%s\n", sqlite3_errmsg(m_pJeppesenDb));
// 			sqlite3_close(m_pJeppesenDb);
// 		}
	}
	printf("open NavImage success\n");
}

CHmsNavImageSql::CHmsNavImageSql(const CHmsNavImageSql&)
{

}


CHmsNavImageSql::~CHmsNavImageSql()
{
}

CHmsNavImageSql& CHmsNavImageSql::GetInstance()
{
	static CHmsNavImageSql sql;
	return sql;
}

std::vector<std::string> CHmsNavImageSql::GetAirportName(std::string icaoChar, bool isFavorite, bool bNullToAll /*= false*/)
{
	std::vector<std::string> vecRet;

	std::string strSql;
	if (isFavorite)
	{
		strSql = "select IcaoCode, AirportName from Airport where IsFavorite=? and IcaoCode like ? order by IcaoCode";
	}
	else
	{
		strSql = "select IcaoCode, AirportName from Airport where IcaoCode like ? order by IcaoCode";
	}
	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return vecRet;
	}

	int tempIndex = 1;
	if (isFavorite)
	{
		sqlite3_bind_int(pStmt, tempIndex++, isFavorite);
	}
	
	if (!icaoChar.empty() || bNullToAll)
	{
		icaoChar += "%";
	}
	sqlite3_bind_text(pStmt, tempIndex++, icaoChar.c_str(), -1, SQLITE_TRANSIENT);

	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		auto icaoCode = (char*)sqlite3_column_text(pStmt, 0);
		auto airportName = (char*)sqlite3_column_text(pStmt, 1);

		std::string str = icaoCode;
		str += ": ";
		str += airportName;
		vecRet.push_back(str);
	}

	sqlite3_finalize(pStmt);

	return vecRet;
}

std::vector<StarSidStu> CHmsNavImageSql::GetStarSidInfo(std::string icaoChar, bool isFavorite, int typeId)
{
	std::vector<StarSidStu> vecRet;

	std::string strSql;
	if (isFavorite)
	{
		strSql = "select Id,Name,IcaoCode,IsFavorite,ImagePath,DrawPlane"
			",FirstAreaType,FirstAreaInfo,SecondAreaType,SecondAreaInfo,RunwayIdent,RunwayBearing,RunwayLon,RunwayLat"
			" from Program where IcaoCode=? and IsFavorite=? and TypeId=?";
	}
	else
	{
		strSql = "select Id,Name,IcaoCode,IsFavorite,ImagePath,DrawPlane"
			",FirstAreaType,FirstAreaInfo,SecondAreaType,SecondAreaInfo,RunwayIdent,RunwayBearing,RunwayLon,RunwayLat"
			" from Program where IcaoCode=? and TypeId=?";
	}
	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return vecRet;
	}

	int tempIndex = 1;
	sqlite3_bind_text(pStmt, tempIndex++, icaoChar.c_str(), -1, SQLITE_TRANSIENT);
	if (isFavorite)
	{
		sqlite3_bind_int(pStmt, tempIndex++, isFavorite);
	}
	sqlite3_bind_int(pStmt, tempIndex++, typeId);

	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		int tempIndex = 0;
		StarSidStu stu;
		stu.id = sqlite3_column_int(pStmt, tempIndex++);
		stu.name = (char*)sqlite3_column_text(pStmt, tempIndex++);
		stu.icaoCode = (char*)sqlite3_column_text(pStmt, tempIndex++);
		stu.isFavorite = sqlite3_column_int(pStmt, tempIndex++) ? true : false;
		stu.imagePath = (char*)sqlite3_column_text(pStmt, tempIndex++);
		stu.bDrawplane = sqlite3_column_int(pStmt, tempIndex++) ? true : false;

		stu.firstAreaType = (NavImageAreaType)sqlite3_column_int(pStmt, tempIndex++);
		if (stu.firstAreaType != NAV_NONE)
		{
			stu.firstAreaInfo = (char*)sqlite3_column_text(pStmt, tempIndex++);
		}
		stu.secondAreaType = (NavImageAreaType)sqlite3_column_int(pStmt, tempIndex++);
		if (stu.secondAreaType != NAV_NONE)
		{
			stu.secondAreaInfo = (char*)sqlite3_column_text(pStmt, tempIndex++);

			stu.runwayIdent = (char*)sqlite3_column_text(pStmt, tempIndex++);
			stu.runwayBearing = sqlite3_column_double(pStmt, tempIndex++);
			stu.runwayLon = sqlite3_column_double(pStmt, tempIndex++);
			stu.runwayLat = sqlite3_column_double(pStmt, tempIndex++);
		}
		vecRet.push_back(stu);
	}

	sqlite3_finalize(pStmt);

	return vecRet;
}

StarSidStu CHmsNavImageSql::GetStarSidInfo(int id)
{
    StarSidStu stu;

    std::string strSql;
    strSql = "select Id,Name,IcaoCode,IsFavorite,ImagePath,DrawPlane"
        ",FirstAreaType,FirstAreaInfo,SecondAreaType,SecondAreaInfo,RunwayIdent,RunwayBearing,RunwayLon,RunwayLat"
        " from Program where Id=?";

    sqlite3_stmt * pStmt = nullptr;
    sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
    if (!pStmt)
    {
        return stu;
    }

    int tempIndex = 1;
    sqlite3_bind_int(pStmt, tempIndex++, id);

    while (SQLITE_ROW == sqlite3_step(pStmt))
    {
        int tempIndex = 0;

        stu.id = sqlite3_column_int(pStmt, tempIndex++);
        stu.name = (char*)sqlite3_column_text(pStmt, tempIndex++);
        stu.icaoCode = (char*)sqlite3_column_text(pStmt, tempIndex++);
		stu.isFavorite = sqlite3_column_int(pStmt, tempIndex++) ? true : false;
        stu.imagePath = (char*)sqlite3_column_text(pStmt, tempIndex++);
		stu.bDrawplane = sqlite3_column_int(pStmt, tempIndex++) ? true : false;

        stu.firstAreaType = (NavImageAreaType)sqlite3_column_int(pStmt, tempIndex++);
        if (stu.firstAreaType != NAV_NONE)
        {
            stu.firstAreaInfo = (char*)sqlite3_column_text(pStmt, tempIndex++);
        }
        stu.secondAreaType = (NavImageAreaType)sqlite3_column_int(pStmt, tempIndex++);
        if (stu.secondAreaType != NAV_NONE)
        {
            stu.secondAreaInfo = (char*)sqlite3_column_text(pStmt, tempIndex++);

            stu.runwayIdent = (char*)sqlite3_column_text(pStmt, tempIndex++);
            stu.runwayBearing = sqlite3_column_double(pStmt, tempIndex++);
            stu.runwayLon = sqlite3_column_double(pStmt, tempIndex++);
            stu.runwayLat = sqlite3_column_double(pStmt, tempIndex++);
        }
        break;
    }

    sqlite3_finalize(pStmt);

    return stu;
}

std::vector<StarSidStu> CHmsNavImageSql::GetFavoriteStarSidInfo()
{
    std::vector<StarSidStu> vecRet;

    std::string strSql;
    strSql = "select Id,Name,IcaoCode,IsFavorite,ImagePath,DrawPlane"
        ",FirstAreaType,FirstAreaInfo,SecondAreaType,SecondAreaInfo,RunwayIdent,RunwayBearing,RunwayLon,RunwayLat"
        " from Program where IsFavorite=1";
    sqlite3_stmt * pStmt = nullptr;
    sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
    if (!pStmt)
    {
        return vecRet;
    }

    while (SQLITE_ROW == sqlite3_step(pStmt))
    {
        int tempIndex = 0;
        StarSidStu stu;
        stu.id = sqlite3_column_int(pStmt, tempIndex++);
        stu.name = (char*)sqlite3_column_text(pStmt, tempIndex++);
        stu.icaoCode = (char*)sqlite3_column_text(pStmt, tempIndex++);
		stu.isFavorite = sqlite3_column_int(pStmt, tempIndex++) ? true : false;
        stu.imagePath = (char*)sqlite3_column_text(pStmt, tempIndex++);
		stu.bDrawplane = sqlite3_column_int(pStmt, tempIndex++) ? true : false;

        stu.firstAreaType = (NavImageAreaType)sqlite3_column_int(pStmt, tempIndex++);
        if (stu.firstAreaType != NAV_NONE)
        {
            stu.firstAreaInfo = (char*)sqlite3_column_text(pStmt, tempIndex++);
        }
        stu.secondAreaType = (NavImageAreaType)sqlite3_column_int(pStmt, tempIndex++);
        if (stu.secondAreaType != NAV_NONE)
        {
            stu.secondAreaInfo = (char*)sqlite3_column_text(pStmt, tempIndex++);

            stu.runwayIdent = (char*)sqlite3_column_text(pStmt, tempIndex++);
            stu.runwayBearing = sqlite3_column_double(pStmt, tempIndex++);
            stu.runwayLon = sqlite3_column_double(pStmt, tempIndex++);
            stu.runwayLat = sqlite3_column_double(pStmt, tempIndex++);
        }
        vecRet.push_back(stu);
    }

    sqlite3_finalize(pStmt);

    return vecRet;
}

void CHmsNavImageSql::ModProgramFavorite(const StarSidStu &stu)
{
	std::string strSql;
	strSql = "update Program set IsFavorite=? where Id=?";
	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return;
	}
	sqlite3_bind_int(pStmt, 1, stu.isFavorite);
	sqlite3_bind_int(pStmt, 2, stu.id);
	
	sqlite3_step(pStmt);

	sqlite3_finalize(pStmt);

	auto count = GetFavoriteCount(stu.icaoCode);
	SetAirportFavorite(stu.icaoCode, (count ? true : false));

    for (auto &c : m_vecFavoriteModifyCalllback)
    {
        if (c)
        {
            c();
        }
    }
}

void CHmsNavImageSql::GetRunwayInfo(StarSidStu &stu)
{
// 	std::string strSql;
// 	strSql = "select Bearing,Longitude,Latitude from Runway where AirportIdent=? and RunwayIdent=?";
// 	sqlite3_stmt * pStmt = nullptr;
// 	sqlite3_prepare(m_pJeppesenDb, strSql.c_str(), -1, &pStmt, nullptr);
// 	if (!pStmt)
// 	{
// 		return;
// 	}
// 	int tempIndex = 1;
// 	sqlite3_bind_text(pStmt, tempIndex++, stu.icaoCode.c_str(), -1, SQLITE_TRANSIENT);
// 	sqlite3_bind_text(pStmt, tempIndex++, stu.runway.c_str(), -1, SQLITE_TRANSIENT);
// 
// 	while (SQLITE_ROW == sqlite3_step(pStmt))
// 	{
// 		int tempIndex = 0;
// 		stu.runway_bearing = sqlite3_column_double(pStmt, tempIndex++);
// 		stu.runway_lon = sqlite3_column_double(pStmt, tempIndex++);
// 		stu.runway_lat = sqlite3_column_double(pStmt, tempIndex++);
// 	}
// 	sqlite3_finalize(pStmt);
// 	return;
}

int CHmsNavImageSql::GetFavoriteCount(const std::string &icaoChar)
{
	int ret = -1;
	std::string strSql;
	strSql = "select count(Id) from Program where IcaoCode=? and IsFavorite=1";
	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return ret;
	}
	sqlite3_bind_text(pStmt, 1, icaoChar.c_str(), -1, SQLITE_TRANSIENT);
	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		ret = sqlite3_column_int(pStmt, 0);
	}
	sqlite3_finalize(pStmt);
	return ret;
}

void CHmsNavImageSql::SetAirportFavorite(const std::string &icaoChar, bool isFavorite)
{
	std::string strSql;
	strSql = "update Airport set IsFavorite=? where IcaoCode=?";
	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return;
	}
	sqlite3_bind_int(pStmt, 1, isFavorite);
	sqlite3_bind_text(pStmt, 2, icaoChar.c_str(), -1, SQLITE_TRANSIENT);

	sqlite3_step(pStmt);

	sqlite3_finalize(pStmt);
}

void CHmsNavImageSql::AddFavoriteModifyCallback(std::function<void()> func)
{
    m_vecFavoriteModifyCalllback.push_back(func);
}
