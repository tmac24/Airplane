#pragma once
#include "HmsFMSTask.h"
#include "HmsGlobal.h"
#include "Svs/_Vec3.h"
#include "DataInterface/HmsDataBus.h"
#include <functional>
#include <vector>

struct PeriodicJob
{
    typedef std::function<void(float)> PeriodicJobFunc;
    PeriodicJob(float period, PeriodicJobFunc);
    float m_period;     //second
    float m_stopwatch;  //second
    PeriodicJobFunc m_job;
};

class CHmsPeriodicCalcTask : public CHmsFMSTask
{
public:
    CHmsPeriodicCalcTask();

    virtual void excute(float delat) override;

    virtual void OnDuControlDataChanged(DUControlDataMember theChnaged) override;

    std::vector<PeriodicJob> m_vecPj;
};