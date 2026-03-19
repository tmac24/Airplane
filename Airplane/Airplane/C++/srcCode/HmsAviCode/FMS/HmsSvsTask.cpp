#include "HmsSvsTask.h"
#include "HmsGlobal.h"
#include "Svs/HmsSvsFlightTube.h"
#include "NavSvs/mathd/Vec3d.h"
#include "Calculate/CalculateZs.h"

CHmsSvsTask::CHmsSvsTask()
{
    m_dataBus = CHmsGlobal::GetInstance()->GetDataBus();
}

CHmsSvsTask::~CHmsSvsTask()
{

}

void CHmsSvsTask::excute(float delta)
{

}

void CHmsSvsTask::OnDuControlDataChanged(DUControlDataMember theChanged)
{
    if (theChanged == MEMBER_WAY_POINTS)
    {
        UpdateEnroute(m_dataBus->GetDuControlData()->GetEnroute());
    }
}


void CHmsSvsTask::UpdateEnroute(std::vector<WptNodeStu2D> wayPoint)
{
    std::vector<SvsWayPointInfo> vecWPi;
    unsigned int idx = 0;
    for (auto& wp : wayPoint)
    {
        auto alt_meter = FeetToMeter(wp.brief.attr.altStu[0].nAltitude);
        Vec3d lla = Vec3d(wp.brief.lon, wp.brief.lat, alt_meter);
        vecWPi.push_back(SvsWayPointInfo(lla, wp.brief.ident, idx));
        idx++;
    }

    //�µĺ�·�����鷢�͵�����
    HmsSvsFlightTubeMgr::getInstance()->_mtx_wpi.lock();
    HmsSvsFlightTubeMgr::getInstance()->_queue_WpiVec.push(vecWPi);
    HmsSvsFlightTubeMgr::getInstance()->_mtx_wpi.unlock();
}
