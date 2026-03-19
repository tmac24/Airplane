#pragma once
#include <vector>
#include "DataInterface/HmsDataBusStuDef.h"

struct HmsAftData;
class CSimDataInterface
{
public:
	CSimDataInterface(void);
	~CSimDataInterface(void);

	void SetListenSocketBind(void * nBind);
	void SetListenUdpSocketBind(void * nBind);
	
	bool SendData(void * bindData, const char * pData, int nLen);
	bool SendUdpData(unsigned int nIP, unsigned short nPort, const char * pData, int nLen);
	bool SendUdpData(const char * strIp, unsigned short nPort, const char * pData, int Len);
	bool SetRemianBuffer(void * bindData, const char * pData, int nLen);

	virtual bool OnAccept(void * bindData){return false;}
	virtual bool OnRecv(void * bindData, const char * pData, int nLen){return false;}
	virtual bool OnRecv(unsigned int nIp, unsigned short nPort, const char * pData, int nLen){return false;}
	virtual bool OnClose(void * bindData){return false;}

	virtual bool AftDataUpdate(HmsAftData * pData) { return false; };
    virtual void GetADSBData(std::vector<HmsADSBShortTraffic> &vec){ };

private:
	void *	m_asyncSocketListenBind;
	void *	m_udpSocketListenBind;
};

