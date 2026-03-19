#include "HmsNavSvsPositionDatabus.h"
#include "DataInterface/network/HmsNetworks.h"
#include "DataInterface/HmsDataBus.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "HmsGlobal.h"

CHmsNavSvsPositionDatabus::CHmsNavSvsPositionDatabus()
{
    m_type = NavSvsPositionMgrType::POSMGR_DATABUS;
}

CHmsNavSvsPositionDatabus::~CHmsNavSvsPositionDatabus()
{

}

CHmsNavSvsPositionDatabus* CHmsNavSvsPositionDatabus::GetInstance()
{
    static CHmsNavSvsPositionDatabus *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsPositionDatabus;
    }
    return s_pIns;
}

void CHmsNavSvsPositionDatabus::Update(float delta)
{
    auto pDataBus = CHmsGlobal::GetInstance()->GetDataBus();
    if (pDataBus && pDataBus->GetData()->bLatLonValid)
    {
        m_lon = pDataBus->GetData()->longitude;
        m_lat = pDataBus->GetData()->latitude;
        m_height = pDataBus->GetMslAltitudeMeter();
        m_yaw = pDataBus->GetData()->trueHeading;
        m_pitch = pDataBus->GetData()->pitch;
        m_roll = pDataBus->GetData()->roll;
        m_speed = pDataBus->GetData()->groundSpeed;
    }
}


