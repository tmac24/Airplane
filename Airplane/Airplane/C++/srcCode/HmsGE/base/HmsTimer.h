#pragma once
#ifdef __vxworks
#else

#ifdef _WIN32
#include <windows.h>
#endif

#endif

#include <functional>

//微妙级计时器
class CHmsMicroSecondTime
{
public:
	CHmsMicroSecondTime();
	~CHmsMicroSecondTime();

	void Start();
	void Stop();

	//获取
	double GetTimeStart();

	//获取当前时间
	double GetTimeCurrent();

	//获取时间间隔
	double GetTimeInterval();

	double GetTimeElapsed();

private:

#ifdef _WIN32

	LARGE_INTEGER		m_timeStart;
	LARGE_INTEGER		m_timeFrequency;

#endif
	double				m_dTimeStart;
};

class CHmsTime
{
public:
	CHmsTime();
	~CHmsTime();

	//记录当前时间
	double RecordCurrentTime();

	//获取时间间隔
	double GetElapsed();

	double GetTime();

private:	
	double					m_dTimeRecord;

	static CHmsMicroSecondTime		s_time;
};

typedef std::function<int()> HmsTimerCallback;

class CHmsTimer
{
public:
	CHmsTimer();
	~CHmsTimer();

	void SetTime(double dTimeInterval, const HmsTimerCallback & callback);

	void StartTimer();

	void EndTimer();

	int Run();

private:
	CHmsMicroSecondTime			m_time;
#ifdef _WIN32
	HANDLE						m_hThread;
#endif
	HmsTimerCallback			m_timerCallBack;
	bool						m_bThreadRun;
	double						m_dTimeInterval;			//时间间隔
	double						m_dTimeLastUpdate;			//最后一次更新时间
};
