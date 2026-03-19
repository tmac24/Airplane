#include "HmsUserContext.h"
#include "HmsConfigXMLReader.h"
#include "HmsAviDisplayUnit.h"
#include "base/HmsScreen.h"
#include "base/HmsFileUtils.h"
#include "Language/HmsLanguageMananger.h"
#include "DataInterface/TerrainServer/HmsTerrainServer.h"
#include "HmsGlobal.h"
#include "DataInterface/TerrainServer/HmsGeoMapLoadManager.h"
#include "HmsSvsMgr.h"
#include "HmsScreenMgr.h"
#include "HmsLog.h"
#include "DataInterface/network/MavLink/HmsMavLinkInterface.h"
#include "DataInterface/network/GDL90/HmsGDL90Interface.h"
#include "DataInterface/HmsDataBus.h"
#include "DataInterface/HmsAdminAreasServer.h"
#include "Database/HmsGeopackageDatabase.h"
#include "Audio/HmsAviAudioEngine.h"
#include "ToolBar/HmsAvionicsDisplayMgr.h"
#include "DataInterface/HmsTrackDataServer.h"

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32
#include "io.h"
#elif HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX
#include <unistd.h>
#endif

#include "Svs/HVTMgr.h"
#include "DataInterface/network/HmsNetworks.h"

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID
#include "HmsAndroidFunc.h"
#endif


#include "FMS/HmsFMS.h"

HmsNetworks g_hmsNetwork;

bool isExistWritableFile(const char* pFileName)
{
	if (!pFileName)
		return false;

	std::string filePath = CHmsFileUtils::getInstance()->getWritablePath();
	filePath += pFileName;
	FILE *pFp = fopen(filePath.c_str(), "r");
	if (pFp)
	{
		fclose(pFp);
		return true;
	}
	return false;
}

void CheckAndCreateDirectory(const std::string & path)
{
	size_t found = path.find_last_of("/\\");
	if (found >= 0)
	{
		std::string strTempPath = path.substr(0, found);
		CHmsFileUtils::getInstance()->createDirectory(strTempPath);
	}
}

void copyData2WritablePath(const char* pFileName)
{
	std::string strPath = CHmsFileUtils::getInstance()->fullPathForFilename(pFileName);
	ssize_t len = 0;
	Data data = CHmsFileUtils::getInstance()->getDataFromFile(strPath.c_str());
	len = data.getSize();

	std::string destPath = CHmsFileUtils::getInstance()->getWritablePath();
	destPath += pFileName;

	CheckAndCreateDirectory(destPath);

	FILE *pFp = fopen(destPath.c_str(), "w+");
	fwrite(data.getBytes(), sizeof(char), len, pFp);
	fclose(pFp);
}

void CheckAndCreateWritablePath(const char * strPath)
{
	if (isExistWritableFile(strPath) == false)
	{
		copyData2WritablePath(strPath);
	}
}

CHmsUserContext* CHmsUserContext::GetInstance()
{
    static CHmsUserContext *s_pContext = nullptr;
    if (!s_pContext)
    {
        s_pContext = new CHmsUserContext();
    }
    return s_pContext;
}


CHmsUserContext::CHmsUserContext()
	: m_scale(1.0f)
{
	
}


CHmsUserContext::~CHmsUserContext()
{
	CHmsGlobal::GetInstance()->GetFms()->Stop();
}

void CHmsUserContext::LoadXml()
{
    std::string strXmlPath = m_stringPathCfg;

#ifdef __vxworks
    if (access("/sata:1/AviRes/", 0) == 0)
    {
        strXmlPath = "/sata:1/AviRes/";
    }
    else if (access("/ata1:1/AviRes/", 0) == 0)
    {
        strXmlPath = "/ata1:1/AviRes/";
    }
    else if (access("/bd16:1/AviRes/", 0) == 0)
    {
        strXmlPath = "/bd16:1/AviRes/";
    }
    else if (access("/bd0:1/AviRes/", 0) == 0)
    {
        strXmlPath = "/bd0:1/AviRes/";
    }
#endif
#ifdef _WIN32
    if (_access("../bin/", 0) == 0)
    {
        strXmlPath = "../bin/";
    }
    else if (_access("../WinBin/", 0) == 0)
    {
        strXmlPath = "../WinBin/";
    }
    else
    {
        strXmlPath = "./";
    }
#endif
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX)
    strXmlPath = "./";
#endif

    if (strXmlPath.size())
    {
#if (HMS_TARGET_PLATFORM != HMS_PLATFORM_ANDROID && HMS_TARGET_PLATFORM != HMS_PLATFORM_IOS)
        strXmlPath += "HmsFlarmConfig.xml";
		m_stringPathCfg = strXmlPath;
#endif
        auto config = CHmsConfigXMLReader::GetSingleConfig(strXmlPath);

        if (config)
        {
            HMSLOG("Find Avionic Res, %s\n", strXmlPath.c_str());
        }
        else
        {
            HMSLOG("Can't find the Avionic Res!");
        }
    }
    else
    {
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
        strXmlPath = "assets/HmsFlarmConfig.xml";
        auto config = CHmsConfigXMLReader::GetSingleConfig(strXmlPath);
        if (config)
		    HMSLOG("Use the assets Avionic Res!");
        else
            HMSLOG("Can't find the assets Avionic Res!");
#else
		HMSLOG("Can't find the Avionic Res!");
#endif 
    }
}

void CHmsUserContext::InitFrame()
{
    LoadXml();

    auto esContect = GetEsContext();

#if (HMS_TARGET_PLATFORM != HMS_PLATFORM_ANDROID) && (HMS_TARGET_PLATFORM != HMS_PLATFORM_IOS)
    esContect->surfXPos = CHmsConfigXMLReader::GetSingleConfig()->GetConfigInfo("Screen/xPos", 0);
    esContect->surfYPos = CHmsConfigXMLReader::GetSingleConfig()->GetConfigInfo("Screen/yPos", 0);
    esContect->surfWidth = CHmsConfigXMLReader::GetSingleConfig()->GetConfigInfo("Screen/width", 2560);
    esContect->surfHeight = CHmsConfigXMLReader::GetSingleConfig()->GetConfigInfo("Screen/height", 1600);
    esContect->viewportWidth = esContect->surfWidth;
    esContect->viewportHeight = esContect->surfHeight;
    esContect->fUpdateInterval = 1 / 60.0f;

    m_designSize = Size(2560, 1600);
#else
    esContect->surfXPos = 0;
    esContect->surfYPos = 0;
    esContect->surfWidth = m_designSize.width;
    esContect->surfHeight = m_designSize.height;
    esContect->viewportWidth = m_viewportSize.width;
    esContect->viewportHeight = m_viewportSize.height;
    esContect->fUpdateInterval = 1 / 30.0f;
#endif
    
    auto nFlag = ES_WINDOW_RGB | ES_WINDOW_ALPHA | ES_WINDOW_DEPTH | ES_WINDOW_STENCIL;
    this->SetGlFlags(nFlag);

    CHmsGlobal::GetInstance()->SetDesignScreen(m_designSize.width, m_designSize.height, 46);

    HMSLOG("Design size=%f x %f", m_designSize.width, m_designSize.height);
}

#if 0
std::string ReplaceParamPath(const std::string & str, const std::string & strParam, const std::string & strData)
{
	std::string strOut = str;
	std::string::size_type pos = 0;
	std::string::size_type srcLen = strParam.size();

	pos = strOut.find(strParam, pos);
	if ((pos != std::string::npos))
	{
		strOut.replace(pos, srcLen, strData);
	}
	return strOut;
}
#endif

void CHmsUserContext::InitScreen()
{
	auto du = GetDisplayUnit();
	du->SetProjection(HmsAviPf::CHmsAviDisplayUnit::Projection::_2D);
	m_scale = 1.0f;

	auto config = CHmsConfigXMLReader::GetSingleConfig();

    m_scale = config->GetConfigInfo("Screen/scale", 1.0f);
	//初始化
    
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
    HmsAviPf::CHmsFileUtils::getInstance()->SetAviDataPath(m_stringAviDataPath);
    m_stringPathSvs = m_stringAviDataPath + "TAWS_DISPLAY_DATA";
    m_stringPathTerrain = m_stringAviDataPath + "SRTMDem";
#elif (HMS_TARGET_PLATFORM != HMS_PLATFORM_ANDROID)
	m_stringPathAviRes = config->GetConfigInfo("ResInfo/ResPath", "/ata1:1/AviRes/");
	m_stringPathTerrain = config->GetConfigInfo("ResInfo/TifPath", "/ata1:1/Data/SRTMDem");
	m_stringPathSvs = config->GetConfigInfo("ResInfo/SvsPath", "/ata1:1/SvsData/TAWS_DISPLAY_DATA");
#else

	auto GetHmsAndroidFolderPath = [=](const char * strConfigKey, const char * strDefault)
	{
		auto strParam = config->GetConfigInfo(strConfigKey, strDefault);
		std::string path = GetAndroidResPath(strParam);
		if (path.empty())
		{
			path = strDefault;
		}
		return path;
	};

	m_stringPathTerrain = GetHmsAndroidFolderPath("ResInfo/TifPath", "SRTMDem");
	m_stringPathSvs = GetHmsAndroidFolderPath("ResInfo/SvsPath", "TAWS_DISPLAY_DATA");

	config->UpdateDynamicPathInfo([](const std::string & str){
		std::string path = GetAndroidResPath(str);
		if (path.empty())
			return str;
		return path;
	});

#if 0
	// test and example of get file and folder at the same level of HmsData
	if(1)
	{
	    //test the ZsTest Folder at the same level of HmsData
		auto tempPath = GetAndroidResPath("../ZsTest");
		tempPath += "test.txt";
		std::string strData = CHmsFileUtils::getInstance()->getStringFromFile(tempPath);
		if(!strData.empty())
		{
			HMSLOG("test data:%s", strData.c_str());
		}

		auto tempPath2 = GetAndroidFilePath("ZsTest/test.txt");
		HMSLOG("PATH:%s", tempPath2.c_str());

        auto tempPath3 = GetAndroidFilePath("../ZsTest/test.txt");
        HMSLOG("PATH:%s", tempPath3.c_str());
	}
#endif


#endif

//     if (m_stringPathNav.size() > 1)
// 	{
//         config->SetMapPathModifyCallback([=](const std::string & str, const std::string &configName){
// 
//             //if (configName == "MapConfig")
// 			std::string strOut = str;
// 			if (!strOut.empty())
// 			{
// 				strOut = CHmsConfigXMLReader::ReplaceParamPath(strOut, "@NavEarthPath", m_stringPathNav);
// 				strOut = CHmsConfigXMLReader::ReplaceParamPath(strOut, "@NavSvsImagePath", m_stringNavSvsImagePath);
// 			}
// 			return strOut;
//         });
// 	}


	//auto bMaster = config->GetConfigInfo("Screen/Master", 1);

    HmsAviPf::CHmsFileUtils::getInstance()->SetResourcePath(m_stringPathAviRes);
    CHmsTerrainTiffReader::SetGeoTiffPath(m_stringPathTerrain);	//TAWS_DISPLAY_DATA
    HmsSvsMgr::SetSvsDataPath(m_stringPathSvs);
    std::string strHvtPath = m_stringPathSvs + "HvtData/";
    HVTMgr::SetHvtDataPath(strHvtPath);

	CHmsAdminAreasServer::GetInstance();

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
	CheckAndCreateWritablePath("navData/scratchpad.db");
	CheckAndCreateWritablePath("navData/HistoryRoute.db");
	CheckAndCreateWritablePath("navData/NavImage.db");
#endif

	//老的高程图下载
    //CHmsGlobal::GetInstance()->GetGeoMapLoadManager()->InitManager(m_stringPathTerrain);

	auto lauguageMananger = CHmsLanguageMananger::GetInstance();
	lauguageMananger->SetLanguageWithName(config->GetConfigInfo("FunctionConfig/DispalyLanguage", "CHN"));
}

void CHmsUserContext::BeforeRuning()
{
    CheckFilesPath();

	CreateAndShowWelcomeScreen();
}

extern std::string g_stringRecordPath;
extern std::string g_stringRecordTrackPath;

void CHmsUserContext::StartAppNetwork()
{
	auto config = CHmsConfigXMLReader::GetSingleConfig();

	HmsDeviceConnectInfo deviceInfo;

	int nUsedDevice = config->GetConfigInfo("DeviceConnect/UsedDevice", 0);
	if (0 < nUsedDevice)
	{
		std::string strDeviceName = "DeviceConnect/Device" + std::to_string(nUsedDevice);	
		config->GetDeviceConnectInfo(strDeviceName, deviceInfo);
	}

	int nUseAndroidSdk = config->GetConfigInfo("FunctionConfig/UseAndroidSdk", 0);
	bool bStartHmsNetwork = true;

	//todo check the deviceInfo
	if ((deviceInfo.strProtocols.size() != 2) || (nUsedDevice == 0))
	{
		deviceInfo.strProtocols = "TG";
		deviceInfo.nRedirect = 1;
		deviceInfo.strServerIp = "192.168.1.1";
	}

	if (deviceInfo.strProtocols.at(0) == 'B')
	{
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
		CHmsGDL90Interface * pGDL90DataInterface = new CHmsGDL90Interface();
		g_hmsNetwork.SetInterface(pGDL90DataInterface);
		StartAndroidBluetoothNetworks();
#endif
	}
	else
	{
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
		CheckAndOpenAndroidWifiService();

		int nUseAutoConnect = config->GetConfigInfo("AutoConnect/Enable", 1);
		std::string strWifiName = config->GetConfigInfo("AutoConnect/SSID", "iHermes-3337");
		if (nUseAutoConnect)
		{
			if (!strWifiName.empty())
			{
				std::string ssidName = "\"" + strWifiName + "\"";
				SetAndroidConnectTheHistoryWifi(ssidName);
			}
		}
#endif

		if (deviceInfo.strProtocols.at(1) == 'G')
		{
			CHmsGDL90Interface * pGDL90DataInterface = new CHmsGDL90Interface();

			//use the GDL90 protocol
			switch (deviceInfo.strProtocols.at(0))
			{
			case 'T'://use the TCP to connect
			{
				if (deviceInfo.nRedirect)
				{
					//g_hmsNetwork.SetConnectInfo(deviceInfo.strServerIp.c_str(), 2000);
					
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
					if (nUseAndroidSdk)
					{
						StartAndroidTcpNetworks(deviceInfo.strServerIp);
						bStartHmsNetwork = false;
					}
#endif

					if (bStartHmsNetwork)
					{
						g_hmsNetwork.SetConnectTcp(deviceInfo.strServerIp.c_str(), 2000);
					}
				}
				else
				{
					//g_hmsNetwork.SetConnectInfo("169.254.1.1", 2000);
					
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
					if (nUseAndroidSdk)
					{
						StartAndroidTcpNetworks("192.168.1.1");
						bStartHmsNetwork = false;
					}
#endif
					if (bStartHmsNetwork)
					{
						g_hmsNetwork.SetConnectTcp("192.168.1.1", 2000);
					}
				}
				break;
			}
			case 'N':
			{
				if (bStartHmsNetwork)
				{
				g_hmsNetwork.SetConnectInfo(deviceInfo.strServerIp.c_str(), 2000);
				}
				
				break;
			}
			case 'U':		//use the UDP to connect
			default:
			{
				//g_hmsNetwork.SetConnectInfo(nullptr, 2000);
				
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
				if (nUseAndroidSdk)
				{
					StartAndroidUdpNetworks();
					bStartHmsNetwork = false;
				}
#endif
				if (bStartHmsNetwork)
				{
				    g_hmsNetwork.SetConnectUdp();
				}
				break;
			}
			}
			g_hmsNetwork.SetInterface(pGDL90DataInterface);
		}
		else if (deviceInfo.strProtocols.at(1) == 'M')
		{
			//g_hmsNetwork.SetConnectInfo(deviceInfo.strServerIp.c_str(), CHmsMavLinkInterface::getMavlinkPort());
			g_hmsNetwork.SetConnectTcp(deviceInfo.strServerIp.c_str(), CHmsMavLinkInterface::getMavlinkPort());
			CHmsMavLinkInterface * mavlinkInterface = new CHmsMavLinkInterface;
			g_hmsNetwork.SetInterface(mavlinkInterface);
		}

		if (bStartHmsNetwork)
		{
			g_hmsNetwork.Start();
		}
	}
	auto databus = CHmsGlobal::GetInstance()->GetDataBus();
	databus->SetCommunicationType(deviceInfo.strProtocols.at(0));
	databus->SetProtocalType(deviceInfo.strProtocols.at(1));
}



void CHmsUserContext::BeforeAviRuning()
{
	CHmsGlobal::GetInstance()->GetFms()->Start();

	auto config = CHmsConfigXMLReader::GetSingleConfig();
	int nAutoRecord = config->GetConfigInfo("FunctionConfig/AutoRecord", 0);
	if (nAutoRecord > 0)
	{
		CHmsAvionicsDisplayMgr::GetInstance()->RunActions("recordFlyData", "on");
	}

	int nNavMode = config->GetConfigInfo("FunctionConfig/NavMode", 2);
	{
		CHmsAvionicsDisplayMgr::GetInstance()->SetAviMode(nNavMode);
	}

    CHmsGlobal::GetInstance()->GetTrackDataServer()->SetRecordCmd(true);
}

void CHmsUserContext::CheckFilesPath()
{
    std::string strWriteFileFolder;
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
    strWriteFileFolder = CHmsFileUtils::getInstance()->getWritablePath();
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
    strWriteFileFolder = m_stringWritePath;
#else
    auto findPos = m_stringPathCfg.find_last_of("/\\");
    if (findPos != std::string::npos)
    {
        strWriteFileFolder = m_stringPathCfg.substr(0, findPos + 1);
    }
#endif
    HMSLOG("WritePath=%s\n", strWriteFileFolder.c_str());


    std::string strLogFileFoloder = strWriteFileFolder + "Logs/";
    g_stringRecordPath = strWriteFileFolder + "Records/";
    g_stringRecordTrackPath = strWriteFileFolder + "Tracks/";

    CheckAndCreateDirectory(g_stringRecordPath);
    CheckAndCreateDirectory(g_stringRecordTrackPath);
    CheckAndCreateDirectory(strLogFileFoloder);

    CHmsLogFile::SetLogFloder(strLogFileFoloder);

    CHmsGlobal::CleanFilesByZeroSize(g_stringRecordPath, ".rda");
    CHmsGlobal::CleanFilesByMoreThanDays(g_stringRecordPath, ".rda", 7);
    CHmsGlobal::CleanFilesByZeroSize(g_stringRecordTrackPath, ".trk");
    CHmsGlobal::CleanFilesByMoreThanDays(g_stringRecordTrackPath, ".trk", 7);
    CHmsGlobal::CleanFilesByZeroSize(g_stringRecordTrackPath, ".log");
    CHmsGlobal::CleanFilesByMoreThanDays(g_stringRecordTrackPath, ".log", 3);
}

void CHmsUserContext::CreateAndShowWelcomeScreen()
{
// 	auto instance = CHmsGeopackageDatabase::GetInstance();
// 	instance->CheckGeopackageDatabase();

	auto s = CHmsGlobal::GetInstance()->GetScreenManger()->GetSceneWelcome(HMS_CALLBACK_0(CHmsUserContext::CreateAndShowAviScreen, this));
	s->SetScale(m_scale);
	SetScreen(s);

	HMSLOG("set welcome screen scale %f", m_scale);
}

void CHmsUserContext::CreateAndShowAviScreen()
{
	auto s = CHmsGlobal::GetInstance()->GetScreenManger()->GetSceneAvionics();
    s->SetScale(0.93);
//	s->SetScale(m_scale);
	SetNextScreen(s);

	HMSLOG("set avinoics screen scale %f", m_scale);

	auto audioEngine = CHmsAviAudioEngine::getInstance();
	//audioEngine->PlayBgMusic();

	StartAppNetwork();
	BeforeAviRuning();
}

void CHmsUserContext::SetPathCfg(std::string str)
{
    m_stringPathCfg = str;
}

void CHmsUserContext::SetPathAviRes(std::string str)
{
    m_stringPathAviRes = str;
}

void CHmsUserContext::SetPathAviData(std::string str)
{
    m_stringAviDataPath = str;
}

void CHmsUserContext::SetPathSvs(std::string str)
{
    m_stringPathSvs = str;
}

void CHmsUserContext::SetPathTerrain(std::string str)
{
    m_stringPathTerrain = str;
}

void CHmsUserContext::SetPathNav(std::string str)
{
    m_stringPathNav = str;
}

void CHmsUserContext::SetPathNavSvsImage(std::string str)
{
    m_stringNavSvsImagePath = str;
}

void CHmsUserContext::SetWritePath(std::string str)
{
    m_stringWritePath = str;
}

void CHmsUserContext::SetDesignSize(Size s)
{
    m_designSize = s;
}

void CHmsUserContext::SetViewportSize(HmsAviPf::Size s)
{
    m_viewportSize = s;
}


