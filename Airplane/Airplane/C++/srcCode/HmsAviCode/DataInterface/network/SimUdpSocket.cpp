#include "SimUdpSocket.h"
#include <stdio.h>
#include "HmsPlatform/HmsPlatformConfig.h"

#if !defined(_WIN32)
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#define MAX_PKT_SIZE 8192

int SimUdpSocketWorker( void * lpParam );

CSimUdpSocket::CSimUdpSocket(void)
{
	m_socketListen = INVALID_SOCKET;
	memset(&m_addrLocal, 0, sizeof(m_addrLocal));
	m_bRunThread = true;
	m_bConnected = false;
	InitSocket();
}


CSimUdpSocket::~CSimUdpSocket(void)
{
	Close();
}

bool CSimUdpSocket::Create(unsigned int nSocketPort /*= 0*/, const char * pSocketAddress /*= NULL*/ )
{
	if (m_socketListen == INVALID_SOCKET)
	{  
		m_addrLocal.sin_family = PF_INET;
		m_addrLocal.sin_port = htons(nSocketPort);
		if (NULL == pSocketAddress)
		{
			m_addrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		else
		{
			if (CheckIP(pSocketAddress))
			{
#if defined(_WIN32)
				m_addrLocal.sin_addr.S_un.S_addr = inet_addr(pSocketAddress);
#else
				m_addrLocal.sin_addr.s_addr = inet_addr(pSocketAddress);
#endif
			}
			else
			{
				m_addrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
			}
		}	

		m_socketListen = socket(AF_INET, SOCK_DGRAM, 0);
#if defined(_WIN32)
		bind(m_socketListen, (SOCKADDR*)&m_addrLocal, sizeof(m_addrLocal));
#else
		bind(m_socketListen, (struct sockaddr*)&m_addrLocal, sizeof(m_addrLocal));
#endif

		int nTimeOut = 1000;
		setsockopt(m_socketListen, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeOut, sizeof(int));
		int nBufferSize = 8 * 1024;
		setsockopt(m_socketListen, SOL_SOCKET, SO_SNDBUF, (char*)&nBufferSize, sizeof(int));
		nBufferSize = 8 * 1024;
		setsockopt(m_socketListen, SOL_SOCKET, SO_RCVBUF, (char*)&nBufferSize, sizeof(int));
		return true;
	}
	return false;
}

bool CSimUdpSocket::Listen()
{
	bool bRet = false;
	if (!m_thread.joinable())
	{
		m_bRunThread = true;
		m_thread =  std::thread(SimUdpSocketWorker, this);
#if 0
#if defined(_WIN32)
		SetThreadPriority(m_thread.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
#else
		setpriority(PRIO_PROCESS, m_thread.native_handle(), -20);
#endif
#endif
		bRet = true;
	}
	return bRet;
}

bool CSimUdpSocket::Close()
{
	bool bRet = false;
	if (m_thread.joinable())
	{
		m_bRunThread = false;
		m_thread.join();
#if defined(_WIN32)
		closesocket(m_socketListen);
#else
        close(m_socketListen);
#endif
		m_socketListen = INVALID_SOCKET;

		bRet = true;
	}
	return bRet;
}


bool CSimUdpSocket::InitSocket()
{
#if defined(_WIN32)
	bool bRet = false;
	WSADATA wsd;
	ZeroMemory(&wsd, sizeof(wsd));

	int nResult = WSAStartup(MAKEWORD(2, 2), &wsd);

	if (0 == nResult)
	{
		bRet = true;
	}	
#else
    bool bRet = true;
#endif
	return bRet;
}

bool CSimUdpSocket::CheckIP( const char * strIP )
{
	bool bRet = true;
	int nIP[4] = {0};
#if defined(_WIN32)
	if (4 == sscanf_s(strIP, "%d.%d.%d.%d", nIP, nIP + 1, nIP + 2, nIP + 3))
#else
	if (4 == sscanf(strIP, "%d.%d.%d.%d", nIP, nIP + 1, nIP + 2, nIP + 3))
#endif
	{
		for (int i=0; i<4; i++)
		{
			if (nIP[i] < 0 || nIP[i] > 255)
			{
				bRet = false;
				break;
			}
		}
	}
	return bRet;
}


bool CSimUdpSocket::CheckConnected()
{
	if (!m_bConnected)
	{
		if (m_pDataInterface)
		{
			m_pDataInterface->OnAccept(this);
		}
		m_bConnected = true;
	}
	return true;
}

bool CSimUdpSocket::CheckDisconnect()
{
	if (m_bConnected)
	{
		if (m_pDataInterface)
		{
			m_pDataInterface->OnClose(this);
		}
		m_bConnected = false;
	}
	return true;
}

void CSimUdpSocket::SetDataInterface( CSimDataInterface * pDataInterface )
{
	m_pDataInterface = pDataInterface;
	if (m_pDataInterface)
	{
		m_pDataInterface->SetListenUdpSocketBind(this);
	}
}

bool CSimUdpSocket::OnRecv( unsigned int nFromIp, unsigned short nFromPort, const char * pData, int nLen )
{
	bool bRet = false;
	if (m_pDataInterface)
	{
		bRet = m_pDataInterface->OnRecv(nFromIp, nFromPort, pData, nLen);
	}
	return bRet;
}

bool CSimUdpSocket::SendData( unsigned int nIp, unsigned short nPort, const char * pData, int nLen )
{
	if (!pData)
	{
		return false;
	}
	bool bRet = false;
	sockaddr_in	addrTo;
	addrTo.sin_addr.s_addr = nIp;
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = nPort;
	int nToLen = sizeof(addrTo);
	int nSend = sendto(m_socketListen, pData, nLen, 0, (sockaddr*)&addrTo, nToLen);
	if (nSend > 0)
	{
		bRet = true;
	}
	return bRet;
}

bool CSimUdpSocket::SendData( const char * strIp, unsigned short nPort, const char * pData, int nLen )
{
	if (!strIp || !pData)
	{
		return false;
	}

	bool bRet = false;	

	if (CheckIP(strIp))
	{
		sockaddr_in	addrFrom;
		addrFrom.sin_addr.s_addr = inet_addr(strIp);
		addrFrom.sin_family = AF_INET;
		addrFrom.sin_port = htons(nPort);
		int nFromLen = sizeof(addrFrom);
		int nSend = sendto(m_socketListen, pData, nLen, 0, (sockaddr*)&addrFrom, nFromLen);

		if (nSend > 0)
		{
			bRet = true;
		}
	}
	return bRet;
}

bool CSimUdpSocket::SendData( const sockaddr_in & addr, const char * pData, int nLen )
{
	if (!pData)
	{
		return false;
	}

	bool bRet = false;
	if (INVALID_SOCKET != m_socketListen)
	{
		int nFromLen = sizeof(addr);
		int nSend = sendto(m_socketListen, pData, nLen, 0, (sockaddr*)&addr, nFromLen);
		if (nSend > 0)
		{
			bRet = true;
		}
	}
	return bRet;
}

bool CSimUdpSocket::TestThread()
{
	return m_bRunThread;
}

int SimUdpSocketWorker( void * lpParam )
{
	CSimUdpSocket * pSimUdpSocket = (CSimUdpSocket*)lpParam;
	sockaddr_in	addrFrom;
#if defined(_WIN32)
	int nFromLen = sizeof(addrFrom);
#else
    socklen_t nFromLen = sizeof(addrFrom);
#endif
	int nRecvLen = 0;
	int nIdelCnt = 0;

	SOCKET listenSocket =  pSimUdpSocket->GetSocket();
	char readBuffer[1680] = {0};

#if defined(_WIN32)
#else
	int  flags = fcntl(listenSocket, F_GETFL, 0);
	fcntl(listenSocket, F_SETFL, flags&~O_NONBLOCK);
#endif

#if defined(_WIN32)
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
    setpriority(PRIO_PROCESS,  gettid(), -20);
#endif


	while (pSimUdpSocket->TestThread())
	{
		nRecvLen = recvfrom(listenSocket, readBuffer, 1024, 0, (sockaddr*)&addrFrom, &nFromLen);
		if (nRecvLen > 0)
		{
			pSimUdpSocket->CheckConnected();

            pSimUdpSocket->OnRecv(addrFrom.sin_addr.s_addr, addrFrom.sin_port, readBuffer, nRecvLen);

			//sendto(listenSocket, readBuffer, nRecvLen, 0, (sockaddr*)&addrFrom, nFromLen);

			nIdelCnt = 0;
		}
		else
		{
			nIdelCnt++;
			if (nIdelCnt > 8)
			{
				nIdelCnt = 0;
				pSimUdpSocket->CheckDisconnect();
			}
		}
		nRecvLen = 0;	
	}
	return 0;
}
