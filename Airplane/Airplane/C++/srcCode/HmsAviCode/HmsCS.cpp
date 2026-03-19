#include "HmsCS.h"

#ifndef WIN32

#define InitializeCriticalSection(x)
#define EnterCriticalSection(x) x->lock();
#define LeaveCriticalSection(x) x->unlock();

#endif


CHmsCS::CHmsCS()
{
	InitializeCriticalSection(&m_cs);
}


CHmsCS::~CHmsCS()
{
}

CHmsCSLocker::CHmsCSLocker(CHmsCS & hmsCS)
:m_pCs(NULL)
{
	m_pCs = hmsCS.GetCS();
	if (m_pCs)
	{
		EnterCriticalSection(m_pCs);
	}
}

CHmsCSLocker::CHmsCSLocker(CHmsCS * pHmsCS)
:m_pCs(NULL)
{
	if (pHmsCS)
	{
		m_pCs = pHmsCS->GetCS();
	}

	if (m_pCs)
	{
		EnterCriticalSection(m_pCs);
	}
}

CHmsCSLocker::~CHmsCSLocker()
{
	if (m_pCs)
	{
		LeaveCriticalSection(m_pCs);
	}
}