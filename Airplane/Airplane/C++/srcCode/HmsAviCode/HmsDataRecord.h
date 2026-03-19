#pragma once
#include "base/HmsTimer.h"
#include "HmsCS.h"

#pragma pack(push)
#pragma pack(1)
struct HmsRecordHead
{
	double  dTimeStamp;
	unsigned int nLen;
};
#pragma pack(pop)

class CHmsDataRecord
{
public:
	CHmsDataRecord();
	~CHmsDataRecord();

	void OnDataRecv(void * pData, int nLen);

	void SetRecordCmd(bool bRecord);

protected:
	bool CreateRecordFile();
	bool ReleaseRecordFile();

private:
	void *				m_hFile;		//the file HANDEL
	bool				m_bIsOpen;
	CHmsTime			m_time;
	CHmsCS				m_cs;
};



