#include "HmsLogFile.h"
#include <sstream>
#include <memory>  
#include <ctime>  
#include <iostream>  
#include <fstream> 
#include <stdio.h>
#include <stdarg.h>

std::string CHmsLogFile::s_strLogFolder;

CHmsLogFile::CHmsLogFile(void)
: m_hFile(nullptr)
, m_bIsOpen(false)
{

}

CHmsLogFile::~CHmsLogFile(void)
{
    ReleaseLogFile();
}

std::string CHmsLogFile::GetLogFileName()
{
    time_t t;
    struct tm *p;
    time(&t);
    p = localtime(&t);

    std::stringstream ss;
    ss << s_strLogFolder.c_str();
    ss.fill('0');
    ss << "LOG";
    ss.width(4);
    ss << (p->tm_year + 1900);
    ss.width(2);
    ss << (p->tm_mon + 1);
    ss.width(2);
    ss << p->tm_mday;
    ss.width(2);
    ss << p->tm_hour;
    ss.width(2);
    ss << p->tm_min;
    ss.width(2);
    ss << p->tm_sec;
    ss << ".log";
    std::string strOut = ss.str();
    return strOut;
}

bool CHmsLogFile::CreateLogFile()
{
    CHmsCSLocker locker(m_cs);

    if (m_bIsOpen)
    {
        return true;
    }

    auto strPath = GetLogFileName();
    auto file = new std::ofstream(strPath, std::ios::binary | std::ios::trunc | std::ios::out);
    //if need add the data begin with the file end, then use the ios::app flag
    //if need record the data at the file begin, then use the ios::trunc
    if (file)
    {
        if (file->is_open())
        {
            m_bIsOpen = true;
            m_hFile = file;
            return true;
        }
        else
        {
            m_bIsOpen = false;
            m_hFile = nullptr;
            delete file;
        }
    }

    return false;
}

bool CHmsLogFile::ReleaseLogFile()
{
    CHmsCSLocker locker(m_cs);
    if (m_hFile)
    {
        std::ofstream * file = (std::ofstream*)m_hFile;
        file->close();
        delete file;

        m_hFile = nullptr;
    }
    m_bIsOpen = false;

    return true;
}

CHmsLogFile * CHmsLogFile::GetInstance()
{
    static CHmsLogFile * s_pInstance = nullptr;
    if (nullptr == s_pInstance)
    {
        s_pInstance = new CHmsLogFile();
        s_pInstance->CreateLogFile();
    }
    return s_pInstance;
}

void CHmsLogFile::SetLogFloder(const std::string & strFloder)
{
    bool bReOpen = false;
    if (s_strLogFolder.empty())
    {
        bReOpen = true;
    }
    if (strFloder != s_strLogFolder)
    {
        s_strLogFolder = strFloder;
        auto logFile = GetInstance();
        if (bReOpen)
        {
            logFile->ReleaseLogFile();
            logFile->CreateLogFile();
        }
    }
}

void CHmsLogFile::Log(const std::string &strMsg)
{
    static int s_nCnt = 0;
    s_nCnt++;
    if (s_nCnt < 1000)
    {
        PrintLog(strMsg);
    }
    else
    {
        PrintLogByNewLineAndTime(strMsg);
        s_nCnt = 0;
    }
}

void CHmsLogFile::LogFormat(const char * strFormat, ...)
{
    va_list _ArgList;
#ifdef _WIN32
    _crt_va_start(_ArgList, strFormat);
    char strTemp[1024] = { 0 };
    vsprintf_s(strTemp, strFormat, _ArgList);
#else
    va_start(_ArgList, strFormat);
    char strTemp[1024] = { 0 };
    vsprintf(strTemp, strFormat, _ArgList);
#endif
    Log(strTemp);
}

void CHmsLogFile::PrintLog(const std::string & strMsg)
{
    if (m_bIsOpen)
    {
        CHmsCSLocker locker(m_cs);

        if (m_hFile)
        {
            std::ofstream * file = (std::ofstream*)m_hFile;
            *file << strMsg;
        }
    }
}

void CHmsLogFile::PrintLogByNewLine(const std::string & strMsg)
{
    if (m_bIsOpen)
    {
        CHmsCSLocker locker(m_cs);

        if (m_hFile)
        {
            std::ofstream * file = (std::ofstream*)m_hFile;
            *file << "\n" << strMsg;
        }
    }
}

void CHmsLogFile::PrintLogByNewLineAndTime(const std::string & strMsg)
{
    if (m_bIsOpen)
    {
        CHmsCSLocker locker(m_cs);

        if (m_hFile)
        {
            std::ofstream * file = (std::ofstream*)m_hFile;
            time_t t = time(0);
            char tmp[64];
            strftime(tmp, sizeof(tmp), "\n[%Y.%m.%d %X %A] ", localtime(&t));
            *file << tmp << strMsg;
        }
    }
}

