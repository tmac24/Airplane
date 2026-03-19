#pragma once

#include <string>

struct T_Frequency
{
	int rowId;		//行id，主键，自动递增
	float frequency;//频率
	int category;	//类型 1COM 2ADF 3VOR
	int recordType;	//记录类型 1Recent 4User
};