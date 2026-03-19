#pragma once

#include <string>
#include "math/Vec2.h"

struct T_AirportCommunication
{
	int rowid;		//行id，sqllite自动生成
	int Id;			//主键id，自动递增
	std::string AreaCode;
	std::string Ident;	
	std::string IcaoCode;	
	std::string CommType;
	double Frequency;
	double Latitude;
	double Longitude;
	double MagneticVar;
	std::string CommName;
};

struct T_Airport
{
	std::string AreaCode;
	std::string AirportIdent;
	
	double Latitude;
	double Longitude;
	std::string AirportName;
};

struct T_NDBNavaid
{
	int rowid;		//行id，sqllite自动生成
	int Id;			//主键id，自动递增
	std::string AreaCode;
	std::string AirportIdent;
	std::string IcaoCode;
	std::string NDBIdent;
	std::string NDBIcaoCode;
	std::string NDBName;
	std::string chType;
	std::string DatumCode;
	double NDBFrequency;
	double Latitude;
	double Longitude;
	double MagneticVar;
	int FacilityElevation;
	std::string FIRIdent;
	std::string UIRIdent;
};

struct T_TerminalNDBNavaid
{
	T_TerminalNDBNavaid()
	{
		AreaCode = "";
	}

	int rowid;		//行id，sqllite自动生成
	int Id;			//主键id，自动递增
	std::string AreaCode;
	std::string AirportIdent;
	std::string IcaoCode;
	std::string NDBIdent;
	std::string NDBIcaoCode;
	std::string NDBName;
	std::string chType;
	std::string DatumCode;
	double NDBFrequency;
	double Latitude;
	double Longitude;
	double MagneticVar;
	int FacilityElevation;
	std::string FIRIdent;
	std::string UIRIdent;
};


struct T_VHFNavaid
{
	T_VHFNavaid()
	{
		AreaCode = "";
	}
	
	int rowid;		//行id，sqllite自动生成
	int Id;			//主键id，自动递增
	std::string AreaCode;
	std::string AirportIdent;
	std::string IcaoCode;
	std::string VorIdent;
	std::string VorIcaoCode;
	float VorFrequency;
	double Latitude;
	double Longitude;
	std::string chType;
	std::string DMEIdent;
	float StationDeclination;
	int DMEElevation;
	std::string FigureMerit;
	float DMEBias;
	int FreProtect;
	std::string DatumCode;
	std::string VorName;
	float MagneticVar;
	int FacilityElevation;
};

struct T_AirportWaypoint
{
	std::string AreaCode;
	std::string RegionCode;
	std::string IcaoCode;
	std::string WpIdent;
	std::string WpIcaoCode;
	std::string WpType;
	double Latitude;
	double Longitude;

	T_AirportWaypoint()
	{
		Latitude = 0;
		Longitude = 0;
	}
};

struct T_AviationDbInfo
{
	int			nRecordCount;
	int			nID;
	char		cProductionOrTest;
	std::string strCycle;
	std::string strEffective;
	std::string strExpires;
	std::string strSupplier;
	std::string strCreateTime;
	std::string strCustomer;
	std::string strVersion;

	T_AviationDbInfo()
	{
		nRecordCount = 0;
		nID = 0;
		cProductionOrTest = '\0';
	}
};

struct DB_AirportInfo
{
    std::string icaoCode;
    std::string airportName;
    int longestRunway;
    int airportElev;
    double latitude;
    double longitude;
    double magneticVar;
    int transitAlt;

    DB_AirportInfo()
    {
        longestRunway = 0;
        airportElev = 0;
        latitude = 0;
        longitude = 0;
        magneticVar = 0;
        transitAlt = 0;
    }
};

struct DB_AirportComm
{
    std::string ident;
    std::string commType;
    float frequency;
    std::string commName;

    DB_AirportComm()
    {
        frequency = 0;
    }
};

struct DB_RunwayInfo
{
    std::string runwayIdent;
    int rwLength;
    int rwWidth;
    float bearing;
    double latitude;
    double longitude;
    int thresholdEle;

    DB_RunwayInfo()
    {
		rwLength = 0;
        rwWidth = 0;
        bearing = 0;
        latitude = 0;
        longitude = 0;
        thresholdEle = 0;
	}

};

struct DB_AirSpaceItem
{
	int sequence;
	char * boundary;
	HmsAviPf::Vec2 lonlat;
	double ArcLat;
	double ArcLon;
	double ArcDist;
	double ArcBearing;
};

struct DB_AirspaceData
{
    std::string restType;
    int sequence;
    std::string boundary;
    double latitude;
    double longitude;
    double arcLat;
    double arcLon;
    double arcDist;
    double arcBearing;
};