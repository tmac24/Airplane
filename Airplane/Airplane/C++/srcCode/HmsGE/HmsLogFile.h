/*---------------------------------------------
* description: 日志类
*
* note:
create by yangyang 2015/11/12
使用方法：CHmsLogFile::GetInstance()->Log("Run...");
日志记录结果：Run...    [2013.12.13 16:38:42 Friday]
---------------------------------------------*/
#pragma once  
#ifndef HMS_LOG_FILE_H_8080  
#define HMS_LOG_FILE_H_8080  

#include <string> 
#include "HmsCS.h"

class CHmsLogFile
{
public:
    static CHmsLogFile * GetInstance();

    static void SetLogFloder(const std::string & strFloder);

    //new line and time record by auto
    void Log(const std::string &strMsg);
    void LogFormat(const char * strFormat, ...);

    void PrintLog(const std::string & strMsg);
    void PrintLogByNewLine(const std::string & strMsg);
    void PrintLogByNewLineAndTime(const std::string & strMsg);

private:
    CHmsLogFile(void);
    virtual ~CHmsLogFile(void);

    std::string GetLogFileName();

    bool CreateLogFile();
    bool ReleaseLogFile();

private:
    void *				m_hFile;		//the file HANDEL
    bool				m_bIsOpen;
    CHmsCS				m_cs;

public:
    static std::string s_strLogFolder;
};

#endif  
