#include "HmsHistoryRouteDatabase.h"
#include "../HmsStringUtils.h"
#include <sstream>

CHmsHistoryRouteDatabase *CHmsHistoryRouteDatabase::m_pInstance = nullptr;
CHmsHistoryRouteDatabase::CHmsHistoryRouteDatabase()
{
}

CHmsHistoryRouteDatabase::~CHmsHistoryRouteDatabase()
{
	Close();
}

CHmsHistoryRouteDatabase* CHmsHistoryRouteDatabase::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CHmsHistoryRouteDatabase();
		m_pInstance->InitDatabase("navData/HistoryRoute.db");
	}

	return m_pInstance;
}

void CHmsHistoryRouteDatabase::SaveHistoryRoute(const HistoryRouteStu &stu)
{
	std::string strSql;
	strSql = "insert into RouteData (Name, Data) values (?,?)";
	sqlite3_stmt * pStmt = CreateSqlObject(strSql);
	if (!pStmt)
	{
		return;
	}
	sqlite3_bind_text(pStmt, 1, stu.name.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_blob(pStmt, 2, stu.vecPoints.data(), stu.vecPoints.size() * sizeof(HistoryRouteOnePointStu), nullptr);
	sqlite3_step(pStmt);

	ReleaseSqlObject(pStmt);
}

void CHmsHistoryRouteDatabase::GetHistoryRoute(std::vector<HistoryRouteStu> &vecArray)
{
	std::string strSql;
	strSql = "select Id,Name,Data from RouteData order by Id desc";
	sqlite3_stmt * pStmt = CreateSqlObject(strSql);
	if (!pStmt)
	{
		return;
	}
	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		std::string name = (char *)sqlite3_column_text(pStmt, 1);

		unsigned char * pData = (unsigned char *)sqlite3_column_blob(pStmt, 2);
		int dataLen = sqlite3_column_bytes(pStmt, 2);

		if (dataLen >= sizeof(HistoryRouteOnePointStu))
		{
			HistoryRouteStu stu;
			stu.name = name;
			stu.vecPoints.resize(dataLen / sizeof(HistoryRouteOnePointStu));
			memcpy(stu.vecPoints.data(), pData, dataLen);

			stu.vecPoints[0].idInDb = sqlite3_column_int(pStmt, 0);
			stu.id = sqlite3_column_int(pStmt, 0);

			vecArray.push_back(stu);
		}
	}
	ReleaseSqlObject(pStmt);
}

void CHmsHistoryRouteDatabase::DeleteHistoryRoute(int id)
{
	std::string strSql;
	strSql = "delete from RouteData where Id=?";
	sqlite3_stmt * pStmt = CreateSqlObject(strSql);
	if (!pStmt)
	{
		return;
	}
	sqlite3_bind_int(pStmt, 1, id);
	sqlite3_step(pStmt);

	ReleaseSqlObject(pStmt);
}

void CHmsHistoryRouteDatabase::SaveUserWpt(const UserWptStu &stu)
{
	std::string strSql;
	strSql = "insert into UserWpt (Name, Lon, Lat) values (?,?,?)";
	sqlite3_stmt * pStmt = CreateSqlObject(strSql);
	if (!pStmt)
	{
		return;
	}
	sqlite3_bind_text(pStmt, 1, stu.name.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_double(pStmt, 2, stu.lon);
	sqlite3_bind_double(pStmt, 3, stu.lat);
	sqlite3_step(pStmt);

	ReleaseSqlObject(pStmt);
}

void CHmsHistoryRouteDatabase::DeleteUserWpt(int id)
{
	std::string strSql;
	strSql = "delete from UserWpt where Id=?";
	sqlite3_stmt * pStmt = CreateSqlObject(strSql);
	if (!pStmt)
	{
		return;
	}
	sqlite3_bind_int(pStmt, 1, id);
	sqlite3_step(pStmt);

	ReleaseSqlObject(pStmt);
}

void CHmsHistoryRouteDatabase::GetUserWpt(std::vector<UserWptStu> &vecUsetWpt)
{
	std::string strSql;
	strSql = "select Id,Name,Lon,Lat from UserWpt order by Id desc";
	sqlite3_stmt * pStmt = CreateSqlObject(strSql);
	if (!pStmt)
	{
		return;
	}
	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		UserWptStu stu;
		stu.idInDb = sqlite3_column_int(pStmt, 0);
		stu.name = (char *)sqlite3_column_text(pStmt, 1);
		stu.lon = sqlite3_column_double(pStmt, 2);
		stu.lat = sqlite3_column_double(pStmt, 3);

		vecUsetWpt.push_back(stu);
	}
	ReleaseSqlObject(pStmt);
}

void CHmsHistoryRouteDatabase::GetFuzzyIdent(std::string ident, std::vector<std::string> &vecRet)
{
    std::stringstream sql;
    sql << "SELECT Name from UserWpt ";
    sql << " where Name like '" << ident << "%' ";

    auto strSql = sql.str();
    auto strUSql = a2u(strSql.c_str());
    auto pStmt = CreateSqlObject(strUSql);

    if (pStmt)
    {
        while (SQLITE_ROW == sqlite3_step(pStmt))
        {
            std::string str1 = Hms_sqlite3_column_text(pStmt, 0);
            vecRet.push_back(str1);
        }
        ReleaseSqlObject(pStmt);
    }
}

bool CHmsHistoryRouteDatabase::GetUserWptLonLatByIdent(float &lon, float &lat, std::string ident)
{
    std::stringstream sql;
    sql << "SELECT Lon, Lat from UserWpt ";
    sql << " where Name='" << ident << "' ";

    auto strSql = sql.str();
    auto strUSql = a2u(strSql.c_str());
    auto pStmt = CreateSqlObject(strUSql);

    bool bRet = false;
    if (pStmt)
    {
        while (SQLITE_ROW == sqlite3_step(pStmt))
        {
            lon = Hms_sqlite3_column_double(pStmt, 0, 0);
            lat = Hms_sqlite3_column_double(pStmt, 1, 0);

            bRet = true;
        }
        ReleaseSqlObject(pStmt);
    }
    return bRet;
}

void CHmsHistoryRouteDatabase::SaveAirspace(const std::vector<AirspaceEditStu> &vecStu)
{
	DeleteAirspace();

	std::string strSql;
	strSql = "insert into AirspaceData (Name, Lon, Lat) values (?,?,?)";

	char *pErrMsg = nullptr;
	sqlite3_exec(m_pSqlite, "begin;", NULL, 0, &pErrMsg);

	for (int i = 0; i < (int)vecStu.size(); ++i)
	{
		sqlite3_stmt * pStmt = CreateSqlObject(strSql);
		if (!pStmt)
		{
			break;
		}
		const AirspaceEditStu &stu = vecStu[i];
		sqlite3_bind_text(pStmt, 1, stu.name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_double(pStmt, 2, stu.lon);
		sqlite3_bind_double(pStmt, 3, stu.lat);
		sqlite3_step(pStmt);

		ReleaseSqlObject(pStmt);
	}
	sqlite3_exec(m_pSqlite, "commit;", NULL, 0, 0);
}

void CHmsHistoryRouteDatabase::GetAirspace(std::vector<AirspaceEditStu> &vecStu)
{
	std::string strSql;
	strSql = "select Id,Name,Lon,Lat from AirspaceData order by Id";
	sqlite3_stmt * pStmt = CreateSqlObject(strSql);
	if (!pStmt)
	{
		return;
	}
	while (SQLITE_ROW == sqlite3_step(pStmt))
	{
		AirspaceEditStu stu;
		stu.idInDb = sqlite3_column_int(pStmt, 0);
		stu.name = (char *)sqlite3_column_text(pStmt, 1);
		stu.lon = sqlite3_column_double(pStmt, 2);
		stu.lat = sqlite3_column_double(pStmt, 3);

		vecStu.push_back(stu);
	}
	ReleaseSqlObject(pStmt);
}

void CHmsHistoryRouteDatabase::DeleteAirspace()
{
	std::string strSql;
	strSql = "delete from AirspaceData";
	sqlite3_stmt * pStmt = CreateSqlObject(strSql);
	if (!pStmt)
	{
		return;
	}
	sqlite3_step(pStmt);

	ReleaseSqlObject(pStmt);
}
