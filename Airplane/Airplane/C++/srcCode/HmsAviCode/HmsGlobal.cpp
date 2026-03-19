#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"
#include "DataInterface/TerrainServer/HmsGeoMapLoadManager.h"
#include "DataInterface/HmsTrackDataServer.h"
#include "Calculate/CalculateZs.h"
#include "HmsScreenMgr.h"
#include "FMS/HmsFMS.h"

CHmsGlobal * CHmsGlobal::s_pHmsGlobal = nullptr;
float CHmsGlobal::s_fDesignMenuBarHeight = 46;/*124*/

float CHmsGlobal::s_fDesignHeight = 1600;
float CHmsGlobal::s_fDesignWidth = 2560;
float CHmsGlobal::s_fFrameHeight = 1600;
float CHmsGlobal::s_fFrameWidth = 2560;

float CHmsGlobal::s_fContentPosY = 0;
float CHmsGlobal::s_fContentPosX = 0;
float CHmsGlobal::s_fScaleForOldMenu = 0.75f;

float CHmsGlobal::s_fDesignScale = 1.0f;

CHmsGlobal::CHmsGlobal()
	: m_pMgrScreen(nullptr)
	, m_pDataBus(nullptr)
	, m_pGeoMapLoadManager(nullptr)
	, m_pTerrainServer(nullptr)
	, m_pNavInfoWindow(nullptr)
	, m_pNavFrame2DRoot(nullptr)
	, m_pNavImageTileMgr(nullptr)
    , m_pNavEarthFrame(nullptr)
	, m_pParallelLayer(nullptr)
    , m_pTrackDataServer(nullptr)
{
    m_pMgrScreen = new CHmsScreenMgr;
	m_pDataBus = new CHmsDataBus;
    m_fms = new CHmsFMS();
}


CHmsGlobal::~CHmsGlobal()
{
    HMS_SAFE_DELETE(m_fms);
}

CHmsGlobal * CHmsGlobal::GetInstance()
{
	if (NULL == s_pHmsGlobal)
	{
		s_pHmsGlobal = new CHmsGlobal;
	}

	return s_pHmsGlobal;
}

CHmsGeoMapLoadManager * CHmsGlobal::GetGeoMapLoadManager()
{
	if (nullptr == m_pGeoMapLoadManager)
	{
		m_pGeoMapLoadManager = new CHmsGeoMapLoadManager;
	}
	return m_pGeoMapLoadManager;
}

CHmsTrackDataServer * CHmsGlobal::GetTrackDataServer()
{
    if (nullptr == m_pTrackDataServer)
    {
        m_pTrackDataServer = new CHmsTrackDataServer();
    }
    return m_pTrackDataServer;
}

void CHmsGlobal::SetRunSceneByAvionics()
{
#if 0
	auto director = Director::getInstance();
	auto srcMgr = GetScreenManger();
	director->runWithScene(srcMgr->GetSceneAvionics());
#endif
}

CHmsLabel * CHmsGlobal::CreateLabel(const char * strText, float fSize)
{
	auto CHmsLabel = CHmsLabel::createWithTTF(a2u(strText), "fonts/msyh.ttc", fSize);//calibri.ttf
	//  	auto config = CHmsLabel->getTTFConfig();
	//  	config.distanceFieldEnabled = true;
	//  	CHmsLabel->setTTFConfig(config);
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateUTF8Label(const char * strText, float fSize)
{
	auto CHmsLabel = CHmsLabel::createWithTTF(strText, "fonts/msyh.ttc", fSize);//calibri.ttf
	//  	auto config = CHmsLabel->getTTFConfig();
	//  	config.distanceFieldEnabled = true;
	//  	CHmsLabel->setTTFConfig(config);
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateUTF8Label(const char * strText, float fSize, const Color4F & color)
{
	auto CHmsLabel = CreateUTF8Label(strText, fSize);
	if (CHmsLabel)
	{
		CHmsLabel->SetTextColor(Color4B(color));
	}
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateUTF8Label(const char * strText, float fSize, bool border)
{
	auto fontPath = "fonts/msyh.ttc";
	if (border)
	{
		fontPath = "fonts/msyhbd.ttc";
	}
	auto CHmsLabel = CHmsLabel::createWithTTF(strText, fontPath, fSize);//calibri.ttf
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateUTF8Label(const char * strText, float fSize, const Color4F & color, bool border)
{
	auto CHmsLabel = CreateUTF8Label(strText, fSize, border);
	if (CHmsLabel)
	{
		CHmsLabel->SetTextColor(Color4B(color));
	}
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateLabel(const char * strText, float fSize, const Color4F & color)
{
	auto CHmsLabel = CreateLabel(strText, fSize);
	if (CHmsLabel)
	{
		CHmsLabel->SetTextColor(Color4B(color));
	}
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateLabel(const char * strText, float fSize, bool border)
{
	auto fontPath = "fonts/msyh.ttc";
	if (border)
	{
		fontPath = "fonts/msyhbd.ttc";
	}
	auto CHmsLabel = CHmsLabel::createWithTTF(a2u(strText), fontPath, fSize);//calibri.ttf
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateLabel(const char * strText, float fSize, const Color4F & color, bool border)
{
	auto CHmsLabel = CreateLabel(strText, fSize, border);
	if (CHmsLabel)
	{
		CHmsLabel->SetTextColor(Color4B(color));
	}
	return CHmsLabel;
}

CHmsLanguageLabel * CHmsGlobal::CreateLanguageLabelUtf8(const char * strText, float fSize, bool border)
{
	auto CHmsLabel = CHmsLanguageLabel::CreateLabel(strText, fSize, border);
	return CHmsLabel;
}

CHmsLanguageLabel * CHmsGlobal::CreateLanguageLabelUtf8(const char * strText, float fSize, const Color4F & color, bool border)
{
	auto CHmsLabel = CHmsLanguageLabel::CreateLabel(strText, fSize, border);
	if (CHmsLabel)
	{
		CHmsLabel->setTextColor(Color4B(color));
	}
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateLanguageLabel(const char * strText, float fSize, bool border)
{
	auto CHmsLabel = CreateLanguageLabelUtf8(strText, fSize, border);
	if (CHmsLabel)
	{
		return CHmsLabel->GetLabel();
	}
	return nullptr;
}

CHmsLabel * CHmsGlobal::CreateLanguageLabel(const char * strText, float fSize, const Color4F & color, bool border)
{
	auto CHmsLabel = CreateLanguageLabelUtf8(strText, fSize, color, border);
	if (CHmsLabel)
	{
		return CHmsLabel->GetLabel();
	}
	return nullptr;
}

CHmsLanguageLabel * CHmsGlobal::CreateLanguageLabelGbk(const char * strText, float fSize, bool border)
{
	auto CHmsLabel = CHmsLanguageLabel::CreateLabelGbk(strText, fSize, border);
	return CHmsLabel;
}

CHmsLanguageLabel * CHmsGlobal::CreateLanguageLabelGbk(const char * strText, float fSize, const Color4F & color, bool border)
{
	auto CHmsLabel = CHmsLanguageLabel::CreateLabelGbk(strText, fSize, border);
	if (CHmsLabel)
	{
		CHmsLabel->setTextColor(Color4B(color));
	}
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateEnglishLabel(const char * strText, float fSize)
{
	auto CHmsLabel = CHmsLabel::createWithTTF(strText, "fonts/calibri.ttf", fSize);
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateEnglishLabel(const char * strText, float fSize, const Color4F & color)
{
	auto CHmsLabel = CreateEnglishLabel(strText, fSize);
	if (CHmsLabel)
	{
		CHmsLabel->SetTextColor(Color4B(color));
	}
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateEnglishLabel(const char * strText, float fSize, bool border)
{
	auto fontPath = "fonts/calibri.ttf";
	if (border)
	{
		fontPath = "fonts/calibrib.ttf";
	}
	auto CHmsLabel = CHmsLabel::createWithTTF(strText, fontPath, fSize);//calibri.ttf
	return CHmsLabel;
}

CHmsLabel * CHmsGlobal::CreateEnglishLabel(const char * strText, float fSize, const Color4F & color, bool border)
{
	auto CHmsLabel = CreateEnglishLabel(strText, fSize, border);
	if (CHmsLabel)
	{
		CHmsLabel->SetTextColor(Color4B(color));
	}
	return CHmsLabel;
}

void CHmsGlobal::SetLabelText(CHmsLabel * pLabel, const char * strText)
{
	if (pLabel)
	{
		pLabel->SetString(a2u(strText));
	}
}

void CHmsGlobal::SetLabelFormat(CHmsLabel * pLabel, const char * strFormat, ...)
{
	if (pLabel)
	{
		va_list _ArgList;
#ifdef _WIN32
        _crt_va_start(_ArgList, strFormat);
		char strTemp[256] = { 0 };
		vsprintf_s(strTemp, strFormat, _ArgList);
#else
        va_start(_ArgList, strFormat);
        char strTemp[256] = { 0 };
        vsprintf(strTemp, strFormat, _ArgList);
#endif
        pLabel->SetString(a2u(strTemp));
	}
}

void CHmsGlobal::SetUTF8LabelText(CHmsLabel * pLabel, const char * strText)
{
	if (pLabel)
	{
        pLabel->SetString(strText);
	}
}

void CHmsGlobal::SetUTF8LabelFormat(CHmsLabel * pLabel, const char * strFormat, ...)
{
	if (pLabel)
	{
		va_list _ArgList;
#ifdef _WIN32
		_crt_va_start(_ArgList, strFormat);
		char strTemp[256] = { 0 };
		vsprintf_s(strTemp, strFormat, _ArgList);
#else
		va_start(_ArgList, strFormat);
		char strTemp[256] = { 0 };
		vsprintf(strTemp, strFormat, _ArgList);
#endif
        pLabel->SetString(strTemp);
	}
}

std::string CHmsGlobal::FormatString(const char * strFormat, ...)
{
	va_list _ArgList;
#ifdef _WIN32
    _crt_va_start(_ArgList, strFormat);
		char strTemp[256] = { 0 };
		vsprintf_s(strTemp, strFormat, _ArgList);
#else
    va_start(_ArgList, strFormat);
    char strTemp[256] = { 0 };
    vsprintf(strTemp, strFormat, _ArgList);
#endif
	return strTemp;
}

#if 0
Rect CHmsGlobal::GetSpriteRect(const Sprite & sprite)
{
	auto rt = sprite.getTextureRect();
	auto ar = sprite.getAnchorPoint();
	Vec2 pos(rt.getMaxX() * ar.x, rt.getMaxY() * ar.y);
	Rect rect(0 - pos.x, 0 - pos.y, rt.getMaxX(), rt.getMaxY());

	return rect;
}

bool CHmsGlobal::IsTouchSprite(Touch * touch, Sprite * sprite)
{
	bool bRet = false;

	if (sprite)
	{
		if (GetSpriteRect(*sprite).containsPoint(sprite->convertTouchToNodeSpaceAR(touch)))
		{
			bRet = true;
		}
	}
	return bRet;
}

class CHmsControlButtom : public extension::ControlButton
{
protected:
	virtual ~CHmsControlButtom(){};
public:
	static CHmsControlButtom* create(Node* CHmsLabel, ui::Scale9Sprite* backgroundSprite)
	{
		CHmsControlButtom *pRet = new (std::nothrow) CHmsControlButtom();
		pRet->initWithLabelAndBackgroundSprite(CHmsLabel, backgroundSprite, false);
		pRet->autorelease();
		return pRet;
	}

	virtual void setSelected(bool enabled) override
	{
		if (enabled == true)
		{
			_state = Control::State::SELECTED;
		}
		else
		{
			_state = Control::State::NORMAL;
		}
		extension::ControlButton::setSelected(enabled);
	}
};

extension::ControlButton * CHmsGlobal::CreateTextBox(const char * strText, float fSize, const Size & size, const Color4F & color)
{
// 	auto btnUp = Scale9Sprite::create("res/Btns/textBoxUp.png");
// 	auto btnDown = Scale9Sprite::create("res/Btns/textBoxDown.png");
	auto btnUp = Scale9Sprite::create("res/Btns/green_edit.png");
	auto btnDown = Scale9Sprite::create("res/Btns/yellow_edit.png");
	auto text = CHmsGlobal::CreateLabel(strText, fSize, color);

	btnUp->setColor(Color3B(color));
	btnDown->setColor(Color3B(color));

	auto btn = CHmsControlButtom::create(text, btnUp);
	btn->setBackgroundSpriteForState(btnDown, extension::Control::State::HIGH_LIGHTED);
	btn->setPreferredSize(size);
	btn->setAdjustBackgroundImage(false);
	btn->setScaleRatio(1.0f);

	return btn;
}

extension::ControlButton * CHmsGlobal::CreateTextBoxNoTransparent(const char * strText, float fSize, const Size & size, const Color4F & color)
{
	auto btnUp = Scale9Sprite::create("res/Btns/textBoxNormal.png");
	auto btnDown = Scale9Sprite::create("res/Btns/textBoxDown.png");
	auto btnSelected = Scale9Sprite::create("res/Btns/textBoxDown.png");
	auto text = CHmsGlobal::CreateLabel(strText, fSize, color);

	btnUp->setColor(Color3B(color));
	btnDown->setColor(Color3B(color));
	btnSelected->setColor(Color3B::GRAY);

	auto btn = CHmsControlButtom::create(text, btnUp);
	btn->setBackgroundSpriteForState(btnDown, extension::Control::State::HIGH_LIGHTED);
	btn->setBackgroundSpriteForState(btnSelected, extension::Control::State::SELECTED);
	btn->setPreferredSize(size);
	btn->setAdjustBackgroundImage(false);
	btn->setScaleRatio(1.0f);

	return btn;
}
void CHmsGlobal::SetTextBoxText(extension::ControlButton * controlButton, const char *strText)
{
	if (controlButton)
	{
		controlButton->setTitleForState(a2u(strText), extension::ControlButton::State::NORMAL);
		controlButton->setTitleForState(a2u(strText), extension::ControlButton::State::HIGH_LIGHTED);
	}
}

void CHmsGlobal::SetTextBoxFormat(extension::ControlButton * controlButton, const char * strFormat, ...)
{
	if (controlButton)
	{
		va_list _ArgList;
#ifdef _WIN32
		_crt_va_start(_ArgList, strFormat);
		char strTemp[256] = { 0 };
		vsprintf_s(strTemp, strFormat, _ArgList);
#else
		va_start(_ArgList, strFormat);
		char strTemp[256] = { 0 };
		vsprintf(strTemp, strFormat, _ArgList);
#endif
		SetTextBoxText(controlButton, strTemp);
	}
}
#endif

void CHmsGlobal::SetDesignScreen(float fWidth, float fHeight, float fMenuBarHeight)
{
	s_fDesignHeight = fHeight;
	s_fDesignWidth = fWidth;
	s_fDesignMenuBarHeight = fMenuBarHeight;
	s_fDesignScale = fWidth / 2560;
}

void CHmsGlobal::SetFrameSize(float fWidth, float fHeight)
{
	s_fFrameHeight = fHeight;
	s_fFrameWidth = fWidth;
}

float CHmsGlobal::GetDesignWidth()
{
	return s_fDesignWidth;
}

float CHmsGlobal::GetDesignHeight()
{
	return s_fDesignHeight;
}

float CHmsGlobal::GetDesignMenuBarHeight()
{
	return s_fDesignMenuBarHeight;
}

float CHmsGlobal::GetDesignContentHeight()
{
	return s_fDesignHeight - s_fDesignMenuBarHeight;
}

float CHmsGlobal::GetDesignContentHeightSmall()
{
	return GetDesignContentHeight() / 4.0f;
}

float CHmsGlobal::GetDesignContentHeightMinusSmall()
{
	return GetDesignContentHeight() / 4.0f * 3.0f;
}

float CHmsGlobal::GetDesignContentWidthSmall()
{
	return s_fDesignWidth / 8.0f;
}

float CHmsGlobal::GetDesignContentWidthHalf()
{
	return s_fDesignWidth / 4.0f;
}

float CHmsGlobal::GetDesignContentWidthNormal()
{
	return s_fDesignWidth / 2.0f;
}

float CHmsGlobal::GetDesignContentWidthStride()
{
	return s_fDesignWidth / 4.0f * 3.0f;
}

float CHmsGlobal::GetDesignContentWidthFull()
{
	return s_fDesignWidth;
}

float CHmsGlobal::GetDesignScale()
{
	return s_fDesignScale;
}

float CHmsGlobal::GetFrameWidth()
{
	return s_fFrameWidth;
}

float CHmsGlobal::GetFrameHeight()
{
	return s_fFrameHeight;
}

void CHmsGlobal::SetContentPos(float x, float y)
{
	s_fContentPosY = y;
	s_fContentPosX = x;
}

float CHmsGlobal::GetContentPoxX()
{
	return s_fContentPosX;
}

float CHmsGlobal::GetContentPosY()
{
	return s_fContentPosY;
}

void CHmsGlobal::SetOldMenuScale(float fScale)
{
	s_fScaleForOldMenu = fScale;
}

float CHmsGlobal::GetOldMenuScale()
{
	return s_fScaleForOldMenu;
}

double CHmsGlobal::GetKmPerLongitude(double dLat)
{
	double absLat = fabs(dLat);
	int nIndex = (int)absLat;
	auto itFind = s_mapLonPerKm.find(nIndex);
	if (itFind == s_mapLonPerKm.end())
	{
		double dKmPerLat = CalculateDistanceKM(0, dLat, 1, dLat);
		s_mapLonPerKm.insert(std::pair<unsigned int, double>(nIndex, dKmPerLat));
		return dKmPerLat;
	}
	else
	{
		return itFind->second;
	}
}

double CHmsGlobal::GetKmPerLatitude()
{
	return s_dLatPerKm;
}

float CHmsGlobal::GetDistance(double dLong1, double dLat1, double dLong2, double dLat2)
{
	auto deltaLong = dLong2 - dLong1;
	auto deltaLat = dLat2 - dLat1;
	auto deltaLongKMeter = deltaLong * GetKmPerLongitude(dLat1);
	auto deltaLatKMeter = deltaLat * GetKmPerLatitude();

	float fDist = float(sqrt(deltaLongKMeter*deltaLongKMeter + deltaLatKMeter*deltaLatKMeter));
	return fDist;
}

float CHmsGlobal::GetGeoAzimuteRad(double dLong1, double dLat1, double dLong2, double dLat2)
{
	auto deltaLong = dLong2 - dLong1;
	auto deltaLat = dLat2 - dLat1;
	auto deltaLongKMeter = deltaLong * GetKmPerLongitude(dLat1);
	auto deltaLatKMeter = deltaLat * GetKmPerLatitude();

	float fAzimute = float(atan2(deltaLongKMeter, deltaLatKMeter));
	return fAzimute;
}

void CHmsGlobal::GetDistanceAndAzimute(double dLong1, double dLat1, double dLong2, double dLat2, float & fDistKm, float & fAzimuteRad)
{
	auto deltaLong = dLong2 - dLong1;
	auto deltaLat = dLat2 - dLat1;
	auto deltaLongKMeter = deltaLong * GetKmPerLongitude(dLat1);
	auto deltaLatKMeter = deltaLat * GetKmPerLatitude();
	fDistKm = float(sqrt(deltaLongKMeter*deltaLongKMeter + deltaLatKMeter*deltaLatKMeter));
	fAzimuteRad = float(atan2(deltaLongKMeter, deltaLatKMeter));
}

float CHmsGlobal::GetGeoAzimuteDegree(double dLong1, double dLat1, double dLong2, double dLat2)
{
	auto deltaLong = dLong2 - dLong1;
	auto deltaLat = dLat2 - dLat1;
	auto deltaLongKMeter = deltaLong * GetKmPerLongitude(dLat1);
	auto deltaLatKMeter = deltaLat * GetKmPerLatitude();

	float fAzimute = float(ToAngleOrg(atan2(deltaLongKMeter, deltaLatKMeter)));
	return fAzimute;
}

void CHmsGlobal::GetDistanceAndAzimuteDegree(double dLong1, double dLat1, double dLong2, double dLat2, float & fDistKm, float & fAzimuteDegree)
{
	auto deltaLong = dLong2 - dLong1;
	auto deltaLat = dLat2 - dLat1;
	auto deltaLongKMeter = deltaLong * GetKmPerLongitude(dLat1);
	auto deltaLatKMeter = deltaLat * GetKmPerLatitude();
	fDistKm = float(sqrt(deltaLongKMeter*deltaLongKMeter + deltaLatKMeter*deltaLatKMeter));
	fAzimuteDegree = float(ToAngleOrg(atan2(deltaLongKMeter, deltaLatKMeter)));
}

int g_nRangeArg[] = { 5, 10, 20, 40, 80, 160, 240, 480, 960 };

#ifndef _ARRAYSIZE
#define _ARRAYSIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

float CHmsGlobal::TrimRangeAndScale(float fRange, float fScale)
{
	auto temp = fRange * fScale;
	if (temp < g_nRangeArg[0])
	{
		temp = g_nRangeArg[0];
	}
	else if (temp > g_nRangeArg[_ARRAYSIZE(g_nRangeArg) - 1])
	{
		temp = g_nRangeArg[_ARRAYSIZE(g_nRangeArg) - 1];
	}
	return temp;
}

bool CHmsGlobal::RangeInc(float & fRange)
{
	for (auto c : g_nRangeArg)
	{
		if (c > fRange)
		{
			fRange = c;
			return true;
		}
	}

	return false;
}

bool CHmsGlobal::RangeDec(float & fRange)
{
	for (int i = _ARRAYSIZE(g_nRangeArg) - 1; i >= 0; i--)
	{
		if (fRange > g_nRangeArg[i])
		{
			fRange = g_nRangeArg[i];
			return true;
		}
	}
	return false;
}

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
//#include "HmsAndroidFunc.h"
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX || HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#else
#include "io.h"
#endif
#include "HmsLog.h"

std::vector<std::string> CHmsGlobal::GetFileListByLessThanSize(const std::string & strFoloder, const std::string & strSuffix, unsigned int nKSize)
{
    std::vector<std::string> vPath;

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX ) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS )
    DIR *dir = opendir(strFoloder.c_str());
    if (nullptr == dir)
    {
        HMSLOG("Can not open dir. Check path or permission!");
        return vPath;
    }

    struct dirent *file;
    // read all the files in dir
    while ((file = readdir(dir)) != NULL)
    {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        {
            HMSLOG("ignore . and ..");
            continue;
        }
        if (file->d_type == DT_DIR) {
            HMSLOG("ignore Folder");
            continue;
        }
        else {
            if (nullptr != strstr(file->d_name, strSuffix.c_str()));
            {
                struct stat statbuff;
                std::string filePath = strFoloder + file->d_name;
                if (stat(filePath.c_str(), &statbuff) < 0){
                    ;
                }
                else
                {
                    if (statbuff.st_size <= nKSize * 1024)
                    {
                        vPath.push_back(filePath);
                    }
                }
            }
        }
    }
    closedir(dir);
#else
    std::string strPath = strFoloder + "*" + strSuffix;

    intptr_t handle;
    _finddata_t findData;
    handle = _findfirst(strPath.c_str(), &findData);    // find the fist file

    if (handle == -1)
    {
        HMSLOG("Failed to find first file!\n");
        return vPath;
    }

    do
    {
        if (findData.attrib & _A_SUBDIR
            && strcmp(findData.name, ".") == 0
            && strcmp(findData.name, "..") == 0)
        {
            HMSLOG("%s\t<dir>\n", findData.name);
        }
        else
        {
            if (nKSize >= findData.size)
            {
                std::string filePath = strFoloder + findData.name;
                vPath.push_back(filePath);
            }
        }
    } while (_findnext(handle, &findData) == 0);    // find the next file

    _findclose(handle); //close the find handle
#endif

    return vPath;
}

int CHmsGlobal::GetPassDays(int nYear, int nMon, int nDay, int nHour, int nMin, int nSec)
{
    struct tm tmPass = { 0 };
    //struct tm *p;
    memset(&tmPass, 0, sizeof(tmPass));
    tmPass.tm_year = nYear - 1900;
    tmPass.tm_mon = nMon - 1;
    tmPass.tm_mday = nDay;
    tmPass.tm_hour = nHour;
    tmPass.tm_min = nMin;
    tmPass.tm_sec = nSec;
    
    time_t timeNow;
    time(&timeNow);
   
    time_t timePass = mktime(&tmPass);

    double diffTime = difftime(timeNow, timePass);
    int nDays = diffTime / (3600 * 24);
    return nDays;
}

int GetPassDaysByFileName(const std::string & strPath)
{
    int nDays = 0;
    auto posFind = strPath.find_last_of("/\\");
    std::string strFileName = strPath.substr(posFind + 1);
    posFind = strFileName.find_first_of("0123456789");
    if (posFind != std::string::npos)
    {
        int nYear = 0;
        int nMonth = 0;
        int nDay = 0;
        int nHour = 0;
        int nMin = 0;
        int nSecond = 0;
        int nRet = sscanf(strFileName.c_str() + posFind, "%04d%02d%02d%02d%02d%02d.", &nYear, &nMonth, &nDay,
            &nHour, &nMin, &nSecond);
        if (nRet == 6)
        {
            nDays = CHmsGlobal::GetPassDays(nYear, nMonth, nDay, nHour, nMin, nSecond);
            HMSLOG("%s %ddays", strPath.c_str(), nDays);
        }
    }
    return nDays;
}

std::vector<std::string> CHmsGlobal::GetFileListByMoreThanDays(const std::string & strFoloder, const std::string & strSuffix, int nDays)
{
    std::vector<std::string> vPath;

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS) 
    DIR *dir = opendir(strFoloder.c_str());
    if (nullptr == dir)
    {
        HMSLOG("Can not open dir. Check path or permission!");
        return vPath;
    }

    struct dirent *file;
    // read all the files in dir
    while ((file = readdir(dir)) != NULL)
    {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        {
            HMSLOG("ignore . and ..");
            continue;
        }
        if (file->d_type == DT_DIR) {
            HMSLOG("ignore Folder");
            continue;
        }
        else {
            if (nullptr != strstr(file->d_name, strSuffix.c_str()));
            {
                struct stat statbuff;
                std::string filePath = strFoloder + file->d_name;
                if (stat(filePath.c_str(), &statbuff) < 0){
                    ;
                }
                else
                {
                    if (GetPassDaysByFileName(file->d_name) >= nDays)
                    {
                        vPath.push_back(filePath);
                    }
                }
            }
        }
    }
    closedir(dir);
#else
    std::string strPath = strFoloder + "*" + strSuffix;

    intptr_t handle;
    _finddata_t findData;
    handle = _findfirst(strPath.c_str(), &findData);    // find the fist file

    if (handle == -1)
    {
        HMSLOG("Failed to find first file!\n");
        return vPath;
    }

    do
    {
        if (findData.attrib & _A_SUBDIR
            && strcmp(findData.name, ".") == 0
            && strcmp(findData.name, "..") == 0)
        {
            HMSLOG("%s\t<dir>\n", findData.name);
        }
        else
        {
            if (GetPassDaysByFileName(findData.name) >= nDays)
            {
                std::string filePath = strFoloder + findData.name;
                vPath.push_back(filePath);
            }
        }
    } while (_findnext(handle, &findData) == 0);    // find the next file

    _findclose(handle); //close the find handle
#endif

    return vPath;
}

bool CleanFiles(const std::vector<std::string> & vPath)
{
    int nRemove = 0;
    int nRemoveError = 0;

    for (auto & c : vPath)
    {
        if (!c.empty())
        {
            if (0 == remove(c.c_str()))
            {
                HMSLOG("delete file %s success!", c.c_str());
                nRemove++;
            }
            else
            {
                HMSLOG("delete file %s Error!", c.c_str());
                nRemoveError++;
            }
        }
    }
    
    return (nRemove > 0) ? true : false;
}

bool CHmsGlobal::CleanFilesByZeroSize(const std::string & strFoloder, const std::string & strSuffix)
{
    bool ret = false;
    std::vector<std::string> vPath = GetFileListByLessThanSize(strFoloder, strSuffix, 0);
    ret = CleanFiles(vPath);
    return ret;
}

bool CHmsGlobal::CleanFilesByMoreThanDays(const std::string & strFoloder, const std::string & strSuffix, int nDays)
{
    bool ret = false;
    std::vector<std::string> vPath = GetFileListByMoreThanDays(strFoloder, strSuffix, nDays);
    ret = CleanFiles(vPath);
    return ret;
}

CHmsTerrainServer * CHmsGlobal::GetTerrainServer()
{
	if (nullptr == m_pTerrainServer)
	{
		m_pTerrainServer = new CHmsTerrainServer;
		//m_pTerrainServer->InitManager("G:\\Data\\SRTMDem");
		//m_pTerrainServer->InitManager("F:\\SRTMDem");
		m_pTerrainServer->Start();
	}
	return m_pTerrainServer;
}

std::map<unsigned int, double> CHmsGlobal::s_mapLonPerKm;

double CHmsGlobal::s_dLatPerKm = CalculateDistanceKM(0, 0, 0, 1);

std::string CHmsGlobal::FormatStringLonLat(float lon, float lat)
{
	if(lon >= 0 && lat >= 0)
	{
		return FormatString("%0.2f\xC2\xB0 N/%0.2f\xC2\xB0 E", fabsf(lat), fabsf(lon));
	}
	else if(lon < 0 && lat < 0)
	{
		return FormatString("%0.2f\xC2\xB0 S/%0.2f\xC2\xB0 W", fabsf(lat), fabsf(lon));
	}
	else if(lon < 0 && lat >= 0)
	{
		return FormatString("%0.2f\xC2\xB0 N/%0.2f\xC2\xB0 W", fabsf(lat), fabsf(lon));
	}
	else if(lon >= 0 && lat < 0)
	{
		return FormatString("%0.2f\xC2\xB0 S/%0.2f\xC2\xB0 E", fabsf(lat), fabsf(lon));
	}
	return "N-E";
};
#include "HmsParallelLayer.h"
void CHmsGlobal::RunToScreen(const std::string & strName)
{
	if (m_pParallelLayer)
	{
		m_pParallelLayer->RunToScreen(strName);
	}
}

void CHmsGlobal::ShowSubScreen(const std::string & strName, bool bShow)
{
	if (m_pParallelLayer)
	{
		m_pParallelLayer->ShowSubScreen(strName, bShow);
	}
}











