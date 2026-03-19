#include "HmsDatabase.h"
#include "base/HmsFileUtils.h"
#include "HmsLog.h"
USING_NS_HMS;

CHmsDatabase::CHmsDatabase()
: m_pSqlite(nullptr)
{

}


CHmsDatabase::~CHmsDatabase()
{
	Close();
}

std::string CHmsDatabase::Hms_sqlite3_column_text(sqlite3_stmt* pStmt, int iCol)
{
    auto ret = (char *)sqlite3_column_text(pStmt, iCol);
    if (!ret)
    {
        return std::string();
    }
    else
    {
        return std::string(ret);
    }
}

int CHmsDatabase::Hms_sqlite3_column_int(sqlite3_stmt* pStmt, int iCol, int defaultValue)
{
    if (sqlite3_column_bytes(pStmt, iCol) == 0)
    {
        return defaultValue;
    }
    else
    {
        return sqlite3_column_int(pStmt, iCol);
    }
}

double CHmsDatabase::Hms_sqlite3_column_double(sqlite3_stmt* pStmt, int iCol, double defaultValue)
{
    if (sqlite3_column_bytes(pStmt, iCol) == 0)
    {
        return defaultValue;
    }
    else
    {
        return sqlite3_column_double(pStmt, iCol);
    }
}

int CHmsDatabase::FuncExec(void * pParam, int nCnt, char ** strArrayTitle, char ** strArrayContent)
{
	int nRet = 0;

	if (pParam)
	{
		const HmsSqliteExecCallback & callback = *(HmsSqliteExecCallback*)pParam;
		if (callback)
		{
			nRet = callback(nCnt, strArrayTitle, strArrayContent);
		}
	}

	return nRet;
}

bool CHmsDatabase::InitDatabase(const std::string & strPath)
{
	auto path = CHmsFileUtils::getInstance()->fullPathForFilename(strPath);
	//HMSLOG("Database:%s", path.c_str());

	return OpenDb(path);
}

bool CHmsDatabase::Exec(const std::string & strSql, const HmsSqliteExecCallback & callback)
{
	if (m_pSqlite)
	{
		int nRet = sqlite3_exec(m_pSqlite, strSql.c_str(), FuncExec, (void*)&callback, nullptr);
		if (nRet == SQLITE_OK)
		{
			return true;
		}
		else
		{
			const char * strError = sqlite3_errmsg(m_pSqlite);
			HMSLOG("Database Exec Error:%s", strError);
		}
	}	
	return false;
}

bool CHmsDatabase::ExecWithTable(const std::string & strSql, const HmsSqliteTableCallback & callback)
{

	bool bRet = false;
	char ** re = nullptr;
	int r = 0, c = 0;

	if (m_pSqlite)
	{
		sqlite3_get_table(m_pSqlite, strSql.c_str(), &re, &r, &c, nullptr);

		if (re)
		{
			if (callback)
			{
				callback(re, r, c);
			}

			sqlite3_free_table(re);
			re = nullptr;
			bRet = true;
		}
	}	

	return bRet;
}

int CHmsDatabase::ExecGetData(const std::string & strSql)
{
	int nRet = 0;

	if (Exec(strSql, [&nRet](int nCnt, char ** strArrayContent, char ** strArrayTitle){
		if (nCnt == 1)
		{
			nRet = atoi(strArrayContent[0]);
		}
		return 0;
	}))
	{
		int c = 0;
	}

	return nRet;
}

bool CHmsDatabase::OpenDb(const std::string & strPath)
{
	if ((m_strDbPath == strPath) && m_pSqlite)
	{
		return true;
	}

	m_strDbPath = strPath;
	if (m_pSqlite)
	{
		sqlite3_close(m_pSqlite);
		m_pSqlite = nullptr;
	}

	int nResult = sqlite3_open(m_strDbPath.c_str(), &m_pSqlite);
	if (nResult != SQLITE_OK)
	{
		m_pSqlite = nullptr;
		return false;
	}
	return true;
}

void CHmsDatabase::Close()
{
	if (m_pSqlite)
	{
		sqlite3_close(m_pSqlite);
		m_pSqlite = nullptr;
	}
}

sqlite3_stmt * CHmsDatabase::CreateSqlObject(const std::string & strSql)
{
	sqlite3_stmt * pStmt = nullptr;
	if (m_pSqlite)
	{
		sqlite3_prepare_v2(m_pSqlite, strSql.c_str(), strSql.length(), &pStmt, nullptr);
	}	
	
	return pStmt;
}

void CHmsDatabase::ReleaseSqlObject(sqlite3_stmt * pStmt)
{
	if (pStmt)
	{
		sqlite3_finalize(pStmt);
	}
}
