#pragma once

#include <string>
#include "math/Vec2.h"
#include "math/Vec3.h"
#include "display/HmsImageNode.h"

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
	float index;
	std::string name;
};

struct WptNodeBrief
{
	WptPointType wType;
	float lon;
	float lat;
	std::string ident;

	WptNodeBrief()
	{
		wType = WptPointType::WPT_WPT;
		lon = 0;
		lat = 0;
	}
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
		else if (this->primaryIndex == stu.primaryIndex)
		{
			if (this->subIndex <= stu.subIndex)
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

struct HistoryRouteStu
{
	int idInDb;
	//表示是点还是程序
	FPL2DNodeType ePriType;
	FPL2DNodeType eSubType;

	union
	{
		struct
		{
			//是点使用这些属性
			WptPointType type;
			float lon;
			float lat;
			char  ident[64];
		}pointAttr;

		struct
		{
			//是程序使用这些属性
			char belongAirport[64];
			char programName[64];
			char transName[64];
			char rnwName[64];
		}programAttr;
	}attr;

	HistoryRouteStu()
	{
		idInDb = 0;
		ePriType = FPL_TYPE_POINT;
		eSubType = FPL_TYPE_POINT;
		memset(&attr, 0, sizeof(attr));
	}
};

