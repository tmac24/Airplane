#include "HmsSVSLoadThread.h"
#include "Module.h"

int svs_running = 0;

CHmsSVSLoadThread* CHmsSVSLoadThread::m_pInstance = nullptr;

CHmsSVSLoadThread::CHmsSVSLoadThread()
: m_pThread(nullptr)
{

}

CHmsSVSLoadThread* CHmsSVSLoadThread::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CHmsSVSLoadThread;
	}
	return m_pInstance;
}

CHmsSVSLoadThread::~CHmsSVSLoadThread()
{
	this->stop();
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}


void CHmsSVSLoadThread::start(void)
{
	if (m_pThread == nullptr)
	{	
		svs_running = true;
		m_pThread = new std::thread(&SvsTerrainDataService);
	}	
}

void CHmsSVSLoadThread::stop(void)
{
	svs_running = false;
	if (m_pThread)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}	
}