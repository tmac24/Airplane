#pragma once
#include "HmsFMSTask.h"
#include "DataInterface/HmsDataBus.h"

class CHmsDataBus;
class CHmsSvsTask : public CHmsFMSTask
{
public:
    CHmsSvsTask();
    ~CHmsSvsTask();
    virtual void excute(float delta) override;
    virtual void OnDuControlDataChanged(DUControlDataMember theChanged);    
private:
    void UpdateEnroute(std::vector<WptNodeStu2D> wayPoint);
    CHmsDataBus			*m_dataBus;
};

