#include "SimDataInterface.h"
//#include "SimAsyncSocket.h"
#include "SimUdpSocket.h"
#include "SimTcpSocket.h"

CSimDataInterface::CSimDataInterface(void)
{
	m_asyncSocketListenBind = 0;
	m_udpSocketListenBind = 0;
}


CSimDataInterface::~CSimDataInterface(void)
{
}

void CSimDataInterface::SetListenSocketBind( void * nBind )
{
	m_asyncSocketListenBind = nBind;
}

bool CSimDataInterface::SendData( void * bindData, const char * pData, int nLen )
{
	bool bRet = false;
	if (m_asyncSocketListenBind)
	{
#if 0
		CSimAsyncSocket * pSimAsyncSocket = (CSimAsyncSocket*)m_asyncSocketListenBind;
		bRet = pSimAsyncSocket->SendData((CSimSocketConnect*)bindData, pData, nLen);
#else
		CSimTcpSocket * pSimTcpSocket = (CSimTcpSocket*)m_asyncSocketListenBind;
		pSimTcpSocket->SendData(pData, nLen);
#endif
	}
	return bRet;
}

bool CSimDataInterface::SetRemianBuffer( void * bindData, const char * pData, int nLen )
{
	bool bRet = false;
	if (m_asyncSocketListenBind)
	{
#if 0
		CSimAsyncSocket * pSimAsyncSocket = (CSimAsyncSocket*)m_asyncSocketListenBind;
		CSimSocketConnect * pConnect = (CSimSocketConnect*)bindData;
		bRet = pConnect->SetRemainBuffer(pData, nLen);
#else
		CSimTcpSocket * pSimTcpSocket = (CSimTcpSocket*)m_asyncSocketListenBind;
		pSimTcpSocket->SendData(pData, nLen);
#endif
	}
	return bRet;
}

void CSimDataInterface::SetListenUdpSocketBind( void * nBind )
{
	m_udpSocketListenBind = nBind;
}

bool CSimDataInterface::SendUdpData(unsigned int nIP, unsigned short nPort, const char * pData, int nLen )
{
	bool bRet = false;
	if (m_udpSocketListenBind)
	{
		CSimUdpSocket * pSimUdpSocket = (CSimUdpSocket*)m_udpSocketListenBind;
		bRet = pSimUdpSocket->SendData(nIP, nPort, pData, nLen);
	}
	return bRet;
}

bool CSimDataInterface::SendUdpData( const char * strIp, unsigned short nPort, const char * pData, int nLen )
{
	bool bRet = false;
	if (m_udpSocketListenBind)
	{
		CSimUdpSocket * pSimUdpSocket = (CSimUdpSocket*)m_udpSocketListenBind;
		bRet = pSimUdpSocket->SendData(strIp, nPort, pData, nLen);
	}
	return bRet;
}
