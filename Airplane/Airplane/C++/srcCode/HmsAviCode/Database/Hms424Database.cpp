#include "Hms424Database.h"
#include "../HmsStringUtils.h"
#include <sstream>

CHms424Database *CHms424Database::m_pInstance = nullptr;
CHms424Database::CHms424Database()
{
}

CHms424Database::~CHms424Database()
{
	Close();
}

CHms424Database* CHms424Database::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CHms424Database();
		m_pInstance->InitDatabase("navData/DataDb.db");
	}

	return m_pInstance;
}

bool CHms424Database::GetAirportCommunicationData(std::vector<T_AirportCommunication> &vAirportComm, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon)
{
	std::stringstream sql;
	sql << "SELECT rowid, Id, AreaCode, Ident, IcaoCode, CommType, Frequency, Latitude, Longitude, MagneticVar, CommName FROM Airport_Communications WHERE ";
	sql << "Longitude >= " << minLon << " and Longitude <= " << maxLon << " and Latitude >= " << minLat << " and Latitude <= " << maxLat;
	sql << " ORDER BY Ident";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 11)
		{
			return false;
		}

		T_AirportCommunication data;
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			data.rowid = sqlite3_column_int(pStmt, 0);
			data.Id = sqlite3_column_int(pStmt, 1);

			char * tempText = (char *)sqlite3_column_text(pStmt, 2);
			if (tempText)
			{
				data.AreaCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 3);
			if (tempText)
			{
				data.Ident = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 4);
			if (tempText)
			{
				data.IcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 5);
			if (tempText)
			{
				data.CommType = tempText;
			}

			data.Frequency = sqlite3_column_double(pStmt, 6);
			data.Latitude = sqlite3_column_double(pStmt, 7);
			data.Longitude = sqlite3_column_double(pStmt, 8);
			data.MagneticVar = sqlite3_column_double(pStmt, 9);


			tempText = (char *)sqlite3_column_text(pStmt, 10);
			if (tempText)
			{
				data.CommName = tempText;
			}

			vAirportComm.push_back(data);
		}
		ReleaseSqlObject(pStmt);

		return true;
	}

	return false;
}

bool CHms424Database::GetNDBNavaidData(std::vector<T_NDBNavaid> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon)
{
	std::stringstream sql;
	sql << "SELECT rowid, Id, AreaCode, AirportIdent, IcaoCode, NDBIdent, NDBIcaoCode, NDBName, chType, DatumCode, NDBFrequency, Latitude, Longitude, MagneticVar, FacilityElevation, FIRIdent, UIRIdent ";
	sql << "FROM NDB_Navaid WHERE ";
	sql << "Longitude >= " << minLon << " and Longitude <= " << maxLon << " and Latitude >= " << minLat << " and Latitude <= " << maxLat;

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 17)
		{
			return false;
		}

		T_NDBNavaid data;
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			data.rowid = sqlite3_column_int(pStmt, 0);
			data.Id = sqlite3_column_int(pStmt, 1);

			char * tempText = (char *)sqlite3_column_text(pStmt, 2);
			if (tempText)
			{
				data.AreaCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 3);
			if (tempText)
			{
				data.AirportIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 4);
			if (tempText)
			{
				data.IcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 5);
			if (tempText)
			{
				data.NDBIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 6);
			if (tempText)
			{
				data.NDBIcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 7);
			if (tempText)
			{
				data.NDBName = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 8);
			if (tempText)
			{
				data.chType = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 9);
			if (tempText)
			{
				data.DatumCode = tempText;
			}

			data.NDBFrequency = sqlite3_column_double(pStmt, 10);
			data.Latitude = sqlite3_column_double(pStmt, 11);
			data.Longitude = sqlite3_column_double(pStmt, 12);
			data.MagneticVar = sqlite3_column_double(pStmt, 13);
			data.FacilityElevation = sqlite3_column_int(pStmt, 14);

			tempText = (char *)sqlite3_column_text(pStmt, 15);
			if (tempText)
			{
				data.FIRIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 16);
			if (tempText)
			{
				data.UIRIdent = tempText;
			}

			vData.push_back(data);
		}
		ReleaseSqlObject(pStmt);

		return true;
	}

	return false;
}

bool CHms424Database::GetTerminalNDBNavaid(std::vector<T_TerminalNDBNavaid> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon)
{
	std::stringstream sql;
	sql << "SELECT rowid, Id, AreaCode, AirportIdent, IcaoCode, NDBIdent, NDBIcaoCode, NDBName, chType, DatumCode, NDBFrequency, Latitude, Longitude, MagneticVar, FacilityElevation, FIRIdent, UIRIdent ";
	sql << "FROM Terminal_NDB_Navaid WHERE ";
	sql << "Longitude >= " << minLon << " and Longitude <= " << maxLon << " and Latitude >= " << minLat << " and Latitude <= " << maxLat;

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 17)
		{
			return false;
		}

		T_TerminalNDBNavaid data;
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			data.rowid = sqlite3_column_int(pStmt, 0);
			data.Id = sqlite3_column_int(pStmt, 1);

			char * tempText = (char *)sqlite3_column_text(pStmt, 2);
			if (tempText)
			{
				data.AreaCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 3);
			if (tempText)
			{
				data.AirportIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 4);
			if (tempText)
			{
				data.IcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 5);
			if (tempText)
			{
				data.NDBIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 6);
			if (tempText)
			{
				data.NDBIcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 7);
			if (tempText)
			{
				data.NDBName = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 8);
			if (tempText)
			{
				data.chType = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 9);
			if (tempText)
			{
				data.DatumCode = tempText;
			}

			data.NDBFrequency = sqlite3_column_double(pStmt, 10);
			data.Latitude = sqlite3_column_double(pStmt, 11);
			data.Longitude = sqlite3_column_double(pStmt, 12);
			data.MagneticVar = sqlite3_column_double(pStmt, 13);
			data.FacilityElevation = sqlite3_column_int(pStmt, 14);

			tempText = (char *)sqlite3_column_text(pStmt, 15);
			if (tempText)
			{
				data.FIRIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 16);
			if (tempText)
			{
				data.UIRIdent = tempText;
			}

			vData.push_back(data);
		}
		ReleaseSqlObject(pStmt);

		return true;
	}

	return false;
}

bool CHms424Database::GetVHFNavaidData(std::vector<T_VHFNavaid> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon)
{
	std::stringstream sql;
	sql << "SELECT rowid, Id, AreaCode, AirportIdent, IcaoCode, VorIdent, VorIcaoCode, VorFrequency, Latitude, Longitude, chType, DMEIdent, StationDeclination, DMEElevation, FigureMerit, DMEBias, FreProtect, DatumCode, VorName, MagneticVar, FacilityElevation ";
	sql << "FROM VHF_Navaid WHERE ";
	sql << "Longitude >= " << minLon << " and Longitude <= " << maxLon << " and Latitude >= " << minLat << " and Latitude <= " << maxLat;

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 21)
		{
			return false;
		}

		T_VHFNavaid data;
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			data.rowid = sqlite3_column_int(pStmt, 0);
			data.Id = sqlite3_column_int(pStmt, 1);

			char * tempText = (char *)sqlite3_column_text(pStmt, 2);
			if (tempText)
			{
				data.AreaCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 3);
			if (tempText)
			{
				data.AirportIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 4);
			if (tempText)
			{
				data.IcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 5);
			if (tempText)
			{
				data.VorIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 6);
			if (tempText)
			{
				data.VorIcaoCode = tempText;
			}

			data.VorFrequency = sqlite3_column_double(pStmt, 7);
			data.Latitude = sqlite3_column_double(pStmt, 8);
			data.Longitude = sqlite3_column_double(pStmt, 9);
			tempText = (char *)sqlite3_column_text(pStmt, 10);
			if (tempText)
			{
				data.chType = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 11);
			if (tempText)
			{
				data.DMEIdent = tempText;
			}

			data.StationDeclination = sqlite3_column_double(pStmt, 12);
			data.DMEElevation = sqlite3_column_int(pStmt, 13);

			tempText = (char *)sqlite3_column_text(pStmt, 14);
			if (tempText)
			{
				data.FigureMerit = tempText;
			}

			data.DMEBias = sqlite3_column_double(pStmt, 15);
			data.FreProtect = sqlite3_column_int(pStmt, 16);

			//float MagneticVar;
			//int FacilityElevation;

			tempText = (char *)sqlite3_column_text(pStmt, 17);
			if (tempText)
			{
				data.DatumCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 18);
			if (tempText)
			{
				data.VorName = tempText;
			}

			data.MagneticVar = sqlite3_column_double(pStmt, 19);
			data.FacilityElevation = sqlite3_column_int(pStmt, 20);

			vData.push_back(data);
		}
		ReleaseSqlObject(pStmt);

		return true;
	}

	return false;
}


bool CHms424Database::GetAirportData(std::vector<T_Airport> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon)
{
	std::stringstream sql;
	sql << "SELECT AreaCode, AirportIdent, Latitude, Longitude, AirportName ";
	sql << "FROM Airport WHERE ";
	sql << "Longitude >= " << minLon << " and Longitude <= " << maxLon << " and Latitude >= " << minLat << " and Latitude <= " << maxLat;

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 5)
		{
			return false;
		}

		T_Airport data;
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			char * tempText = (char *)sqlite3_column_text(pStmt, 0);
			if (tempText)
			{
				data.AreaCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 1);
			if (tempText)
			{
				data.AirportIdent = tempText;
			}

			data.Latitude = sqlite3_column_double(pStmt, 2);
			data.Longitude = sqlite3_column_double(pStmt, 3);

			tempText = (char *)sqlite3_column_text(pStmt, 4);
			if (tempText)
			{
				data.AirportName = tempText;
			}
			vData.push_back(data);
		}
		ReleaseSqlObject(pStmt);

		return true;
	}

	return false;
}

bool CHms424Database::GetAirportLonLatByIdent(float &lon, float &lat, std::string &ident)
{
	std::stringstream sql;
	sql << "SELECT Longitude, Latitude ";
	sql << "FROM Airport WHERE ";
	sql << "AirportIdent == '" << ident << "'";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	bool bRet = false;

	if (pStmt)
	{
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			lon = sqlite3_column_double(pStmt, 0);
			lat = sqlite3_column_double(pStmt, 1);

			bRet = true;
		}
		ReleaseSqlObject(pStmt);
	}

	return bRet;
}

bool CHms424Database::GetLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode
    , std::string tableName, std::string columnName1, std::string columnName2)
{
    std::stringstream sql;
    sql << "SELECT Longitude, Latitude from " << tableName;
    sql << " where " << columnName1 << "='" << icaoCode << "' and "<< columnName2 << "='" << ident << "' ";

    auto strSql = sql.str();
    auto strUSql = a2u(strSql.c_str());
    auto pStmt = CreateSqlObject(strUSql);

    bool bRet = false;
    if (pStmt)
    {
        while (SQLITE_ROW == sqlite3_step(pStmt))
        {
            lon = sqlite3_column_double(pStmt, 0);
            lat = sqlite3_column_double(pStmt, 1);

            bRet = true;
        }
        ReleaseSqlObject(pStmt);
    }
    return bRet;
}

bool CHms424Database::GetAirportLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode)
{
    return GetLonLatByIdent(lon, lat, ident, icaoCode, "Airport", "IcaoCode", "AirportIdent");
}

bool CHms424Database::GetWaypointLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode)
{
    bool bRet = false;
    if (GetLonLatByIdent(lon, lat, ident, icaoCode, "Airport_Terminal_Waypoints", "WpIcaoCode", "WpIdent"))
    {
        bRet = true;
    }
    else
    {
        if (GetLonLatByIdent(lon, lat, ident, icaoCode, "Enroute_Waypoints", "WpIcaoCode", "WpIdent"))
        {
            bRet = true;
        }
    }
    return bRet;
}

bool CHms424Database::GetVorLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode)
{
    return GetLonLatByIdent(lon, lat, ident, icaoCode, "VHF_Navaid", "VorIcaoCode", "VorIdent");
}

bool CHms424Database::GetNDBLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode)
{
    return GetLonLatByIdent(lon, lat, ident, icaoCode, "NDB_Navaid", "NDBIcaoCode", "NDBIdent");
}

bool CHms424Database::GetAirportLonLatHeightByIdent(double &lon, double &lat, double &height, std::string &ident)
{
	std::stringstream sql;
	sql << " SELECT Longitude, Latitude, AirportElev";
	sql << " FROM Airport WHERE ";
	sql << " AirportIdent == '" << ident << "'";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	bool bRet = false;

	if (pStmt)
	{
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			lon = sqlite3_column_double(pStmt, 0);
			lat = sqlite3_column_double(pStmt, 1);
			height = sqlite3_column_double(pStmt, 2);

			bRet = true;
		}
		ReleaseSqlObject(pStmt);
	}

	return bRet;
}

bool CHms424Database::GetNavLonLatByIdent(float &lon, float &lat, std::string &ident)
{
	std::stringstream sql;
	sql << "SELECT Longitude, Latitude ";
	sql << "FROM VHF_Navaid WHERE ";
	sql << "VorIdent == '" << ident << "'";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	bool bRet = false;

	if (pStmt)
	{
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			lon = sqlite3_column_double(pStmt, 0);
			lat = sqlite3_column_double(pStmt, 1);

			bRet = true;
		}
		ReleaseSqlObject(pStmt);
	}

	return bRet;
}

bool CHms424Database::GetRunwayInfo(std::string airport, std::string runway, McduAirportRunway &info)
{
	std::stringstream sql;
	sql << "SELECT RwLength,Bearing,Latitude,Longitude,ThresholdEle from Runway ";
	sql << " where AirportIdent='" << airport << "' ";

    //数据库有问题，长度为4的暂时用like查询
    if (runway.length() == 4)
    {
        sql << " and RunwayIdent like '" << runway << "' ";
    }
    else
    {
        sql << " and RunwayIdent='" << runway << "' ";
    }
	

	bool bRet = false;
	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			info.nRunwayLength = sqlite3_column_int(pStmt, 0);
			info.nRunwayDirection = sqlite3_column_double(pStmt, 1);
			info.dLatitude = sqlite3_column_double(pStmt, 2);
			info.dLongitude = sqlite3_column_double(pStmt, 3);
			info.nAltitude = sqlite3_column_int(pStmt, 4);

			bRet = true;
		}
		ReleaseSqlObject(pStmt);
	}

	return bRet;
}

bool CHms424Database::GetAviationDbInfo(std::vector<T_AviationDbInfo> & vData)
{
	bool bRet = false;

	std::stringstream sql;
	sql << "SELECT Id, Version, Production_Test, RecordCount, CycleData, CreateData, EndData_Chars, CreateTime, Supplier, Customer ";
	sql << "FROM Header_Information";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 10)
		{
			return false;
		}

		T_AviationDbInfo data;

		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			data.nID = sqlite3_column_int(pStmt, 0);

			char * tempText = (char *)sqlite3_column_text(pStmt, 1);
			if (tempText)
			{
				data.strVersion = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 2);
			if (tempText)
			{
				data.cProductionOrTest = tempText[0];
			}

			data.nRecordCount = sqlite3_column_int(pStmt, 3);

			tempText = (char *)sqlite3_column_text(pStmt, 4);
			if (tempText)
			{
				data.strCycle = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 5);
			if (tempText)
			{
				data.strEffective = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 6);
			if (tempText)
			{
				data.strExpires = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 7);
			if (tempText)
			{
				data.strCreateTime = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 8);
			if (tempText)
			{
				data.strSupplier = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 9);
			if (tempText)
			{
				data.strCustomer = tempText;
			}

			vData.push_back(data);
		}
		ReleaseSqlObject(pStmt);

		if (vData.size() > 0)
		{
			return true;
		}
	}

	return false;
}

DB_AirportInfo CHms424Database::GetAirportInfo(std::string icaoCode)
{
	std::stringstream sql;
	sql << "SELECT AirportIdent,AirportName,LongestRunway,AirportElev,Latitude,Longitude,MagneticVar,TransitAlt from Airport ";
	sql << " where AirportIdent='" << icaoCode << "' ";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	DB_AirportInfo stu;
	if (pStmt)
	{
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			stu.icaoCode = icaoCode;
			stu.airportName = Hms_sqlite3_column_text(pStmt, 1);
			stu.longestRunway = sqlite3_column_int(pStmt, 2);
			stu.airportElev = sqlite3_column_int(pStmt, 3);
			stu.latitude = sqlite3_column_double(pStmt, 4);
			stu.longitude = sqlite3_column_double(pStmt, 5);
			stu.magneticVar = sqlite3_column_double(pStmt, 6);
			stu.transitAlt = Hms_sqlite3_column_int(pStmt, 7, -1);
			break;
		}
		ReleaseSqlObject(pStmt);
	}

	return stu;
}

void CHms424Database::GetAirportComm(std::string icaoCode, std::map<std::string, std::vector<DB_AirportComm> > & mapData)
{
	std::stringstream sql;
	sql << "SELECT Ident,CommType,Frequency,CallSign from Airport_Communications ";
	sql << " where Ident='" << icaoCode << "' ";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			DB_AirportComm stu;
			stu.ident = icaoCode;
			stu.commType = Hms_sqlite3_column_text(pStmt, 1);
			stu.frequency = sqlite3_column_double(pStmt, 2);
			stu.commName = Hms_sqlite3_column_text(pStmt, 3);

			mapData[stu.commType].push_back(stu);
		}
		ReleaseSqlObject(pStmt);
	}
}

void CHms424Database::GetAirportRunway(std::string icaoCode, std::vector<DB_RunwayInfo> &vecData)
{
	std::stringstream sql;
	sql << "SELECT RunwayIdent,RwLength,RwWidth,Bearing,Latitude,Longitude,ThresholdEle from Runway ";
	sql << " where AirportIdent='" << icaoCode << "' ";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			DB_RunwayInfo stu;
			stu.runwayIdent = Hms_sqlite3_column_text(pStmt, 0);
			stu.rwLength = sqlite3_column_int(pStmt, 1);
			stu.rwWidth = sqlite3_column_int(pStmt, 2);
			stu.bearing = sqlite3_column_double(pStmt, 3);
			stu.latitude = sqlite3_column_double(pStmt, 4);
			stu.longitude = sqlite3_column_double(pStmt, 5);
			stu.thresholdEle = sqlite3_column_int(pStmt, 6);

			vecData.push_back(stu);
		}
		ReleaseSqlObject(pStmt);
	}
}

void CHms424Database::GetFuzzyIdent(std::string ident, std::vector<std::string> &vecRet)
{
    auto SearchFunc = [=, &vecRet](std::string tableName, std::string columnName1, std::string columnName2, std::string keyIdent)
    {
        std::stringstream sql;
        sql << "SELECT " << columnName1 << " , "<< columnName2 << " from " << tableName;
        sql << " where " << columnName2 << " like '" << keyIdent << "%' ";

        auto strSql = sql.str();
        auto strUSql = a2u(strSql.c_str());
        auto pStmt = CreateSqlObject(strUSql);

        if (pStmt)
        {
            while (SQLITE_ROW == sqlite3_step(pStmt))
            {
                std::string str1 = Hms_sqlite3_column_text(pStmt, 0);
                std::string str2 = Hms_sqlite3_column_text(pStmt, 1);
                if (str1.size() > 0 && str2.size() > 0)
                {
                    vecRet.push_back(str2 + ":" + str1);
                }            
            }
            ReleaseSqlObject(pStmt);
        }
    };

    if (ident.empty())
    {
        return;
    }
    SearchFunc("Airport", "IcaoCode", "AirportIdent", ident);
    SearchFunc("Airport_Terminal_Waypoints", "WpIcaoCode", "WpIdent", ident);
    SearchFunc("Enroute_Waypoints", "WpIcaoCode", "WpIdent", ident);
    SearchFunc("VHF_Navaid", "VorIcaoCode", "VorIdent", ident);
    SearchFunc("NDB_Navaid", "NDBIcaoCode", "NDBIdent", ident);
}

bool CHms424Database::GetAirportWaypointData(std::vector<T_AirportWaypoint> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon)
{
	std::stringstream sql;
	sql << "SELECT AreaCode, RegionCode, IcaoCode, WpIdent, WpIcaoCode, WpType, Latitude, Longitude ";
	sql << "FROM Airport_Terminal_Waypoints WHERE ";
	sql << "Longitude >= " << minLon << " and Longitude <= " << maxLon << " and Latitude >= " << minLat << " and Latitude <= " << maxLat;

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 8)
		{
			return false;
		}

		T_AirportWaypoint data;
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			char * tempText = (char *)sqlite3_column_text(pStmt, 0);
			if (tempText)
			{
				data.AreaCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 1);
			if (tempText)
			{
				data.RegionCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 2);
			if (tempText)
			{
				data.IcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 3);
			if (tempText)
			{
				data.WpIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 4);
			if (tempText)
			{
				data.WpIcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 5);
			if (tempText)
			{
				data.WpType = tempText;
			}

			data.Latitude = sqlite3_column_double(pStmt, 6);
			data.Longitude = sqlite3_column_double(pStmt, 7);

			vData.push_back(data);
		}
		ReleaseSqlObject(pStmt);

		return true;
	}

	return false;
}

bool CHms424Database::GetEnrouteWaypointData(std::vector<T_AirportWaypoint> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon)
{
	std::stringstream sql;
	sql << "SELECT AreaCode, RegionCode, IcaoCode, WpIdent, WpIcaoCode, WpType, Latitude, Longitude ";
	sql << "FROM Enroute_Waypoints WHERE ";
	sql << "Longitude >= " << minLon << " and Longitude <= " << maxLon << " and Latitude >= " << minLat << " and Latitude <= " << maxLat;

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);

	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 8)
		{
			return false;
		}

		T_AirportWaypoint data;
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			char * tempText = (char *)sqlite3_column_text(pStmt, 0);
			if (tempText)
			{
				data.AreaCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 1);
			if (tempText)
			{
				data.RegionCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 2);
			if (tempText)
			{
				data.IcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 3);
			if (tempText)
			{
				data.WpIdent = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 4);
			if (tempText)
			{
				data.WpIcaoCode = tempText;
			}

			tempText = (char *)sqlite3_column_text(pStmt, 5);
			if (tempText)
			{
				data.WpType = tempText;
			}

			data.Latitude = sqlite3_column_double(pStmt, 6);
			data.Longitude = sqlite3_column_double(pStmt, 7);

			vData.push_back(data);
		}
		ReleaseSqlObject(pStmt);

		return true;
	}

	return false;
}

// 
// bool CNavDatabase::GetIlsDmeData(std::vector<ILS_DME> & vIlsDme, const CSimEarthCoord & pos, const float & fOffset /*= 1*/)
// {
// 	bool bRet = false;
// 
// 
// 
// 	return bRet;
// }
// #include "HmsGlobal.h"
// bool CNavDatabase::GetAirportsData(std::vector<HmsWptInfo> & vWptInfo, const CSimEarthCoord & pos, const float & fOffset /*= 5*/)
// {
// 	bool bRet = false;
// 
// // 	std::stringstream s;
// // 
// // 	s << "SELECT [ICAO四字码],[区域],[机场名称],[纬度],[经度],[海拔(英尺)] FROM [机场信息] WHERE((([经度] >= ";
// // 	s << pos.dLongitude - fOffset << ")AND([经度] <= " << pos.dLongitude + fOffset << "))AND(([纬度] >= ";
// // 	s << pos.dLatitude - fOffset << ")AND([纬度] <= " << pos.dLatitude + fOffset << "))); ";
// // 
// // 	auto strSql = s.str();
// // 	auto strUSql = a2u(strSql.c_str());
// // 	auto pStmt = CreateSqlObject(strUSql);
// // 	if (pStmt)
// // 	{
// // 		auto colCnt = sqlite3_column_count(pStmt);
// // 
// // 		if (colCnt != 6)
// // 		{
// // 			return false;
// // 		}
// // 
// // 		HmsWptInfo hwi;
// // 		hwi.eType = Apt;
// // 
// // 		while (SQLITE_ROW == sqlite3_step(pStmt))
// // 		{
// // 			auto tempText = (char*)sqlite3_column_text(pStmt,0);
// // 			if (tempText)
// // 			{
// // 				hwi.strName = tempText;
// // 			}
// // 
// // 			hwi.dLat = sqlite3_column_double(pStmt, 3);
// // 			hwi.dLong = sqlite3_column_double(pStmt, 4);
// // 			
// // 			tempText = (char*)sqlite3_column_text(pStmt, 1);
// // 			if (tempText)
// // 			{
// // 				hwi.strDescribe = tempText;
// // 			}
// // 
// // 			tempText = (char*)sqlite3_column_text(pStmt, 2);
// // 			if (tempText)
// // 			{
// // 				if (hwi.strDescribe.size())
// // 				{
// // 					hwi.strDescribe += ";";
// // 				}
// // 				hwi.strDescribe += tempText;
// // 			}
// // 
// // 			vWptInfo.push_back(hwi);
// // 			bRet = true;
// // 		}
// // 		ReleaseSqlObject(pStmt);
// // 	}
// 
// 	return bRet;
// }
// 
// bool CNavDatabase::GetNavData(std::vector<HmsWptInfo> & vWptInfo, const CSimEarthCoord & pos, const float & fOffset /*= 1*/)
// {
// 	GetNdbData(vWptInfo, pos, fOffset);
// 	GetVhfData(vWptInfo, pos, fOffset);
// 	if (vWptInfo.size())
// 	{
// 		return true;
// 	}
// 	return false;
// }
// 
// bool CNavDatabase::GetNdbData(std::vector<HmsWptInfo> & vWptInfo, const CSimEarthCoord & pos, const float & fOffset /*= 1*/)
// {
// 	bool bRet = false;
// 
// // 	std::stringstream s;
// // 
// // 
// // 	s << "SELECT [ID],[频率(KHz)],[类型],[名字],[纬度],[经度],[海拔(英尺)] FROM [NDB导航台] WHERE((([经度] >= ";
// // 	s << pos.dLongitude - fOffset << ")AND([经度] <= " << pos.dLongitude + fOffset << "))AND(([纬度] >= ";
// // 	s << pos.dLatitude - fOffset << ")AND([纬度] <= " << pos.dLatitude + fOffset << "))); ";
// // 
// // 	auto strSql = s.str();
// // 	auto strUSql = a2u(strSql.c_str());
// // 	auto pStmt = CreateSqlObject(strUSql);
// // 	if (pStmt)
// // 	{
// // 		auto colCnt = sqlite3_column_count(pStmt);
// // 
// // 		if (colCnt != 7)
// // 		{
// // 			return false;
// // 		}
// // 
// // 		HmsWptInfo hwi;
// // 		hwi.eType = Nav_NDB;
// // 
// // 		while (SQLITE_ROW == sqlite3_step(pStmt))
// // 		{
// // 			auto tempText = (char*)sqlite3_column_text(pStmt, 0);
// // 			if (tempText)
// // 			{
// // 				hwi.strName = tempText;
// // 			}
// // 
// // 			hwi.dLat = sqlite3_column_double(pStmt, 4);
// // 			hwi.dLong = sqlite3_column_double(pStmt, 5);
// // 
// // 			tempText = (char*)sqlite3_column_text(pStmt, 2);
// // 			if (tempText)
// // 			{
// // 				hwi.strDescribe = tempText;
// // 			}
// // 
// // 			tempText = (char*)sqlite3_column_text(pStmt, 1);
// // 			if (tempText)
// // 			{
// // 				if (hwi.strDescribe.size())
// // 				{
// // 					hwi.strDescribe += ";";
// // 				}
// // 				hwi.strDescribe += tempText;
// // 			}
// // 
// // 			tempText = (char*)sqlite3_column_text(pStmt, 3);
// // 			if (tempText)
// // 			{
// // 				if (hwi.strDescribe.size())
// // 				{
// // 					hwi.strDescribe += ";";
// // 				}
// // 				hwi.strDescribe += tempText;
// // 			}
// // 
// // 			vWptInfo.push_back(hwi);
// // 			bRet = true;
// // 		}
// // 		ReleaseSqlObject(pStmt);
// // 	}
// 
// 	return bRet;
// }
// 
// bool CNavDatabase::GetVhfData(std::vector<HmsWptInfo> & vWptInfo, const CSimEarthCoord & pos, const float & fOffset /*= 1*/)
// {
// 	bool bRet = false;
// 
// // 	std::stringstream s;
// // 
// // 
// // 	s << "SELECT [ID],[频率(MHz)],[类型],[名字],[纬度],[经度],[海拔(英尺)] FROM [VHF导航台] WHERE((([经度] >= ";
// // 	s << pos.dLongitude - fOffset << ")AND([经度] <= " << pos.dLongitude + fOffset << "))AND(([纬度] >= ";
// // 	s << pos.dLatitude - fOffset << ")AND([纬度] <= " << pos.dLatitude + fOffset << "))); ";
// // 
// // 	auto strSql = s.str();
// // 	auto strUSql = a2u(strSql.c_str());
// // 	auto pStmt = CreateSqlObject(strUSql);
// // 	if (pStmt)
// // 	{
// // 		auto colCnt = sqlite3_column_count(pStmt);
// // 
// // 		if (colCnt != 7)
// // 		{
// // 			return false;
// // 		}
// // 
// // 		HmsWptInfo hwi;
// // 		hwi.eType =  Nav_VOR;
// // 
// // 		while (SQLITE_ROW == sqlite3_step(pStmt))
// // 		{
// // 			auto tempText = (char*)sqlite3_column_text(pStmt, 0);
// // 			if (tempText)
// // 			{
// // 				hwi.strName = tempText;
// // 			}
// // 
// // 			hwi.dLat = sqlite3_column_double(pStmt, 4);
// // 			hwi.dLong = sqlite3_column_double(pStmt, 5);
// // 
// // 			tempText = (char*)sqlite3_column_text(pStmt, 2);
// // 			if (tempText)
// // 			{
// // 				hwi.strDescribe = tempText;
// // 			}
// // 
// // 			tempText = (char*)sqlite3_column_text(pStmt, 1);
// // 			if (tempText)
// // 			{
// // 				if (hwi.strDescribe.size())
// // 				{
// // 					hwi.strDescribe += ";";
// // 				}
// // 				hwi.strDescribe += tempText;
// // 			}
// // 
// // 			tempText = (char*)sqlite3_column_text(pStmt, 3);
// // 			if (tempText)
// // 			{
// // 				if (hwi.strDescribe.size())
// // 				{
// // 					hwi.strDescribe += ";";
// // 				}
// // 				hwi.strDescribe += tempText;
// // 			}
// // 
// // 			vWptInfo.push_back(hwi);
// // 			bRet = true;
// // 		}
// // 		ReleaseSqlObject(pStmt);
// // 	}
// 
// 	return bRet;
// }

bool CHms424Database::GetRestrectionAirSpaceData(void(*Action)(DB_AirSpaceItem * data))
{
	std::stringstream sql;
	sql << " SELECT Sequence,Boundary,Latitude,Longitude,ArcLat,ArcLon,ArcDist,ArcBearing";
	sql << " FROM Restrictive_Airspace";

	auto strSql = sql.str();
	auto strUSql = a2u(strSql.c_str());
	auto pStmt = CreateSqlObject(strUSql);
	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);
		if (colCnt != 8) { return false; }

		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			DB_AirSpaceItem *data = new DB_AirSpaceItem();
			char* tempText;
			data->sequence = sqlite3_column_int(pStmt, 0);
			tempText = (char *)sqlite3_column_text(pStmt, 1);
			if (tempText)
			{
				data->boundary = (char*)malloc(strlen(tempText) + 1);
				strcpy(data->boundary, tempText);
			}
			else
			{
				data->boundary = nullptr;
			}
			data->lonlat.y = sqlite3_column_double(pStmt, 2);
			data->lonlat.x = sqlite3_column_double(pStmt, 3);
			data->ArcLat = sqlite3_column_double(pStmt, 4);
			data->ArcLon = sqlite3_column_double(pStmt, 5);
			data->ArcDist = sqlite3_column_double(pStmt, 6);
			data->ArcBearing = sqlite3_column_double(pStmt, 7);

			Action(data);
		}
		ReleaseSqlObject(pStmt);
		return true;
	}

	return false;
}

void CHms424Database::GetRestrectionAirSpaceData(std::vector<DB_AirspaceData> &vec)
{
    std::stringstream sql;
    sql << "SELECT RestType,Sequence,Boundary,Latitude,Longitude,ArcLat,ArcLon,ArcDist,ArcBearing from Restrictive_Airspace ";

    auto strSql = sql.str();
    auto strUSql = a2u(strSql.c_str());
    auto pStmt = CreateSqlObject(strUSql);

    if (pStmt)
    {
        while (SQLITE_ROW == sqlite3_step(pStmt))
        {
            int index = 0;
            DB_AirspaceData stu;
            stu.restType = Hms_sqlite3_column_text(pStmt, index++);
            stu.sequence = sqlite3_column_int(pStmt, index++);
            stu.boundary = Hms_sqlite3_column_text(pStmt, index++);
            stu.latitude = Hms_sqlite3_column_double(pStmt, index++, 999);
            stu.longitude = Hms_sqlite3_column_double(pStmt, index++, 999);
            stu.arcLat = sqlite3_column_double(pStmt, index++);
            stu.arcLon = sqlite3_column_double(pStmt, index++);
            stu.arcDist = sqlite3_column_double(pStmt, index++);
            stu.arcBearing = sqlite3_column_double(pStmt, index++);
            vec.push_back(stu);
        }
        ReleaseSqlObject(pStmt);
    }
}
