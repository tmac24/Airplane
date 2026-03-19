#include "Hangar.h"
#include "AircraftModels.h"


void Hangar::AddAircraft(Plane3D* aircraft)
{
    if (aircraft)
    {
        if (ExistAircraft(aircraft->getModelName()))
        {
            //already existed in the hanger
            //release it
            delete _map_aircrafts[aircraft->getModelName()];
        }
        //add it
        _map_aircrafts[aircraft->getModelName()] = aircraft;
    }
}

Plane3D* Hangar::GetAircraft(std::string modelName)
{
    if (ExistAircraft(modelName))
    {
        return _map_aircrafts[modelName];
    }
    return nullptr;
}

bool Hangar::ExistAircraft(std::string modelName)
{
    return _map_aircrafts.find(modelName) != _map_aircrafts.end();
}

std::vector<std::string> Hangar::GetAircraftNameList(void)
{
    std::vector<std::string> namelist;
    for (auto &it : _map_aircrafts)
    {
        namelist.push_back(it.first);
    }
    return namelist;
}

bool Hangar::RemoveAircraft(std::string modelName)
{
    if (!ExistAircraft(modelName))
    {
        return true;
    }
    delete _map_aircrafts[modelName];
    _map_aircrafts.erase(modelName);
    return true;
}

bool Hangar::RemoveAllAircraft(void)
{
    for (auto& it : _map_aircrafts)
    {
        delete it.second;
    }
    _map_aircrafts.clear();
    return true;
}

HangarInitThread::HangarInitThread(Hangar &hangar, const HangarInitThreadCallback& callback)
: m_bHangarReady(false), m_pHangar(&hangar)
{    
    //m_pThread = new std::thread(&HangarInitThread::HangarInit, this, callback);
    //todo: release thread
    std::cout << "load aircraft model start" << std::endl;
    HangarInit(callback);//todo: 需要分离出纯的数据加载部分,再用子线程加载。
    std::cout << "load aircraft model finish" << std::endl;
}

void HangarInitThread::HangarInit(const HangarInitThreadCallback& callback)
{
    do
    {
        if (m_pHangar == NULL)
        {
            m_bHangarReady = false;
            break;
        }

        auto models = aft_models::GetSingleModelInfo();
        if (models)
        {
            for (auto md : *models)
            {
                Plane3D* pTemp = Plane3D::create(md._modelName, md._filepath);
                m_pHangar->AddAircraft(pTemp);
            }
        }
        m_bHangarReady = true;

    } while (0);

    callback(m_bHangarReady);

    return;
}

bool HangarInitThread::IsHangarReady(void)
{
    return m_bHangarReady;
}
