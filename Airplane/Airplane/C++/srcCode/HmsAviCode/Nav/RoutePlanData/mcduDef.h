#ifndef _MCDU_SIM_MAKE_DEF_HEADER_H______
#define _MCDU_SIM_MAKE_DEF_HEADER_H______

#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
//#include "SimPfPosition.h"

#include "../../Calculate/PositionZs.h"
#include "HmsStandardDef.h"

#pragma pack(push)
//#pragma pack(1)
#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define MCDU_SCREEN_BORDER				12	//边距
#define MCDU_SCREEN_LINE_SPACE			8	//行间距
#define MCDU_SCREEN_SMALL_TEXT			15	//小文字大小
#define MCDU_SCREEN_NORMAL_TEXT			20	//大文字大小
#define MCDU_SCREEN_LINE_SPACE_SMALL	3	//两文字之间的间距

struct McduPosCtl
{
	double dLineBorder;
	double dLineBegin;
	double dLineEnd;
	double dLineWidth;
	double dLineSpace;					//行间距
	double dLineSpcaeSmall;				//小行间距，大小文字之间
	double dLineSmallTextHeight;		//小文字行高
	double dLineNormalTextHeight;		//大文字行高
	double dLineLSKHeight;				//行选择键行高
#if _NAV_SVS_LOGIC_JUDGE
	McduPosCtl()
	{
		dLineBorder = 0.0;
		dLineBegin = 0.0;
		dLineEnd = 0.0;
		dLineWidth = 0.0;
		dLineSpace = 0.0;
		dLineSpcaeSmall = 0.0;
		dLineSmallTextHeight = 0.0;
		dLineNormalTextHeight = 0.0;
		dLineLSKHeight = 0.0;
	}
#else
	McduPosCtl();
#endif
	
};

enum FMC_MODE
{
	FMC_MODE_PRE_FLIGHT,
	FMC_MODE_TAKEOFF_AND_CLIMB,
	FMC_MODE_CRUISE,
	FMC_MODE_DESCENT_AND_APPROACH,
};


#define MAKE_INPUT_ID(pageID,dataID) ((int)(((short)(dataID)) | ((int)((short)(pageID))) << 16))
#define GET_PAGE_ID(inputID) ((short)(inputID>>16))
#define GET_DATA_ID(inputID) ((short)(inputID))

#define MCDU_PAGE_ID_UNDEFINE		0xFFFFFFFF
#define MCDU_PAGE_ID_IDENT			0x00
#define MCDU_PAGE_ID_INIT_REF_INDEX 0x01
#define MCDU_PAGE_ID_MENU			0x02
#define MCDU_PAGE_ID_POS_INIT		0x03
#define MCDU_PAGE_ID_POS_REF		0x04
#define MCDU_PAGE_ID_POS_SHIFT		0x05
#define MCDU_PAGE_ID_PERF_INIT		0x06
#define MCDU_PAGE_ID_TAKEOFF_PEF	0x07
#define MCDU_PAGE_ID_APPROACH_REF	0x08
#define MCDU_PAGE_ID_RTE			0x09
#define MCDU_PAGE_ID_ECON_CLB		0x0a
#define MCDU_PAGE_ID_ECON_CRZ		0x0b
#define MCDU_PAGE_ID_DES			0x0c
#define MCDU_PAGE_ID_LEGS			0x0d
#define MCDU_PAGE_ID_DEP_ARR_INDEX  0x0e
#define MCDU_PAGE_ID_HOLD			0x0f
#define MCDU_PAGE_ID_PROGRESS		0x11
#define MCDU_PAGE_ID_N1_LIMIT		0x12
#define MCDU_PAGE_ID_FIX_INFO		0x13
#define MCDU_PAGE_ID_NAV_STATUS		0x14
#define MCDU_PAGE_ID_OFFSET			0x15
#define MCDU_PAGE_ID_DEPARTURES		0x16
#define MCDU_PAGE_ID_ARRIVALS		0x17
#define MCDU_PAGE_ID_SEL_DESIRED	0x18

#define MCDU_PAGE_POS_INIT_AIRPORT	0x01

#define MCDU_PAGE_RTE_ORIGIN		0x01
#define MCDU_PAGE_RTE_DEST			0x02
#define MCDU_PAGE_RTE_CO_ROUTE		0x03
#define MCDU_PAGE_RTE_FLY_N0		0x04

struct McduAirportInfo
{
	char strICAO[5];
	double dLatitude;
	double dLongitude;

	McduAirportInfo()
	{
		strICAO[0] = 0;
		strICAO[1] = 0;
		strICAO[2] = 0;
		strICAO[3] = 0;
		strICAO[4] = 0;
		dLatitude = 0.0;
		dLongitude = 0.0;
	}
};

struct FmcIdent 
{
	std::string strNavData;
	std::string strOpProgram;
	std::string strCoData;
	std::string strModel;
	std::string strEngin;
};

struct McduNavFixInfo
{
	char strFixDescribe[25];
	char strFixName[6];
	double dLatitude;
	double dLongitude;

	McduNavFixInfo()
	{
		memset(this, 0, sizeof(McduNavFixInfo));
	}
};

struct McduNavRteInfo
{
	char	strRteCode[6];		//航路代码
	int		nRteID;				//代码ID	
	char	strRteName[6];		//航路名称（5个）
	double	dLatitude;
	double	dLongitude;

	McduNavRteInfo()
	{
		memset(this, 0, sizeof(McduNavRteInfo));
	}
};

struct McduNavAidInfo
{
	char strAidDescribe[25];
	char strAidName[6];
	char strAidType[6];
	double dLatitude;
	double dLongitude;
	float  fAidFrequency;
	char cAidType;		//H:Hight L:Low T:Terminal N:NDB
	McduNavAidInfo()
	{
		memset(this, 0, sizeof(McduNavAidInfo));
	}
};

struct McduAirportRunway
{
	char	strAirportDescribe[25];				//机场描述
	char	strAirportICAO[5];					//机场4字码
	char	strRunwayName[4];					//跑道名称  
	int		nRunwayLength;						//跑道长度，英尺
	int		nRunwayDirection;					//跑道方向
	double	dLatitude;
	double	dLongitude;
	float	fIlsFrequency;						//ILS频率
	int		nDirection;							//方面和跑道方向相同
	int		nAltitude;							//跑道海拔

	McduAirportRunway()
	{
		memset(this, 0, sizeof(McduAirportRunway));
	}
};

struct LineParam
{
	int		nLineIndex;
	bool	isLeft;
	int		nParamIndex;

	LineParam()
	{
		nLineIndex = 0;
		isLeft = true;
		nParamIndex = 0;
	}

	LineParam(int lineIndex, bool bLeft, int paramIndex)
	{
		nLineIndex = lineIndex;
		isLeft = bLeft;
		nParamIndex = paramIndex;
	}

	void SetData(int lineIndex, bool bLeft = true, int paramIndex = 0)
	{
		nLineIndex = lineIndex;
		isLeft = bLeft;
		nParamIndex = paramIndex;
	}
};

#define ALT_LIMIT_NOUSE  0x00
#define ALT_LIMIT_EQUAL  0x01
#define ALT_LIMIT_ABOVE	 0x02
#define ALT_LIMIT_BELOW	 0x04

struct AltLimit
{
	double		dAltitude;
	int			nCtrlFlags;	

	AltLimit()
	{
		dAltitude = 0.0;
		nCtrlFlags = ALT_LIMIT_EQUAL;
	}
	void Clean(){dAltitude=0.0; nCtrlFlags=ALT_LIMIT_NOUSE;}
	void CleanFlags(){nCtrlFlags=ALT_LIMIT_NOUSE;}
	void AddLimitBelow(){nCtrlFlags|=ALT_LIMIT_BELOW;}
	void AddLimitAbove(){nCtrlFlags|=ALT_LIMIT_ABOVE;}
	void AddLimitAt(){nCtrlFlags|=ALT_LIMIT_EQUAL;}
};

#define WAY_CTRL_POINT_FLAG_BIT_HDG			(1<<0)
#define WAY_CTRL_POINT_FLAG_BIT_HDG2HDG		(1<<1)
#define WAY_CTRL_POINT_FLAG_BIT_DISTANCE	(1<<2)
#define WAY_CTRL_POINT_FLAG_BIT_ALT_LIMIT	(1<<3)
#define WAY_CTRL_POINT_FLAG_BIT_FIX_USE		(1<<4)
#define WAY_CTRL_POINT_FLAG_BIT_TUEN_DIRECT (1<<5)
#define WAY_CTRL_POINT_FLAG_BIT_RNW			(1<<6)
#define WAY_CTRL_POINT_FLAG_BIT_HOLD		(1<<7)

// struct McduWayCtrlPoint
// {
// 	std::string			strName;			//名称，航电名称
// 	EarthCoord2D	earthCoord;
// 	AltLimit		altLimit[2];		//两个高度限制
// 	double			dDistance;
// 	int				nOrgHDG;
// 	int				nDstHDG;
// 	char			cType;			//T,H,F::::TRK,HDG,FIX
// 	bool			bOverFly;
// 	int				nTurn;			//0,no use   1:right  -1:left
// 	int				nLegTime;
// 	int				flags;
// 
// 	McduWayCtrlPoint()
// 	{
// 		dDistance = 0;
// 		nOrgHDG = 0;
// 		nDstHDG = 0;
// 		cType = 0;
// 		bOverFly = false;
// 		nTurn = 0;
// 		nLegTime = 0;
// 		flags = 0;
// 	}
// 	void Clean()
// 	{
// 		strName.clear();
// 		earthCoord.Clean();
// 		altLimit[0].Clean();
// 		altLimit[1].Clean();
// 		dDistance = 0;
// 		nOrgHDG = 0;
// 		nDstHDG = 0;
// 		cType = 0;
// 		bOverFly = false;
// 		nTurn = 0;
// 		nLegTime = 0;
// 		flags = 0;
// 	}
// };

enum ROUTE_TYPE
{
	ROUTE_TYPE_INIT = 0,
	ROUTE_TYPE_DISCONTINUTIY,			//不连续点
	ROUTE_TYPE_DEP_RNW,
	ROUTE_TYPE_DEP_SID,
	ROUTE_TYPE_DEP_SID_TRANS,
	ROUTE_TYPE_FIX,
	ROUTE_TYPE_ARR_STAR_TRANS,
	ROUTE_TYPE_ARR_STAR,
	ROUTE_TYPE_ARR_APPR_TRANS,
	ROUTE_TYPE_ARR_APPR,
	ROUTE_TYPE_ARR_RNW,
	ROUTE_TYPE_LAST_ADD,
};

enum FIX_STATE
{
	FIX_STATE_IN_ACTIVE = 0,
	FIX_STATE_ACTIVE,
	FIX_STATE_MODIFY,
};

enum FIX_TYPE
{
	FIX_TYPE_INIT = 0,

	FIX_TYPE_DISCONTINUITY,

	FIX_TYPE_FIX,
	FIX_TYPE_FIX_ALTITUDE,
	FIX_TYPE_FIX_SPEED,
	FIX_TYPE_FIX_SPEED_AND_ALTITUDE,
	FIX_TYPE_FIX_UNDEFINE,
	//其中空置左转直飞，右转直飞，飞跃

	FIX_TYPE_RNW,

	FIX_TYPE_HOLD_DEST,
	FIX_TYPE_HOLD_TIME,
	FIX_TYPE_HOLD_UNDEFINE,

	FIX_TYPE_COURSE_VECTORS,
	FIX_TYPE_COURSE_ALTITUDE,
	FIX_TYPE_COURSE_DISTANCE_FIX,
	FIX_TYPE_COURSE_RADIAL_TO_FIX,
	FIX_TYPE_COURSE_RADIAL_FROM_FIX,
	FIX_TYPE_COURSE_UNDEFINE,
};

struct FmcFixCtrl 
{
	int  nDirectType:4;		//-1:left  0:none   1:right
	int  nOverfly:4;			//0:not	1:overfly
	int  nSpeedCnt:8;			//0:not have speed ctrl   1:have Speed ctrl
	int  nAltitudeCnt:8;		//0:none   1:one Altitude  2:two Altitude
	int  nIsHdg:8;			//0:TRK		1:HDG

	FmcFixCtrl()
	{
		nDirectType = 0;
		nOverfly = 0;
		nSpeedCnt = 0;
		nAltitudeCnt = 0;
		nIsHdg = 0;
	}

	void Clean()
	{
		nDirectType = 0;
		nOverfly = 0;
		nSpeedCnt = 0;
		nAltitudeCnt = 0;
		nIsHdg = 0;
	}


};

struct FmcFixStr
{
	std::string			strFixName;
	EarthCoord2D	coordFix;

	void Clean()
	{
		strFixName.clear();
		coordFix.Clean();
	}
};

struct FmcFixChar
{
	char			strFixName[16];
	EarthCoord2D	coordFix;

	FmcFixChar()
	{
		memset(strFixName, 0, sizeof(strFixName));
	}

	FmcFixChar(const FmcFixStr & fix)
	{
		memset(strFixName, 0, sizeof(strFixName));
		int nLen = fix.strFixName.length();
		if (nLen > 15)
		{
			nLen = 15;
		}

		if (nLen)
		{
			auto tempData = fix.strFixName.c_str(); 
			memcpy(strFixName, tempData, nLen);
		}

		coordFix = fix.coordFix;
	}

	void Clean()
	{
		memset(strFixName, 0, sizeof(strFixName));
		coordFix.Clean();
	}

	void SetName(const std::string &str)
	{
		if (str.size() < sizeof(strFixName))
		{
			strcpy(strFixName, str.c_str());
		}
		else
		{
			memcpy(strFixName, str.c_str(), sizeof(strFixName) - 1);
			strFixName[15] = 0;
		}
	}
};

struct FmcAltitude
{	
	union
	{
		struct  
		{
			unsigned char bAt:1;
			unsigned char bBelow:1;
			unsigned char bAbove:1;
		}bitType;
		char cType;	
	} uLimit;
	int nAltitude;//英尺

	FmcAltitude()
	{
		Clean();
	}

	void Clean()
	{
		uLimit.cType = 0;
		nAltitude = -1;
	}

#if 0
	std::string ToString() const
	{
		std::stringstream ss;
		if (nAltitude > 9999)
		{
			ss << "FL" << nAltitude / 100;
		}
		else
		{
			ss << nAltitude;
		}
		
		if (uLimit.bitType.bAbove)
		{
			ss << 'A';
		}
		else if(uLimit.bitType.bBelow)
		{
			ss << 'B';
		}
		return ss.str();
	}
#endif
};

struct McduWayCtrlPoint
{
	ROUTE_TYPE		eRouteType;		//线路类型
	FIX_TYPE		eFixType;		//Fix类型
	FIX_STATE		eFixState;		//Fix状态
	FmcFixChar		fix;			//航点、包括跑道等，具体信息有FIX_TYPE决定
	FmcFixCtrl		ctrl;			//控制信息
	short			nHeading;		//航向
	short			nRadial;		//切线方向
	short			nSpeed;			//速度
	short			nLegData;		//circle time, 两个航向交点
	float			fDistance;		//距离
	FmcAltitude		nAltitude[2];	//高度控制

	McduWayCtrlPoint()
	{
		eRouteType = ROUTE_TYPE_INIT;
		eFixType = FIX_TYPE_INIT;
		eFixState = FIX_STATE_IN_ACTIVE;
		nHeading = 0;
		nRadial = 0;
		nSpeed = 0;
		nLegData = 0;
		fDistance = 0;
	}

	void Clean()
	{
		eRouteType = ROUTE_TYPE_INIT;
		eFixType = FIX_TYPE_INIT;
		eFixState = FIX_STATE_IN_ACTIVE;
		fix.Clean();
		ctrl.Clean();
		nHeading = 0;
		nRadial = 0;
		nSpeed = 0;
		nLegData = 0;
		fDistance = 0;
		nAltitude[0].Clean();
		nAltitude[1].Clean();
	}

	std::string SpeedToString()
	{
		std::stringstream ss;
		if (ctrl.nSpeedCnt)
		{
			ss << nSpeed;
		}
		else
		{
			ss << "----";
		}
		return ss.str();
	}
};

struct McduRteElement
{
	int nID;			//用于存贮，行编号。用于更新
	union{
		struct  
		{
			unsigned char bSpeed:1;					//更改速度
			unsigned char bAlt:1;					//更改高度
			unsigned char bDiscontinuityThen:1;		//显示输入不连续点
			unsigned char bDiscontinultyTitle:1;	//显示不连续点标题
		}bCtrl;
		unsigned char cCtrl;
	}uCtrl;

	std::string strFixName;
	std::string	strFixTitle;	//上面一排信息		航向、距离、下滑道角度
	std::string strFixContent;	//下面一排信息		速度、高度控制。

	McduRteElement();

	void Init();

	void Clean();
};

struct SimFmcFixInfo 
{
	ROUTE_TYPE		eRouteType;		//线路类型
	FIX_TYPE		eFixType;		//Fix类型
	FIX_STATE		eFixState;		//Fix状态
	FmcFixChar		fix;			//航点、包括跑道等，具体信息有FIX_TYPE决定
	FmcFixCtrl		ctrl;			//控制信息
	short			nHeading;		//航向
	short			nRadial;		//切线方向
	short			nSpeed;			//速度
	short			nLegData;		//circle time
	float			fDistance;		//距离
	FmcAltitude		nAltitude[2];	//高度控制

	SimFmcFixInfo()
	{
		eRouteType = ROUTE_TYPE_INIT;
		eFixType = FIX_TYPE_INIT;
		eFixState = FIX_STATE_IN_ACTIVE;
		nHeading = 0;
		nRadial = 0;
		nSpeed = 0;
		nLegData = 0;
		fDistance = 0;
	}

	SimFmcFixInfo(const McduWayCtrlPoint & waypoint)
	{
		eRouteType = waypoint.eRouteType;
		eFixType = waypoint.eFixType;
		eFixState = waypoint.eFixState;
		fix = waypoint.fix;
		ctrl = waypoint.ctrl;
		nHeading = waypoint.nHeading;
		nRadial = waypoint.nRadial;
		nSpeed = waypoint.nSpeed;
		nLegData = waypoint.nLegData;
		fDistance = waypoint.fDistance;
		nAltitude[0] = waypoint.nAltitude[0];
		nAltitude[1] = waypoint.nAltitude[1];

	}

	void Clean()
	{
		eRouteType = ROUTE_TYPE_INIT;
		eFixType = FIX_TYPE_INIT;
		eFixState = FIX_STATE_IN_ACTIVE;
		fix.Clean();
		ctrl.Clean();
		nHeading = 0;
		nRadial = 0;
		nSpeed = 0;
		nLegData = 0;
		fDistance = 0;
		nAltitude[0].Clean();
		nAltitude[1].Clean();
	}
};

struct McduRunwayInfo
{
	char							runwayName[16];
	std::vector<McduWayCtrlPoint>	vWayPoint;

	McduRunwayInfo()
	{
		ClearName();
	}

	void ClearName()
	{
		memset(runwayName, 0, sizeof(runwayName));
	}

	void SetName(const std::string &str)
	{
		if (str.size() < sizeof(runwayName))
		{
			strcpy(runwayName, str.c_str());
		}
		else
		{
			memcpy(runwayName, str.c_str(), sizeof(runwayName) - 1);
			runwayName[15] = 0;
		}
	}
};

struct McduTRANSITIONInfo
{
	char							transName[16];
	std::vector<McduWayCtrlPoint>	vWayPoint;

	McduTRANSITIONInfo()
	{
		ClearName();
	}

	void ClearName()
	{
		memset(transName, 0, sizeof(transName));
	}

	void SetName(const std::string &str)
	{
		if (str.size() < sizeof(transName))
		{
			strcpy(transName, str.c_str());
		}
		else
		{
			memcpy(transName, str.c_str(), sizeof(transName) - 1);
			transName[15] = 0;
		}
	}
};

struct McduSIDInfo
{
	std::string						sidName;
	std::vector<McduWayCtrlPoint>	vWayPoint;
	std::multimap<std::string, McduTRANSITIONInfo> mapTrans;
	std::vector<std::string>			vRnwName;
};

struct McduSTARInfo
{
	std::string						starName;
	std::vector<McduWayCtrlPoint>	vWayPoint;
	std::multimap<std::string, McduTRANSITIONInfo> mapTrans;
	std::vector<std::string>			vRnwName;
};

struct McduProgramInfo
{
	ROUTE_TYPE		eRouteType;
	char			name[16];
	std::vector<McduWayCtrlPoint>	vWayPoint;
	std::map<std::string, McduTRANSITIONInfo> mapTrans;
	std::map<std::string, McduRunwayInfo> mapRunway;

	McduProgramInfo()
	{
		ClearName();
	}

	void ClearName()
	{
		memset(name, 0, sizeof(name));
	}
};

struct McduAPPROACHEInfo
{
	std::string						approacheName;
	std::vector<McduWayCtrlPoint>	vWayPoint;
	std::multimap<std::string, McduTRANSITIONInfo> mapTrans;
};

enum N1_LIMIT_STATUS
{
	N1_LIMIT_STATUS_SELECT,
	N1_LIMIT_STATUS_ACT,
};

enum N1_TO_STATUS
{
	N1_TO_STATUS_TO,
	N1_TO_STATUS_TO_1,
	N1_TO_STATUS_TO_2,
	N1_TO_STATUS_TO_B,
};

enum N1_CLB_STATUS
{
	N1_CLB_STATUS_CLB,
	N1_CLB_STATUS_CLB_1,
	N1_CLB_STATUS_CLB_2,
};

struct N1Limit
{
	N1_TO_STATUS		to;
	N1_LIMIT_STATUS		toLimit;
	N1_CLB_STATUS		clb;
	N1_LIMIT_STATUS		clbLimit;

	N1Limit()
	{
		to = N1_TO_STATUS_TO;
		clb = N1_CLB_STATUS_CLB;
		toLimit	= N1_LIMIT_STATUS_SELECT;
		clbLimit = N1_LIMIT_STATUS_SELECT;
	}
};

enum ARRIVAL_STATUS
{
	ARRIVAL_STATUS_NULL,
	ARRIVAL_STATUS_SELECT,
	ARRIVAL_STATUS_ACTIVE,
};

struct ArrivalSelect
{
	ARRIVAL_STATUS	dsStar;
	ARRIVAL_STATUS	dsStarTrans;
	ARRIVAL_STATUS	dsAppro;
	ARRIVAL_STATUS	dsApproTrans;
	std::string			strStar;
	std::string			strStarTrans;
	std::string			strAppro;
	std::string			strApproTrans;


	ArrivalSelect()
	{
		dsStar = ARRIVAL_STATUS_NULL;
		dsStarTrans = ARRIVAL_STATUS_NULL;
		dsAppro = ARRIVAL_STATUS_NULL;
		dsApproTrans = ARRIVAL_STATUS_NULL;
	}

	void Clean()
	{
		dsStar = ARRIVAL_STATUS_NULL;
		dsStarTrans = ARRIVAL_STATUS_NULL;
		dsAppro = ARRIVAL_STATUS_NULL;
		dsApproTrans = ARRIVAL_STATUS_NULL;

		strStar.clear();
		strStarTrans.clear();
		strAppro.clear();
		strApproTrans.clear();
	}
};

enum DEPARTURE_STATUS
{
	DEPARTURE_STATUS_NULL,
	DEPARTURE_STATUS_SELECT,
	DEPARTURE_STATUS_ACTIVE,
};

struct DepartureSelect
{
	DEPARTURE_STATUS	dsSid;
	DEPARTURE_STATUS	dsTrans;
	DEPARTURE_STATUS	dsRwy;
	std::string				strSid;
	std::string				strTrans;
	std::string				strRwy;


	DepartureSelect()
	{
		dsSid = DEPARTURE_STATUS_NULL;
		dsTrans = DEPARTURE_STATUS_NULL;
		dsRwy = DEPARTURE_STATUS_NULL;
	}

	void Clean()
	{
		dsRwy = DEPARTURE_STATUS_NULL;
		dsTrans = DEPARTURE_STATUS_NULL;
		dsSid = DEPARTURE_STATUS_NULL;

		strSid.clear();
		strTrans.clear();
		strRwy.clear();
	}
};

/************************************************************************
函数名称：	int FindRunway(const QVector<McduWayCtrlPoint> & vWayPoint, int nFindPos, std::string & runway);
函数功能：	查找跑道数据
函数参数：	
返回值：	在容器中的位置 -1查询失败
作者：		ZS	
修改日期：	<2013:8:13 11>   
************************************************************************/
//int FindRunway(const std::vector<McduWayCtrlPoint> & vWayPoint, int nFindPos, std::string & runway);
#pragma pack(pop)
#endif