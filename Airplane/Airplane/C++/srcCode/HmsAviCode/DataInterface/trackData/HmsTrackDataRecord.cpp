#include "HmsTrackDataRecord.h"
#include <fstream>
#include <string>
#include "time.h"
#include <sstream>
#include <stdio.h>

std::string g_stringRecordTrackPath;

std::string CreateCurrentTrackRecordName()
{
	time_t t;
	struct tm *p;
	time(&t);
	p = localtime(&t);
	
	std::stringstream ss;
    ss << g_stringRecordTrackPath.c_str();
	ss.fill('0');
	ss << "Track";
	ss.width(4);
	ss << (p->tm_year +1900);
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
	ss << ".trk";
	std::string strOut = ss.str();
	return strOut;
}

CHmsTrackDataRecord::CHmsTrackDataRecord()
	: m_hFile(nullptr)
	, m_bIsOpen(false)
{

}

CHmsTrackDataRecord::~CHmsTrackDataRecord()
{
	ReleaseRecordFile();
}

void CHmsTrackDataRecord::OnDataRecv(void * pData, int nLen)
{
	if (!m_bIsOpen)
	{
		return;
	}

	CHmsCSLocker locker(m_cs);

	if (m_hFile)
	{
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
		std::ofstream * file = (std::ofstream*)m_hFile;
		file->write((char*)pData, nLen);
#else
		int file = (int)m_hFile;
		write(file, pData, nLen);
#endif
	}
}

void CHmsTrackDataRecord::SetRecordCmd(bool bRecord)
{
	if (bRecord)
	{
		CreateRecordFile();
	}
	else
	{
		ReleaseRecordFile();
        CreateRecordFile();
	}
}

void CHmsTrackDataRecord::RecordNewFile()
{
    if (m_bIsOpen)
    {
        ReleaseRecordFile();
        CreateRecordFile();
    }
}

void CHmsTrackDataRecord::FlushRecordFile()
{
    CHmsCSLocker locker(m_cs);

    if (m_hFile)
    {
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
        std::ofstream * file = (std::ofstream*)m_hFile;
        file->flush();
#else
        int file = (int)m_hFile;
        flush(file);
#endif
    }
}

bool CHmsTrackDataRecord::CreateRecordFile()
{
	CHmsCSLocker locker(m_cs);

	if (m_bIsOpen)
	{
		return true;
	}
	
    std::string path = CreateCurrentTrackRecordName();
	const char * strPath = path.c_str();
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
	auto file = new std::ofstream(strPath, std::ios::binary | std::ios::trunc | std::ios::out);
    //if need add the data begin with the file end, then use the ios::app flag
    //if need record the data at the file begin, then use the ios::trunc
	if (file)
	{
		if (file->is_open())
		{
			m_bIsOpen = true;
			m_hFile = file;
			m_time.RecordCurrentTime();
			return true;
		}
		else
		{
			m_bIsOpen = false;
			m_hFile = nullptr;
			delete file;
		}
	}
#else
	int file = open(strPath, O_RDWR|O_CREAT);
    //O_APPEND record at the end of the file
    //O_TRUNC  record at the begin of the file
	if (file != nullptr)
	{
		m_bIsOpen = true;
		m_hFile = file;
		m_time.RecordCurrentTime();
		return true;
	}
	else
	{
		m_bIsOpen = false;
		m_hFile = nullptr;
	}
#endif
	return false;
}

bool CHmsTrackDataRecord::ReleaseRecordFile()
{
	CHmsCSLocker locker(m_cs);
	if (m_hFile)
	{
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
		std::ofstream * file = (std::ofstream*)m_hFile;
        file->flush();
		file->close();
		delete file;
#else
		int file = (int)m_hFile;
		flush(file);
        close(file);
#endif
		m_hFile = nullptr;
	}
	m_bIsOpen = false;

	return true;
}

