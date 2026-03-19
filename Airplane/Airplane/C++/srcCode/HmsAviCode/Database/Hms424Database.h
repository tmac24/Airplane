#pragma once
#include <vector>
//#include "SimEarthCoord.h"
#include "HmsDatabase.h"
#include "Hms424TableDef.h"

#include "../Nav/RoutePlanData/mcduDef.h"

class CHms424Database : public CHmsDatabase
{
public:
	static CHms424Database* GetInstance();

	bool GetAirportData(std::vector<T_Airport> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon);

	//获取机场COM数据 vAirportComm输出 minLat、maxLat、minLon、maxLon所需获取的经纬度范围
	bool GetAirportCommunicationData(std::vector<T_AirportCommunication> &vAirportComm, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon);

	//获取NDB导航数据 vAirportComm输出 minLat、maxLat、minLon、maxLon所需获取的经纬度范围
	bool GetNDBNavaidData(std::vector<T_NDBNavaid> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon);

	//获取终端NDB导航数据 vAirportComm输出 minLat、maxLat、minLon、maxLon所需获取的经纬度范围
	bool GetTerminalNDBNavaid(std::vector<T_TerminalNDBNavaid> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon);

	//获取终端NDB导航数据 vAirportComm输出 minLat、maxLat、minLon、maxLon所需获取的经纬度范围
	bool GetVHFNavaidData(std::vector<T_VHFNavaid> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon);

	bool GetAirportWaypointData(std::vector<T_AirportWaypoint> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon);

	bool GetEnrouteWaypointData(std::vector<T_AirportWaypoint> &vData, const double &minLat, const double &maxLat, const double &minLon, const double &maxLon);

// 	bool GetIlsDmeData(std::vector<ILS_DME> & vIlsDme, const CSimEarthCoord & pos, const float & fOffset = 1);
// 
// 	bool GetAirportsData(std::vector<HmsWptInfo> & vWptInfo, const CSimEarthCoord & pos, const float & fOffset = 5);
// 
// 	bool GetNavData(std::vector<HmsWptInfo> & vWptInfo, const CSimEarthCoord & pos, const float & fOffset = 1);
// 
// 	bool GetNdbData(std::vector<HmsWptInfo> & vWptInfo, const CSimEarthCoord & pos, const float & fOffset = 1);
// 	bool GetVhfData(std::vector<HmsWptInfo> & vWptInfo, const CSimEarthCoord & pos, const float & fOffset = 1);

	bool GetAirportLonLatByIdent(float &lon, float &lat, std::string &ident);
    bool GetAirportLonLatHeightByIdent(double &lon, double &lat, double &height, std::string &ident);
	bool GetNavLonLatByIdent(float &lon, float &lat, std::string &ident);

	bool GetRunwayInfo(std::string airport, std::string runway, McduAirportRunway &info);

	bool GetRestrectionAirSpaceData(void(*)(DB_AirSpaceItem * data));
    void GetRestrectionAirSpaceData(std::vector<DB_AirspaceData> &vec);

	bool GetAviationDbInfo(std::vector<T_AviationDbInfo> & vData);

    DB_AirportInfo GetAirportInfo(std::string icaoCode);
    void GetAirportComm(std::string icaoCode, std::map<std::string, std::vector<DB_AirportComm> > & mapData);
    void GetAirportRunway(std::string icaoCode, std::vector<DB_RunwayInfo> &vecData);

    void GetFuzzyIdent(std::string ident, std::vector<std::string> &vecRet);

    bool GetAirportLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode);
    bool GetWaypointLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode);
    bool GetVorLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode);
    bool GetNDBLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode);

private:
	CHms424Database();
	~CHms424Database();

    bool GetLonLatByIdent(float &lon, float &lat, std::string ident, std::string icaoCode
        , std::string tableName, std::string columnName1, std::string columnName2);

	static CHms424Database *m_pInstance;
};

