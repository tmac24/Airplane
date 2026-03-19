#include "HmsDataRecord.h"
#include <fstream>
#include <string>
#include "time.h"
#include <sstream>
#include <stdio.h>

std::string g_stringRecordPath;

std::string CreateCurrentRecordName()
{
	time_t t;
	struct tm *p;
	time(&t);
	p = localtime(&t);
	
	std::stringstream ss;
	ss << g_stringRecordPath.c_str();
	ss.fill('0');
	ss << "RecordData";
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
	ss << ".rda";
	std::string strOut = ss.str();
	return strOut;
}

CHmsDataRecord::CHmsDataRecord()
	: m_hFile(nullptr)
	, m_bIsOpen(false)
{

}

CHmsDataRecord::~CHmsDataRecord()
{
	ReleaseRecordFile();
}

void CHmsDataRecord::OnDataRecv(void * pData, int nLen)
{
	if (!m_bIsOpen)
	{
		return;
	}

	CHmsCSLocker locker(m_cs);

	if (m_hFile)
	{
		HmsRecordHead header;
		header.dTimeStamp = m_time.GetElapsed();
		header.nLen = nLen;	

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
		std::ofstream * file = (std::ofstream*)m_hFile;
		file->write((char*)&header, sizeof(header));
		file->write((char*)pData, nLen);
#else
		int file = (int)m_hFile;
		write(file, (char*)&header, sizeof(header));
		write(file, pData, nLen);
#endif
	}
}

void CHmsDataRecord::SetRecordCmd(bool bRecord)
{
	if (bRecord)
	{
		CreateRecordFile();
	}
	else
	{
		ReleaseRecordFile();
	}
}

bool CHmsDataRecord::CreateRecordFile()
{
	CHmsCSLocker locker(m_cs);

	if (m_bIsOpen)
	{
		return true;
	}
	
	std::string path = CreateCurrentRecordName();
	const char * strPath = path.c_str();
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
	auto file = new std::ofstream(strPath, std::ios::binary | std::ios::trunc | std::ios::out);
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

bool CHmsDataRecord::ReleaseRecordFile()
{
	CHmsCSLocker locker(m_cs);
	if (m_hFile)
	{
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
		std::ofstream * file = (std::ofstream*)m_hFile;
		file->close();
		delete file;
#else
		int file = (int)m_hFile;
		close(file);
#endif
		m_hFile = nullptr;
	}
	m_bIsOpen = false;

	return true;
}

