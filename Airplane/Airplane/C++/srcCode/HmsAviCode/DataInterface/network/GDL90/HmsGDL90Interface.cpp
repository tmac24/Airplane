#include "HmsGDL90Interface.h"
#include "DataInterface/HmsDataBus.h"
#include "HmsGlobal.h"
#include "Calculate/CalculateZs.h"
#include "HmsGE/external/wmm/HmsMagneticDecl.h"
#include "HmsLog.h"

CHmsGDL90Interface::CHmsGDL90Interface()
{
	m_nRecvUdpIp = 0;
	m_nRecvUdpPort = 0;
}


CHmsGDL90Interface::~CHmsGDL90Interface()
{
}

bool CHmsGDL90Interface::OnRecv(void * bindData, const char * pData, int nLen)
{
	CDataProc::GetInstance()->Data((char*)pData, nLen);
	return true;
}

bool CHmsGDL90Interface::OnRecv(unsigned int nIp, unsigned short nPort, const char * pData, int nLen)
{
	if (m_nRecvUdpIp != nIp)
	{
		m_nRecvUdpIp = nIp;
	}
	if (nPort != m_nRecvUdpPort)
	{
		m_nRecvUdpPort = nPort;
	}
    CDataProc::GetInstance()->Data((char*)pData, nLen);
    return true;
}

void CHmsGDL90Interface::SendSaveMagneticCorrectionMsg()
{
	char strMsg[11] = { 0x4C, 0x45, 0x0F, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	SendGDL90Data(strMsg, 11);
}

void CHmsGDL90Interface::SendEraseMagneticCorrectionMsg()
{
	char strMsg[11] = { 0x4C, 0x45, 0x0F, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	SendGDL90Data(strMsg, 11);
}

void CHmsGDL90Interface::SendKeepLinkStatusMsg()
{
    //char strMsg[11] = { 0x4C, 0x45, 0x0F, 0x01, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    //the error msg of read the pressure sensor

    char strMsg[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SendGDL90Data(strMsg, 7);
}

void CHmsGDL90Interface::SendGDL90Data(const char * pData, int nLen)
{
	char strBuffer[1024] = { 0 };
	int nPos = 0;

	strBuffer[nPos++] = 0x7E;

	//change message.
	for (int i = 0; i < nLen; ++i)
	{
		switch (pData[i])
		{
		case 0x7E:
			strBuffer[nPos++] = 0x7D;
			strBuffer[nPos++] = 0x5E;
			break;
		case 0x7D:
			strBuffer[nPos++] = 0x7D;
			strBuffer[nPos++] = 0x5D;
			break;
		default:
			strBuffer[nPos++] = pData[i];
			break;
		}
	}

	unsigned short _crc = CDataProc::GetInstance()->MatchCRC(pData, nLen);
	unsigned char tempCrc[2];
	tempCrc[0] = (unsigned char)(_crc);
	tempCrc[1] = (unsigned char)((_crc & 0xFF00) >> 8);

	for (int i = 0; i < 2; ++i)
	{
		switch (tempCrc[i])
		{
		case 0x7E:
			strBuffer[nPos++] = 0x7D;
			strBuffer[nPos++] = 0x5E;
			break;
		case 0x7D:
			strBuffer[nPos++] = 0x7D;
			strBuffer[nPos++] = 0x5D;
			break;
		default:
			strBuffer[nPos++] = tempCrc[i];
			break;
		}
	}

	strBuffer[nPos++] = 0x7E;

	if (m_nRecvUdpIp != 0)
	{
		this->SendUdpData(m_nRecvUdpIp, m_nRecvUdpPort, strBuffer, nPos);
	}
	SendData(nullptr, strBuffer, nPos);
}

bool CHmsGDL90Interface::OnAccept(void * bindData)
{
	CHmsGlobal::GetInstance()->GetDataBus()->SetDeviceConnect(true);
	HMSLOG("GDL90 Device connected!\n");
	return true;
}

bool CHmsGDL90Interface::OnClose(void * bindData)
{
	CHmsGlobal::GetInstance()->GetDataBus()->SetDeviceConnect(false);
	HMSLOG("GDL90 Device disconnected!\n");
	return true;
}

float TirmAttitudeData(float fData, float fMax, float fMin); 
float TrimData(float fData, float fLimit);
float TrimData(float fData, float fMax, float fMin);

bool CHmsGDL90Interface::AftDataUpdate(HmsAftData * pData)
{
	if (!pData)
	{
		return false;
	}
	AftDataiLevil tempData = { 0 };
	bool bRun = CDataProc::GetInstance()->GetData(&tempData);
	HmsAftData & m_data = *pData;

	if (bRun)
	{
		if (tempData.bLatLonValid)
		{
			m_data.bLatLonValid = true;			///whether or not latitude and longitude is valid: 0 :false 1:true.
			m_data.latitude = tempData.ownshipReport.latitude;				///latitude,  unit degree.
			m_data.longitude = tempData.ownshipReport.longitude;				///longitude, unit degree.
			m_data.altitudeFt = tempData.ownshipReport.altitude;

			m_data.verticalSpeed = tempData.ownshipReport.verticalVelocity;
			m_data.groundSpeed = TrimData(tempData.ownshipReport.horizontalVelocity, 999, 0);


			m_data.geoAltitudeFt = tempData.geoAltitude.altitude;

			if (tempData.ownshipReport.tt == 1)
			{
				m_data.bGpsTrackValid = true;
				m_data.trueTrack = tempData.ownshipReport.trackOrHeading;
				if (m_data.trueTrack < 0)
				{
					m_data.trueTrack += 360;
				}
				m_data.nTrueTrackDataFrom = 1;
			}
			else
			{
				m_data.bGpsTrackValid = false;
                m_data.trueTrack = m_data.simTrack;
                m_data.nTrueTrackDataFrom = 2;
			}

			if (tempData.eDeviceType == AHRS_DEVICE_TYPE::iLevil)
			{
				m_data.mslAltitudeFt = tempData.geoAltitude.altitude + tempData.statusGpsMsg.fAltitudeVariation;
				m_data.altitudeFt = m_data.mslAltitudeFt;
			}
			else
			{
				m_data.mslAltitudeFt = tempData.geoAltitude.altitude;
			}
		}
		else
		{
			m_data.bLatLonValid = false;
// 			m_data.altitudeFt = tempData.ownshipReport.altitude;
// 			m_data.geoAltitudeFt = tempData.geoAltitude.altitude;
// 			m_data.mslAltitudeFt = tempData.ownshipReport.altitude;
		}

		//common AHRS data
		m_data.roll = tempData.ahrsAirMsg.Roll;						///roll angle,unit degree [-180, 180].
		m_data.pitch = tempData.ahrsAirMsg.Pitch;					///pitch angle, unit degree [-90, 90].
		m_data.magneticHeading = tempData.ahrsAirMsg.Yaw;						///yaw(heading),unit degree [0,  360].
		m_data.sideSliAngle = TrimData(-tempData.ahrsAirMsg.TurnCoordinator, 45) / (45.0f*2.0f);				///Slip-indicator,unit degree[-90, 90].
		m_data.yawRate = tempData.ahrsAirMsg.YawRate;					///Yaw rate, unit degree/sec.					

		m_data.GLode = tempData.ahrsAirMsg.GLoad;

		if (tempData.eDeviceType == AHRS_DEVICE_TYPE::iLevil)
		{
			m_data.bValidAirData = true;
			//m_data.verticalSpeed = tempData.ahrsAirMsg.VerticalSpeed;										///unit feet/min[-32,000 to 32,000 feet/min].
			m_data.IndicatedAirspeed = tempData.ahrsAirMsg.IndicatedAirspeed;								///speed, unit KNOT[(-3200 to 3200 KTS].
			m_data.pressureAltitudeFt = tempData.ahrsAirMsg.PressureAltitude;									///Altitude in feet at 29.92 inHg.Unsigned word where sea level is represented by 5000 ft[-5,000 to 60,000 feet].
		}
		else
		{
			m_data.IndicatedAirspeed = tempData.ownshipReport.horizontalVelocity;
			m_data.pressureAltitudeFt = tempData.geoAltitude.altitude;
			m_data.bValidAirData = false;
		}

		m_data.version = tempData.statusMsg.version;							///iLevil version.
		m_data.batteryLevel = tempData.statusMsg.batteryLevel;						///battery Level[0, 100].
		m_data.bDeviceIsCharging = tempData.statusMsg.isCharging;


		m_data.trueHeading = HmsMagneticHeadingToTrueHeading(m_data.longitude, m_data.latitude, FeetToMeter(m_data.mslAltitudeFt) / 1000, m_data.magneticHeading);
		m_data.nTrueHeadingDataFrom = 2;

		return true;
	}
	return false;
}

void CHmsGDL90Interface::GetADSBData(std::vector<HmsADSBShortTraffic> &vec)
{
    std::vector<ShortTrafficObjectInfo> vecShortTraffic;
    CDataProc::GetInstance()->GetShortTraffic(vecShortTraffic);

    for (auto &stu : vecShortTraffic)
    {
        char callsign[9] = { 0 };
        memcpy(callsign, stu.callsign, sizeof(stu.callsign));

        if (strlen(callsign) > 0)
        {
            HmsADSBShortTraffic adsb;
            adsb.callsign = callsign;
            adsb.longitude = stu.longitude;
            adsb.latitude = stu.latitude;
            adsb.altitude = stu.altitude;
            adsb.trackHeading = stu.trackHeading;
            vec.push_back(adsb);
        }
    }
}
