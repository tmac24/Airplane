#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include "../Nav/HmsNavDataStruct.h"
#include "HmsFMSTask.h"

class CHmsFMS
{
public:
	CHmsFMS();
	~CHmsFMS();

	void Init();

	void Start();

	void Stop();

private:
	std::thread			*m_pThread;
	std::mutex			m_mutexThread;
	bool				m_bThreadRun;
	bool				m_isInited;

	std::vector<CHmsFMSTask*> m_taskList;
	void Run();
};

