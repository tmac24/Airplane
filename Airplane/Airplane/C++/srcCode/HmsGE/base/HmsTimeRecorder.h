#pragma once

#ifdef __cplusplus

#include "HmsTimer.h"
#include <vector>
#include <map>
#include <string>

struct HmsTimeStatus
{
	double			dMin;
	double			dMax;

	double			dTotal;
	double			dAverage;
	int				nRecodeCnt;

	std::string		strName;
	double			dLastTime;

	HmsTimeStatus();
};

class CHmsTimeRecorder
{
public:
	static CHmsTimeRecorder * GetInstance(int nResetTimes = 1000);

	CHmsTimeRecorder(int nResetTimes);

	unsigned int GetRecorderID(const char * strName);
	void StartRecord(unsigned int nIndex);
	void EndRecord(unsigned int nIndex);
	void PrintRecord(unsigned int nIndex);

	void PrintAll();

private:
	std::map<std::string, int>	 m_mNameIndex;
	std::vector<HmsTimeStatus>   m_vTimeStatus;
	int							 m_nResetTimes;
	CHmsTime					 m_time;
};

extern "C" void HmsTimeRecorderInit(int nResetTimes);
extern "C" int  HmsTimeRecorderGetRecorderID(const char * strName);
extern "C" void HmsTimeRecorderStartRecord(unsigned int nIndex);
extern "C" void HmsTimeRecorderEndRecord(unsigned int nIndex);
extern "C" void HmsTimeRecorderPrintRecord(unsigned int nIndex);
extern "C" void HmsTimeRecorderPrintAll();

#else
void HmsTimeRecorderInit(int nResetTimes);
int  HmsTimeRecorderGetRecorderID(const char * strName);
void HmsTimeRecorderStartRecord(unsigned int nIndex);
void HmsTimeRecorderEndRecord(unsigned int nIndex);
void HmsTimeRecorderPrintRecord(unsigned int nIndex);
void HmsTimeRecorderPrintAll();
#endif