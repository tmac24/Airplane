#ifdef WIN32

#include <windows.h>
#include <time.h>
#else
#endif
#include "Global.h"

#include "Log.h"

#ifdef __linux
#include <stdarg.h>
#endif

#ifdef __vxworks
#include <stdarg.h>
#endif

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)

#endif

#include "HmsPrint.h"

pCLOG g_log =0; 

char* CLOG_GetColor(LOG_LEVEL color)
{
	switch (color)
	{
	case LOG_LEVEL_ALWAYS:
		return "#00FF00";
		break;
	case	LOG_LEVEL_FATAL:
		return "#FF0000";
		break;
	case	LOG_LEVEL_WARN:
		return "#FFFFFF";
		break;
	case	LOG_LEVEL_NOTICE:
		return "#4995B1";
		break;
	case	LOG_LEVEL_INFO:
		return "#4995B1";
		break;
	case	LOG_LEVEL_DEBUG_INFO:
		return "#4995B1";
		break;
	case	LOG_LEVEL_DEBUG_FP:
		return "#4995B1";
	case	LOG_LEVEL_DEBUG_TMP:
		return "#4995B1";
		break;
	default:
		return "#000000";
		break;
	}  
}

pCLOG  GetCLOG(void)
{ 
	if (!g_log)
	{
		g_log = (pCLOG)malloc(sizeof(CLOG));
		if(g_log==NULL)
		{
			printf("GetCLOG Failed, g_log = (pCLOG)malloc(sizeof(CLOG))  error\n");
			//system("pause");//xyh 直接返回0,避免假死.
			return 0;
		}
		g_log->m_bEnabled = 1;
		memset(g_log,0,sizeof(CLOG));
		g_log->MaxFileNumber = 10;
		g_log->MaxFileSize = 100;
		g_log->m_fontSize = 4;  
		g_log->loglevel = LOG_LEVEL_FATAL;

		CLOG_GetFile(g_log);  
	} 
	return g_log;
}
void LogSetLogLevel(LOG_LEVEL le)
{
	pCLOG plog = GetCLOG();
	if (plog)
	{
		plog->loglevel = le;
	}
}

LOG_LEVEL LogGetLogLevel()
{ 
	return  GetCLOG()->loglevel  ;
}
int CLOG_GetFile( pCLOG plog  )
{
	if (0 == plog){ return 0; }

	FILE*  pFile = 0; 
	time_t ttime;
	struct tm*    pCurTime;
	char   cLogStart[128] = { 0 };
	//printf("CLOG_GetFile\n");
	time(&ttime);
	pCurTime = localtime(&ttime);
	strftime(cLogStart, 128, "Log Started on %m/%d/%Y at %H:%M:%S", pCurTime);
	printf("log file name :%d\n",/*plog->m_szFilename,*/sizeof(plog->m_szFilename));
	memset(plog->m_szFilename,0,sizeof(plog->m_szFilename));
	sprintf(plog->m_szFilename,"%s_%d.html","runLog",plog->FileNumber);
	printf("log file name :%s\n",plog->m_szFilename);
	pFile = fopen(plog->m_szFilename, "wt"); 

	if (!pFile)
	{
		printf("CLOG_GetFile---> %s failed \n",plog->m_szFilename);
		return 0;
	}
	{
		unsigned int size = 0;
		char writeString[2048]={0};
		sprintf(writeString, "<HTML>\n<TITLE>日志%s :%s</TITLE>\n"
			"<BODY BGCOLOR= ffffff>\n"
			"<FONT COLOR= FFFFFF size=7>%s</FONT><BR><BR>\n"
			"</BODY>\n</HTML>", plog->m_szFilename,cLogStart, cLogStart);
		size = strlen(writeString);
		plog->FileSize += size;
		fprintf(pFile,"%s",writeString);
	} 

	plog->m_bEnabled = 1;
	fclose(pFile);
	return 1;
}


void  CLOG_Destory()
{
	if (g_log)
	{
		free(g_log);
		g_log = 0;
	}
}
void  NeedAddFile(pCLOG plog)
{
	if (plog && plog->FileSize >= plog->MaxFileSize * 1024 * 1024)
	{
		plog->FileNumber ++;
		plog->FileSize =0;
		if (	plog->FileNumber ==  plog->MaxFileNumber)
		{
			plog->FileNumber=0;
		}
		CLOG_GetFile(g_log);  
	} 
}

void LogWrite(LOG_LEVEL le, char* szString, ...)
{  
	pCLOG plog = GetCLOG();
	if (plog && plog->loglevel == le)
	{
		va_list va;
		time_t ttime;
		struct tm* pCurTime;

		char writeString[2048] = { 0 };
		char szParsedString[1024] = { 0 };;
		char szLogTime[32] = { 0 };;
		time(&ttime);
		pCurTime = localtime(&ttime);
		strftime(szLogTime, 32, "%H:%M:%S", pCurTime);
		va_start(va, szString);
		vsprintf(szParsedString, szString, va);
		va_end(va);
		{
			sprintf(writeString, "%s %s\n", szLogTime, szParsedString);
		}
		HmsPrint("%s\n", writeString);
		//	printf("%s\n", writeString);
	}
}

void LogSetLogId(unsigned int le)
{
	pCLOG plog = GetCLOG();
	if (plog)
	{
		plog->logid = le;
	}
}
void LogIdWrite(unsigned int le, char* szString, ...)
{ 


}
