#pragma once
#ifdef WIN32
#include <windows.h>
#else
#include <mutex>
#define CRITICAL_SECTION std::recursive_mutex
#endif

class CHmsCS
{
public:
	CHmsCS();
	~CHmsCS();

	CRITICAL_SECTION * GetCS(){ return &m_cs; }

private:
	CRITICAL_SECTION		m_cs;
};

class CHmsCSLocker
{
public:
	CHmsCSLocker(CHmsCS & hmsCS);
	CHmsCSLocker(CHmsCS * pHmsCS);
	~CHmsCSLocker();

private:
	CRITICAL_SECTION * m_pCs;
};

