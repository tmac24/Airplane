#include "HmsMavLinkInterface.h"
#include "DataInterface/HmsDataBus.h"
#include "HmsGlobal.h"
#include "Calculate/CalculateZs.h"

CHmsMavLinkInterface::CHmsMavLinkInterface()
{
}


CHmsMavLinkInterface::~CHmsMavLinkInterface()
{
}

bool CHmsMavLinkInterface::OnRecv(void * bindData, const char * pData, int nLen)
{
	m_mavlinkData = m_mavLinkDecoder.decode(pData, nLen);
	return true;
}

bool CHmsMavLinkInterface::OnRecv(unsigned int nIp, unsigned short nPort, const char * pData, int nLen)
{
    m_mavlinkData = m_mavLinkDecoder.decode(pData, nLen);
    return true;
}

bool CHmsMavLinkInterface::OnAccept(void * bindData)
{
	CHmsGlobal::GetInstance()->GetDataBus()->SetDeviceConnect(true);
	printf("MavLink connected!\n");
	return true;
}

bool CHmsMavLinkInterface::OnClose(void * bindData)
{
	CHmsGlobal::GetInstance()->GetDataBus()->SetDeviceConnect(false);
	printf("MavLink disconnected!\n");
	return true;
}

bool CHmsMavLinkInterface::AftDataUpdate(HmsAftData * pData)
{
	pData->bLatLonValid = m_mavlinkData.positionValid;
	pData->latitude = m_mavlinkData.position.lat / 10000000.0;
	pData->longitude = m_mavlinkData.position.lon / 10000000.0;
	pData->altitudeFt = m_mavlinkData.position.alt / 1000.0f;

	pData->bValidAttitude = m_mavlinkData.attitudeValid;
	pData->roll = ToAngleOrg(m_mavlinkData.attitude.roll);
	pData->pitch = ToAngleOrg(m_mavlinkData.attitude.pitch);
	pData->magneticHeading = ToAngleOrg(m_mavlinkData.attitude.yaw);
	if (pData->magneticHeading < 0)
	{
		pData->magneticHeading = 360 + pData->magneticHeading;
	}
	//pData->sideSliAngle;
	pData->yawRate = ToAngleOrg(m_mavlinkData.attitude.yawspeed);

	pData->bValidAirData = m_mavlinkData.vfrHudValid;
	pData->groundSpeed = MeterSToKnot(m_mavlinkData.vfrHud.airspeed);
	pData->verticalSpeed = MeterSToFeetM(m_mavlinkData.vfrHud.climb);
	pData->geoAltitudeFt = m_mavlinkData.vfrHud.alt;

	if (m_mavlinkData.imuValid)
	{
		pData->GLode = m_mavlinkData.imu.zacc;
	}
	return true;
}