#pragma once

#include <thread>
#include <mutex>
#include <functional>
#include <queue>

#include <iostream>


class CHmsSVSLoadThread
{
public:
	static CHmsSVSLoadThread* GetInstance();
	virtual ~CHmsSVSLoadThread();
	void start(void);
	void stop(void);
private:
	CHmsSVSLoadThread();	
	std::thread* m_pThread;
	std::mutex m_mutex;
	static CHmsSVSLoadThread* m_pInstance;	
};

