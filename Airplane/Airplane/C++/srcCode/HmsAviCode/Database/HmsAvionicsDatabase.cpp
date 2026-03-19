#include "HmsAvionicsDatabase.h"
#include "../HmsStringUtils.h"
#include <sstream>
#include <string.h>

CHmsAvionicsDatabase* CHmsAvionicsDatabase::m_pInstance = nullptr;
CHmsAvionicsDatabase::CHmsAvionicsDatabase()
{

}


CHmsAvionicsDatabase::~CHmsAvionicsDatabase()
{
}

CHmsAvionicsDatabase* CHmsAvionicsDatabase::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CHmsAvionicsDatabase();
		m_pInstance->InitDatabase("ProgramData/avionics.db");
	}

	return m_pInstance;
}

bool CHmsAvionicsDatabase::GetFrequencyData(std::vector<T_Frequency> & vDatas, int category, int recordType, int limit)
{
	std::stringstream s;

	s << "SELECT [rowid],[Frequency] FROM [Frequency] WHERE RecordType = ";
	s << recordType << " AND Category = " << category << " ORDER BY rowid DESC" << " LIMIT " << limit;

	auto strSql = s.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);
	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 2)
		{
			return false;
		}

		T_Frequency data;
		data.recordType = recordType;
		data.category = category;

		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			data.rowId = sqlite3_column_int(pStmt, 0);
			data.frequency = sqlite3_column_double(pStmt, 1);
			vDatas.push_back(data);
		}
		ReleaseSqlObject(pStmt);

		return true;
	}

	return false;
}

void CHmsAvionicsDatabase::SaveFrequency(T_Frequency data)
{
	sqlite3_exec(m_pSqlite,"begin;",0,0,0);  
	sqlite3_stmt *stmt;  
	const char* sql = "INSERT INTO [Frequency] (Frequency, Category, RecordType) VALUES (?,?,?)";  
	sqlite3_prepare_v2(m_pSqlite,sql,strlen(sql),&stmt,0);  
	  
	sqlite3_reset(stmt);  
	sqlite3_bind_double(stmt,1,data.frequency);  
	sqlite3_bind_int(stmt,2,data.category);  
	sqlite3_bind_int(stmt,3,data.recordType);

	sqlite3_step(stmt);  
	sqlite3_finalize(stmt); 

	sqlite3_exec(m_pSqlite,"commit;",0,0,0);
}

void CHmsAvionicsDatabase::DeleteFrequencyByFrequency(float fFrequency, int category, int recordType)
{
	std::stringstream sql;
	sql << "DELETE FROM [Frequency] WHERE Frequency = " << fFrequency;
	sql << " AND Category = " << category << " AND RecordType = " << recordType;

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	this->Exec(strUSql, nullptr);
}

std::string CHmsAvionicsDatabase::GetFlightId()
{
	std::string flightId = "";

	std::string strSql = "SELECT [Code] FROM [FlightId]";
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);
	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			char * tempText = (char*)sqlite3_column_text(pStmt, 0);
			if (tempText)
			{
				flightId = tempText;
			}
			break;
		}
		ReleaseSqlObject(pStmt);
	}

	return flightId;
}

void CHmsAvionicsDatabase::UpdateFlightId(std::string newFlightId)
{
	std::stringstream sql;
	sql << "UPDATE FlightId SET Code = '" << newFlightId << "' WHERE rowid = 1";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	this->Exec(strUSql, nullptr);
}

