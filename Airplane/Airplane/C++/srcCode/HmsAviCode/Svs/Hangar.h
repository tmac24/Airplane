#pragma once
#include "plane3d_.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <thread>

class Hangar
{
public:
    void AddAircraft(Plane3D* aircraft);
    Plane3D* GetAircraft(std::string modelName);
    bool ExistAircraft(std::string modelName);
    std::vector<std::string> GetAircraftNameList(void);
    bool RemoveAircraft(std::string modelName);
    bool RemoveAllAircraft(void);
private:
    std::unordered_map<std::string, Plane3D*> _map_aircrafts;
};

class HangarInitThread
{
public:
    typedef std::function<void(bool)> HangarInitThreadCallback;
    HangarInitThread(Hangar &hanger, const HangarInitThreadCallback& callback);
    bool IsHangarReady(void);
private:   
    void HangarInit(const HangarInitThreadCallback& callback);
    Hangar* m_pHangar;
    bool m_bHangarReady;
    std::thread* m_pThread;
};

