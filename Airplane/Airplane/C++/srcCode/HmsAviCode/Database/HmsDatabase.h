#pragma once
#include <string>
#include <functional>

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

struct  sqlite3;
struct sqlite3_stmt;

typedef std::function<int(int nCnt, char ** strArrayTitle, char ** strArrayContent)> HmsSqliteExecCallback;
typedef std::function<int(char ** strResult, int nRow, int nCol)> HmsSqliteTableCallback;

class CHmsDatabase
{
public:
	CHmsDatabase();
	~CHmsDatabase();

protected:
	bool InitDatabase(const std::string & strPath);

    std::string Hms_sqlite3_column_text(sqlite3_stmt* pStmt, int iCol);
    int Hms_sqlite3_column_int(sqlite3_stmt* pStmt, int iCol, int defaultValue);
    double Hms_sqlite3_column_double(sqlite3_stmt* pStmt, int iCol, double defaultValue);

	bool Exec(const std::string & strSql, const HmsSqliteExecCallback & callback);

	bool ExecWithTable(const std::string & strSql, const HmsSqliteTableCallback & callback);

	int ExecGetData(const std::string & strSql);

	static int FuncExec(void * pParam, int nCnt, char ** strArrayTitle, char ** strArrayContent);

	bool OpenDb(const std::string & strPath);

	void Close();

	sqlite3_stmt * CreateSqlObject(const std::string & strSql);

	void ReleaseSqlObject(sqlite3_stmt * pStmt);

	std::string			m_strDbPath;
	sqlite3 *			m_pSqlite;
};


