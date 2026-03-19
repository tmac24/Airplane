#include "HmsToolBarDataViewer.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"
#include "Nav/HmsNavComm.h"
#include "Calculate/CalculateZs.h"
#include <sstream>

USING_NS_HMS;

CHmsToolBarDataViewer * CHmsToolBarDataViewer::create()
{
	CHmsToolBarDataViewer *ret = new CHmsToolBarDataViewer();
	
	if (ret && ret->Init())
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsToolBarDataViewer::CHmsToolBarDataViewer()
{
	m_fMargin = 15;
}

bool CHmsToolBarDataViewer::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	Size size(2 * m_fMargin, 2 * m_fMargin);
	m_bg = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png");
	if (m_bg)
	{
		m_bg->SetColor(Color3B(14, 25, 37));
		m_bg->SetOpacity(127);
		m_bg->SetAnchorPoint(Vec2(0, 1.0f));
		m_bg->SetPosition(0, 0);
		this->AddChild(m_bg);
	}


	Vec2 pos = Vec2(m_fMargin, -m_fMargin);
	float fDynamcContentWidth = 0;

	auto createTitle = [=, &pos, &fDynamcContentWidth](const char * strTitle)
	{
		auto title = CHmsGlobal::CreateLanguageLabel(strTitle, 24, true);
		if (title)
		{
			auto size = title->GetContentSize();
			title->SetAnchorPoint(Vec2(0.0, 1.0f));
			title->SetPosition(pos);
			pos.y -= size.height + m_fMargin;
			this->AddChild(title);


			if (fDynamcContentWidth < size.width)
				fDynamcContentWidth = size.width;
		}


		return title;
	};

	auto createContent = [=, &pos, &fDynamcContentWidth](const char * strContent)
	{
		//auto content = CHmsGlobal::CreateLanguageLabel(strContent, 21, true);
		auto content = CHmsLabel::createWithTTF(strContent, "fonts/simsun.ttc", 24);
		if (content)
		{
			auto size = content->GetContentSize();
			content->SetAnchorPoint(Vec2(0.0, 1.0f));
			content->SetPosition(pos + Vec2(m_fMargin, 0));
			pos.y -= size.height/* + floorf(m_fMargin*1.2f)*/;
			this->AddChild(content);

			if (fDynamcContentWidth < size.width)
				fDynamcContentWidth = size.width;
		}
		return content;
	};

	m_labelDeviceTitle = createTitle("Device Raw Data");
	m_labelDeviceData = createContent(GetDeviceData().c_str());
	m_labelCorrectTitle = createTitle("Corrected Used Data");
	m_labelCorrectData = createContent(GetCorrectedData().c_str());


	size.width += fDynamcContentWidth + m_fMargin;
	size.height = -pos.y;
	m_bg->SetContentSize(size);

	SetScheduleUpdate();
	return true;
}

void CHmsToolBarDataViewer::Update(float delta)
{
	auto dataBus = CHmsGlobal::GetInstance()->GetDataBus();
	bool bIsMetric = dataBus->GetDuControlData()->IsMetricEnable();
	auto aftData = dataBus->GetData();

	if (m_labelDeviceData)
	{
		CHmsGlobal::SetLabelText(m_labelDeviceData, GetDeviceData().c_str());
	}
	if (m_labelCorrectData)
	{
		CHmsGlobal::SetLabelText(m_labelCorrectData, GetCorrectedData().c_str());
	}
}

std::string CHmsToolBarDataViewer::GetLonLatString(double dLon, double dLat)
{
	double dTemp = abs(dLon);
	int nLonDeg = dTemp;
	dTemp -= nLonDeg;
	dTemp *= 60;
	int nLonMin = (int)dTemp;
	int nLonSec = (dTemp - nLonMin) * 60;
	char cLon = (dLon > 0.0f) ? 'E': 'W';

	dTemp = abs(dLat);
	int nLatDeg = dTemp;
	dTemp -= nLatDeg;
	dTemp *= 60;
	int nLatMin = (int)dTemp;
	int nLatSec = (dTemp - nLatMin) * 60;
	char cLat = (dLat > 0.0f) ? 'N':'S';
	char strLonLat[200];
	sprintf(strLonLat, "%03d\302\260%02d\xE2\x80\xB2%02d%c %02d\302\260%02d\xE2\x80\xB2%02d%c", nLonDeg, nLonMin, nLonSec, cLon, nLatDeg, nLatMin, nLatSec, cLat);
	return strLonLat;
}

std::string CHmsToolBarDataViewer::GetLonString(double dLon)
{
	double dTemp = abs(dLon);
	int nLonDeg = dTemp;
	dTemp -= nLonDeg;
	dTemp *= 60;
	int nLonMin = (int)dTemp;
	int nLonSec = (dTemp - nLonMin) * 60;
	char cLon = (dLon > 0.0f) ? 'E' : 'W';
	char strLonLat[200];
	sprintf(strLonLat, "%010.6lf (%03d\302\260%02d\xE2\x80\xB2%02d%c)", dLon , nLonDeg, nLonMin, nLonSec, cLon);
	return strLonLat;
}

std::string CHmsToolBarDataViewer::GetLatString(double dLat)
{
	double dTemp = abs(dLat);
	int nLatDeg = dTemp;
	dTemp -= nLatDeg;
	dTemp *= 60;
	int nLatMin = (int)dTemp;
	int nLatSec = (dTemp - nLatMin) * 60;
	char cLat = (dLat > 0.0f) ? 'N' : 'S';
	char strLonLat[200];
	sprintf(strLonLat, "%09.6lf (%02d\302\260%02d\xE2\x80\xB2%02d%c)", dLat, nLatDeg, nLatMin, nLatSec, cLat);
	return strLonLat;
}

std::string & std_string_format(std::string & _str, const char * _Format, ...)
{ 
	va_list marker;	
	va_start(marker, _Format); 	
	int num_of_chars = vsnprintf(nullptr, 0, _Format, marker);
	if (num_of_chars > (int)_str.capacity())
	{ 
		_str.resize(num_of_chars + 1); 
	} 	
	vsprintf((char *)_str.c_str(), _Format, marker); 	
	va_end(marker);	return _str; 
}

#include "DataInterface/network/GDL90/DataProc.h"
#include "DataInterface/HmsDataBus.h"

template<class T>
void FillData(std::stringstream & s, const char * strTitle, T t, bool bValiad)
{
	s << strTitle;
	if (bValiad)
	{
		s << t << '\n';
	}
	else
	{
		s << "---\n";
	}
}

template<class T>
void FillData(std::stringstream & s, const char * strTitle, T t, const char * strUnit, bool bValiad)
{
	s << strTitle;
	if (bValiad)
	{
		s << t << strUnit << '\n';
	}
	else
	{
		s << "---\n";
	}
}

#define UTF8_DEGREE "\302\260"
std::string MakeStringLink(const std::string & str1, const std::string & str2)
{
	return str1 + str2;
}

std::string CHmsToolBarDataViewer::GetDeviceData()
{
	std::stringstream s;

	auto data = CHmsGlobal::GetInstance()->GetDataBus()->GetData();
	AftDataiLevil info = {0};
	CDataProc::GetInstance()->GetData(&info);
	bool bDevConnect = data->bDeviceConnected;
	bool bGpsIsValid = bDevConnect && data->bLatLonValid;

	s.setf(std::ios::fixed);
	s.precision(2);
	FillData(s, "Device Power       ", data->batteryLevel, (info.statusMsg.isCharging)?" Charging":"", bDevConnect);
	
	FillData(s, "Roll               ", info.ahrsAirMsg.Roll, UTF8_DEGREE, bDevConnect);
	FillData(s, "Pitch              ", info.ahrsAirMsg.Pitch, UTF8_DEGREE, bDevConnect);
	FillData(s, "Yaw                ", info.ahrsAirMsg.Yaw, UTF8_DEGREE, bDevConnect);
	FillData(s, "TurnCoord          ", info.ahrsAirMsg.TurnCoordinator, UTF8_DEGREE, bDevConnect);
	FillData(s, "YawRate            ", info.ahrsAirMsg.YawRate, "\302\260/s", bDevConnect);
	FillData(s, "G Load             ", info.ahrsAirMsg.GLoad, "G", bDevConnect);
	FillData(s, "Indicated Airspeed ", info.ahrsAirMsg.IndicatedAirspeed, "KTS", bDevConnect);
	FillData(s, "Pressure Altitude  ", info.ahrsAirMsg.PressureAltitude, "ft", bDevConnect);
	
	s.precision(6);
	FillData(s, "GPS Longitude      ", GetLonString(info.ownshipReport.longitude), bGpsIsValid);
	FillData(s, "GPS Latitude       ", GetLatString(info.ownshipReport.latitude), bGpsIsValid);
	s.precision(2);
	FillData(s, "GPS Altitude       ", info.ownshipReport.altitude, "feet", bGpsIsValid);
	FillData(s, "GPS Hor Velocity   ", info.ownshipReport.horizontalVelocity, bGpsIsValid);
	FillData(s, "GPS Ver Velocity   ", info.ownshipReport.verticalVelocity, "fpm", bGpsIsValid);
	std::string strTemp;
	switch (info.ownshipReport.tt)
	{
	case 0:
		strTemp = " not valid";
		break;
	case 1:
		strTemp = " TRK TRU";
		break;
	case 2:
		strTemp = " HDG MAG";
		break;
	default:
		strTemp = " HDG TRU";
		break;
	}
	FillData(s, "GPS TrackOrHeading ", info.ownshipReport.trackOrHeading, MakeStringLink(UTF8_DEGREE, strTemp).c_str(), bGpsIsValid);
	FillData(s, "Report Type        ", (info.ownshipReport.reportUpdate == 0) ? "updated" : "extrapolated", bGpsIsValid);

	FillData(s, "GPS Geo Altitude   ", info.geoAltitude.altitude, "feet", bGpsIsValid);
	FillData(s, "GPS Rate           ", int(info.statusGpsMsg.nOutputRate), "Hz", bGpsIsValid);
	FillData(s, "GPS Satellites     ", (int)(info.statusGpsMsg.nSatellites), bGpsIsValid);
	FillData(s, "GPS WAAS           ", (info.statusGpsMsg.WaasStatus == 0) ? "No SBAS" : "SBAS", bGpsIsValid);
	FillData(s, "GPS Signal Level   ", info.statusGpsMsg.power, bGpsIsValid);
	FillData(s, "GPS MSL            ", info.statusGpsMsg.fAltitudeVariation + info.geoAltitude.altitude, bGpsIsValid);
	
	return s.str();
#if 0
	std::string strOut;
	std_string_format(strOut, "Device Power %s", "--");
	return strOut;
#endif
}

std::string CHmsToolBarDataViewer::GetCorrectedData()
{
	std::stringstream s;

	auto data = CHmsGlobal::GetInstance()->GetDataBus()->GetData();
	bool bShow = true;

	s.setf(std::ios::fixed);
	s.precision(2);

	FillData(s, "Device Power       ", data->batteryLevel, bShow);
	FillData(s, "G Load             ", data->GLode, "G", bShow);
	FillData(s, "Pitch              ", data->pitch, "\302\260", bShow);
	FillData(s, "Roll               ", data->roll, "\302\260", bShow);
	FillData(s, "Magnetic Heading   ", data->magneticHeading, "\302\260", bShow);
	FillData(s, "True Heading       ", data->trueHeading, "\302\260", bShow);
	FillData(s, "True Track         ", data->trueTrack, bShow);

	FillData(s, "Yaw Rate           ", data->yawRate, "\302\260/s", bShow);
	FillData(s, "Side Slip Angle    ", data->sideSliAngle, "\302\260", bShow);
	FillData(s, "Longitude          ", GetLonString(data->longitude), bShow);
	FillData(s, "Latitude           ", GetLatString(data->latitude), bShow);
	FillData(s, "Altitude           ", data->altitudeFt, "feet", bShow);
	FillData(s, "Geo Altitude       ", data->geoAltitudeFt, "feet", bShow);
	FillData(s, "MSL Altitude       ", data->mslAltitudeFt, "feet", bShow);

	FillData(s, "Vertical Speed     ", data->verticalSpeed, "ft/m", bShow);
	FillData(s, "Ground Speed       ", data->groundSpeed, "kts", bShow);

	return s.str();
}
