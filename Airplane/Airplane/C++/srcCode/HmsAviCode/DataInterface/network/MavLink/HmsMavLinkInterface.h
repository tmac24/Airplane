#pragma once
#include "DataInterface/network/SimDataInterface.h"
#include "HmsMavLinkDecoder.h"
#include <string>


class CHmsMavLinkInterface : public CSimDataInterface
{
public:
	CHmsMavLinkInterface();
	~CHmsMavLinkInterface();

	static std::string getDefaultMavlinkIp()
	{
		return "192.168.4.1";
	}

	static int getMavlinkPort()
	{
		return 6789;
	}

	virtual bool OnRecv(void * bindData, const char * pData, int nLen) override;

	virtual bool OnAccept(void * bindData) override;

	virtual bool OnClose(void * bindData) override;

	virtual bool AftDataUpdate(HmsAftData * pData) override;


    virtual bool OnRecv(unsigned int nIp, unsigned short nPort, const char * pData, int nLen) override;

private:
	CHmsMavLinkDecoder m_mavLinkDecoder;
	CHmsMavlinkAVIData m_mavlinkData;

};

