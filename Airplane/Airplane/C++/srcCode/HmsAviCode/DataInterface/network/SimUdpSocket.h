#pragma once
#if defined(_WIN32)
#include <WinSock2.h>
#pragma comment( lib, "ws2_32.lib" )
#elif defined(__linux) || defined(__ANDROID__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define SOCKET int
#define INVALID_SOCKET 0

#elif defined(__APPLE__)

/* ================= iOS / macOS ================= */
#include <TargetConditionals.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

typedef int SOCKET;
#define INVALID_SOCKET (-1)

#else
#error Unsupported platform

#endif
#include "HmsCS.h"
#include "Buffer.h"
#include "SimDataInterface.h"
#include <thread>


class CSimUdpSocket
{
public:
	CSimUdpSocket(void);
	~CSimUdpSocket(void);

	bool Create(unsigned int nSocketPort = 0, const char * pSocketAddress = NULL);

	bool Listen();

	bool Close();

	void SetDataInterface(CSimDataInterface * pDataInterface);

	SOCKET GetSocket(){return m_socketListen;}

public:
	bool OnRecv(unsigned int nFromIp, unsigned short nFromPort, const char * pData, int nLen);

	bool SendData(unsigned int nIp, unsigned short nPort, const char * pData, int nLen);
	bool SendData(const char * strIp, unsigned short nPort, const char * pData, int nLen);
	bool SendData(const sockaddr_in & addr, const char * pData, int nLen);

	bool TestThread();

	bool CheckIP(const char * strIP);

	bool CheckConnected();
	bool CheckDisconnect();

private:
	bool InitSocket();
	void CloseSocket();

private:
	std::thread						m_thread;
	sockaddr_in						m_addrLocal;
	SOCKET							m_socketListen;
	bool							m_bRunThread;
	bool							m_bConnected;

	CSimDataInterface	*			m_pDataInterface;

};

