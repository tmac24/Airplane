#include "base/HmsTimer.h"
#ifndef _WIN32
#include <sys/time.h>
#endif

CHmsMicroSecondTime::CHmsMicroSecondTime()
{
#ifdef _WIN32
	m_timeStart.QuadPart = 0;
	m_timeFrequency.QuadPart = 1000000;
#endif
	m_dTimeStart = 0;
	Start();
}


CHmsMicroSecondTime::~CHmsMicroSecondTime()
{
	auto c = 32 * 10e-10;
}

void CHmsMicroSecondTime::Start()
{
#ifdef _WIN32
	QueryPerformanceFrequency(&m_timeFrequency);
	QueryPerformanceCounter(&m_timeStart);

	m_dTimeStart = (double)m_timeStart.QuadPart / m_timeFrequency.QuadPart;
#else
	struct timeval curtime;
	gettimeofday(&curtime, NULL);
	m_dTimeStart =curtime.tv_sec + curtime.tv_usec*1e-6;
#endif
}

void CHmsMicroSecondTime::Stop()
{

}

double CHmsMicroSecondTime::GetTimeStart()
{
	return m_dTimeStart;
}

double CHmsMicroSecondTime::GetTimeCurrent()
{
#ifdef _WIN32
	LARGE_INTEGER timeCurrent;
	QueryPerformanceCounter(&timeCurrent);
	double dTime = (double)(timeCurrent.QuadPart) / m_timeFrequency.QuadPart;
#else
	struct timeval curtime;
	gettimeofday(&curtime, NULL);
	auto dTime = curtime.tv_sec + curtime.tv_usec*1e-6;
#endif
	return dTime;
}

double CHmsMicroSecondTime::GetTimeInterval()
{
#ifdef _WIN32 
	LARGE_INTEGER timeCurrent;
	QueryPerformanceCounter(&timeCurrent);
	double dElapsed = (double)(timeCurrent.QuadPart - m_timeStart.QuadPart) / m_timeFrequency.QuadPart;
#else
	struct timeval curtime;
	gettimeofday(&curtime, NULL);
	double curTime = curtime.tv_sec + curtime.tv_usec*1e-6;
	double dElapsed = curTime - m_dTimeStart;
#endif
	return dElapsed;
}

double CHmsMicroSecondTime::GetTimeElapsed()
{
#ifdef _WIN32
	LARGE_INTEGER timeCurrent;
	QueryPerformanceCounter(&timeCurrent);
	double dElapsed = (double)(timeCurrent.QuadPart - m_timeStart.QuadPart) / m_timeFrequency.QuadPart;
#else
	struct timeval curtime;
	gettimeofday(&curtime, NULL);
	double curTime = curtime.tv_sec + curtime.tv_usec*1e-6;
	double dElapsed = curTime - m_dTimeStart;
#endif
	return dElapsed;
}

#ifndef _WIN32
#define DWORD int
#define WINAPI 
#define LPVOID void *
#endif

DWORD WINAPI HmsTimerThread(LPVOID pParam)
{
	CHmsTimer * pTimer = (CHmsTimer*)pParam;

	if (pTimer)
	{
		return pTimer->Run();
	}
	return 0;
}

CHmsTimer::CHmsTimer()
	: m_timerCallBack(nullptr)
	, m_bThreadRun(false)
	, m_dTimeInterval(0.000010)
	, m_dTimeLastUpdate(0)
{
#if _WIN32
	m_hThread = NULL;
#endif
	
}

CHmsTimer::~CHmsTimer()
{

}

void CHmsTimer::SetTime(double dTimeInterval, const HmsTimerCallback & callback)
{
	m_timerCallBack = callback;
	m_dTimeInterval = dTimeInterval;
}

void CHmsTimer::StartTimer()
{
	EndTimer();

	DWORD dwThreadID = 0;
	m_bThreadRun = true;
#ifdef _WIN32
	m_hThread = CreateThread(NULL, 0, HmsTimerThread, this, 0, &dwThreadID);
	SetThreadPriority(m_hThread, THREAD_PRIORITY_HIGHEST);
#else
	
	
#endif
}

void CHmsTimer::EndTimer()
{
	m_bThreadRun = false;

#ifdef _WIN32
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		m_hThread = NULL;
		//TerminateThread(m_hThread, 0);
	}
#endif
}

int CHmsTimer::Run()
{
	m_time.Start();
	m_dTimeLastUpdate = m_time.GetTimeStart();
	double dTimeCurrent = 0;
	double dTimeElapsed = 0;
	while (m_bThreadRun)
	{
		dTimeCurrent = m_time.GetTimeCurrent();
		dTimeElapsed = dTimeCurrent - m_dTimeLastUpdate;
		if (dTimeElapsed >= m_dTimeInterval)
		{
			if (m_timerCallBack)
			{
				m_timerCallBack();
			}
			m_dTimeLastUpdate = dTimeCurrent;
		}
		else
		{
			;//后面可以优化

		}
	}
	m_time.Stop();

	return 0;
}

CHmsTime::CHmsTime()
	: m_dTimeRecord(0)
{
	s_time.Start();
}

CHmsTime::~CHmsTime()
{
	s_time.Stop();
}

double CHmsTime::RecordCurrentTime()
{
	 m_dTimeRecord = s_time.GetTimeCurrent();
	 return m_dTimeRecord;
}

double CHmsTime::GetElapsed()
{
	return s_time.GetTimeCurrent() - m_dTimeRecord;
}

double CHmsTime::GetTime()
{
	return s_time.GetTimeCurrent();
}

CHmsMicroSecondTime CHmsTime::s_time;

