#pragma once

#include "HmsStandardDef.h"
#include <mutex>

class CHmsNavSvsLoadCountMgr
{
public:
    static CHmsNavSvsLoadCountMgr* GetInstance();
    void Update(float delta);
    void AddSendDemCount();
    void AddRecvDemCount();
    void AddSendTexCount();
    void AddRecvTexCount();

private:
    CHmsNavSvsLoadCountMgr();
    ~CHmsNavSvsLoadCountMgr();

private:
    HmsUint64 m_sendDemCount;
    HmsUint64 m_recvDemCount;
    HmsUint64 m_sendTexCount;
    HmsUint64 m_recvTexCount;
    int m_maxDiff;
    float m_percent;
    bool m_bIsLoadFinished;
    std::mutex m_mutex;

    friend class CHmsNavSvsLayer;
};



