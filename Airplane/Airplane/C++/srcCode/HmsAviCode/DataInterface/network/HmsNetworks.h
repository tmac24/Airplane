#pragma once

#include "SimDataInterface.h"
#include "SimTcpSocket.h"
#include "SimUdpSocket.h"
//#include "..\HmsTimer.h"

class HmsNetworks
{
public:
	HmsNetworks();
	~HmsNetworks();

	void SetConnectInfo(const char * strIp, int nPort, int nUdpPort = 43211);

	void SetConnectTcp(const char * strIp, int nPort);

	void SetConnectUdp(int nUdpPort = 43211);

	void SetInterface(CSimDataInterface * dataInterface);

	bool Start();

	bool Close();

	CSimDataInterface * getInterface();

	void SendDataToDevice(const char * strData, int nDataLen);

private:
	CSimUdpSocket	m_udp;
	CSimTcpSocket   m_tcp;
	//CHmsTime		m_timeRecvTcp;
	bool 			m_bStartTcp;
	bool			m_bStartUdp;
	CSimDataInterface * m_dataInterface;
};
