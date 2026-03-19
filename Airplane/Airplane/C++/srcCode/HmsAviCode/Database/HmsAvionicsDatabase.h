#pragma once
#include "HmsDatabase.h" 
#include "HmsAvionicsTableDef.h"
#include <vector>

class CHmsAvionicsDatabase : public CHmsDatabase
{
public:
	static CHmsAvionicsDatabase* GetInstance();

	//获取频率数据 category 1=COM 2=ADF 3=VOR   recordType 1=Recent 4=User
	bool GetFrequencyData(std::vector<T_Frequency> & vDatas, int category, int recordType, int limit);
	//保存一个频率
	void SaveFrequency(T_Frequency data);
	//根据频率值删除该条频率记录 category 1=COM 2=ADF 3=VOR   recordType 1=Recent 4=User
	void DeleteFrequencyByFrequency(float fFrequency, int category, int recordType);

	//从数据库中获取上次使用的FlightId
	std::string GetFlightId();
	//更新FlightId
	void UpdateFlightId(std::string newFlightId);
private:
	CHmsAvionicsDatabase();
	~CHmsAvionicsDatabase();

private:
	static CHmsAvionicsDatabase *m_pInstance;

};

