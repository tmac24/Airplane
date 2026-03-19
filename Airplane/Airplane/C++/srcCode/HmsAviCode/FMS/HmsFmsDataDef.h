#pragma once

#include <string>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

enum PhaseOfFlight	//飞行阶段
{
	NONE,		//无
	DPRT,		//离场
	TERM,		//终端
	ENR,		//航路
	APP			//进近
};

struct FmsCalData
{
	FmsCalData()
	{
		valid = false;
		course = 0.0;
		cdiRatio = 0.0;
		toFrom = 0;
		phaseOfFlight = "";
		flySegment = 0;
	}

	bool valid;			//数据有效性
	float course;		//航道方向
	float cdiRatio;		//偏离比例 -2至2
	int toFrom;			//向背台标识 -1背台 0无 1向台
	int flySegment;		//飞行航路段
	bool changeCourse;  //改变航道
	std::string phaseOfFlight;//飞行阶段
};
/*
struct HmsCtrlData		//Control Data
{
	float curCourse;	//current selected course
	bool  isOBSMode;	//is OBS Mode
};
*/