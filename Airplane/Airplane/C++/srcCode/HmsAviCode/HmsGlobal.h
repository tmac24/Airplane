#pragma once
#include "math/HmsMath.h"
#include "HmsStringUtils.h"
#include "HmsCS.h"
#include "DataInterface/TerrainServer/HmsTerrainServer.h"
#include "Language/HmsLanguageLabel.h"

using namespace HmsAviPf;

class CHmsScreenMgr;
class CHmsDataBus;
class CHmsGeoMapLoadManager;
class CHmsFMS;

class CHmsNavInfoWindow;
class CHmsFrame2DRoot;
class CHmsNavEarthFrame;
class CHmsNavImageTileMgr;
class CHmsParallelLayer;
class CHmsTrackDataServer;

class CHmsGlobal
{
public:
	CHmsGlobal();
	~CHmsGlobal();

	CHmsScreenMgr * GetScreenManger(){ return m_pMgrScreen; }

	CHmsDataBus * GetDataBus(){ return m_pDataBus; }

	CHmsFMS * GetFms() { return m_fms; }

	CHmsGeoMapLoadManager * GetGeoMapLoadManager();

    CHmsTrackDataServer * GetTrackDataServer();

	CHmsCS * GetHmsCS(){ return &m_cs; }

	void SetRunSceneByAvionics();

	void SetNavEarthFramePointer(CHmsNavEarthFrame *p){ m_pNavEarthFrame = p; }
	CHmsNavEarthFrame * GetNavEarthFramePointer(){ return m_pNavEarthFrame; }

	void SetParallelLayer(CHmsParallelLayer * pLayer){ m_pParallelLayer = pLayer; }
	CHmsParallelLayer * GetParallelLayer(){ return m_pParallelLayer; }
	void RunToScreen(const std::string & strName);
	void ShowSubScreen(const std::string & strName, bool bShow);

public:
	static CHmsGlobal * GetInstance();

	static void SetDesignScreen(float fWidth, float fHeight, float fMenuBarHeight);
	static void SetFrameSize(float fWidth, float fHeight);
	static void SetContentPos(float x, float y);
	static void SetOldMenuScale(float fScale);

	static float GetDesignWidth();
	static float GetDesignHeight();
	static float GetDesignMenuBarHeight();
	static float GetDesignContentHeight();
	static float GetDesignContentHeightSmall();
	static float GetDesignContentHeightMinusSmall();
	static float GetDesignContentWidthSmall();
	static float GetDesignContentWidthHalf();
	static float GetDesignContentWidthNormal();
	static float GetDesignContentWidthStride();
	static float GetDesignContentWidthFull();
	static float GetDesignScale();

	static float GetFrameWidth();
	static float GetFrameHeight();

	static float GetContentPoxX();
	static float GetContentPosY();

	static float GetOldMenuScale();

public:
    static CHmsLabel * CreateLanguageLabel(const char * strText, float fSize, bool border = false);
    static CHmsLabel * CreateLanguageLabel(const char * strText, float fSize, const Color4F & color, bool border = false);

	static CHmsLanguageLabel * CreateLanguageLabelUtf8(const char * strText, float fSize, bool border = false);
	static CHmsLanguageLabel * CreateLanguageLabelUtf8(const char * strText, float fSize, const Color4F & color, bool border = false);
	
	static CHmsLanguageLabel * CreateLanguageLabelGbk(const char * strText, float fSize, bool border = false);
	static CHmsLanguageLabel * CreateLanguageLabelGbk(const char * strText, float fSize, const Color4F & color, bool border = false);

    static CHmsLabel * CreateLabel(const char * strText, float fSize);
    static CHmsLabel * CreateLabel(const char * strText, float fSize, const Color4F & color);
    static CHmsLabel * CreateLabel(const char * strText, float fSize, bool border);
    static CHmsLabel * CreateLabel(const char * strText, float fSize, const Color4F & color, bool border);

    static CHmsLabel * CreateUTF8Label(const char * strText, float fSize);
    static CHmsLabel * CreateUTF8Label(const char * strText, float fSize, const Color4F & color);
    static CHmsLabel * CreateUTF8Label(const char * strText, float fSize, bool border);
    static CHmsLabel * CreateUTF8Label(const char * strText, float fSize, const Color4F & color, bool border);

    static CHmsLabel * CreateEnglishLabel(const char * strText, float fSize);
    static CHmsLabel * CreateEnglishLabel(const char * strText, float fSize, const Color4F & color);
    static CHmsLabel * CreateEnglishLabel(const char * strText, float fSize, bool border);
    static CHmsLabel * CreateEnglishLabel(const char * strText, float fSize, const Color4F & color, bool border);

	static void SetLabelText(CHmsLabel * pLabel, const char * strText);
	static void SetLabelFormat(CHmsLabel * pLabel, const char * strFormat, ...);
	static void SetUTF8LabelText(CHmsLabel * pLabel, const char * strText);
	static void SetUTF8LabelFormat(CHmsLabel * pLabel, const char * strFormat, ...);

	static std::string FormatString(const char * strFormat, ...);
	static std::string FormatStringLonLat(float lon, float lat);
#if 0
	static Rect GetSpriteRect(const Sprite & sprite);
	static bool IsTouchSprite(Touch * touch, Sprite * sprite);
	static extension::ControlButton * CreateTextBox(const char * strText, float fSize, const Size & size,
		const Color4F & color = Color4F::GREEN);
	static extension::ControlButton * CreateTextBoxNoTransparent(const char * strText, float fSize, const Size & size,
		const Color4F & color = Color4F::GREEN);
	static void SetTextBoxText(extension::ControlButton * controlButton, const char *strText);
	static void SetTextBoxFormat(extension::ControlButton * controlButton, const char * strFormat, ...);
#endif

	static double GetKmPerLongitude(double dLat);
	static double GetKmPerLatitude();
	static float GetDistance(double dLong1, double dLat1, double dLong2, double dLat2);
	static float GetGeoAzimuteRad(double dLong1, double dLat1, double dLong2, double dLat2);
	static float GetGeoAzimuteDegree(double dLong1, double dLat1, double dLong2, double dLat2);
	static void GetDistanceAndAzimute(double dLong1, double dLat1, double dLong2, double dLat2, float & fDistKm, float & fAzimuteRad);
	static void GetDistanceAndAzimuteDegree(double dLong1, double dLat1, double dLong2, double dLat2, float & fDistKm, float & fAzimuteDegree);

	static float TrimRangeAndScale(float fRange, float fScale);
	static bool RangeInc(float & fRange);
	static bool RangeDec(float & fRange);

    static std::vector<std::string> GetFileListByLessThanSize(const std::string & strFoloder, const std::string & strSuffix, unsigned int nKSize);
    static std::vector<std::string> GetFileListByMoreThanDays(const std::string & strFoloder, const std::string & strSuffix, int nDays);
    static bool CleanFilesByZeroSize(const std::string & strFoloder, const std::string & strSuffix);
    static bool CleanFilesByMoreThanDays(const std::string & strFoloder, const std::string & strSuffix, int nDays);
    static int GetPassDays(int nYear, int nMon, int nDay, int nHour, int nMin, int nSec);

    CHmsTerrainServer * GetTerrainServer();

	void SetNavInfoWindowPointer(CHmsNavInfoWindow *p){ m_pNavInfoWindow = p; }
	CHmsNavInfoWindow * GetNavInfoWindowPointer(){ return m_pNavInfoWindow; }

	void SetNavFrame2DPointer(CHmsFrame2DRoot *p){ m_pNavFrame2DRoot = p; }
	CHmsFrame2DRoot * GetNavFrame2DPointer(){ return m_pNavFrame2DRoot; }

	void SetNavImageTileMgrPointer(CHmsNavImageTileMgr *p){ m_pNavImageTileMgr = p; }
	CHmsNavImageTileMgr* GetNavImageTileMgrPointer(){ return m_pNavImageTileMgr; }

private:
	CHmsCS				        m_cs;
	CHmsScreenMgr		    *   m_pMgrScreen;
	CHmsDataBus			    *   m_pDataBus;			//the data bus
	CHmsGeoMapLoadManager	*   m_pGeoMapLoadManager;
    CHmsTrackDataServer     *   m_pTrackDataServer;
	CHmsFMS * m_fms;

	CHmsNavInfoWindow		*   m_pNavInfoWindow;
	CHmsFrame2DRoot			*   m_pNavFrame2DRoot;
	CHmsNavImageTileMgr		*   m_pNavImageTileMgr;
	CHmsNavEarthFrame		*   m_pNavEarthFrame;

	CHmsTerrainServer		*   m_pTerrainServer;
	CHmsParallelLayer		*   m_pParallelLayer;

	static CHmsGlobal * s_pHmsGlobal;
	static float		s_fDesignWidth;
	static float		s_fDesignHeight;
	static float		s_fDesignMenuBarHeight;
	static float		s_fFrameWidth;
	static float		s_fFrameHeight;
	static float		s_fContentPosX;
	static float		s_fContentPosY;
	static float		s_fScaleForOldMenu;
	static float		s_fDesignScale;				//used for setting page item

private:
	static double									s_dLatPerKm;			
	static std::map<unsigned int, double>			s_mapLonPerKm;			
};


