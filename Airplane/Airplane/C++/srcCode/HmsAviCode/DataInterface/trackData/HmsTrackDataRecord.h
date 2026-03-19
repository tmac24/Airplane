#pragma once
#include "base/HmsTimer.h"
#include "HmsCS.h"

class CHmsTrackDataRecord
{
public:
	CHmsTrackDataRecord();
	~CHmsTrackDataRecord();

	void OnDataRecv(void * pData, int nLen);

	void SetRecordCmd(bool bRecord);

    void RecordNewFile();

    void FlushRecordFile();

protected:
	bool CreateRecordFile();
	bool ReleaseRecordFile();

private:
	void *				m_hFile;		//the file HANDEL
	bool				m_bIsOpen;
	CHmsTime			m_time;
	CHmsCS				m_cs;
};



