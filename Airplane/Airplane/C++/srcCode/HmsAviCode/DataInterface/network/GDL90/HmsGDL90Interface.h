#pragma once
#include "DataInterface/network/SimDataInterface.h"
#include "DataProc.h"
#include <string>

class CHmsGDL90Interface : public CSimDataInterface
{
public:
	CHmsGDL90Interface();
	~CHmsGDL90Interface();

	static int GetUdpPort()
	{
		return 43211;
	}

	static int GetTcpPort()
	{
		return 2000;
	}

	virtual bool OnRecv(void * bindData, const char * pData, int nLen) override;

	virtual bool OnAccept(void * bindData) override;

	virtual bool OnClose(void * bindData) override;

	virtual bool AftDataUpdate(HmsAftData * pData) override;

    virtual void GetADSBData(std::vector<HmsADSBShortTraffic> &vec) override;


    virtual bool OnRecv(unsigned int nIp, unsigned short nPort, const char * pData, int nLen) override;

	void SendSaveMagneticCorrectionMsg();
	void SendEraseMagneticCorrectionMsg();
    void SendKeepLinkStatusMsg();

	void SendGDL90Data(const char * pData, int nLen);

private:
	unsigned int		m_nRecvUdpIp;
	unsigned short		m_nRecvUdpPort;
};

