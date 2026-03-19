#ifndef _DATE_TYPE_DEFINE__H_
#define  _DATE_TYPE_DEFINE__H_

typedef enum _WarningLevel
{
	WarningLevel_RED,
	WarningLevel_YELLOW ,
	WarningLevel_NONE
}WarningLevel_;


typedef struct _CWarningPoint
{
	double lon;
	double lat;
	WarningLevel_ warningLevel;
}CWarningPoint,*pCWarningPoint;




/*
#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef _W64 unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif

*/




 

#endif