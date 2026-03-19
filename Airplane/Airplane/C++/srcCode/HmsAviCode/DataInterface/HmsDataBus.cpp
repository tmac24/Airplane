#include "HmsDataBus.h"
#include "DataInterface/network/GDL90/DataProc.h"
#include <string>
#include "network/SimDataInterface.h"
#include "network/HmsNetworks.h"
#include "HmsGE/external/wmm/HmsMagneticDecl.h"
#include "Calculate/CalculateZs.h"
#include "HmsConfigXMLReader.h"
#define METET_PER_FEET    0.3048f

CHmsDataBus::CHmsDataBus()
{
	Init();

	m_filterSpeed.SetInitValue(m_data.groundSpeed);
	m_filterAlt.SetInitValue(m_data.geoAltitudeFt);
	m_filterVerticalSpeed.SetInitValue(m_data.verticalSpeed);
	m_fliterYaw.SetInitValue(m_data.magneticHeading);
}

CHmsDataBus::~CHmsDataBus()
{
}

void CHmsDataBus::Init()
{
	memset(&m_data, 0, sizeof(m_data));
    /*
	m_data.latitude = 30.58;
	m_data.longitude = 103.92;
	m_data.pressureAltitude = 3560;
	m_data.airspeed = 80;
     */
    //m_data.latitude = 40 + 17.0/60 + 3.0/(60*60);
    //m_data.longitude = 116 + 13.0/60 + 55.0/(60*60);
#if 0
    m_data.latitude = 40.288864;
    m_data.longitude = 116.233122;
#else
	//m_data.latitude = 30.624328;
	//m_data.longitude = 104.066778;

	auto config = CHmsConfigXMLReader::GetSingleConfig();

	m_data.latitude = 40.242245;//40.24150000;//观看电塔处
	m_data.longitude = 116.311946;//116.31283333;
	m_data.altitudeFt = 872;//791
	

	m_data.latitude = config->GetConfigInfo("DataBusInitData/Latitude", m_data.latitude);
	m_data.longitude = config->GetConfigInfo("DataBusInitData/Longitude", m_data.longitude);
	m_data.altitudeFt = config->GetConfigInfo("DataBusInitData/Altitude", m_data.altitudeFt);

	m_data.pressureAltitudeFt = m_data.altitudeFt;//ft
	m_data.geoAltitudeFt = m_data.altitudeFt;//ft
	m_data.mslAltitudeFt = m_data.altitudeFt;
	CalculateTrueHeading();

#endif
    //m_data.pressureAltitude = 395;
    //m_data.geoAltitude = 396;
    m_data.groundSpeed = 2;
	m_data.bDeviceConnected = false;
	m_data.bDeviceIsCharging = false;

	m_dataCorrection.pitch = 0;
	m_dataCorrection.roll = 0;

	m_bUseDataCorrrection = false;

	m_duControlData = new CHmsDuControlData();
}

float TirmAttitudeData(float fData, float fMax, float fMin)
{
	float fOut = 0;

	if (fData < fMin)
	{
		fOut = fMax + (fData - fMin);
	}
	else if (fData > fMax)
	{
		fOut = fMin + (fData - fMax);
	}
	else
	{
		fOut = fData;
	}
	return fOut;
}

float TrimData(float fData, float fLimit)
{
	float fOut = fData;
	if (fOut > fLimit)
	{
		fOut = fLimit;
	}
	else if (fOut < -fLimit)
	{
		fOut = -fLimit;
	}
	return fOut;
}

float TrimData(float fData, float fMax, float fMin)
{
	float fOut = fData;
	if (fOut < fMin)
	{
		fOut = fMin;
	}
	else if (fOut > fMax)
	{
		fOut = fMax;
	}
	return fOut;
}

extern HmsNetworks g_hmsNetwork;
void CHmsDataBus::UpdateDataBus(float delta)
{
    static double s_updateSecond = 0;
    s_updateSecond += delta;

	if (g_hmsNetwork.getInterface() != nullptr)
	{
		if (g_hmsNetwork.getInterface()->AftDataUpdate(&m_data))
		{
			if (m_bUseDataCorrrection)
			{
				m_data.roll += m_dataCorrection.roll;
				m_data.pitch += m_dataCorrection.pitch;

				m_data.roll = TirmAttitudeData(m_data.roll, 180, -180);
				m_data.pitch = TirmAttitudeData(m_data.pitch, 90, -90);
			}

			FillData();
		}

        std::vector<HmsADSBShortTraffic> vecADSB;
        g_hmsNetwork.getInterface()->GetADSBData(vecADSB);
        for (auto &stu : vecADSB)
        {
            stu.updateTime = s_updateSecond;
            m_mapADSBShortTraffic[stu.callsign] = stu;
        }
        static double s_updateSecondLast = 0;
        if (s_updateSecond - s_updateSecondLast > 1)
        {
            s_updateSecondLast = s_updateSecond;
            for (auto iter = m_mapADSBShortTraffic.begin(); iter != m_mapADSBShortTraffic.end();)
            {
                if (s_updateSecond - iter->second.updateTime > 3)
                {
                    iter = m_mapADSBShortTraffic.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }
	}
}
	
float CHmsDataBus::GetAltitudeMeter()
{
	return m_data.pressureAltitudeFt * METET_PER_FEET;
}

float CHmsDataBus::GetAltitudeFeet()
{
	return m_data.pressureAltitudeFt;
}

float CHmsDataBus::GetGeoAltitudeMeter()
{
	return m_data.geoAltitudeFt * METET_PER_FEET;
}

float CHmsDataBus::GetGeoAltitudeFeet()
{
	return m_data.geoAltitudeFt;
}

float CHmsDataBus::GetMslAltitudeMeter()
{
	return m_data.mslAltitudeFt * METET_PER_FEET;
}

float CHmsDataBus::GetMslAltitudeFeet()
{
	return m_data.mslAltitudeFt;
}

float CHmsDataBus::GetHeightMeter()
{
    return m_data.heightAboveTerrainMeter;
}

float CHmsDataBus::GetHeightFeet()
{
    return m_data.heightAboveTerrainMeter / METET_PER_FEET;
}

void CHmsDataBus::SetDeviceConnect(bool bConnected)
{
	m_data.bDeviceConnected = bConnected;
}

void CHmsDataBus::SetDataCorrection(bool bUse)
{
	m_bUseDataCorrrection = bUse;
	if (m_bUseDataCorrrection)
	{
		m_dataCorrection.pitch = -m_data.pitch;
		m_dataCorrection.roll = -m_data.roll;
	}
}

void CHmsDataBus::SetHeight(float height)
{
    m_data.heightAboveTerrainMeter = height;
    if (!m_data.bHeightAboveTerrainValid)//bHeightValid todo
    {
        m_data.bHeightAboveTerrainValid = true;
    }
}

void CHmsDataBus::SetCommunicationType(char cType)
{
	if (cType == 'B')
	{
		m_data.cCommunicationType = 'B';
	}
	else if ((cType == 'T') || (cType == 'U') || (cType == 'N') || (cType == 'W'))
	{
		m_data.cOrgCommunicationType = cType;
		m_data.cCommunicationType = 'W';
	}
}

void CHmsDataBus::SetProtocalType(char cType)
{
	m_data.cProtocalType = cType;
}

void CHmsDataBus::CalculateTrueHeading()
{
	m_data.trueHeading = HmsMagneticHeadingToTrueHeading(m_data.longitude, m_data.latitude, FeetToMeter(m_data.mslAltitudeFt) / 1000, m_data.magneticHeading);
	m_data.nTrueHeadingDataFrom = 2;
}

void CHmsDataBus::FillData()
{
	m_data.groundSpeed = m_filterSpeed.GetFilterValue(m_data.groundSpeed);
	m_data.geoAltitudeFt = m_filterAlt.GetFilterValue(m_data.geoAltitudeFt);
	m_data.verticalSpeed = m_filterVerticalSpeed.GetFilterValue(m_data.verticalSpeed);
	m_data.magneticHeading = m_fliterYaw.GetCicle0360FilterValue(m_data.magneticHeading);
}
