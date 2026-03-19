#pragma once
#include "AviDataDef.h"
#include <queue>
#include <string>

class CHmsDataBus_Old
{
public:
	CHmsDataBus_Old();
	~CHmsDataBus_Old();

	HmsLocalAftData & GetData(){ return m_data; }

	void AddAftStatusMsg(const HmsAftStatusMsg & msg);
	bool GetAftStatusMsg(HmsAftStatusMsg & msg);
	
	HmsInstrumentFailed * GetInstruFailed(){ return &m_dataInstruFailed; }


	//the svs info
	void SetSvsInfo(int nType, const std::string & strPath);
	int GetSvsType(){ return m_nSvsType; }
	std::string GetSvsVideoPath(){ return m_strSvsVideoPath; }

private:
	HmsLocalAftData						m_data;
	HmsInstrumentFailed					m_dataInstruFailed;		//“«±Ì ß–ß
	std::queue<HmsAftStatusMsg>			m_qAftStatusMsg;

	int									m_nSvsType;		//the type of the svs. 0 is camera, 1 is shared memory, 2 is video
	std::string							m_strSvsVideoPath;
};

