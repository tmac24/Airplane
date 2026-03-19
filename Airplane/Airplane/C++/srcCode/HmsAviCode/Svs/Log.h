

#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <stdarg.h>

#include <time.h>

 

typedef enum _LOG_LEVEL {
	LOG_LEVEL_ALWAYS = 0x0,
	LOG_LEVEL_FATAL = 0x1,
	LOG_LEVEL_WARN = 0x2,
	LOG_LEVEL_NOTICE = 0x3,
	LOG_LEVEL_INFO = 0x4,
	LOG_LEVEL_DEBUG_INFO = 0x5,
	LOG_LEVEL_DEBUG_FP = 0x6,
	LOG_LEVEL_DEBUG_TMP=0x7,
	LOG_LEVEL_NOMEG=0x8,
	LOG_LEVEL_ALL = LOG_LEVEL_ALWAYS | LOG_LEVEL_FATAL | LOG_LEVEL_WARN | LOG_LEVEL_NOTICE |
	LOG_LEVEL_INFO | LOG_LEVEL_DEBUG_INFO | LOG_LEVEL_DEBUG_FP |LOG_LEVEL_DEBUG_TMP
}LOG_LEVEL;

typedef struct  _CLOG
{
	char m_szFilename[256];

	int m_bEnabled;

	int m_fontSize;

	unsigned int FileSize;
	unsigned int FileNumber;
	/*最大日志文件个数 >=1*/
	unsigned int MaxFileNumber;
	/*单个文件最大大小 MB */
	unsigned int MaxFileSize; 


	LOG_LEVEL loglevel;

	unsigned int logid;
}CLOG, *pCLOG;


static int CLOG_GetFile(pCLOG p);
void  CLOG_Write(char* szString, ...);
static char* CLOG_GetColor(LOG_LEVEL color);
void  CLOG_Enable(pCLOG p);
void  CLOG_Disable(pCLOG p);
static int CLOG_IsEnabled(pCLOG p);

void  CLOG_Construct(pCLOG p, char* szFilename);
void  CLOG_BindFunc(pCLOG p);
static pCLOG  GetCLOG();
void   CLOG_Destory();


void LogWrite(LOG_LEVEL le, char* szString, ...);

void LogIdWrite(unsigned int logid, char* szString, ...);

void HmsPrint(const char * strFormat, ...);

LOG_LEVEL LogGetLogLevel();

void LogSetLogLevel(LOG_LEVEL le);

void LogSetLogId(unsigned int le);

extern pCLOG g_log;


#endif 
