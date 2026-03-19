#include "HmsNavSvsLoadCountMgr.h"

CHmsNavSvsLoadCountMgr::CHmsNavSvsLoadCountMgr()
{
    m_sendDemCount = 0;
    m_recvDemCount = 0;
    m_sendTexCount = 0;
    m_recvTexCount = 0;
    m_maxDiff = 0;
    m_percent = 0;
    m_bIsLoadFinished = false;
}

CHmsNavSvsLoadCountMgr::~CHmsNavSvsLoadCountMgr()
{

}

CHmsNavSvsLoadCountMgr* CHmsNavSvsLoadCountMgr::GetInstance()
{
    static CHmsNavSvsLoadCountMgr* s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsLoadCountMgr;
    }
    return s_pIns;
}

void CHmsNavSvsLoadCountMgr::Update(float delta)
{
    int texDiff = m_sendTexCount - m_recvTexCount;

    if (texDiff > 130)
    {
        m_bIsLoadFinished = false;
    }
    if (texDiff > m_maxDiff)
    {
        m_maxDiff = texDiff;
    }
    static float s_loadElapse = 0;
    if (texDiff < 5)
    {
        s_loadElapse += delta;
        if (s_loadElapse > 1)
        {
            s_loadElapse = 0;
            m_maxDiff = 0;
            m_percent = 1;
            m_bIsLoadFinished = true;
        }
    }
    else
    {
        m_percent = 1.0 - texDiff / (float)m_maxDiff;
    }
}

void CHmsNavSvsLoadCountMgr::AddSendDemCount()
{
    m_mutex.lock();
    ++m_sendDemCount;
    m_mutex.unlock();
}

void CHmsNavSvsLoadCountMgr::AddRecvDemCount()
{
    m_mutex.lock();
    ++m_recvDemCount;
    m_mutex.unlock();
}

void CHmsNavSvsLoadCountMgr::AddSendTexCount()
{
    m_mutex.lock();
    ++m_sendTexCount;
    m_mutex.unlock();
}

void CHmsNavSvsLoadCountMgr::AddRecvTexCount()
{
    m_mutex.lock();
    ++m_recvTexCount;
    m_mutex.unlock();
}


