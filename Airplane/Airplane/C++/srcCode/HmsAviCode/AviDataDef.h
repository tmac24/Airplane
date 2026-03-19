#ifndef _ZS_DEMO_DEFINE_H
#define _ZS_DEMO_DEFINE_H

#if 0
#include "datadef\h_dds_typedef.h"
#else
#pragma pack(push)
#pragma pack(1)

#define TOPICE_NAME_ENG_FUEL "eng_fuel_stat"
#define TOPICE_NAME_PFD_DATA "pfd_data_stat"
//#define TOPICE_NAME_MCP_INFO "mcp_info_stat"
#define TOPICE_NAME_MCP_INFO "mcp_info_back_stat"
#define TOPIC_NAME_AIRCRAFT_INFO "dds_aircraft_status"

struct adbc{
	float a;
	float b;
};

typedef struct DDS_CraftData
{
	DDS_CraftData()
	{
		mBase.a = 0;
		mBase.b = 0;
		mGearStatus = 0;
		mflapStatus = 0;//襟翼状态
		mWindValue = 0; //风速
		mCloudValue = 0;
		mRadioAlt = 0;
		mPicthAngle = 0;
		mRollAngle = 0;
		mHeading = 0;
		mIAS = 0;
		mTAS = 0;       //真空速,knot
		mVertical_Speed = 0; //垂直速度, ft/min
		mLon = 0;
		mLat = 0;
		mAlt = 0;
	}

	struct adbc mBase;
	char    mAircraftType[10];
	float     mGearStatus;//起落架状态
	float     mflapStatus;//襟翼状态
	int     mWindValue; //风速
	int     mCloudValue;//云数据
	float   mRadioAlt;//无线电高（英尺）
	float   mPicthAngle;//俯仰角（度）
	float   mRollAngle;//翻滚角（度）
	float   mHeading;//航向角（度）
	float   mIAS;//指示空速（k/not）
	float   mTAS;       //真空速（k/not）
	float   mVertical_Speed; //垂直速度（ft/min）
	double  mLon; //经度（度）
	double  mLat; //纬度（度）
	double  mAlt; //高度（英尺）

	int len;
	float Ext[1];//外界大气温度 暂定
}HmsCraftData;

//燃油引擎
typedef struct Eng_Fuel{
	Eng_Fuel()
	{
		fuelFlow = 0;
		fuelPressure = 0;
		remainOil = 0;
		lOilpressure = 0;
		lOilTemp = 0;
		fuelTemp = 0;
		EGT = 0;

		N1 = 0;
		N2 = 0;
		rpm = 0;
	};
	float fuelFlow; //燃油流量
	float fuelPressure;//燃油油压
	float remainOil;//剩余油量
	float lOilpressure; //滑润油压
	float lOilTemp; //滑润油温
	float fuelTemp; //燃油温度
	float EGT; // 排气温度

	float N1;
	float N2;

	float rpm;		//转速
}ENGFUEL;


//PFD
typedef struct Pfd_Data
{
	Pfd_Data()
	{
		ias = 0;
		winSpd = 0;
		attAngle = 0;
		overLoad = 0;
		gs = 0;
		mach = 0;

		rollAngle = 0;
		pitchAngle = 0;
		heading = 0;

		trueAlt = 0;
		radarAlt = 0;
		//rest = false;
		sideslip = 0;
	};

	float ias; //真空速
	float winSpd; //风速
	float novalue;
	float attAngle; //攻角
	float overLoad; //过载
	float gs;//地速
	float mach; //马赫

	float rollAngle;//滚转角
	float pitchAngle; //俯仰角
	float heading; //航向

	float trueAlt;//真实高度
	float radarAlt; //雷达高度
	float sideslip;  //侧滑 AOB
	//bool rest;//气压高度
}PFDDATA;

typedef struct Mcp_Info
{
	Mcp_Info()
	{
		heading = 0;
		altitude = 0;
		mach = 0;
		vertSpd = 0;

		ap = false;
		alt = false;
		spd = false;
		at = false;
		hdg = false;
		nav = false;
		vs = false;
	};

	float heading;//航向
	float altitude;//高度
	float mach;//马赫
	float vertSpd;//vert spd

	bool ap;
	bool alt;
	bool spd;
	bool at;
	bool hdg;
	bool nav;
	bool vs;
}MCPINFO;

#pragma pack(pop)
#endif

struct DemoData
{
	DDS_CraftData	aftData;
};

extern DemoData g_demoData;

struct HmsAftSpd
{
	float	fIAS;		//空速		feet/s
	float	fGC;		//地速		Km/h
	float	fMACH;		//马赫数	m
	float	fCAS;		//校正空速	calibreated Air Speed;
	float	fTAS;		//真空速	feet/s
};

struct HmsAftPos
{
	double dLongitude; //经度,deg;
	double dLatitude;  //纬度,deg;
	double dAltitude;  //海拔高度,m;
};

struct HmsAftAtt		//姿态
{
	float fPitch;       //俯仰,deg
	float fRoll;        //滚转,deg
	union
	{
		float fHeading; //航向,deg
		float fyaw;     //偏航,deg
	};
};

struct HmsLocalAftData
{
	HmsAftPos	pos;		//位置
	HmsAftSpd	spd;		//速度
	HmsAftAtt	att;		//姿态
	int			nRedBlue;   //1：红方，2：绿方；
	float		fVerticalSpd;
	float		fWinSpd;	//风速
	float		fAoa;		//攻角
	float		fG;		//过载

	float		fRadarAlt; //雷达高度
};

#define AFT_STATUS_MSG_ENG_FIRE				1
#define AFT_STATUS_MSG_ENG_FAILED			2
#define AFT_STATUS_MSG_ENG_SMOKE			3

#define AFT_STATUS_MSG_INSTU_SPD				1
#define AFT_STATUS_MSG_INSTU_ALT				2
#define AFT_STATUS_MSG_INSTU_ATT				3
#define AFT_STATUS_MSG_INSTU_TRN				4
#define AFT_STATUS_MSG_INSTU_VS					5
#define AFT_STATUS_MSG_INSTU_HSI				6

enum class AftMsgType
{
	Warning,		//警告
	Failed,			//失效
};

struct HmsAftStatusMsg
{
	AftMsgType	eAftMsgType;
	int			nMsgID;				//消息id
	int			nMsgStatus;			//消息状态  0 解除  1 有效
};

struct HmsInstrumentFailed
{
	bool bAirspeed;
	bool bAltitude;
	bool bAttitude;
	bool bTurnCoord;
	bool bVerticalSpd;
	bool bHsi;
};

#endif