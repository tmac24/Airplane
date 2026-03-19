#include "DataProc.h"
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>
#include "HmsLog.h"
#include <string.h>

#define MIN(a, b) (((a)>(b)) ? (b) : (a))
#define BUFSIZE  128
#define CONSTVAL 256
#define TENTH    0.1f

#define PRINT_DOCODE_LOG 0

inline short MakeShort(char a, char b)
{
	return (((unsigned char)a << 8) | (unsigned char)b);
}

inline short MakeShortFirst12Bit(char a, char b)
{
    return (((unsigned char)a << 4) | (unsigned char)b >> 4);
}

inline unsigned short MakeUnsignedShort(char a, char b)
{
	return (((unsigned char)a << 8) | (unsigned char)b);
}

#if 0
CDataProc::CDataProc()
: m_buffer(0)
, m_total(BUFSIZE)
, m_curent(0)
, m_bMakeMsg(false)
, m_MakeIndex(2)
, m_bIsRunning(false)
{
	m_buffer = new char[m_total+1];
	memset(m_buffer, 0x00, sizeof(char)*(m_total+1));
	CreateCRC();

}
#else
CDataProc::CDataProc()
	: m_bIsRunning(false)
	, m_eDeviceType(AHRS_DEVICE_TYPE::noDevice)
	, m_nCmdReply(0)
{
	memset(&m_data, 0, sizeof(m_data));
	CreateCRC();
}
#endif

CDataProc::~CDataProc()
{
	m_bufferRemain.Empty();
}


CDataProc* CDataProc::GetInstance()
{
	static CDataProc _instance;
	return &_instance;
}

void CDataProc::CreateCRC()
{
	unsigned short crc = 0;
	for (unsigned short i = 0; i < 256; i++)
	{
		crc = (i << 8);
		for (unsigned short bit = 0; bit < 8; bit++)
		{
			crc = (crc << 1) ^ ((crc & 0x8000) ? 0x1021 : 0);
		}

		m_vCrc.push_back(crc);
	}
}

unsigned short CDataProc::MatchCRC(const char* pdata, unsigned short len)
{
	unsigned short crc = 0;

	for (unsigned short i = 0; i < len; i++)
	{
		unsigned short t = m_vCrc[(crc >> 8) & 0xFF];
		crc = t ^ (crc << 8) ^ (unsigned char)(pdata[i]);
	}

	return crc;
}

void CDataProc::Data(const char *pdata, int len)
{
	m_dataRecord.OnDataRecv((void*)pdata, len);

	if (m_bufferRemain.GetLength() > 0)
	{
		CBuffer buffer(pdata, len);
		m_bufferRemain += buffer;
	}
	else
	{
		m_bufferRemain.SetData(pdata, len);
	}

	std::vector<CBuffer*> vMsg = SegmentationMsgs();
	{
		CHmsCSLocker locker(m_cs);
		for (auto c : vMsg)
		{
			if (c)
			{
				DecodeMsg(*c);
				delete c;
			}
		}
	}
	vMsg.clear();

// 	for (int i = 0; i < len; ++i)
// 	{
// 		MakeMessage(pdata[i]);
// 	}
 	m_bIsRunning = true;
}

std::vector<CBuffer*> CDataProc::SegmentationMsgs()
{
	auto buffer = m_bufferRemain.GetBuffer();
	int nLen = m_bufferRemain.GetLength();
	int nMsgBegin = 0;
	int nMsgEnd = 0;

	std::vector<CBuffer*> vBuffer;

	for (int i = 0; i < nLen; i++)
	{
		//skip the bad data, that is not begin with 0x7E
		while ((buffer[i] != 0x7E) && (i < nLen))
			i++;

		//if the 0x7E, is the last message's end, throw it away
		if (buffer[i + 1] == 0x7E)
			i++;

		nMsgBegin = i;

		//find the message's end
		i++;
		while ((buffer[i] != 0x7E) && (i < nLen))
			i++;

        if (i < nLen && buffer[i] == 0x7E)
		{
			nMsgEnd = i;
			if (nMsgEnd != nMsgBegin)
			{
				CBuffer * tempBuffer = new CBuffer(buffer + nMsgBegin, nMsgEnd - nMsgBegin);
				vBuffer.push_back(tempBuffer);
			}
		}
		else
		{
			if (nMsgBegin < nLen)
			{
				CBuffer temp(buffer + nMsgBegin, nLen - nMsgBegin);
				m_bufferRemain = temp;
				return vBuffer;
			}
			break;
		}
	}
	m_bufferRemain.Empty();
	return vBuffer;
}

void CDataProc::DecodeMsg(CBuffer & bufferMsg)
{
	//Trim Buffer
	//"0x7D 0x5E" becomes 0x7E.
	//"0x7D 0x5D" becomes 0x7D.
	char * buffer = bufferMsg.GetBuffer();
	int nLen = bufferMsg.GetLength();
	int i = 0;
	while (i<nLen)
	{
		if (buffer[i] == 0x7D)
		{
			switch (buffer[i + 1])
			{
			case 0x5E:
			{
				buffer[i] = 0x7E;
				if(i+2 < nLen)
				{
					memmove(buffer + i + 1, buffer + i + 2, nLen - i - 2);
				}
				nLen--;
				i++;
				break;
			}

			case 0x5D:
				//buffer[i] = 0x7D;
				if(i+2 < nLen)
				{
					memmove(buffer + i + 1, buffer + i + 2, nLen - i - 2);
				}
				nLen--;
				i++;
				break;
			}
		}
		i++;
	}
	
	ResolveData(buffer + 1, nLen - 1);
}



void CDataProc::ResolveData(const char *pdata, int len)
{
	if (pdata == nullptr || len <= 2)
	{
		return;
	}

	//check crc
	unsigned short crc = MatchCRC(pdata, len - 2);

	if (((unsigned char)pdata[len - 2] == (crc & 0xFF)) && ((unsigned char)pdata[len - 1] == ((crc & 0xFF00) >> 8)))
	{
		switch ((unsigned char)pdata[0])
		{
		case 0x00: Handle_HeartBeat(pdata, len);   break;//ADS-B heartbeat and GPS fix flag
		case 0x0A:
			Handle_Ownship(pdata, len);
			break;// GPS Lat,Long,track,speed..(DO falseT use altitude reported here)
		case 0x8A:
			Handle_Hms_Ownship(pdata, len);
			break;
		case 0x0B:
			Handle_GeoAltitude(pdata, len);
			break; // GPS altitude
		case 0x8B:
			Handle_Hms_GeoAltitude(pdata, len);
			break;
		case 0x07: 
			//Handle_UplinkMessage(pdata, len);
			break;//ADS-B weather
		case 0x14: 
			Handle_ShortTrafficReport(pdata, len);
			break;//ADS-B traffic
		case 0x1E:
		case 0x1F:
			//Handle_BasicAndLongTrafficReport(pdata, len);
			break;
		case 0x7A:
        {
            Handle_XGPS190_DeviceReport(pdata, len);
            break;
        }
		case 0x7C:
			Handle_DeviceResponse(pdata, len);
			break;
        case 0x79:
        {
            //Handle_XGPS190_DetailedSatData(pdata, len);
            break;
		}
		case 0x4C:
		{
			if (pdata[1] == 0x45)//Levil data.
			{
				switch (pdata[2])
				{
				case 0x00: Handle_LevilStatus(pdata, len); break;//battery percent, firmware version etc
				case 0x01: Handle_LevilAHRS(pdata, len);   break;//roll,pitch,heading etc..
				case 0x07: Handle_LevilGpsStatus(pdata, len);   break;//Gps Status
				case 0x05: Handle_LevilEngine(pdata, len);      break;//engine information
				case 0x0A: Handle_LevilAUX(pdata, len);    break;//miscellaneous data embedded in GDL90 format
				case 0x0F: Handle_LevilConfigurationCommand(pdata, len); break;
				default:   break;
				}
			}
			break;
		}
		case 0x44:
		{
			if (pdata[1] == 0x4E)			//XGPS190 AHRS Message Format 鈥?Preliminary V2
			{
				switch (pdata[2])
				{
					case 0x01:
						Handle_XGPS190_AHRS_V2(pdata, len);
						break;
					default:
						break;
				}
			}
		}
		default:   break;
		}
	}
	else
	{
		int a = 0;
	}
}

void CDataProc::Handle_HeartBeat(const char *pdata, int len)
{
	if (len >= 5) 
	{   
        //determine if there is GPS fix:
		if (pdata[1] & 0x80){
			m_data.bLatLonDecodeValid = true;
		}
        else
        {
            m_data.bLatLonDecodeValid = false;
            m_data.bLatLonValid = false;
        }

		m_data.nUtcSeconds = ((pdata[2] & 0x80) << 9) + (pdata[3]&0xFF) + ((pdata[4]&0xFF) << 8);
	}
}

short GetLowerShortdata(char cH, char cL, short dataMask, short checkMask)
{
	short dataOut = MakeShort(cH, cL) & dataMask;
	if (dataOut & checkMask)
	{
		dataOut |= (~dataMask);
	}
	return dataOut;
}

void CDataProc::Handle_Ownship(const char *pdata, int len)
{
	if (len >= 17 && m_data.bLatLonDecodeValid)
	{
        long long nTempCheck = 0;
        memcpy(&nTempCheck, pdata + 5, 6);
        if (nTempCheck == 0)
        {
            m_data.bLatLonValid = false;
            return;
        }
        m_data.bLatLonValid = true;
		//Latitude.
		int tempInt = (pdata[5] << 16) + (((unsigned char)pdata[6]) << 8) + (unsigned char)pdata[7];
		m_data.ownshipReport.latitude = (tempInt & 0x800000)
			? ((-1)*(double)(((tempInt ^ 0xFFFFFF) + 1) * 90.0f) / 4194304.0f)
			: ((double)((double)tempInt * 90.0f) / 4194304.0f);

		//Longitude.
		tempInt = (pdata[8] << 16) + (((unsigned char)pdata[9]) << 8) + (unsigned char)pdata[10];
		m_data.ownshipReport.longitude = (tempInt & 0x800000)
			? ((-1)*((double)(((tempInt ^ 0xFFFFFF) + 1) * 90.0f) / 4194304.0f)- 360)
			: (((double)((double)tempInt * 90.0f) / 4194304.0f) /*- 360*/);

        short tempData = MakeShortFirst12Bit(pdata[11], pdata[12]);
        if ((tempData & 0xFFF) != 0xFFF)
        {
            m_data.ownshipReport.altitude = tempData * 25 - 1000;
            m_data.ownshipReport.bAltitudeValid = 1;
        }
        else
        {
            m_data.ownshipReport.bAltitudeValid = 0;
        }
		//m_data.ownshipReport.altitude = (short)((((unsigned short)pdata[11]) << 4) + ((((unsigned short)pdata[12]) & 0xF0) >> 4)) * 25 - 1000;

		m_data.ownshipReport.uMiscellaneousIndicators = (pdata[12] & 0x0F);

		//ground speed.
        tempData = MakeShortFirst12Bit(pdata[14], pdata[15]);
        if ((tempData & 0xFFF) != 0xFFF)
        {
            m_data.ownshipReport.horizontalVelocity = tempData;
            m_data.ownshipReport.bHorizontalVelocityValid = 1;
        }
        else
        {
            m_data.ownshipReport.bHorizontalVelocityValid = 0;
        }
		//m_data.ownshipReport.horizontalVelocity = (((unsigned short)pdata[14]) << 4) + ((((unsigned short)pdata[15]) & 0xF0) >> 4);

		//GPS vertical speed.
		//the right 
		//m_data.ownshipReport.verticalVelocity = GetLowerShortdata(pdata[15], pdata[16], 0x0FFF, 0x0800) * 64;
		//the fixed code for xgps
		m_data.ownshipReport.verticalVelocity = pdata[16] * 64;

		//heading.
		m_data.ownshipReport.trackOrHeading = (unsigned char)pdata[17] *(360.0 / 256);
	}
}

void CDataProc::Handle_GeoAltitude(const char *pdata, int len)
{
	if (len > 2)
	{
		m_data.geoAltitude.altitude = ((short(pdata[1]) << 8) + (unsigned char)pdata[2]) * 5;

        auto temp = MakeUnsignedShort(pdata[3], pdata[4]);
        m_data.geoAltitude.bVerticalWarnng = (temp & 0x8000)?1:0;
        temp &= 0x7FFF;
        if (temp != 0x7FFF)
        {
            m_data.geoAltitude.meterVFOM = temp;
            m_data.geoAltitude.bMeterVFOMValid = 1;
        }
        else
        {
            m_data.geoAltitude.bMeterVFOMValid = 0;
        }
	}
}

void CDataProc::Handle_LevilStatus(const char *pdata, int len)
{
	if (len >= 10)
	{
		m_data.statusMsg.version = ((float)pdata[4]) / 10.0;
		m_data.statusMsg.batteryLevel = (int)pdata[5];
		m_data.statusMsg.uErrorFlag = (unsigned short)pdata[6] * 256 + pdata[7];
		
		if (pdata[3] == 0x02)
		{
			//check GPS status
			m_data.statusMsg.WaasGpsStatus = pdata[8];
			m_data.statusMsg.AuxPortType = pdata[9];
		}

		SetDeviceType(AHRS_DEVICE_TYPE::iLevil);
	}
}

void CDataProc::Handle_LevilAHRS  (const char *pdata, int len)
{
#if 0
	if ((len < 24) ||
		(pdata[4] == 0x7F && (unsigned char)pdata[5] == 0xFF) ||
		(pdata[6] == 0x7F && (unsigned char)pdata[7] == 0xFF) ||
		(pdata[8] == 0x7F && (unsigned char)pdata[9] == 0xFF))
	{
		return;
	}

	//self.protocol = @"GDL90";
	m_data.roll  = (float)(((short)pdata[4]) * CONSTVAL + ((unsigned char)pdata[5])) * TENTH;
	m_data.pitch = (float)(((short)pdata[6]) * CONSTVAL + ((unsigned char)pdata[7])) * TENTH;
	m_data.yaw   = (float)(((short)pdata[8]) * CONSTVAL + ((unsigned char)pdata[9])) * TENTH;
	m_data.sideSliAngle = (float)(((short)pdata[10]) * CONSTVAL + ((unsigned char)pdata[11])) * TENTH;
	m_data.yawRate = (float)(((short)pdata[12]) * CONSTVAL + ((unsigned char)pdata[13])) * TENTH;
	float gForces = (float)(((short)pdata[14]) * CONSTVAL + ((unsigned char)pdata[15])) * TENTH;

	m_data.airspeed = (float)(((short)pdata[16]) * CONSTVAL + ((unsigned char)pdata[17]))* TENTH;
	m_data.altitude = (((short)pdata[18]) * CONSTVAL + ((unsigned char)pdata[19]));
	m_data.Verticalspeed = (((short)pdata[20]) * CONSTVAL + ((unsigned char)pdata[21]));

	if ((m_data.airspeed != 0x7FFF) && (m_data.altitude != 0xFFFF) && (m_data.Verticalspeed != 0x7FFF))
	{
		m_data.airspeed *= TENTH;
		m_data.altitude -= 5000;
	}
#else
	if (len >= 24)
	{
		if (pdata[4] == 0x7F && pdata[5] == 0xFF){
			m_data.ahrsAirMsg.bRollValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bRollValid = 0x01;
			m_data.ahrsAirMsg.Roll = MakeShort(pdata[4], pdata[5]) * 0.1f;
		}

		if (pdata[6] == 0x7F && pdata[7] == 0xFF){
			m_data.ahrsAirMsg.bPitchValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bPitchValid = 0x01;
			m_data.ahrsAirMsg.Pitch = MakeShort(pdata[6], pdata[7]) * 0.1f;
		}

		if (pdata[8] == 0x7F && pdata[9] == 0xFF){
			m_data.ahrsAirMsg.bYawValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bYawValid = 0x01;
			m_data.ahrsAirMsg.Yaw = MakeShort(pdata[8], pdata[9]) * 0.1f;
		}

		if (pdata[10] == 0x7F && pdata[11] == 0xFF){
			m_data.ahrsAirMsg.bInclinationValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bInclinationValid = 0x01;
			m_data.ahrsAirMsg.YawRate = MakeShort(pdata[10], pdata[11]) * 0.1f;
		}

		if (pdata[12] == 0x7F && pdata[13] == 0xFF){
			m_data.ahrsAirMsg.bTurnCoordinatorValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bTurnCoordinatorValid = 0x01;
			m_data.ahrsAirMsg.TurnCoordinator = MakeShort(pdata[12], pdata[13]) * 0.1f;
		}

		if (pdata[14] == 0x7F && pdata[15] == 0xFF){
			m_data.ahrsAirMsg.bGLoadValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bGLoadValid = 0x01;
			m_data.ahrsAirMsg.GLoad = MakeShort(pdata[14], pdata[15]) * 0.1f;
		}

		if (pdata[16] == 0x7F && pdata[17] == 0xFF){
			m_data.ahrsAirMsg.bIndicatedAirspeedValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bIndicatedAirspeedValid = 0x01;
			m_data.ahrsAirMsg.IndicatedAirspeed = MakeShort(pdata[16], pdata[17]) * 0.1f;
		}

		if (pdata[18] == 0xFF && pdata[19] == 0xFF){
			m_data.ahrsAirMsg.bPressureAltitudeValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bPressureAltitudeValid = 0x01;
			m_data.ahrsAirMsg.PressureAltitude = MakeUnsignedShort(pdata[18], pdata[19]) - 5000;
		}

		if (pdata[20] == 0x7F && pdata[21] == 0xFF){
			m_data.ahrsAirMsg.bVerticalSpeedValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bVerticalSpeedValid = 0x01;
			m_data.ahrsAirMsg.VerticalSpeed = MakeShort(pdata[20], pdata[21]);
		}
	}
#endif
}

void CDataProc::Handle_LevilEngine(const char *pdata, int len)
{
	if ( len<60 ){ return; }

	unsigned short Tachometer = ((unsigned short)(pdata[4])) * CONSTVAL + (unsigned char)pdata[5];
	unsigned char OilPressure = pdata[6];
	short OilTemperature = ((short)(pdata[7])) * CONSTVAL + (unsigned char)pdata[8];
	unsigned short CoolantTemp = ((unsigned short)(pdata[9])) * CONSTVAL + (unsigned char)pdata[10];
	unsigned short FuelFlow = ((unsigned short)(pdata[11])) * CONSTVAL + (unsigned char)pdata[12];
	unsigned char  OutSideTemp = (unsigned char)pdata[13] - 50;
	short chat1 = ((short)(pdata[14])) * CONSTVAL + pdata[15];
	short chat2 = ((short)(pdata[16])) * CONSTVAL + pdata[17];
	short chat3 = ((short)(pdata[18])) * CONSTVAL + pdata[19];
	short chat4 = ((short)(pdata[20])) * CONSTVAL + pdata[21];
	short chat5 = ((short)(pdata[22])) * CONSTVAL + pdata[23];
	short chat6 = ((short)(pdata[24])) * CONSTVAL + pdata[25];
	short egt1 = ((short)(pdata[26])) * CONSTVAL + pdata[27];
	short egt2 = ((short)(pdata[28])) * CONSTVAL + pdata[29];
	short egt3 = ((short)(pdata[30])) * CONSTVAL + pdata[31];
	short egt4 = ((short)(pdata[32])) * CONSTVAL + pdata[33];
	short egt5 = ((short)(pdata[34])) * CONSTVAL + pdata[35];
	short egt6 = ((short)(pdata[36])) * CONSTVAL + pdata[37];
	float volts = (float)(((unsigned short)pdata[38]) * CONSTVAL + (unsigned char)pdata[39]) / 10.0;
	short aux1 = ((short)(pdata[40])) * CONSTVAL + pdata[41];
	short aux2 = ((short)(pdata[42])) * CONSTVAL + pdata[43];
	short aux3 = ((short)(pdata[44])) * CONSTVAL + pdata[45];
	short aux4 = ((short)(pdata[46])) * CONSTVAL + pdata[47];
	short aux5 = ((short)(pdata[48])) * CONSTVAL + pdata[49];
	short aux6 = ((short)(pdata[50])) * CONSTVAL + pdata[51];
	char CarbHeat = pdata[52];
	float FuelQty = (float)(((unsigned short)pdata[53]) * CONSTVAL + (unsigned char)pdata[54]) / 10.0;
	float HourMeter = (float)(((unsigned short)pdata[55]) * CONSTVAL + (unsigned char)pdata[56]) / 10.0;
	float BarometricPressure = (float)(((unsigned short)pdata[57]) * CONSTVAL + (unsigned char)pdata[58]) / 1000.0;
	unsigned short FuelFlowTimeUtilEmptyHours = (unsigned short)pdata[59];
	unsigned short FuelFlowTimeUtilEmptyMins  = (unsigned short)pdata[60];
}

void CDataProc::Handle_LevilGpsStatus(const char * pdata, int len)
{
	if (len >= 15)
	{
		m_data.statusGpsMsg.WaasStatus = pdata[4];
		m_data.statusGpsMsg.nSatellites = pdata[5];
		m_data.statusGpsMsg.power = MakeShort(pdata[6], pdata[7]) * 0.1f;
		m_data.statusGpsMsg.nOutputRate = pdata[8];
		m_data.statusGpsMsg.fAltitudeVariation = MakeShort(pdata[11], pdata[12]);
	}
}

void CDataProc::Handle_LevilAUX(const char *pdata, int len)
{

}

void CDataProc::Handle_LevilConfigurationCommand(const char *pdata, int len)
{
	if (len >= 10)
	{
		HMSLOG("configuration command reply package=%x cmd=%x", pdata[3], pdata[4]);
		if (pdata[3] == 0x01)
		{
			m_nCmdReply = pdata[4];
			switch (pdata[4])
			{
			case 0x0E:
				HMSLOG("Save Magnetic correction.");
				break;
			case 0x0F:
				HMSLOG("Erase Magnetic correction");
				break;
			case 0xFF:
				HMSLOG("CRC Error!");
				break;
			default:
				break;
			}
		}
	}
}

bool CDataProc::GetData(AftDataiLevil *pInfo)
{
	CHmsCSLocker locker(m_cs);
	if (pInfo && m_bIsRunning)
	{
		*pInfo = m_data;
		return true;
	}
	return false;
}

void CDataProc::GetShortTraffic(std::vector<ShortTrafficObjectInfo> &vec)
{
    CHmsCSLocker locker(m_cs);
    vec.insert(vec.end(), m_vecShortTraffic.begin(), m_vecShortTraffic.end());
    m_vecShortTraffic.clear();
}

int CDataProc::GetCmdPeply()
{
	int nRet = m_nCmdReply;
	m_nCmdReply = 0;
	return nRet;
}

void CDataProc::Handle_Hms_Ownship(const char *pdata, int len)
{
	if (len >= 27 && m_data.bLatLonDecodeValid)
	{
        m_data.bLatLonValid = true;
		//Latitude.
#if 0
		int tempInt = (pdata[5] << 16) + (((unsigned char)pdata[6]) << 8) + (unsigned char)pdata[7];
		m_data.ownshipReport.latitude = (tempInt & 0x800000)
			? ((-1)*(double)(((tempInt ^ 0xFFFFFF) + 1) * 90.0f) / 4194304.0f)
			: ((double)((double)tempInt * 90.0f) / 4194304.0f);

		//Longitude.
		tempInt = (pdata[8] << 16) + (((unsigned char)pdata[9]) << 8) + (unsigned char)pdata[10];
		m_data.ownshipReport.longitude = (tempInt & 0x800000)
			? ((-1)*((double)(((tempInt ^ 0xFFFFFF) + 1) * 90.0f) / 4194304.0f) - 360)
			: (((double)((double)tempInt * 90.0f) / 4194304.0f) /*- 360*/);
#else
		memcpy(&m_data.ownshipReport.latitude, pdata + 5, 8);
		memcpy(&m_data.ownshipReport.longitude, pdata + 5 + 8, 8);
#endif

		m_data.ownshipReport.altitude = (short)((((unsigned short)pdata[21]) << 4) + ((((unsigned short)pdata[22]) & 0xF0) >> 4)) * 25 - 1000;

		m_data.ownshipReport.uMiscellaneousIndicators = (pdata[22] & 0x0F);

		//ground speed.
		m_data.ownshipReport.horizontalVelocity = (((unsigned short)pdata[24]) << 4) + ((((unsigned short)pdata[25]) & 0xF0) >> 4);

#if 0
		//GPS vertical speed.
		m_data.ownshipReport.verticalVelocity = (short)(((unsigned short)pdata[26]) + ((((unsigned char)pdata[25]) & 0x0F) << 8)) * 64;
#else

		m_data.ownshipReport.verticalVelocity = GetLowerShortdata(pdata[25], pdata[26], 0x0FFF, 0x0800) * 64;

#endif

		//heading.
		m_data.ownshipReport.trackOrHeading = pdata[27] * (360 / 256);
	}
}

void CDataProc::Handle_Hms_GeoAltitude(const char *pdata, int len)
{
	if (len > 4)
	{
		//m_data.geoAltitude.altitude = ((short(pdata[1]) << 8) + (unsigned char)pdata[2]) * 5;
		memcpy(&m_data.geoAltitude.altitude, pdata + 1, 4);
	}
}

extern "C" void SendMsg(char * strMsg);

void CDataProc::Handle_XGPS190_AHRS_V2(const char *pdata, int len)
{
	if (len == 17)
	{
		if (pdata[3] == 0x7F && pdata[4] == 0xFF){
			m_data.ahrsAirMsg.bRollValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bRollValid = 0x01;
			m_data.ahrsAirMsg.Roll = MakeShort(pdata[3], pdata[4]) * 0.1f;
		}

		if (pdata[5] == 0x7F && pdata[6] == 0xFF){
			m_data.ahrsAirMsg.bPitchValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bPitchValid = 0x01;
			m_data.ahrsAirMsg.Pitch = MakeShort(pdata[5], pdata[6]) * 0.1f;
		}

		if (pdata[7] == 0x7F && pdata[8] == 0xFF){
			m_data.ahrsAirMsg.bYawValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bYawValid = 0x01;
			m_data.ahrsAirMsg.Yaw = MakeShort(pdata[7], pdata[8]) * 0.1f;
		}

#if 0
		m_data.ahrsAirMsg.bInclinationValid = 0x00;

		if (pdata[9] == 0x7F && pdata[10] == 0xFF){
			m_data.ahrsAirMsg.bTurnCoordinatorValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bTurnCoordinatorValid = 0x01;
			m_data.ahrsAirMsg.TurnCoordinator = MakeShort(pdata[9], pdata[10]) * 0.1f;
		}

		//Yaw Rate
		MakeShort(pdata[11], pdata[12]) * 0.1f;
#else
		if (pdata[9] == 0x7F && pdata[10] == 0xFF){
			m_data.ahrsAirMsg.bTurnCoordinatorValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bTurnCoordinatorValid = 0x01;
			m_data.ahrsAirMsg.TurnCoordinator = MakeShort(pdata[9], pdata[10]) * 0.1f;
		}

		m_data.ahrsAirMsg.YawRate = MakeShort(pdata[11], pdata[12]) * 0.1f;;
#endif

		if (pdata[13] == 0x7F && pdata[14] == 0xFF){
			m_data.ahrsAirMsg.bGLoadValid = 0x00;
		}
		else{
			m_data.ahrsAirMsg.bGLoadValid = 0x01;
			m_data.ahrsAirMsg.GLoad = MakeShort(pdata[13], pdata[14]) * 0.1f;
		}

#if 0
		char strOut[1024] = {0};
		sprintf(strOut, "AHRS: %.2f %.2f %.2f", m_data.ahrsAirMsg.Roll, m_data.ahrsAirMsg.Pitch, m_data.ahrsAirMsg.Yaw);
		SendMsg(strOut);
#endif
	}
}

void CDataProc::Handle_XGPS190_DeviceReport(const char *pdata, int len)
{
    /*
     <Byte#1 Upper Byte of Battery Voltage>
     <Byte#2 Lower Byte of Battery Voltage>
     <Byte#5 System Flags>
     */
    float batteryVoltage = 0.0f;
    bool isCharging = false;
    unsigned short vbat = 0;
    vbat = pdata[1] << 8;
    vbat |= (pdata[2] & 0xFF);
    float batLevel = (float)(vbat - 3500) / 600.0f;
    if (batLevel > 1.0) batteryVoltage = 1.0;
    else if (batLevel < 0) batteryVoltage = 0.0;
    else batteryVoltage = batLevel;

    if( pdata[5] & 0x04 )
        isCharging = true;

    m_data.statusMsg.version = 2.3f;
    m_data.statusMsg.batteryLevel = (int)(batteryVoltage*100);
	m_data.statusMsg.isCharging = isCharging;

	SetDeviceType(AHRS_DEVICE_TYPE::XGPS);
}

void CDataProc::SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

struct HmsGPSSatUsedDetail
{
	int nSatNum;
	int nSatElev;
	int nSatAzi;
	float fSatSnr;

	HmsGPSSatUsedDetail()
	{
		nSatNum = 0;
		nSatElev = 0;
		nSatAzi = 0;
		fSatSnr = 0;
	}
};

struct HmsGSPDetailedSatInfo
{
	int		fixType;				//extract whether the fix type is 1=no fix, 2=2D fix or 3=3D fix
	bool    bGpsPositionIsValid;		
	bool	bWassInUse;
	int		nNumberOfSatInUse;
	int		nNumberOfSatInView;
	int		nSatsUsedInPosCalc;
	float	fHdop;
	std::vector<int>	vSatInDOPCalc;
	std::vector<HmsGPSSatUsedDetail> vSatDetail;

	HmsGSPDetailedSatInfo()
	{
		fixType = 0;;
		bGpsPositionIsValid = false;
		bWassInUse = false;
		nNumberOfSatInUse = 0;
		nNumberOfSatInView = 0;
		nSatsUsedInPosCalc = 0;
		fHdop = 0.0f;
	}
};
#include <string>
void CDataProc::Handle_XGPS190_DetailedSatData(const char *pdata, int len)
{
	std::string str = std::string(pdata, len);
	std::vector<std::string> vSentences;

	SplitString(str, vSentences, "\0x0D\0x0A");
	if (vSentences.size() >= 1)
	{
		HmsGSPDetailedSatInfo satInfo;

		// First sentence is the PGSA sentence
		std::vector<std::string> vElementsInGPSASentence;
		std::string & strGPSA = vSentences.at(0);
		SplitString(strGPSA, vElementsInGPSASentence, ",");

		// Sentence will look like:
		//		eg1. PGSA,A,1,,,,,,,,,,,,,0.0,0.0,0.0*30
		//		eg2. PGSA,A,3,24,14,22,31,11,,,,,,,,3.7,2.3,2.9*3D
		//
		// Skytraq chipset:
		// e.g. PGSA,A,1,,,,,,,,,,,,,0.0,0.0,0.0*30     (no signal)
		//
		// uBlox chipset:
		// e.g. PGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30      (no signal)
		//      PGSA,A,3,02,29,13,12,48,10,25,05,,,,,3.93,2.06,3.35*0D

		/* Wikipedia (http://en.wikipedia.org/wiki/Dilution_of_precision_(GPS)) has a good synopsis on how to interpret
		DOP values:

		DOP Value	Rating		Description
		---------	---------	----------------------
		1			Ideal		This is the highest possible confidence level to be used for applications demanding
		the highest possible precision at all times.
		1-2		Excellent	At this confidence level, positional measurements are considered accurate enough to meet
		all but the most sensitive applications.
		2-5		Good		Represents a level that marks the minimum appropriate for making business decisions.
		Positional measurements could be used to make reliable in-route navigation suggestions to
		the user.
		5-10		Moderate	Positional measurements could be used for calculations, but the fix quality could still be
		improved. A more open view of the sky is recommended.
		10-20		Fair		Represents a low confidence level. Positional measurements should be discarded or used only
		to indicate a very rough estimate of the current location.
		>20		Poor		At this level, measurements are inaccurate by as much as 300 meters and should be discarded.

		*/
		if (vElementsInGPSASentence.size() == 18)
		{
			
			satInfo.fixType = std::stoi(vElementsInGPSASentence.at(2).c_str());
			satInfo.bGpsPositionIsValid = (satInfo.fixType > 1) ? true : false;
			satInfo.bWassInUse = false;

			// extract HDOP
			satInfo.fHdop = std::stof(vElementsInGPSASentence.at(16));

			// Extract the number of satellites used in the position fix calculation.
			// WAAS/EGfalseS/MSAS satellite numbers are 33, 34, 35, 37, 39, 42, 44, 47, 48, 50, 51 according to
			// here: http://www.gpsinformation.org/dale/dgps.htm#waas & http://gpsinformation.net/exe/waas.html
			for (int i = 3; i < 15; i++)
			{
				std::string & strTemp = vElementsInGPSASentence.at(i);
				if (strTemp.size() > 0)
				{
					int nSatInDop = std::stoi(strTemp);
					satInfo.vSatInDOPCalc.push_back(nSatInDop);
					if (nSatInDop > 32)
					{
						satInfo.bWassInUse = true;
					}
				}
			}
			satInfo.nNumberOfSatInUse = satInfo.vSatInDOPCalc.size();
		}

		// Next sentences are the PGSV sentences
        for (int i = 1; i<(int)vSentences.size() - 1; i++)
		{
			std::vector<std::string> vEleInPGSVSentence;
			std::string & strPGSV = vSentences.at(i);
			SplitString(strPGSV, vEleInPGSVSentence, ",");

			if (vEleInPGSVSentence.size() >= 3)
			{
				satInfo.nNumberOfSatInView = std::stoi(vEleInPGSVSentence.at(3));
			}

			if (0 == satInfo.nNumberOfSatInView)
			{
				//no sat info
			}
			else
			{
				int nSatNum = 0;
				int nSatElev = 0;
				int nSatAzi = 0;
				float fSatSnr = 0;
				bool bInUse = false;
				int nNumOfSatsInSententce = 0;

				// The number of satellites described in a sentence can vary up to 4.
				if (vEleInPGSVSentence.size() < 10)
					nNumOfSatsInSententce = 1;
				else if (vEleInPGSVSentence.size() < 14)
					nNumOfSatsInSententce = 2;
				else if (vEleInPGSVSentence.size() < 18)
					nNumOfSatsInSententce = 3;
				else 
					nNumOfSatsInSententce = 4;

				for (int j = 1; j < nNumOfSatsInSententce; j++)
				{
					int nIndex = j * 4 + 4;
					HmsGPSSatUsedDetail satUsedDetail;

					satUsedDetail.nSatNum = std::stoi(vEleInPGSVSentence.at(j));
					satUsedDetail.nSatElev = std::stoi(vEleInPGSVSentence.at(j+1));
					satUsedDetail.nSatAzi = std::stoi(vEleInPGSVSentence.at(j+2));
					satUsedDetail.fSatSnr = std::stof(vEleInPGSVSentence.at(j+3));

					// On random occasions, either the data is bad or the parsing fails. Handle any not-a-number conditions.
					if (std::isnan(satUsedDetail.fSatSnr))
						satUsedDetail.fSatSnr = 0.0f;
					
					satInfo.vSatDetail.push_back(satUsedDetail);
				}

			}
		}
	}
}

#define kUATPayloadApplicationDataLength    424     // see page 31, section 4.1.2 of GDL90 spec for this value
#define kLatLonBitResolution				(90/4194304.0)
#define kMaxNumberOfEmptyBlocks             125     // see page D-18 for this value
#define kFISBTextLength                      63     // See page 40, section 5.2.4 of GDL90 spec
#define kTrackHeadingResolution				1.40625     // See page 18, Table 8 of GDL90 spec

#define kLowResolutionCONUSDataScaling      9
#define kMediumResolutionCONUSDataScaling   5
#define kHighResoltuionCONUSDataScaling     1
#define kNumberOfBinsPerRow                 32
#define kNumberOfRowsPerBlock               4
#define kNumberOfBinsPerBlock               128     // see page 36, section 5.1.4 of GDL90 spec for this value

// Bytes 0-2: 23 bits of ground station latitude info (does not include LSB bit 8 of byte 2)
inline double GetUplinkLat(unsigned char * strData)
{
	int tempInt = char(strData[0]);
	tempInt <<= 8;
	tempInt |= strData[1];
	tempInt <<= 8;
	tempInt |= (strData[2] & 0xFE);
	tempInt >>= 1;

	double latitude = (tempInt & 0x400000)
		? (-1 * ((tempInt ^ 0x7FFFFF) + 1) * kLatLonBitResolution) - 180
		: tempInt * kLatLonBitResolution;
	return latitude;
}

// bytes 2-5: 24 bits of longitude info (starts with bit 8 of byte 2)
inline double GetUplinkLong(unsigned char * strData)
{
	int tempInt = strData[1];
	tempInt <<= 8;
	tempInt |= strData[2];
	tempInt <<= 8;
	tempInt |= (strData[3] & 0xFE);
	tempInt >>= 1;
	if (strData[0] & 0x01)
	{
		tempInt |= 0xFF800000;
	}

	double longitude = (tempInt & 0x800000)
		? (-1 * ((tempInt ^ 0xFFFFFF) + 1) * kLatLonBitResolution) - 360
		: tempInt * kLatLonBitResolution;
	return longitude;
}

void CDataProc::Handle_UplinkMessage(const char *pdata, int len)
{
	if (len < 10)
		return;
	
	unsigned char * strUatPaylod = (unsigned char*)(pdata + 4);
	// Message payload is 432 bytes. The first 8 are a UAT-specific header
	// and the remaining 424 bytes are Application Data organized into I-Frames.
	// IFrames: first 9 bits are length of valid frame data, next 3 bits are for resolution, next 4 bits are for
	// frame type, and remaining bits are frame data
	
	//tower position
	double dLat = GetUplinkLat(strUatPaylod);
	double dLon = GetUplinkLong(strUatPaylod + 2);
	// byte 5, LSB: the position valid bit
	bool bPositionValid = bool(strUatPaylod[5] & 0x01);

	//this is the tower position

	// byte 6, bit 3: application data valid bit
    bool bApplicationDataValid = ((strUatPaylod[6] & 0x20) != 0);
	if (false == bApplicationDataValid)
		return;
	
	// byte 6, bits 4-8: slot ID
	unsigned char slotID = strUatPaylod[6] & 0x1f;

	// byte 7, bit 1-4: TIS-B site ID. If zero, the broadcasting station is not broadcasting TIS-B data
	unsigned char tisbSiteID = (strUatPaylod[7] & 0xf0) >> 4;

	//self.towerList |= (1 << tisbSiteID);

	// byte 9-432: application data (multiple iFrames).
	unsigned short ptr = 8;
	unsigned short iFrameLength = 1;
	unsigned char frameType = 0;
	unsigned char iFrameCount = 0;

	while ((ptr < kUATPayloadApplicationDataLength) && (iFrameLength > 0))
	{
		iFrameLength = strUatPaylod[ptr] << 1;
		iFrameLength |= (strUatPaylod[ptr + 1] & 0x80) >> 7;

		if (PRINT_DOCODE_LOG)
		{
			printf("\nptr = %d. byte A = 0x%.2X. byte B = 0x%.2X.\n", ptr, strUatPaylod[ptr], strUatPaylod[ptr + 1]);
			printf("iFrameLength = 0x%.6X or %d\n", iFrameLength, iFrameLength);
		}

		frameType = (strUatPaylod[ptr + 1] & 0xf);
		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte C = 0x%.2X.\n", strUatPaylod[ptr + 1]);
			printf("frameType = 0x%X or %d\n", frameType, frameType);
			printf("Application data is %s.\n", (frameType == 0) ? "FIS-B APDU" : "reserved use data");
		}

		if ((iFrameLength > 0) && (frameType == 0) && ((ptr + iFrameLength) < len))
		{
			DecodeSingleIFrame(strUatPaylod + ptr, iFrameLength + 2);
			//self.messageCount++;
		}
		else break;

		iFrameCount++;
		if (0) 
			printf("iFrame count = %d.", iFrameCount);

		ptr += iFrameLength + 2;

	}
}

typedef struct
{
	// hour/min/sec in Zulu time
	unsigned char		hour;
	unsigned char		min;
	unsigned char		sec;

	// Several different kinds of traffic types. There is one allowed case where a traffic target is
	// either a TIS-B target or a ADS-R target (with an ICAO address in both cases), so both tisbTarget
	// and adsrTarget will be true.
	bool				adsbTarget;
	bool				tisbTarget;
	bool				surfaceVehicle;
	bool				adsbBeacon;
	bool				adsrTarget;
	bool				icaoAddressIsSelfAssigned;
	unsigned int		icaoAddress;

	float				latitude;
	float				longitude;
	bool				altitudeType;               // true = pressure altitude, false = geometric altitude
	int					altitude;                   // in feet. values less than -1000 = no altitude data
	float				nic;                        // in meters
	bool				airborne;                   // true = airborne, false = on the ground
	bool				supersonic;                 // true = supersonic, false = subsonic
	short				northerlyVelocity;          // in knots, only valid for airborne vehicles. Positive if N, negative if S
	short				easterlyVelocity;           // in knots, only valid for airborne vehicles. Positive if E, negative if W
	unsigned short		groundSpeed;                // in knots, only valid for surface vehicles
	bool				northerlyVelocityAvail;     // false = no valid velocity reported. Velocity value should be considered invalid
	bool				easterlyVelocityAvail;      // false = no valid velocity reported. Velocity value should be considered invalid
	bool				groundSpeedAvail;           // false = no valid groundspeed reported. Speed value should be considered invalid

	// if trueTrackAngle, magenticHeading and trueHeading are all false, the heading data not available and heading value will be -1
	bool				trueTrackAngle;             // true if heading value is a track angle
	bool				magneticHeading;            // true if heading value is magnetic
	bool				trueHeading;                // true if heading value is true
	float				heading;                    // in degrees. If heading information not available, value = -1

	// for airborne vehicles, there is velocity information. if not airborne, there is vehicle size information
	bool				verticalVelocitySource;     // true = barometric source, false = geometric source
	bool				verticalVelocityDirection;  // true = down, false = up
	short				verticalVelocityRate;       // in feet per minute

	// for ground-based objects, size & GPS antenna offset information can be available.
	bool				vehicleSizeIsValid;
	unsigned char		length;                     // length of aircraft/vehicle in meters
	float				width;                      // width of aircraft/vehicle in meters
	bool				gpsAntennaInfoIsValid;
	bool				gpsAntennaOffsetAxis;               // true = longitudinal antenna offset, false = lateral antenna offset
	bool				gpsAntennaOffsetSide;               // valid for lateral offset only: true = right, false = left
	unsigned char		gpsAntennaOffsetDistance;           // in meters, from the longitudinal (roll) axis of the aircraft or from the nose of the aircraft

	// depending on the type of transmitter source
	bool				adsbTransmittingSourceCoupledToUTCTime;
	unsigned char		uplinkFeedback;             // max number of successful uplink messages received
	unsigned char		tisbSiteID;

	bool				isBasicMessage;             // true if it's a Basic traffic message, false if Long
	// Basic traffic messages only have the information above. Long traffic reports have these additional items:

	// Emmitter category is the description of the aircraft, e.g. large vortex heavy, paraglider, UAV, etc.
	char				emitterCategoryDescr[38];

	// The tail number is sometimes available
	char				callSignFlightPlanID[9];

	// The type of emergency is sometimes available
	char				statusDescr[24];

	// Other
	unsigned char		mopsVersion;
	unsigned char		sil;
	unsigned char		transmitMSO;
	unsigned char		sda;
	unsigned char		nacP;
	unsigned char		nacV;
	bool				nicBaro;
	unsigned char		capabilityCodes;
	unsigned char		operationalModes;
	bool				csid;
	bool				silSUPP;
	unsigned char		geoVertAcc;
	bool				saFlag;
	bool				nicSUPP;

} LongTrafficObjectInfo;

double calculateDegrees(unsigned char highByte, unsigned char midByte, unsigned char lowByte)
{
	unsigned int position = 0;

	double xx;

	position = highByte;
	position <<= 8;
	position |= midByte;
	position <<= 8;
	position |= lowByte;

	if (position & 0x800000)
	{
		int yy;

		position |= 0xFF000000;

		yy = (int)position;
		xx = (double)(yy);
	}
	else
	{
		xx = (position & 0x7FFFFF);
	}

	xx *= kLatLonBitResolution;

	return xx;
}

struct ZuluTime
{
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
};

ZuluTime calculateZuluTime(unsigned char highByte, unsigned char midByte, unsigned char lowByte)
{
	// Time is 24-bit number with with 80 ns resolution.
	// See page 26 of the GDL90 spec for more info: Table 13.

	uint32_t timeOfReception = 0;
	timeOfReception = highByte << 16;
	timeOfReception |= midByte << 8;
	timeOfReception |= lowByte;

	float hours = timeOfReception * 0.00008 / 3600;
	float minutes = (hours - floor(hours)) * 60;
	float seconds = (minutes - floor(minutes)) * 60;


	ZuluTime zulu;
	zulu.hours = (unsigned char)hours;
	zulu.minutes = (unsigned char)minutes;
	zulu.seconds = (unsigned char)seconds;
	return zulu;
}

struct Base40Bytes
{
	unsigned char B2;
	unsigned char B1;
	unsigned char B0;
} ;

Base40Bytes convert_from_base40(unsigned long valueToConvert)
{
	Base40Bytes bb;
	double bTwo, bOne, bZero;
	double val, firstFraction, secondFraction;

	val = (float)valueToConvert;

	firstFraction = val / 1600.0f;
	bTwo = floor(firstFraction);

	secondFraction = (firstFraction - bTwo);
	secondFraction *= 40;
	bOne = floor((float)secondFraction);

	bZero = val - (bTwo * 1600) - (bOne * 40);

	bb.B2 = (unsigned char)bTwo;
	bb.B1 = (unsigned char)bOne;
	bb.B0 = (unsigned char)bZero;

	return bb;
}

void CDataProc::Handle_ShortTrafficReport(const char *pdata, int len)
{
	ShortTrafficObjectInfo stoi;
	unsigned char * msg = (unsigned char *)pdata + 1;

	// upper nibble of first byte is the traffic alert status
	stoi.status = 0;
	stoi.status = (msg[0] & 0xF0) >> 4;

	/* lower nibble of first byte is the address type:
	* 0 = ADS-B with ICAO address
	* 1 = ADS-B with self-assigned address
	* 2 = TIS-B with ICAO address
	* 3 = TIS-B with track file ID
	* 4 = surface vehicle
	* 5 = ground station beacon
	* 6-15 = reserved
	*/
	stoi.addressType = 0;
	stoi.addressType = msg[0] & 0x0F;

	// next three bytes are the traffic's ICAO address
	stoi.icaoAddress = 0;
	stoi.icaoAddress = (msg[1] << 16) | (msg[2] << 8) | msg[3];

	// next 3 bytes are latitude value with resolution = 180 / (2^23) degrees
	stoi.latitude = 0;
	stoi.latitude = calculateDegrees(msg[4], msg[5], msg[6]);

	// next 3 bytes are longitude value with resolution = 180 / (2^23) degrees
	stoi.longitude = 0;
	stoi.longitude = calculateDegrees(msg[7], msg[8], msg[9]);

	// next 12 bits are altitude
	unsigned short upper = msg[10] << 4;
	unsigned short lower = (msg[11] & 0xF0) >> 4;
	unsigned short alt = upper | lower;
	alt *= 25;
	alt -= 1000;
	stoi.altitude = alt;

	/* next nibble is miscellaneous indicators:
	* bit 3   bit 2    bit 1    bit 0
	*   x       x        0        0    = trackHeading not valid
	*   x       x        0        1    = trackHeading is true track angle
	*   x       x        1        0    = trackHeading is magnetic heading
	*   x       x        1        1    = trackHeading is true heading
	*   x       0        x        x    = report is updated
	*   x       1        x        x    = report is extrapolated
	*   0       x        x        x    = on ground
	*   1       x        x        x    = airborne
	*/
	stoi.miscIndicators = msg[11] & 0x0F;

	// Next nibble is the navigation integrity category (nic). (See GDL-90 spec pg. 21.)
	stoi.nic = 0;
	stoi.nic = (msg[12] & 0xF0) >> 4;

	// Next nibble is navigation accuracy category for position (nacP)
	// (See GDL-90 spec pg. 21.)
	stoi.nacP = 0;
	stoi.nacP = msg[12] & 0x0F;

	// next 12 bits are horizontal velocity in knots. (0xFFF = unknown)
	stoi.horizVelocity = 0;
	upper = msg[13] << 4;
	lower = (msg[14] & 0xF0) >> 4;
	stoi.horizVelocity = upper | lower;

	// next 12 bits are vertical velocity in units of 64 fpm. (0xFFF = unknown)
	stoi.vertVelocity = 0;
	upper = (msg[14] & 0x0F) << 8;
	lower = msg[15];
	stoi.vertVelocity = (upper | lower) * 64;

	// next nibble is the track heading with resolution = 360/256
	stoi.trackHeading = (float)msg[16] * 360.0 / 256.0;

	// next byte is emitter category. see GDL-90 spec page 23
	stoi.emitterCategory = msg[17];

	// next 8 bytes are callsign
	stoi.callsign[0] = msg[18];
	stoi.callsign[1] = msg[19];
	stoi.callsign[2] = msg[20];
	stoi.callsign[3] = msg[21];
	stoi.callsign[4] = msg[22];
	stoi.callsign[5] = msg[23];
	stoi.callsign[6] = msg[24];
	stoi.callsign[7] = msg[25];

	// next 4 bits are emergency/priority code
	stoi.emergencyPriorityCode = (msg[26] & 0xF0) >> 4;

    m_vecShortTraffic.push_back(stoi);

	if (PRINT_DOCODE_LOG)
	{
		printf("Traffic info:");

		printf("byte[0] = 0x%2X. Traffic status = %d.", msg[0], stoi.status);
		printf("byte[0] = 0x%2X. Address type = %d.", msg[0], stoi.addressType);
		printf("byte[1] = 0x%2X. byte[2] = 0x%2X. byte[3] = 0x%2X.\nICAO Address = 0x%4X = %d.", msg[1], msg[2], msg[3], stoi.icaoAddress, stoi.icaoAddress);
		printf("byte[4] = 0x%2X. byte[5] = 0x%2X. byte[6] = 0x%2X.\nlatitude = %.6f.", msg[4], msg[5], msg[6], stoi.latitude);
		printf("byte[7] = 0x%2X. byte[8] = 0x%2X. byte[9] = 0x%2X.\nlongitude = %.6f.", msg[7], msg[8], msg[9], stoi.longitude);
		printf("byte[10] = 0x%2X. byte[11] = 0x%2X. altitude = %d.", msg[10], msg[11], stoi.altitude);
		printf("byte[11] = 0x%2X. misc indicators = 0x%2X = %d.", msg[11], stoi.miscIndicators, stoi.miscIndicators);
		printf("byte[12] = 0x%2X. nic = %d.", msg[12], stoi.nic);
		printf("byte[12] = 0x%2X. nacP = %d.", msg[12], stoi.nacP);
		printf("byte[13] = 0x%2X. byte[14] = 0x%2X. horizVel = %d.", msg[13], msg[14], stoi.horizVelocity);
		printf("byte[14] = 0x%2X. byte[15] = 0x%2X. horizVel = %d.", msg[14], msg[15], stoi.vertVelocity);
		printf("byte[16] = 0x%2X. track / heading = %.1f.", msg[16], stoi.trackHeading);
		printf("byte[17] = 0x%2X. Address type = %d.", msg[17], stoi.emitterCategory);
		printf("byte[18] = 0x%2X. byte[19] =x 0x%2X. byte[20] = 0x%2X. byte[21] = 0x%2X.", msg[18], msg[19], msg[20], msg[21]);
		printf("byte[22] = 0x%2X. byte[23] = 0x%2X. byte[24] = 0x%2X. byte[25] = 0x%2X.", msg[22], msg[23], msg[24], msg[25]);
		printf("Call sign is %c%c%c%c%c%c%c%c.", stoi.callsign[0], stoi.callsign[1], stoi.callsign[2], stoi.callsign[3], stoi.callsign[4], stoi.callsign[5], stoi.callsign[6], stoi.callsign[7]);
		printf("byte[26] = 0x%2X. priority code = %d.", msg[26], stoi.emergencyPriorityCode);
	}

	// TODO: DO SOMETHING INTERESTING WITH THE TRAFFIC DATA
}

char * bytes_to_binary(unsigned long val, unsigned char numberOfBytes)
{
	static char b[40];
	unsigned long z;
	unsigned char spacer = 0;
	char *p = &b[0];

	unsigned char length = (8 * numberOfBytes) - 1;
	for (z = pow(2, length); z > 0; z >>= 1)
	{
		spacer++;
		*p++ = (val & z) ? '1' : '0';
		if (spacer == 8)
		{
			*p++ = ' ';
			spacer = 0;
		}
	}
	*p++ = '\0';
	return b;
}

void CDataProc::Handle_BasicAndLongTrafficReport(const char *pdata, int len)
{
	LongTrafficObjectInfo ltoi;
	unsigned char * msg = (unsigned char *)pdata + 2;

	ltoi.callSignFlightPlanID[0] = '\0';
	ltoi.statusDescr[0] = '\0';

	// Passed in is the Basic UAT Report without the header byte or trailing 0x7e flag
	if (PRINT_DOCODE_LOG)
	{
		printf("\nparseTrafficReport. Message is:");
		for (int j = 0; j < len; j++) printf("0x%.2X ", msg[j]);
		printf("\n");
	}

	// First three bytes are the time of reception, LSB first.
	ZuluTime zuluTime = calculateZuluTime(msg[2], msg[1], msg[0]);

	ltoi.hour = zuluTime.hours;
	ltoi.min = zuluTime.minutes;
	ltoi.sec = zuluTime.seconds;

	if (PRINT_DOCODE_LOG)
		printf("timeOfReception = %d:%d:%d Zulu", ltoi.hour, ltoi.min, ltoi.sec);

	// ADS-B message payload starts at byte 4 of the passed in message.
	// The message payload begins with a 4 byte header, and the state vector follows.
	// byte [3]: first 5 bits are the payload type code, indicating whether the traffic info is ADS-B,
	// ADS-R, TIS-B or surface vehicle
	unsigned char payloadTypeCode = 0;
	payloadTypeCode = (msg[3] & 0xF8) >> 3;

	if (PRINT_DOCODE_LOG) 
		printf("\nbyte [3] = 0x%.2X. payload type code = %@\n", msg[3], bytes_to_binary(payloadTypeCode,1));

	switch (payloadTypeCode) {
	case 0:
		ltoi.adsbTarget = true;
		ltoi.tisbTarget = false;
		ltoi.surfaceVehicle = false;
		ltoi.adsbBeacon = false;
		ltoi.adsrTarget = false;
		ltoi.icaoAddressIsSelfAssigned = false;
		if (PRINT_DOCODE_LOG) printf("Traffic is ADS - B target with ICAO 24 bit address");
		break;

	case 1:
		ltoi.adsbTarget = true;
		ltoi.tisbTarget = false;
		ltoi.surfaceVehicle = false;
		ltoi.adsbBeacon = false;
		ltoi.adsrTarget = false;
		ltoi.icaoAddressIsSelfAssigned = true;
		if (PRINT_DOCODE_LOG) printf("Traffic is ADS-B target with temporary self-assigned address");
		break;

	case 2:
		// product ID 2 are always this payloadTypeCode at Daytona
		ltoi.adsbTarget = false;
		ltoi.tisbTarget = true;
		ltoi.surfaceVehicle = false;
		ltoi.adsbBeacon = false;
		ltoi.adsrTarget = true;
		ltoi.icaoAddressIsSelfAssigned = false;
		if (PRINT_DOCODE_LOG) printf("Traffic is TIS-B or ADS-rebroadcast target with ICAO 24 bit address");
		break;

	case 3:
		ltoi.adsbTarget = false;
		ltoi.tisbTarget = true;
		ltoi.surfaceVehicle = false;
		ltoi.adsbBeacon = false;
		ltoi.adsrTarget = false;
		ltoi.icaoAddressIsSelfAssigned = false;
		if (PRINT_DOCODE_LOG) printf("Traffic is TIS-B target with track file identifier");
		break;

	case 4:
		ltoi.adsbTarget = false;
		ltoi.tisbTarget = false;
		ltoi.surfaceVehicle = true;
		ltoi.adsbBeacon = false;
		ltoi.adsrTarget = false;
		ltoi.icaoAddressIsSelfAssigned = false;
		if (PRINT_DOCODE_LOG) printf("Traffic is a surface vehicle");
		break;

	case 5:
		ltoi.adsbTarget = false;
		ltoi.tisbTarget = false;
		ltoi.surfaceVehicle = false;
		ltoi.adsbBeacon = true;
		ltoi.adsrTarget = false;
		ltoi.icaoAddressIsSelfAssigned = false;
		if (PRINT_DOCODE_LOG) printf("Traffic is a fixed ADS-B beacon");
		break;

	case 6:
		ltoi.adsbTarget = false;
		ltoi.tisbTarget = false;
		ltoi.surfaceVehicle = false;
		ltoi.adsbBeacon = false;
		ltoi.adsrTarget = true;
		ltoi.icaoAddressIsSelfAssigned = true;
		if (PRINT_DOCODE_LOG) printf("Traffic is ADS-rebroadcast target with non-ICAO address");
		break;

	case 7:
		ltoi.adsbTarget = false;
		ltoi.tisbTarget = false;
		ltoi.surfaceVehicle = false;
		ltoi.adsbBeacon = false;
		ltoi.adsrTarget = false;
		ltoi.icaoAddressIsSelfAssigned = false;
		if (PRINT_DOCODE_LOG) printf("ERROR! Traffic is using a reserved value.");
		break;

	default:
		break;
	}

	// byte [3]: last 3 bits of the fourth byte are the address qualifier
	unsigned char addressQualifier = 0;
	addressQualifier = msg[3] & 0x07;
	if (PRINT_DOCODE_LOG) printf("\nbyte [3] = 0x%.2X. address qualifier = %s.\n", msg[3],
		bytes_to_binary(addressQualifier, 1));

	// bytes [4-6]: 24 bit number representing the ICAO address of an object
	unsigned int icaoAddr = 0;
	icaoAddr = msg[4] << 16;
	icaoAddr |= msg[5] << 8;
	icaoAddr |= msg[6];
	if (PRINT_DOCODE_LOG)
	{
		printf("\nbyte 4 = 0x%.2X. byte 5 = 0x%.2X. byte 6 = 0x%.2X. \n", msg[4], msg[5], msg[6]);
		printf("icaoAddress = 0x%.6X.\n", icaoAddr);
	}
	ltoi.icaoAddress = icaoAddr;

	// bytes [7-9]: 23 bits of latitude info (does not include bit 8 of byte 9)
	unsigned int lat = 0;
	lat = msg[7];
	lat <<= 8;
	lat |= msg[8];
	lat <<= 8;
	lat |= (msg[9] & 0xFE);
	lat >>= 1;

	bool isSouth = ((lat & 0x800000) != 0);
	float degLat = lat * kLatLonBitResolution;
	if (isSouth) degLat *= 1;
	ltoi.latitude = degLat;

	if (PRINT_DOCODE_LOG)
	{
		printf("\nbyte 7 = 0x%.2X. byte 8 = 0x%.2X. byte 9 = 0x%.2X. \n", msg[7], msg[8], msg[9]);
		printf("latitude = 0x%.6X.\n", lat);
		printf("latitude is %.4f.\n", degLat);
	}

	// bytes [9-12]: 24 bits of longitude info (starts with bit 8 of byte 9)
	unsigned int lon = 0;
	lon = msg[10];
	lon <<= 8;
	lon |= msg[11];
	lon <<= 8;
	lon |= (msg[12] & 0xFE);
	lon >>= 1;
	if (msg[9] & 0x01) lon |= 0x800000;

	bool isWest = ((lon & 0x800000) != 0);
	float degLon = (lon & 0x7fffff) * kLatLonBitResolution;
	if (isWest) degLon = -1 * (180.0 - degLon);
	ltoi.longitude = degLon;

	if (PRINT_DOCODE_LOG)
	{
		printf("\nbyte 9 = 0x%.2X. byte 10 = 0x%.2X. byte 11 = 0x%.2X. byte 12 = 0x%.2X. \n", msg[9], msg[10], msg[11], msg[12]);
		printf("longitude = 0x%.6X.\n", lon);
		printf("latitude is %.4f.\n", degLon);
	}

	// byte [12], bit 8: altitude type 
	bool altitudeType = msg[12] & 0x01;
	if (PRINT_DOCODE_LOG) printf("\nbyte 12 = 0x%.2X. altitude type is %d.\n", msg[12], (int)altitudeType);
	ltoi.altitudeType = altitudeType;

	// byte [13-14]: 12 bits of altitude info (only uses upper nibble of byte 14)
	unsigned short codedAltitude = 0;
	codedAltitude = msg[13] << 4;
	codedAltitude |= ((msg[14] & 0xf0) >> 4);
	int geometricAltitude = (codedAltitude * 25) - 1025;
	if (PRINT_DOCODE_LOG)
	{
		printf("\nbyte 13 = 0x%.2X. byte 14 = 0x%.2X.\n", msg[13], msg[14]);
		printf("codedAltitude = 0x%.4X.\n", codedAltitude);
		printf("geometricAltitude is %d.", geometricAltitude);
	}
	ltoi.altitude = geometricAltitude;

	// byte 14: lowest 4 bits representing Navigation Integrity Category (pg. 41)
	float nicList[17] = { -1, 37040, 14816, 7408, 3704, 1852, 1111.2f, 555.6f, 370.4f, 185.2f, 75, 25, 7.5f, -1, -1, -1, -1 };
	char index = msg[14] & 0x04;
	float nic = nicList[index];
	if (PRINT_DOCODE_LOG) printf("\nbyte 14 = 0x%.2X. nic is %d.\n", msg[14], (int)nic);
	ltoi.nic = nic;

	// byte 15, bits 0 & 1: Airborne / ground state
	bool airborne = !(msg[15] & 0x80);
	bool supersonic = ((msg[15] & 0x40) != 0);
	char agState = msg[15] & 0x02;
	if (PRINT_DOCODE_LOG)
	{
		printf("\nbyte 15 = 0x%.2X. air/ground state is %d\n", msg[15], agState);
		printf("object is%sin the air.\n", (airborne) ? " " : " not ");
		printf("object is%ssupersonic.\n", (supersonic) ? " " : " not ");
	}
	ltoi.airborne = airborne;
	ltoi.supersonic = supersonic;

	// Bytes 15-18: bits 4-8 of byte 15 thru bit 1 of byte 18. Contains information about an object's airborne
	// vertical and horizontal velocity vectors, or alternatively the speed and heading of an object on the ground.
	// Determine data based on Airborne/ground state:
	bool horizVelocityIsSoutherly, horizVelocityIsWesterly;
	unsigned short vel = 0;
	unsigned short tracking = 0;
	short northVelocityMagnitude = 0;
	short eastVelocityMagnitude = 0;
	unsigned char groundspeed = 0;
	char multiplier = 1;
	char tahType = 0;
	float objectHeading = 0;

	if (airborne)   // data is horizontal velocity
	{
		if (supersonic) multiplier = 4;

		vel = (msg[15] & 0x0f) << 6;
		vel |= ((msg[16] & 0xfd) >> 2);
		horizVelocityIsSoutherly = ((msg[15] & 0x10) != 0);

		if (vel == 0)
		{
			ltoi.northerlyVelocity = 0;
			ltoi.northerlyVelocityAvail = false;
		}
		else
		{
			northVelocityMagnitude = (vel - 1) * multiplier;
			ltoi.northerlyVelocityAvail = true;

			if (horizVelocityIsSoutherly) northVelocityMagnitude *= -1;
			ltoi.northerlyVelocity = northVelocityMagnitude;
		}
		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte 15 = 0x%.2X. byte 16 = 0x%.2X.\n", msg[15], msg[16]);
			printf("velocity = 0x%.4X. multiplier is %d.\n", vel, multiplier);
			printf("northVelocityMagnitude is %d. direction is %s.\n", northVelocityMagnitude,
				(horizVelocityIsSoutherly) ? "south" : "north");
		}

		vel = 0;
		if (msg[16] & 0x01) vel = 0x200;
		vel |= msg[17] << 1;
		if (msg[18] & 0x80) vel |= 0x01;
		horizVelocityIsWesterly = ((msg[16] & 0x02) != 0);

		if (vel == 0)
		{
			ltoi.easterlyVelocity = 0;
			ltoi.easterlyVelocityAvail = false;
		}
		else
		{
			eastVelocityMagnitude = (vel - 1) * multiplier; // (vel * multiplier) - multiplier;
			ltoi.easterlyVelocityAvail = true;

			if (horizVelocityIsWesterly) eastVelocityMagnitude *= -1;
			ltoi.easterlyVelocity = eastVelocityMagnitude;
		}

		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte 16 = 0x%.2X. byte 17 = 0x%.2X. byte 18 = 0x%.2X.\n", msg[16], msg[17], msg[18]);
			printf("velocity = 0x%.4X. multiplier is %d.\n", vel, multiplier);
			printf("eastVelocityMagnitude is %d. direction is %s.\n", eastVelocityMagnitude,
				(horizVelocityIsWesterly) ? "west" : "east");
		}

	}
	else    // object is on the ground
	{
		vel = (msg[15] & 0x0f) << 6;
		vel |= ((msg[16] & 0xfd) >> 2);
		groundspeed = vel - 1;

		if (vel == 0)
		{
			ltoi.groundSpeed = 0;
			ltoi.groundSpeedAvail = false;
		}
		else
		{
			ltoi.groundSpeed = groundspeed;
			ltoi.groundSpeedAvail = false;
		}

		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte 15 = 0x%.2X. byte 16 = 0x%.2X.\n", msg[15], msg[16]);
			printf("velocity = 0x%.4X.\n", vel);
			printf("groundspeed is %d.\n", groundspeed);
		}
	}

	tracking = msg[17] << 1;
	if (msg[18] & 0x80) tracking |= 0x01;
	objectHeading = tracking * 0.703125;
	ltoi.heading = objectHeading;

	tahType = msg[16] & 0x02;

	switch (tahType) {
	case 1:
		ltoi.trueTrackAngle = true;
		ltoi.magneticHeading = false;
		ltoi.trueHeading = false;
		break;

	case 2:
		ltoi.trueTrackAngle = false;
		ltoi.magneticHeading = true;
		ltoi.trueHeading = false;
		break;

	case 3:
		ltoi.trueTrackAngle = false;
		ltoi.magneticHeading = false;
		ltoi.trueHeading = true;
		break;

	default:    // this condition indicates that heading data is no avialable.
		ltoi.trueTrackAngle = false;
		ltoi.magneticHeading = false;
		ltoi.trueHeading = false;
		ltoi.heading = -1;
		break;
	}

	if (PRINT_DOCODE_LOG)
	{
		printf("\nbyte 17 = 0x%.2X. byte 18 = 0x%.2X.\n", msg[17], msg[18]);
		printf("tracking = 0x%.4X. heading is %.1f.\n", tracking, objectHeading);
	}

	// Byte 18, bits 2-8 and byte 19, bits 1-4: Vertical velocity or A/V size
	if (airborne)
	{
		bool vertVelocitySourceisBarometric = ((msg[18] & 0x40) != 0);
		bool vertVelocityIsDownward = ((msg[18] & 0x20) != 0);
		unsigned short verticalRate = 0;
		unsigned short verticalRateFPM = 0;

		verticalRate = (msg[18] & 0x1f) << 4;
		verticalRate |= ((msg[19] & 0xf0) >> 4);
		verticalRateFPM = (verticalRate * 64) - 64;

		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte 18 = 0x%.2X. byte 19 = 0x%.2X.\n", msg[18], msg[19]);
			printf("vertical velocity source is %s.\n", (vertVelocitySourceisBarometric) ? "Barometric" : "GNSS");
			printf("direction of vertical velocity is %s.\n", (vertVelocityIsDownward) ? "downward" : "upward");
			printf("verticalRate = 0x%.4X. verticalRateFPM is %d.\n", verticalRate, verticalRateFPM);
		}
		ltoi.verticalVelocitySource = vertVelocitySourceisBarometric;
		ltoi.verticalVelocityDirection = vertVelocityIsDownward;
		ltoi.verticalVelocityRate = verticalRateFPM;

		ltoi.vehicleSizeIsValid = false;
		ltoi.length = 0;
		ltoi.width = 0;
		ltoi.gpsAntennaInfoIsValid = false;
		ltoi.gpsAntennaOffsetAxis = false;
		ltoi.gpsAntennaOffsetSide = false;
		ltoi.gpsAntennaOffsetDistance = 0;
	}
	else
	{
		float width = 0;
		unsigned char length = 0;
		unsigned char lengthCode = 0;
		bool widthCode = 0;

		lengthCode = ((msg[18] & 0x70) >> 4);
		widthCode = ((msg[18] & 0x08) != 0);

		length = (lengthCode * 10) + 15;
		if ((lengthCode == 0) && (widthCode == 0)) length = 0;

		float widthList[15] = { 23, 28.5, 34, 33, 38, 39.5, 45, 45, 52, 59.5, 67, 72.5, 80, 80, 90 };
		char index = ((msg[18] & 0x78) >> 3);
		width = widthList[index];

		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte 18 = 0x%.2X.\n", msg[18]);
			printf("lengthCode = 0x%.2X. widthCode = %d. index = %d.\n", lengthCode, (widthCode) ? 1 : 0, index);
			printf("length = %d. width = %.1f.\n", length, width);
		}
		ltoi.length = length;
		ltoi.width = width;
		if ((msg[18] & 0x78) == 0) ltoi.vehicleSizeIsValid = false;
		else ltoi.vehicleSizeIsValid = true;

		// per spec: "When on ground, the UAT transmitting subsystem shall alternate the transmission of lateral and longitudinal offsets
		// each second."
		// This version of the SDK does not handle this condition.
		if (((msg[18] & 0x01) && (msg[19] & 0xC0)) == 0) ltoi.gpsAntennaInfoIsValid = false;
		else
		{
			ltoi.gpsAntennaInfoIsValid = true;
			if (msg[18] & 0x02) // GPS antenna offset is lateral
			{
				ltoi.gpsAntennaOffsetAxis = false;
				ltoi.gpsAntennaOffsetSide = (msg[18] & 0x01);
				ltoi.gpsAntennaOffsetDistance = ((msg[19] & 0xC0) >> 6) * 2;
			}
			else                // GPS antenna offset is longitudinal
			{
				ltoi.gpsAntennaOffsetAxis = true;
				ltoi.gpsAntennaOffsetSide = false;

				unsigned char longitudinalOffset = 0;
				longitudinalOffset = (msg[18] & 0x01) << 4;
				longitudinalOffset |= ((msg[19] & 0xF0) >> 4);
				ltoi.gpsAntennaOffsetDistance = (longitudinalOffset - 1) * 2;
			}
		}

		ltoi.verticalVelocitySource = false;
		ltoi.verticalVelocityDirection = false;
		ltoi.verticalVelocityRate = 0;
	}

	// If the address qualifier is 2, 3 or 6 then the last nibble in the State Vector is the TIS-B site ID.
	// Otherwise the last nibble contains UTC coupling & uplink feedback values

	if ((addressQualifier == 2) || (addressQualifier == 3) || (addressQualifier == 6))
	{
		ltoi.tisbSiteID = msg[19] & 0x0F;
		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte 19 = 0x%.2X or %s\n", msg[19], bytes_to_binary(msg[19],1));
			printf("TIS-B site ID = %d. %s errors.\n", ltoi.tisbSiteID, (ltoi.tisbSiteID > 0) ? "No" : "WARNING:");
		}

		ltoi.uplinkFeedback = 0;
		ltoi.adsbTransmittingSourceCoupledToUTCTime = false;
	}
	else
	{
		ltoi.adsbTransmittingSourceCoupledToUTCTime = ((msg[19] & 0x08) != 0);
		if (payloadTypeCode < 2)
		{
			unsigned char uplinkFeedbackBits = msg[19] & 0x07;
			float uplinkList[8] = { 0, 13, 21, 25, 28, 30, 31, 32 };
			ltoi.uplinkFeedback = uplinkList[uplinkFeedbackBits];
		}

		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte 19 = 0x%.2X or %s\n", msg[19], bytes_to_binary(msg[19],1));
			printf("UTC Coupling = %s. uplinkFeedback = %d.\n",
				(ltoi.adsbTransmittingSourceCoupledToUTCTime) ? "Yes" : "No", ltoi.uplinkFeedback);
		}

		ltoi.tisbSiteID = 0;
	}

	if (payloadTypeCode == 0)
		// The payload for basic message will end here. Long message payloads will continue.
	{
		ltoi.isBasicMessage = true;
	}

	else if ((payloadTypeCode == 1) || (payloadTypeCode == 3)) // Continuing on with the extended report information...
	{
		if (PRINT_DOCODE_LOG) printf("Parsing long message");
		ltoi.isBasicMessage = false;

		Base40Bytes bb;
		unsigned char emitterCategory = 0;
		unsigned char callSignFlightPlanID[9];
		unsigned short data = 0;
		unsigned char callSignCharEncoding[40] = {
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
			'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
			'U', 'V', 'W', 'X', 'Y', 'Z', ' ', '\0', '\0', '\0'
		};

		data = (msg[20] << 8) | msg[21];
		bb = convert_from_base40(data);
		emitterCategory = bb.B2;
		callSignFlightPlanID[0] = callSignCharEncoding[bb.B1];
		callSignFlightPlanID[1] = callSignCharEncoding[bb.B0];

		data = (msg[22] << 8) | msg[23];
		bb = convert_from_base40(data);
		callSignFlightPlanID[2] = callSignCharEncoding[bb.B2];
		callSignFlightPlanID[3] = callSignCharEncoding[bb.B1];
		callSignFlightPlanID[4] = callSignCharEncoding[bb.B0];

		data = (msg[24] << 8) | msg[25];
		bb = convert_from_base40(data);
		callSignFlightPlanID[5] = callSignCharEncoding[bb.B2];
		callSignFlightPlanID[6] = callSignCharEncoding[bb.B1];
		callSignFlightPlanID[7] = callSignCharEncoding[bb.B0];

		callSignFlightPlanID[8] = '\0';

		char * emitters[] = { "None", "Light: <15,000 lbs", "Small: 15,000 - 75,000 lbs",
			"Large: 75,000 to 300,000 lbs", "High Vortex Large", "Heavy: >300,000 lbs",
			"Highly maneuverable", "Rotocraft", "Unknown", "Glider/Sailplane", "Lighter than air",
			"Parachutist / Sky diver", "Ultralight/Hanglider/Paraglider", "Unknown", "UAV",
			"Space/transatmospheric vehicle", "Unknown", "Surface Vehicle - Emergency vehicle",
			"Surface vehicle - surface vehicle", "Point Obstacle", "Cluster Obstacle",
			"Line Obstacle", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
			"Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
			"Unknown", "Unknown", "Unknown", "Unknown", "Unknown" };


		strcpy(ltoi.emitterCategoryDescr, emitters[emitterCategory]);
		strcpy(ltoi.callSignFlightPlanID, (const char *)callSignFlightPlanID);

		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte 20 = 0x%.2X. byte 21 = 0x%.2X.\n", msg[20], msg[21]);
			printf("\nbyte 22 = 0x%.2X. byte 23 = 0x%.2X.\n", msg[22], msg[23]);
			printf("\nbyte 24 = 0x%.2X. byte 25 = 0x%.2X.\n", msg[24], msg[25]);
			printf("Emitter category is %d.\n", emitterCategory);
			printf("Call sign / flight plan ID is %s.\n", callSignFlightPlanID);
		}

		unsigned char status = (msg[26] & 0xE0) >> 5;
		char * statusTypes[] = {
			"No Emergency", "General Emergency", "Medical Emergency",
			"Minimum Fuel", "No Communications", "Unlawful Interference", "Downed Aircraft",
			"Unknown" };

		strcpy(ltoi.statusDescr, statusTypes[status]);
		ltoi.mopsVersion = (msg[26] & 0x1C) >> 2;
		ltoi.sil = msg[26] & 0x02;
		ltoi.transmitMSO = (msg[27] & 0xFC) >> 2;
		ltoi.sda = msg[27] & 0x02;
		ltoi.nacP = (msg[28] & 0xF0) >> 4;
		ltoi.nacV = (msg[28] & 0x0E) >> 1;
		ltoi.nicBaro = msg[28] & 0x01;
		ltoi.capabilityCodes = (msg[29] & 0xE0) >> 5;
		ltoi.operationalModes = (msg[29] & 0x1C) >> 2;
		ltoi.csid = ((msg[29] & 0x02) != 0);
		ltoi.silSUPP = msg[29] & 0x01;
		ltoi.geoVertAcc = (msg[30] & 0xC0) >> 6;
		ltoi.saFlag = ((msg[30] & 0x20) != 0);
		ltoi.nicSUPP = ((msg[30] & 0x10) != 0);

		if (PRINT_DOCODE_LOG)
		{
			printf("\nbyte[26] = %s.", bytes_to_binary(msg[26], 1));
			printf("status = %d. MOPS version = %d. SIL = %d.", status, ltoi.mopsVersion, ltoi.sil);
			printf("\nbyte[27] = %s.", bytes_to_binary(msg[27], 1));
			printf("transmit MSO = %d. SDA = %d.", ltoi.transmitMSO, ltoi.sda);
			printf("\nbyte[28] = %s.", bytes_to_binary(msg[28], 1));
			printf("NACp = %d. NACv = %d. NICbaro = %d.", ltoi.nacP, ltoi.nacV, ltoi.nicBaro);
			printf("\nbyte[29] = %s.", bytes_to_binary(msg[29], 1));
			printf("capability codes = %d. operational modes = %d. csid = %d. SILsupp = %d.",
				ltoi.capabilityCodes, ltoi.operationalModes, ltoi.csid, ltoi.silSUPP);
			printf("\nbyte[30] = %s.", bytes_to_binary(msg[30], 1));
			printf("Geo Vert Acc = %d. SA flag = %d. NICsupp = %d.", ltoi.geoVertAcc, ltoi.saFlag, ltoi.nicSUPP);
		}
	}
	else
	{
		printf("UNHANDLED LONG MESSAGE: Payload type code is %d.", payloadTypeCode);
		return;
	}

	// TODO: DO SOMETHING INTERESTING WITH THE TRAFFIC DATA
}

void CDataProc::Handle_DeviceResponse(const char *pdata, int len)
{

}

//=========================================================
// getBits -- retrieve random number of bits from a stream
//			  return MSB first
//=========================================================
static unsigned char*	s_pBits;
static unsigned char	s_nBitsLeft;

static void getBitsInit(unsigned char* p)
{
	s_pBits = p;
	s_nBitsLeft = 8;
}

static unsigned int getBit(void)
{
	unsigned int	r;

	if (s_nBitsLeft == 0)
	{
		s_pBits++;
		s_nBitsLeft = 8;
	}

	s_nBitsLeft--;

	r = *s_pBits;

	r >>= s_nBitsLeft;
	r &= 0x01;

	return r;
}
static  unsigned int getBits(unsigned char numBits)
{
	unsigned int	val = 0;

	while (numBits--)
	{
		val <<= 1;
		val |= getBit();
	}

	return val;
}

void CDataProc::DecodeSingleIFrame(unsigned char * data, int iFrameLength)
{
	// Bytes 0 and 1 are the length and frame type. We're here because frameLength was not 0 and frameType
	// indicates FIS-B APDU data.
	// So here we'll parse APDU header, which starts at the 3rd byte in the iFrame (frame data section):
	// Applications Methods Flag bit is first.
	// Geographic Locator Flag bit is second.
	// Provider-specific Flag bit is third.
	// Product ID is next 11 bits
	// Segmentation flag is bit 15
	// Time Option bits are bits 16 & 17
	// Hours is bits 18-21
	// Minutes is 22-28
	// Finally, the frame data starts at byte[6]

	unsigned char	*	p;
	unsigned char		flagAppMethod = 0;
	unsigned char		flagGeoLocator = 0;
	unsigned char		flagProviderSpec = 0;

	unsigned short		productID;
	unsigned char		segFlag;
	unsigned char		timeOpts;
	unsigned char		month = 0;
	unsigned char		day = 0;
	unsigned char		hours;
	unsigned char		mins;
	unsigned char		secs = 0;
	unsigned short		productFileLength = 0;
	unsigned short		apduNumber = 0;
	unsigned short		productFileID = 0;

	unsigned short		length;


	getBitsInit(&data[2]);

	if (getBits(1))
		flagAppMethod = 1;

	if (getBits(1))
		flagGeoLocator = 1;

	if (getBits(1))
		flagProviderSpec = 1;

	// 11 bits productID
	productID = getBits(11);

	if (flagAppMethod) {
		getBits(8);
	}

	if (flagGeoLocator) {
		getBits(20);
	}

	segFlag = getBits(1);

	timeOpts = getBits(2);

	// 00 - No day, No sec
	// 01 - sec
	// 10 - day
	// 11 - day, sec
	if (timeOpts & 0x02) {
		month = getBits(4);
		day = getBits(5);
	}
	hours = getBits(5);
	mins = getBits(6);
	if (timeOpts & 0x01) {
		secs = getBits(6);
	}

	if (segFlag) {

		if (0)     // do it the DO-267A way
		{
			productFileLength = getBits(12);
			apduNumber = getBits(12);

			if (PRINT_DOCODE_LOG)
			{
				printf("Segmentation block 24 bit representation:");
				printf("Product file length = %d", productFileLength);
				printf("APDU Number = %d", apduNumber);
			}
		}
		else        // do it the mitre way
		{
			productFileID = getBits(10);
			productFileLength = getBits(9);
			apduNumber = getBits(9);

			if (PRINT_DOCODE_LOG)
			{
				printf("Segmentation block 28 bit representation:");
				printf("Product file ID = %d", productFileID);
				printf("Product file length = %d", productFileLength);
				printf("APDU Number = %d", apduNumber);
			}

		}
		printf("------> skipping iframe product ID=%d %d %d\r\n", productID, segFlag, iFrameLength);
		return;
	}

	p = ++s_pBits;	// skip padding bits (0 to 7 bits)

	length = iFrameLength - (p - data);

	if (PRINT_DOCODE_LOG) {
		printf("------> iframe product ID=%d %d %d\r\n", productID, segFlag, iFrameLength);
	}

	switch (productID) {
	case 8:		DecodeProductID8(p, length); break;
	case 9:		DecodeProductID9(p, length); break;
	case 10:	DecodeProductID10(p, length); break;
	case 11:	DecodeProductID11(p, length); break;
	case 12:	DecodeProductID12(p, length); break;
	case 13:	DecodeProductID13(p, length); break;
	case 63:	DecodeProductID63(p, length); break;
	case 64:	DecodeProductID64(p, length); break;
	case 413:	DecodeProductID413(p, length); break;
	default:
		//printf("product ID %d.\r\n", productID);
		break;
	}
}

void CDataProc::DecodeProductID8(unsigned char * data, int iFrameLength)
{

}

void CDataProc::DecodeProductID9(unsigned char * data, int iFrameLength)
{

}

void CDataProc::DecodeProductID10(unsigned char * data, int iFrameLength)
{

}

void CDataProc::DecodeProductID11(unsigned char * data, int iFrameLength)
{

}

void CDataProc::DecodeProductID12(unsigned char * data, int iFrameLength)
{

}

void CDataProc::DecodeProductID13(unsigned char * data, int iFrameLength)
{

}

void CDataProc::DecodeProductID63(unsigned char * data, int iFrameLength)
{

}

void CDataProc::DecodeProductID64(unsigned char * data, int iFrameLength)
{

}

void CDataProc::DecodeProductID413(unsigned char * data, int iFrameLength)
{

}

void CDataProc::DecodeGraphicalWeatherData(unsigned char * data, unsigned short iFrameDataLength, bool isCONUS)
{
	// First two bytes of the iFrame were length and frame type.
	// Second 4 bytes were the APDU header info.
	// Here we're start parsing at the 7th byte where the block reference indicator starts
	// Block reference indicator is first 3 bytes
	bool elementIdentifier = ((data[0] & 0x80) != 0);
	bool hemisphere = ((data[0] & 0x40) != 0);
	unsigned char conusRes = (data[0] & 0x30) >> 4;
	unsigned int blockNumber = 0;
	blockNumber = (data[0] & 0x0F) << 16;
	blockNumber |= (data[1] << 8);
	blockNumber |= data[2];

	unsigned char conusResolution;
	if (conusRes == 0) conusResolution = kHighResoltuionCONUSDataScaling;
	else if (conusRes == 1) conusResolution = kMediumResolutionCONUSDataScaling;
	else conusResolution = kLowResolutionCONUSDataScaling;

	if (PRINT_DOCODE_LOG)
	{
		printf("Product ID is %s:", (isCONUS) ? "64":"63");
		printf("byte 0 = 0x%.2X. byte 1 = 0x%.2X. byte 2 = 0x%.2X. \n", data[0], data[1], data[2]);
		printf("Element Identifier = %s. Hemisphere = %s.\n", (elementIdentifier) ? "RLE data" : "Empty data",
			(hemisphere) ? "Southern" : "Northern");
		printf("Bin scale factor: %d  ", conusResolution);
		printf("block number = 0x%.6X or %d.\n", blockNumber, blockNumber);
		//[self convertBlockNumberToLatLon : blockNumber];
	}

	if (PRINT_DOCODE_LOG && isCONUS) 
		printf("data = 0x%.2X Block # %d. Bin scale factor = %d.", data[0], blockNumber, conusResolution);

	unsigned char emptyBlockData[kNumberOfBinsPerBlock];
	for (int i = 0; i < kNumberOfBinsPerBlock; i++) emptyBlockData[i] = 0;

	// Decompress RLE data into memory
	if (elementIdentifier)
	{
		unsigned char blockData[kNumberOfBinsPerBlock];
		unsigned short ptr = 3;
		unsigned short numberOfBins, i, j;

		for (unsigned char b = 0; b < kNumberOfBinsPerBlock; b++) blockData[b] = 9;

		j = 0;
		while (ptr < iFrameDataLength)
		{
			numberOfBins = ((data[ptr] & 0xF8) >> 3) + 1;
			if (PRINT_DOCODE_LOG)
				printf("block # %d: data is 0x%2X. %d bins with intensity %d\n", blockNumber, data[ptr], numberOfBins, data[ptr] & 0x07);
			for (i = 0; i < numberOfBins; i++)
			{
				blockData[j] = (data[ptr] & 0x07);
				j++;
			}
			ptr++;
		}

		if (PRINT_DOCODE_LOG)
		{
			printf("Block number: %d.\n", blockNumber);
			for (int k = 0; k < 32; k++) printf("%2d ", blockData[k]);
			printf("\n");
			for (int k = 32; k < 64; k++) printf("%2d ", blockData[k]);
			printf("\n");
			for (int k = 64; k < 96; k++) printf("%2d ", blockData[k]);
			printf("\n");
			for (int k = 96; k < 128; k++) printf("%2d ", blockData[k]);
			printf("\n");
		}

		if (isCONUS)
		{
// 			[self expandCONUSDataBlocks : [NSData dataWithBytes : blockData length : kNumberOfBinsPerBlock]
// 			isEmptyBlock : false
// 					   blockNumber : blockNumber
// 								 scaleFactor : conusResolution];
		}
		else
		{
// 			NSData *block = [[NSData alloc] initWithBytes:blockData length : 128];
// 			[self.fisbBlocks setObject : block forKey : [NSNumber numberWithUnsignedLong : blockNumber]];
		}
	}
	else  // The block represents a group of empty block
	{
		// Upper nibble of byte 3 is the initial bitmap data, the lower nibble is the length of the bitmap
		unsigned char bitmapLength = data[3] & 0xF;

		// Determine which blocks are empty, aka decode the bitmap
#if 0
		if (isCONUS)
		{
			[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : blockNumber scaleFactor : conusResolution];
			if (data[3] & 0x10)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + 1) scaleFactor : conusResolution];
			if (data[3] & 0x20)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + 2) scaleFactor : conusResolution];
			if (data[3] & 0x40)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + 3) scaleFactor : conusResolution];
			if (data[3] & 0x80)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + 4) scaleFactor : conusResolution];

			for (unsigned char i = 1; i < bitmapLength; i++)
			{
				if (data[i + 3] & 0x01)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + (i * 8) - 3) scaleFactor : conusResolution];
				if (data[i + 3] & 0x02)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + (i * 8) - 2) scaleFactor : conusResolution];
				if (data[i + 3] & 0x04)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + (i * 8) - 1) scaleFactor : conusResolution];
				if (data[i + 3] & 0x08)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + (i * 8) - 0) scaleFactor : conusResolution];
				if (data[i + 3] & 0x10)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + (i * 8) + 1) scaleFactor : conusResolution];
				if (data[i + 3] & 0x20)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + (i * 8) + 2) scaleFactor : conusResolution];
				if (data[i + 3] & 0x40)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + (i * 8) + 3) scaleFactor : conusResolution];
				if (data[i + 3] & 0x80)[self expandCONUSDataBlocks : nil isEmptyBlock : YES blockNumber : (blockNumber + (i * 8) + 4) scaleFactor : conusResolution];
			}
		}
		else
		{
			[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : blockNumber]];
			if (data[3] & 0x10)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + 1)]];
			if (data[3] & 0x20)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + 2)]];
			if (data[3] & 0x40)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + 3)]];
			if (data[3] & 0x80)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + 4)]];

			for (unsigned char i = 1; i < bitmapLength; i++)
			{
				if (data[i + 3] & 0x01)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + (i * 8) - 3)]];
				if (data[i + 3] & 0x02)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + (i * 8) - 2)]];
				if (data[i + 3] & 0x04)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + (i * 8) - 1)]];
				if (data[i + 3] & 0x08)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + (i * 8) - 0)]];
				if (data[i + 3] & 0x10)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + (i * 8) + 1)]];
				if (data[i + 3] & 0x20)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + (i * 8) + 2)]];
				if (data[i + 3] & 0x40)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + (i * 8) + 3)]];
				if (data[i + 3] & 0x80)[self.fisbBlocks setObject : [NSNull null] forKey : [NSNumber numberWithUnsignedLong : (blockNumber + (i * 8) + 4)]];
			}
		}

		if (PRINT_DOCODE_LOG)
		{
			printf("Block number %d is empty...", blockNumber);
			if (data[3] & 0x10) printf("Block number %d is empty...", blockNumber + 1);
			if (data[3] & 0x20) printf("Block number %d is empty...", blockNumber + 2);
			if (data[3] & 0x40) printf("Block number %d is empty...", blockNumber + 3);
			if (data[3] & 0x80) printf("Block number %d is empty...", blockNumber + 4);

			for (unsigned char i = 1; i < bitmapLength; i++)
			{
				if (data[i + 3] & 0x01) printf("Block number %d is empty...", (blockNumber + (i * 8) - 3));
				if (data[i + 3] & 0x02) printf("Block number %d is empty...", (blockNumber + (i * 8) - 2));
				if (data[i + 3] & 0x04) printf("Block number %d is empty...", (blockNumber + (i * 8) - 1));
				if (data[i + 3] & 0x08) printf("Block number %d is empty...", (blockNumber + (i * 8) - 0));
				if (data[i + 3] & 0x10) printf("Block number %d is empty...", (blockNumber + (i * 8) + 1));
				if (data[i + 3] & 0x20) printf("Block number %d is empty...", (blockNumber + (i * 8) + 2));
				if (data[i + 3] & 0x40) printf("Block number %d is empty...", (blockNumber + (i * 8) + 3));
				if (data[i + 3] & 0x80) printf("Block number %d is empty...", (blockNumber + (i * 8) + 4));
			}
		}
#endif
	}
}

void CDataProc::SetDeviceType(AHRS_DEVICE_TYPE eType)
{
	m_eDeviceType = eType;
	m_data.eDeviceType = m_eDeviceType;
}
