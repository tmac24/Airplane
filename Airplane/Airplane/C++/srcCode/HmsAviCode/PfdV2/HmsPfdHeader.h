#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sstream>

typedef enum AirSpeedSign
{
	ASS_LINE = 0x00,
	ASS_TEXT = 0x01,
	ASS_TEXT_LEFT  = 0x02,
	ASS_TEXT_RIGHT = 0x03,
};

#define REF_VALUE1 (1e-2)
#define REF_VALUE2 (13-3)
#define DEF_PERCISION (1)
#define PFD_POINTER_BK_COLOR (Color4F(0.0, 0.0, 0.0, 1.0))/*指针背景色.*/
#define UNIT_COLOR (Color4B(0x00, 0xFF, 0xFF, 0xFF))
#define PFD_CONTINUE(_CONDITION_) if(_CONDITION_){continue;}
#define PFD_RETURN(_CONDITION_)   if(_CONDITION_){return;}
#define PFD_RETURN_VAL(_CONDITION_, _VALUE_)   if(_CONDITION_){return _VALUE_;}
#define HALF_VALUE(_VALUE_)		((_VALUE_) * 0.5f)
#define FOURTH_VALUE(_VALUE_)	((_VALUE_) * 0.25f)
#define THIRD_VALUE(_VALUE_)    ((_VALUE_) * 0.333f)

#define FUNC_FLOAT_EQUAL(_VALUE_, _BC_VALUE_, _REF_VALUE_) ((fabsf(_VALUE_ - _BC_VALUE_) < (_REF_VALUE_)) ? true : false)

static std::string GetDightString(int iValue, int nBit)
{
	std::stringstream os;
	os.fill('0');
	os.width(nBit);
	os << iValue;
	return os.str();
}

static std::string GetDightFloatString(float fValue, int nBit)
{
	std::stringstream os;
	os.setf(std::ios::fixed);
	os.precision(nBit);
	os << fValue;
	return os.str();
}

/*角度限制到-180-180之间.*/
static float _pfd_angle_normal(float angle)
{
	float curangle = fmodf(angle, 360.0f);
	if (curangle > 180.0f){
		curangle -= 360.0f;
	}
	else if (curangle < -180.0f){
		curangle += 360.0f;
	}

	return curangle;
}
static float _pfd_angle_normal_360(float angle)
{
	float curangle = fmodf(angle, 360.0f);
	if (curangle < 0.0f){
		curangle += 360.0f;
	}

	return curangle;
}

static char* _pfd_GBKToUTF8(const char* chGBK)
{
	static char pUTF8Buf[1024] = { 0 };
#ifdef _WIN32
	DWORD dWideBufSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)chGBK, -1, NULL, 0);
	wchar_t * pWideBuf = (wchar_t*)malloc(dWideBufSize * sizeof(wchar_t));
	memset(pWideBuf, 0, dWideBufSize);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)chGBK, -1, pWideBuf, dWideBufSize);

	DWORD dUTF8BufSize = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pWideBuf, -1, NULL, 0, NULL, NULL);
	memset(pUTF8Buf, 0, dUTF8BufSize);
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pWideBuf, -1, pUTF8Buf, dUTF8BufSize, NULL, NULL);
	free(pWideBuf);
#endif
	
	return pUTF8Buf;
}

#define ANGLE_NORMAL_180(_ANGLE_) _pfd_angle_normal(_ANGLE_)
#define ANGLE_NORMAL_360(_ANGLE_) _pfd_angle_normal_360(_ANGLE_)
#define INT_TO_STRING(_VALUE_, _PRECISION_) GetDightString((int)(_VALUE_), _PRECISION_)
#define FLOAT_TO_STRING(_VALUE_, _PRECISION_) GetDightFloatString(_VALUE_, _PRECISION_)
#define PFD_GBK2UTF8(_VALUE_) _pfd_GBKToUTF8(_VALUE_)