#pragma once

#include <string>
#include "math/Vec2.h"
#include "math/Vec3.h"
#include "base/RefPtr.h"
#include "display/HmsImageNode.h"
#include "RoutePlanData/mcduDef.h"

enum class FrameEventState
{
	DRAG_OLD_NODE,
	DRAG_NEW_NODE,
	DRAG_NULL
};

struct OBSScaleLabelStu
{
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode> nodeLabel;
	HmsAviPf::Vec3 earthPos;
	bool bEnable;

	OBSScaleLabelStu()
	{
		nodeLabel = nullptr;
		bEnable = false;
	}
};

struct NavLineAttrStu
{
	std::vector<bool> vecIsVisible;
	std::vector<HmsAviPf::Vec3> vecEarthPos;
	std::vector<HmsAviPf::Vec2> vec2DPos;

	void Clear()
	{
		vecIsVisible.clear();
		vecEarthPos.clear();
		vec2DPos.clear();
	}
};

struct PlaneSegStu
{
	HmsAviPf::Vec2 planeLonLat;
	HmsAviPf::Vec2 wptLonLat;
	NavLineAttrStu airplaneLine;

	bool bIsObsMode;
	float fObsAngle;
	NavLineAttrStu obsFull;
	NavLineAttrStu obsDash;

	//OBS刻度
	std::vector<OBSScaleLabelStu> vecScale;

    PlaneSegStu()
    {
        bIsObsMode = false;
        fObsAngle = 0;
    }
};

enum class WptPointType
{
	WPT_VOR = 0x01,
	WPT_WPT = 0x02,
	WPT_USERWPT = 0x04,
	WPT_NDB = 0x08,
	WPT_AIRPORT = 0x10,
	WPT_PROGRAM = 0x20
};

struct AltitudeDataStu
{
	float lon;
	float lat;
	float distance;
	float altitude;
};

struct NavNodeStu
{
	int index;
	std::string name;
};

enum SearchShapeType
{
	SHAPE_NULL = 0,
	SHAPE_END,
	SHAPE_SPIRAL,
	SHAPE_RECT_SPIRAL,
	SHAPE_RECT
};

struct SearchShapeStu
{
	SearchShapeType type;
	//两个螺旋属性
	float radius;
	
	//共用
	float space;
	float angle;

	//矩形属性
	float width;
	float height;
	float posInSide;//0 ~ 1

	SearchShapeStu()
	{
		type = SHAPE_NULL;
		radius = 30;
		space = 5;
		angle = 0;
		width = 30;
		height = 30;
		posInSide = 0.5;
	}
};

struct AdditionAttrStu
{
	FmcAltitude altStu[2];
	SearchShapeStu searchShape;
};

struct WptNodeBrief
{
	WptPointType wType;
	float lon;
	float lat;
	std::string ident;
	
	AdditionAttrStu attr;

	WptNodeBrief()
	{
		wType = WptPointType::WPT_WPT;
		lon = 0;
		lat = 0;
	}
};

struct WptNodeStu2D
{
	WptNodeBrief brief;
	McduWayCtrlPoint ctrlPoint;

	HmsAviPf::Vec3 earthPos;
	HmsAviPf::CHmsNode *pNode;

	bool bEnableNode;//false时不参与插值，不显示
	std::vector<bool> vecInsertLonLatIsVisible;
	std::vector<HmsAviPf::Vec3> vecInsertEarthPos;
	std::vector<HmsAviPf::Vec2> vecInsert2DPos;

	WptNodeStu2D()
	{
		pNode = nullptr;
		bEnableNode = true;
	}
	WptNodeStu2D(McduWayCtrlPoint &ctrlPoint)
	{
		brief.wType = WptPointType::WPT_WPT;
		brief.lon = ctrlPoint.fix.coordFix.dLongitude;
		brief.lat = ctrlPoint.fix.coordFix.dLatitude;
		brief.ident = ctrlPoint.fix.strFixName;
		brief.attr.altStu[0] = ctrlPoint.nAltitude[0];
		brief.attr.altStu[1] = ctrlPoint.nAltitude[1];

		this->ctrlPoint = ctrlPoint;

		pNode = nullptr;
		bEnableNode = true;
	}
// 	bool operator==(const WptNodeStu2D & stu) const
// 	{
// 		return (this->brief.wType == stu.brief.wType) && (this->brief.ident == stu.brief.ident);
// 	}
};

struct FPLNodeIndex
{
	int primaryIndex;
	int subIndex;

	FPLNodeIndex()
	{
		primaryIndex = -1;
		subIndex = -1;
	}
	FPLNodeIndex(int p, int s)
	{
		primaryIndex = p;
		subIndex = s;
	}
	bool operator==(const FPLNodeIndex& stu) const
	{
		return (this->primaryIndex == stu.primaryIndex) && (this->subIndex == stu.subIndex);
	}
	bool operator<(const FPLNodeIndex& stu) const
	{
		if (this->primaryIndex < stu.primaryIndex)
		{
			return true;
		}
		else if (this->primaryIndex > stu.primaryIndex)
		{
			return false;
		}
		else
		{
			if (this->subIndex < stu.subIndex)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
};

enum FPL2DNodeType
{
	FPL_TYPE_POINT = 0,
	FPL_TYPE_PROGRAM,
	FPL_TYPE_PROGRAM_SID,
	FPL_TYPE_PROGRAM_STAR,
	FPL_TYPE_PROGRAM_APP
};

struct FPL2DNodeInfoStu
{
	//标识是单个点还是组
	FPL2DNodeType ePriType;
	FPL2DNodeType eSubType;
	
	//识别码或组名
	std::string	ident;
	
	//是单个点时这些属性不可用
	std::string belongAirport;
	std::string	programName;
	std::string	transName;
	std::string	rnwName;

	std::vector<WptNodeStu2D>	vWayPoint;

	FPL2DNodeInfoStu()
	{
		ePriType = FPL_TYPE_POINT;
		eSubType = FPL_TYPE_POINT;
	}

// 	bool operator==(const FPL2DNodeInfoStu& stu) const
// 	{
// 		return (this->ePriType == stu.ePriType) && (this->eSubType == stu.eSubType) && (this->ident == stu.ident);
// 	}
};

struct HistoryRouteOnePointStu
{
	int idInDb;
	//表示是点还是程序
	FPL2DNodeType ePriType;
	FPL2DNodeType eSubType;

	struct _PointAttr
	{
		//是点使用这些属性
		WptPointType type;
		float lon;
		float lat;
		char  ident[64];
		FmcAltitude altStu[2];
		SearchShapeStu searchShape;

		_PointAttr()
		{
			type = WptPointType::WPT_WPT;
			lon = 0;
			lat = 0;
			memset(ident, 0, 64);
		}
		void SetIdent(const std::string &str)
		{
			if (str.length() < 64)
			{
				strcpy(ident, str.c_str());
			}
		}
	}pointAttr;

	struct _ProgramAttr
	{
		//是程序使用这些属性
		char belongAirport[64];
		char programName[64];
		char transName[64];
		char rnwName[64];

		_ProgramAttr()
		{
			memset(this, 0, sizeof(_ProgramAttr));
		}
		void SetBelongAirport(const std::string &str)
		{
			if (str.length() < 64)
			{
				strcpy(belongAirport, str.c_str());
			}
		}
		void SetProgramName(const std::string &str)
		{
			if (str.length() < 64)
			{
				strcpy(programName, str.c_str());
			}
		}
		void SetTransName(const std::string &str)
		{
			if (str.length() < 64)
			{
				strcpy(transName, str.c_str());
			}
		}
		void SetRnwName(const std::string &str)
		{
			if (str.length() < 64)
			{
				strcpy(rnwName, str.c_str());
			}
		}
	}programAttr;

	HistoryRouteOnePointStu()
	{
		idInDb = 0;
		ePriType = FPL_TYPE_POINT;
		eSubType = FPL_TYPE_POINT;
	}

	std::string GetIdent() const
	{
		std::string retStr;
		if (ePriType == FPL_TYPE_POINT)
		{
			retStr = pointAttr.ident;
		}
		else
		{
			std::string trans = programAttr.transName;
			std::string program = programAttr.programName;
			std::string runway = programAttr.rnwName;
			if (eSubType == FPL_TYPE_PROGRAM_SID)
			{
				if (trans.empty())
				{
					retStr = runway + "." + program;
				}
				else
				{
					retStr = runway + "." + program + "." + trans;
				}
			}
			else if (eSubType == FPL_TYPE_PROGRAM_STAR)
			{
				if (trans.empty())
				{
					retStr = program + "." + runway;
				}
				else
				{
					retStr = trans + "." + program + "." + runway;
				}
			}
			else
			{
				if (trans.empty())
				{
					retStr = program;
				}
				else
				{
					retStr = trans + "." + program;
				}
			}
		}
		return retStr;
	}
};

struct HistoryRouteStu
{
	int id;
	std::string name;
	std::vector<HistoryRouteOnePointStu> vecPoints;
};

struct FplSyncProgramAttrStu
{
	FPLNodeIndex index;
	AdditionAttrStu attr;
};

struct AirspaceStu
{
	std::vector<HmsAviPf::Vec2> vecData;
};

struct AirspaceEditStu
{
	int idInDb;
	std::string name;
	float lon;
	float lat;

	AirspaceEditStu()
	{
		idInDb = 0;
		lon = 0;
		lat = 0;
	}
};

struct UserWptStu
{
	int idInDb;
	std::string name;
	float lon;
	float lat;

	UserWptStu()
	{
		idInDb = 0;
		lon = 0;
		lat = 0;
	}
};
