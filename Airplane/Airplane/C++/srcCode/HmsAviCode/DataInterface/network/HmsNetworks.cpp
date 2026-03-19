#include "HmsNetworks.h"

HmsNetworks::HmsNetworks()
{
	m_bStartTcp = false;
	m_bStartUdp = false;
	m_dataInterface = nullptr;
}

HmsNetworks::~HmsNetworks()
{

}

void HmsNetworks::SetConnectInfo(const char * strIp, int nPort, int nUdpPort)
{
	m_udp.Create(nUdpPort);
	if(nullptr != strIp)
	{
		m_bStartTcp = true;
		m_tcp.Create(strIp, nPort);
	}
	m_bStartUdp = true;
}

void HmsNetworks::SetConnectTcp(const char * strIp, int nPort)
{
	if (nullptr != strIp)
	{
		m_bStartTcp = true;
		m_tcp.Create(strIp, nPort);
	}
}

void HmsNetworks::SetConnectUdp(int nUdpPort /*= 43211*/)
{
	m_udp.Create(nUdpPort);
	m_bStartUdp = true;
}

void HmsNetworks::SetInterface(CSimDataInterface * dataInterface)
{
	if (dataInterface)
	{
		m_tcp.SetDataInterface(dataInterface);
		m_udp.SetDataInterface(dataInterface);
		m_dataInterface = dataInterface;
	}	
}

CSimDataInterface * HmsNetworks::getInterface()
{
	return m_dataInterface;
}

void HmsNetworks::SendDataToDevice(const char * strData, int nDataLen)
{
	if (m_bStartTcp)
	{
		m_tcp.SendData(strData, nDataLen);
	}
// 	if (m_bStartUdp)
// 	{
// 		m_udp.SendData(nullptr, strData, nDataLen);
// 	}
}

bool HmsNetworks::Start()
{
	bool bRet = false;
	if (m_bStartTcp)
	{
		m_tcp.Run();
	}
	if (m_bStartUdp)
	{
		m_udp.Listen();
	}
	return bRet;
}

bool HmsNetworks::Close()
{
	bool bRet = false;
	if (m_bStartTcp)
	{
		m_tcp.Close();
	}
	if (m_bStartUdp)
	{
		m_udp.Close();
	}
	return bRet;
}

