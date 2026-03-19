#include "SimTcpSocket.h"
#include <stdio.h>

#if defined(_WIN32)
#include <Mstcpip.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/tcp.h>
#endif

#include "HmsPrint.h"

#define MAX_PKT_SIZE 8192
#define RECONECT_INTERVAL       1000

int SimTcpSocketWorker( void * lpParam );

CSimTcpSocket::CSimTcpSocket(void)
{
	m_socketListen = INVALID_SOCKET;
	memset(&m_addrLocal, 0, sizeof(m_addrLocal));
	m_bRunThread = true;
	InitSocket();
}


CSimTcpSocket::~CSimTcpSocket(void)
{
	Close();
}

bool CSimTcpSocket::Create(const char * pServerIp, unsigned int nServerPort, unsigned int nSocketPort /*= 0*/, const char * pSocketAddress /*= NULL*/)
{
	if (m_socketListen == INVALID_SOCKET)
	{
		m_addrServer.sin_family = PF_INET;
		m_addrServer.sin_port = htons(nServerPort);
		m_addrServer.sin_addr.s_addr = inet_addr(pServerIp);

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
				m_addrLocal.sin_addr.s_addr = inet_addr(pSocketAddress);
			}
			else
			{
				m_addrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
			}
		}	

		return true;
	}
	return false;
}

void CSimTcpSocket::CloseSocket()
{
    if (m_socketListen != INVALID_SOCKET)
    {
#if defined(_WIN32)
        closesocket(m_socketListen);
#else
        close(m_socketListen);
#endif
        m_socketListen = INVALID_SOCKET;
    }
}

void CSimTcpSocket::CreateSocket()
{
    CloseSocket();
		
    m_socketListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#if defined(_WIN32)
    bind(m_socketListen, (SOCKADDR*)&m_addrLocal, sizeof(m_addrLocal));

		int nTimeOut = 1000;
		setsockopt(m_socketListen, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeOut, sizeof(int));
		int bNoDelay = 1;
		setsockopt(m_socketListen, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(int));
#else
    bind(m_socketListen, (struct sockaddr*)&m_addrLocal, sizeof(m_addrLocal));

    struct timeval timeout={3,0};//3s
    setsockopt(m_socketListen,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
    setsockopt(m_socketListen,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	int flag = 1;
	int nRet = setsockopt(m_socketListen, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
	if (nRet == -1)
	{
		printf("Couldn't setsockopt(TCP_NODELAY)\n");
	}
#endif

    int nBufferSize =8 * 1024;
    //nBufferSize = 8 * 1024;
	setsockopt(m_socketListen,SOL_SOCKET,SO_SNDBUF,(char*)&nBufferSize,sizeof(int));
	setsockopt(m_socketListen,SOL_SOCKET,SO_RCVBUF,(char*)&nBufferSize,sizeof(int));
        //int bKeepAlive = 1;
        //int nKeepAlive = setsockopt(m_socketListen, SOL_SOCKET, TCP_QUICKACK, &bKeepAlive, sizeof(int));

}

bool CSimTcpSocket::Connect()
{
    bool bRet = false;

    CreateSocket();

    if(connect(m_socketListen, (sockaddr*)&m_addrServer, sizeof(m_addrServer)) != -1)
    {
#if defined(_WIN32)
        tcp_keepalive inKeepAlive = {0};
        unsigned long ulInLen = sizeof(tcp_keepalive);
        tcp_keepalive outKeepAlive = {0};
        unsigned long ulOutLen = sizeof(tcp_keepalive);
        unsigned long ulBytesReturn = 0;

        //璁剧疆socket鐨刱eep alive涓?0绉掞紝骞朵笖鍙戦€佹鏁颁负3娆?
        inKeepAlive.onoff = 1;
        inKeepAlive.keepaliveinterval = 1000;
        inKeepAlive.keepalivetime = 3;

        //涓洪€夊畾鐨凷OCKET璁剧疆Keep Alive锛屾垚鍔熷悗SOCKET鍙€氳繃Keep Alive鑷姩妫€娴嬭繛鎺ユ槸鍚︽柇寮€
		if (WSAIoctl(m_socketListen, SIO_KEEPALIVE_VALS, (LPVOID)&inKeepAlive, ulInLen, (LPVOID)&outKeepAlive, ulOutLen, &ulBytesReturn, NULL, NULL) == SOCKET_ERROR)
        {
           
        }
#else
//         int keepalive = 1;
//         setsockopt(m_socketListen,SOL_SOCKET,SO_KEEPALIVE,(void*)(&keepalive),(socklen_t)sizeof(keepalive));
// 
//         int keepalive_time = 30;
//         setsockopt(m_socketListen, IPPROTO_TCP, TCP_KEEPIDLE,(void*)(&keepalive_time),(socklen_t)sizeof(keepalive_time));
//         int keepalive_intvl = 3;
//         setsockopt(m_socketListen, IPPROTO_TCP, TCP_KEEPINTVL,(void*)(&keepalive_intvl),(socklen_t)sizeof(keepalive_intvl));
//         int keepalive_probes= 3;
//         setsockopt(m_socketListen, IPPROTO_TCP, TCP_KEEPCNT,(void*)(&keepalive_probes),(socklen_t)sizeof(keepalive_probes));
		int flag = 1;
		int nRet = setsockopt(m_socketListen, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
		if (nRet == -1)
		{
			printf("Couldn't setsockopt(TCP_NODELAY)\n");
		}
#endif
        bRet = true;
    }
    return  bRet;
}

bool CSimTcpSocket::OnAccept()
{
	if (m_pDataInterface)
	{
		return m_pDataInterface->OnAccept(this);
	}
	return false;
}

bool CSimTcpSocket::OnClose()
{
	if (m_pDataInterface)
	{
		return m_pDataInterface->OnClose(this);
	}
	return false;
}

bool CSimTcpSocket::Run()
{
	bool bRet = false;
	if (!m_thread.joinable())
	{
		m_bRunThread = true;

		m_thread =  std::thread(SimTcpSocketWorker, this);
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

bool CSimTcpSocket::Close()
{
	bool bRet = false;
	if (m_thread.joinable())
	{
		m_bRunThread = false;
		m_thread.join();

        CloseSocket();

		bRet = true;
	}
	return bRet;
}


bool CSimTcpSocket::InitSocket()
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

bool CSimTcpSocket::CheckIP( const char * strIP )
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


void CSimTcpSocket::SetDataInterface( CSimDataInterface * pDataInterface )
{
	m_pDataInterface = pDataInterface;
	if (m_pDataInterface)
	{
		m_pDataInterface->SetListenSocketBind(this);
	}
}

bool CSimTcpSocket::OnRecv( const char * pData, int nLen )
{
	bool bRet = false;
	if (m_pDataInterface)
	{
		bRet = m_pDataInterface->OnRecv(this, pData, nLen);
	}
	return bRet;
}

bool CSimTcpSocket::SendData( const char * pData, int nLen )
{
	bool bRet = false;
    if (INVALID_SOCKET != m_socketListen)
    {
#if defined(_WIN32)
        int nSend = send(m_socketListen, pData, nLen, 0);
#else
		ssize_t nSend = send(m_socketListen, pData, nLen, 0);
#endif
        if (nSend > 0)
        {
            bRet = true;
        }
    }
	return bRet;
}

bool CSimTcpSocket::TestThread()
{
	return m_bRunThread;
}

int SimTcpSocketWorker( void * lpParam )
{
	CSimTcpSocket * pSimTcpSocket = (CSimTcpSocket*)lpParam;
    char recvBuff[MAX_PKT_SIZE] = {0};
	int nRecvLen = 0;

	char readBuffer[1024] = {0};

    HmsPrint("Run The Tcp Server!\n");
	while (pSimTcpSocket->TestThread())
	{
        if (!pSimTcpSocket->Connect())
        {
            std::this_thread::sleep_for (std::chrono::milliseconds(5));
            HmsPrint("TCP Connect Retry!\n");
            continue;
        }

        SOCKET listenSocket =  pSimTcpSocket->GetSocket();

        HmsPrint("TCP Connected\n");
		pSimTcpSocket->OnAccept();

        while (pSimTcpSocket->TestThread())
        {
            nRecvLen = recv(listenSocket, (char*)recvBuff, MAX_PKT_SIZE, 0);

            if (nRecvLen <= 0)
            {
#if defined(_WIN32)
				if (nRecvLen != 0)
                {
                    DWORD nError = GetLastError();
                    if (WSAETIMEDOUT == nError)
                    {
                        continue;
                }
                }
#else
                if (nRecvLen == -1)
                {
                    int nErr = errno;
                    if (nErr == EAGAIN || nErr == EWOULDBLOCK)
                        continue;
                }
#endif
                break;
            }

            pSimTcpSocket->OnRecv(recvBuff, nRecvLen);
            nRecvLen = 0;
        }

		pSimTcpSocket->OnClose();
        HmsPrint("TCP Disconnect!\n");
        std::this_thread::sleep_for (std::chrono::milliseconds(RECONECT_INTERVAL));
        HmsPrint("TCP sleep over!\n");
	}
	return 0;
}
