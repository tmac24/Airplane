#include "HmsToolBarDisplayFlyData.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"
#include "Nav/HmsNavComm.h"
#include "Calculate/CalculateZs.h"

USING_NS_HMS;

CHmsToolBarDisplayFlyData * CHmsToolBarDisplayFlyData::create(const HmsAviPf::Size & size)
{
	CHmsToolBarDisplayFlyData *ret = new CHmsToolBarDisplayFlyData();
	
	if (ret && ret->Init(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsToolBarDisplayFlyData::CHmsToolBarDisplayFlyData()
{
	m_fMargin = 20;
}

bool CHmsToolBarDisplayFlyData::Init(const HmsAviPf::Size & size)
{
	Rect rect(Vec2(), size);
	if (!InitWithTexture(nullptr, rect, false))
	{
		return false;
	}
	SetColor(Color3B(14, 25, 37));
	this->SetOpacity(127);
	this->SetAnchorPoint(Vec2(0, 0));

	Vec2 pos(m_fMargin, 0);
	m_labelDistanceToNext = CreateDisplayLabel("Current WPT DTG", "-------", pos);
	m_labelEteNext = CreateDisplayLabel("Current WPT ETA", "-------", pos);
	m_labelEteDest = CreateDisplayLabel("Dest ETA", "-------", pos);
	m_labelGroundSpeed = CreateDisplayLabel("Ground Speed", "-------", pos);
	m_labelGpsAltitude = CreateDisplayLabel("GPS Altitude", "-------", pos);
	m_labelClearance = CreateDisplayLabel("Clearance Height", "-------", pos);
	m_labelMagneticHeading = CreateDisplayLabel("Magnetic Heading", "------", pos);
	m_labelTrack = CreateDisplayLabel("True Track", "-------", pos);
	//m_labelAccuracy = CreateDisplayLabel("Accuracy", "-------", pos);
	m_labelVerticalSpeed = CreateDisplayLabel("Vertical Speed", "-------", pos);
	m_labelCurrentCoord = CreateDisplayLabel("Current Coordinates", "-------", pos, true);

	SetScheduleUpdate();
	return true;
}

CHmsLabel * CHmsToolBarDisplayFlyData::CreateDisplayLabel(const char * strTitle, const char * strContent, HmsAviPf::Vec2 & pos, bool bMutiLine)
{
	const float c_fVerticalMargin = 4.0f;
	Vec2 posMove(GetItemWidth(), 0);
	float fOffsetX = posMove.x / 2.0f;
	float fFontContent = 36;
	if (bMutiLine)
	{
		fFontContent -= 4;
	}

	auto title = CHmsGlobal::CreateLanguageLabel(strTitle, 24, true);
	auto content = CHmsGlobal::CreateLanguageLabel(strContent, fFontContent, true);

	if (title && content)
	{
		auto titleHeight = title->GetContentSize().height;
		auto height = this->GetContentSize().height;

		title->SetPosition(pos + Vec2(fOffsetX + 2, height - titleHeight / 2.0f - c_fVerticalMargin));
		title->SetAnchorPoint(Vec2(0.5, 0.5));

		content->SetPosition(pos + Vec2(fOffsetX, (height - titleHeight) / 2.0f));
		content->SetAnchorPoint(Vec2(0.5, 0.5));

		this->AddChild(title);
		this->AddChild(content);
	}


	pos += posMove;
	return content;
}

//#define KnotKMH				1.852


void CHmsToolBarDisplayFlyData::Update(float delta)
{
	auto dataBus = CHmsGlobal::GetInstance()->GetDataBus();
	bool bIsMetric = dataBus->GetDuControlData()->IsMetricEnable();
	auto aftData = dataBus->GetData();

    std::vector<NavLogTableStu> vecNavLogTable;
    int curIndex = CHmsNavComm::GetInstance()->UpdateTableETA(aftData->groundSpeed, vecNavLogTable);

	//jun 鏆傛椂鍙嶈浆
	if (bIsMetric)
	{
        if (curIndex >= 0 && curIndex < (int)vecNavLogTable.size())
		{
			CHmsGlobal::SetLabelFormat(m_labelDistanceToNext, "%.0f nm", vecNavLogTable[curIndex].remainNM);
			CHmsGlobal::SetLabelFormat(m_labelEteNext, vecNavLogTable[curIndex].remainTimeStr.c_str(), 1);
			CHmsGlobal::SetLabelFormat(m_labelEteDest, vecNavLogTable.back().remainTimeStr.c_str(), 1);
		}
		else
		{
			CHmsGlobal::SetLabelFormat(m_labelDistanceToNext, "-------", 1);
			CHmsGlobal::SetLabelFormat(m_labelEteNext, "-------", 1);
			CHmsGlobal::SetLabelFormat(m_labelEteDest, "-------", 1);
		}
		
		if (aftData->bLatLonValid)
		{
			CHmsGlobal::SetLabelFormat(m_labelGroundSpeed, "%.0f kn", aftData->groundSpeed);
			CHmsGlobal::SetLabelFormat(m_labelGpsAltitude, "%.0f ft", dataBus->GetMslAltitudeFeet());
			auto tempFeet = dataBus->GetHeightFeet();
			if (tempFeet < 15)
			{
				CHmsGlobal::SetLabelFormat(m_labelClearance, "<15ft", tempFeet);
			}
			else
			{
				CHmsGlobal::SetLabelFormat(m_labelClearance, "%.0f ft", tempFeet);
			}
			CHmsGlobal::SetLabelFormat(m_labelVerticalSpeed, "%d f/min", aftData->verticalSpeed);
		}
		else
		{
			CHmsGlobal::SetLabelFormat(m_labelGroundSpeed, "-------", aftData->groundSpeed);
			CHmsGlobal::SetLabelFormat(m_labelGpsAltitude, "-------", dataBus->GetMslAltitudeFeet());
			CHmsGlobal::SetLabelFormat(m_labelClearance, "-------", 0);
			CHmsGlobal::SetLabelFormat(m_labelVerticalSpeed, "-------", aftData->verticalSpeed);
		}

		
        CHmsGlobal::SetUTF8LabelFormat(m_labelMagneticHeading, "%03d\302\260", (int)round(aftData->magneticHeading));
        CHmsGlobal::SetUTF8LabelFormat(m_labelTrack, "%03d\302\260", (int)round(aftData->trueTrack));
		//CHmsGlobal::SetLabelFormat(m_labelAccuracy, "-------", 1);
		CHmsGlobal::SetUTF8LabelFormat(m_labelCurrentCoord, "%s", GetLonLatString(aftData->longitude, aftData->latitude).c_str());
	}
	else
	{
        if (curIndex >= 0 && curIndex < (int)vecNavLogTable.size())
		{
			CHmsGlobal::SetLabelFormat(m_labelDistanceToNext, "%.0f km", vecNavLogTable[curIndex].remainNM * KnotKMH);
			CHmsGlobal::SetLabelFormat(m_labelEteNext, vecNavLogTable[curIndex].remainTimeStr.c_str(), 1);
			CHmsGlobal::SetLabelFormat(m_labelEteDest, vecNavLogTable.back().remainTimeStr.c_str(), 1);
		}
		else
		{
			CHmsGlobal::SetLabelFormat(m_labelDistanceToNext, "-------", 1);
			CHmsGlobal::SetLabelFormat(m_labelEteNext, "-------", 1);
			CHmsGlobal::SetLabelFormat(m_labelEteDest, "-------", 1);
		}

		if (aftData->bLatLonValid)
		{
			CHmsGlobal::SetLabelFormat(m_labelGroundSpeed, "%.0f km/h", aftData->groundSpeed * KnotKMH);
			CHmsGlobal::SetLabelFormat(m_labelGpsAltitude, "%.0f m", dataBus->GetMslAltitudeMeter());
			auto tempMeter = dataBus->GetHeightMeter();
			if (tempMeter < 5)
			{
				CHmsGlobal::SetLabelFormat(m_labelClearance, "<5m", tempMeter);
			}
			else
			{
				CHmsGlobal::SetLabelFormat(m_labelClearance, "%.0f m", tempMeter);
			}
			CHmsGlobal::SetLabelFormat(m_labelVerticalSpeed, "%.0lf m/min", FeetToMeter(aftData->verticalSpeed));
		}
		else
		{
			CHmsGlobal::SetLabelFormat(m_labelGroundSpeed, "-------");
			CHmsGlobal::SetLabelFormat(m_labelGpsAltitude, "-------");
			CHmsGlobal::SetLabelFormat(m_labelClearance, "-------");
			CHmsGlobal::SetLabelFormat(m_labelVerticalSpeed, "-------");
		}
		
        CHmsGlobal::SetUTF8LabelFormat(m_labelMagneticHeading, "%03d\302\260", (int)round(aftData->magneticHeading));
        CHmsGlobal::SetUTF8LabelFormat(m_labelTrack, "%03d\302\260", (int)round(aftData->trueTrack));
		//CHmsGlobal::SetLabelFormat(m_labelAccuracy, "-------", 1);
		CHmsGlobal::SetUTF8LabelFormat(m_labelCurrentCoord, "%s", GetLonLatString(aftData->longitude, aftData->latitude).c_str());
	}
}

std::string CHmsToolBarDisplayFlyData::GetLonLatString(double dLon, double dLat)
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
	//\xC2\xB0  E2\x80\xB2   E2\x80\xB3
	sprintf(strLonLat, "%03d\xC2\xB0%02d\xE2\x80\xB2%02d%c\n %02d\xC2\xB0%02d\xE2\x80\xB2%02d%c", nLonDeg, nLonMin, nLonSec, cLon, nLatDeg, nLatMin, nLatSec, cLat);
	return strLonLat;
}

float CHmsToolBarDisplayFlyData::GetItemWidth()
{
	return (this->GetContentSize().width - 2 * m_fMargin) / 10;
}
