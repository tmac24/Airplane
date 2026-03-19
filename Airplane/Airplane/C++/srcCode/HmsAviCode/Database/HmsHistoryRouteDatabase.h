#pragma once

#include <vector>
#include <map>
#include "HmsDatabase.h"
#include "../Nav/HmsNavDataStruct.h"

class CHmsHistoryRouteDatabase : public CHmsDatabase
{
public:
	static CHmsHistoryRouteDatabase* GetInstance();

	void SaveHistoryRoute(const HistoryRouteStu &stu);
	void GetHistoryRoute(std::vector<HistoryRouteStu> &vecArray);
	void DeleteHistoryRoute(int id);

	void SaveUserWpt(const UserWptStu &stu);
	void DeleteUserWpt(int id);
	void GetUserWpt(std::vector<UserWptStu> &vecUsetWpt);
    void GetFuzzyIdent(std::string ident, std::vector<std::string> &vecRet);
    bool GetUserWptLonLatByIdent(float &lon, float &lat, std::string ident);

	void SaveAirspace(const std::vector<AirspaceEditStu> &vecStu);
	void GetAirspace(std::vector<AirspaceEditStu> &vecStu);
	void DeleteAirspace();

private:
	CHmsHistoryRouteDatabase();
	~CHmsHistoryRouteDatabase();

	static CHmsHistoryRouteDatabase *m_pInstance;
};

