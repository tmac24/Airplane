#pragma once
#include "../Nav/HmsNavDataStruct.h"
#include "HmsDuControlData.h"
#include <vector>
#include <chrono>

class CHmsFMSTask
{
public:
    CHmsFMSTask(){ 
        t0ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()); 
    };

    virtual void excute(float delta) = 0;

    virtual void OnDuControlDataChanged(DUControlDataMember theChnaged) = 0;

public:
    std::chrono::milliseconds t0ms;
};
