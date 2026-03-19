#include "base/HmsTimeRecorder.h"


HmsTimeStatus::HmsTimeStatus()
{
	dMin = 0;
	dMax = 0;
	dTotal = 0;
	dAverage = 0;
	nRecodeCnt = 0;
	dLastTime = 0;
}


CHmsTimeRecorder * CHmsTimeRecorder::GetInstance(int nResetTimes /*= 1000*/)
{
	static CHmsTimeRecorder * s_pTimeRecorder = nullptr;
	if (nullptr == s_pTimeRecorder)
	{
		s_pTimeRecorder = new CHmsTimeRecorder(nResetTimes);
	}
	return s_pTimeRecorder;
}

CHmsTimeRecorder::CHmsTimeRecorder(int nResetTimes)
{
	m_nResetTimes = nResetTimes;
	m_time.RecordCurrentTime();
}

unsigned int CHmsTimeRecorder::GetRecorderID(const char * strName)
{
	std::string tempString = strName;
	auto itFind = m_mNameIndex.find(tempString);
	if (itFind != m_mNameIndex.end())
	{
		return itFind->second;
	}
	else
	{
		int nRet = m_vTimeStatus.size();
		m_mNameIndex[tempString] = nRet;
		HmsTimeStatus hts;
		hts.strName = tempString;
		hts.nRecodeCnt = m_nResetTimes;
		m_vTimeStatus.push_back(hts);
		return nRet;
	}
}

void CHmsTimeRecorder::StartRecord(unsigned int nIndex)
{
	if (nIndex < m_vTimeStatus.size())
	{
		auto & ts = m_vTimeStatus.at(nIndex);
		ts.dLastTime = m_time.GetTime();
	}
}

void CHmsTimeRecorder::EndRecord(unsigned int nIndex)
{
	if (nIndex < m_vTimeStatus.size())
	{
		auto & ts = m_vTimeStatus.at(nIndex);
		double dTempTime = m_time.GetTime() - ts.dLastTime;
		if (ts.nRecodeCnt >= m_nResetTimes)
		{
			if (ts.dTotal > 0.0)
			{
				PrintRecord(nIndex);
			}
			ts.dMax = dTempTime;
			ts.dMin = dTempTime;
			ts.dTotal = dTempTime;
			ts.nRecodeCnt = 1;
		}
		else
		{
			ts.nRecodeCnt++;
			ts.dTotal += dTempTime;
			if (ts.dMax < dTempTime)
				ts.dMax = dTempTime;
			if (ts.dMin > dTempTime)
				ts.dMin = dTempTime;
		}
	}
}

void CHmsTimeRecorder::PrintRecord(unsigned int nIndex)
{
	if (nIndex < m_vTimeStatus.size())
	{
		auto & ts = m_vTimeStatus.at(nIndex);
		ts.dAverage = ts.dTotal / ts.nRecodeCnt;
		printf("\nTimeStatus:%s[%d] min=%.6f max=%.6f average=%.6f\n", ts.strName.c_str(), ts.nRecodeCnt, ts.dMin, ts.dMax, ts.dAverage);
	}
}

void CHmsTimeRecorder::PrintAll()
{
	for (unsigned int i = 0; i < (unsigned int)m_vTimeStatus.size(); i++)
	{
		PrintRecord(i);
	}
}


extern "C" void HmsTimeRecorderInit(int nResetTimes)
{
	CHmsTimeRecorder::GetInstance(nResetTimes);
}

extern "C" int  HmsTimeRecorderGetRecorderID(const char * strName)
{
	return CHmsTimeRecorder::GetInstance()->GetRecorderID(strName);
}
extern "C" void HmsTimeRecorderStartRecord(unsigned int nIndex)
{
	CHmsTimeRecorder::GetInstance()->StartRecord(nIndex);
}
extern "C" void HmsTimeRecorderEndRecord(unsigned int nIndex)
{
	CHmsTimeRecorder::GetInstance()->EndRecord(nIndex);
}
extern "C" void HmsTimeRecorderPrintRecord(unsigned int nIndex)
{
	CHmsTimeRecorder::GetInstance()->PrintRecord(nIndex);
}
extern "C" void HmsTimeRecorderPrintAll()
{
	CHmsTimeRecorder::GetInstance()->PrintAll();
}
