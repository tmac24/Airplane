#include "HmsFMS.h"
#include <string>
#include "DataInterface/HmsDataBus.h"
#include "HmsNavTask.h"
#include "../HmsGlobal.h"
#include "HmsPeriodicCalcTask.h"
#include "HmsSvsTask.h"

#define HMS_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)

CHmsFMS::CHmsFMS()
{
    m_pThread = nullptr;
    m_bThreadRun = false;
    m_isInited = false;
}

CHmsFMS::~CHmsFMS()
{
    Stop();
}

void CHmsFMS::Init()
{
    if (m_isInited) return;

    CHmsPeriodicCalcTask *periodicCalcTask = new CHmsPeriodicCalcTask();
    CHmsSvsTask *svsTask = new CHmsSvsTask();
    CHmsNavTask *navTask = new CHmsNavTask();

    m_taskList.push_back(periodicCalcTask);
    m_taskList.push_back(svsTask);
    m_taskList.push_back(navTask);

    CHmsDuControlData *duControlData = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
    duControlData->SubscribeMemberChanged(HMS_CALLBACK_1(CHmsNavTask::OnDuControlDataChanged, navTask), MEMBER_SEL_WPT_INDEX);
    duControlData->SubscribeMemberChanged(HMS_CALLBACK_1(CHmsNavTask::OnDuControlDataChanged, navTask), MEMBER_WAY_POINTS);
    duControlData->SubscribeMemberChanged(HMS_CALLBACK_1(CHmsSvsTask::OnDuControlDataChanged, svsTask), MEMBER_WAY_POINTS);

    m_isInited = true;
}

void CHmsFMS::Start()
{
    if (nullptr == m_pThread)
    {
        Init();
        m_bThreadRun = true;
        m_pThread = new std::thread(&CHmsFMS::Run, this);
    }
}

void CHmsFMS::Stop()
{
    m_bThreadRun = false;
    if (m_pThread)
    {
        m_pThread->join();
        delete m_pThread;
        m_pThread = nullptr;
    }
}

void CHmsFMS::Run()
{
    Init();
    while (m_bThreadRun)
    {
        for (CHmsFMSTask *taskItem : m_taskList)
        {
            auto time_now = std::chrono::system_clock::now();
            auto t1ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
            auto deltaMs = t1ms - taskItem->t0ms;
            taskItem->t0ms = t1ms;

            float delta = deltaMs.count() / 1000.0F;

            taskItem->excute(delta);
        }

#ifdef  __vxworks
        sleep(200);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
#endif
    }
}