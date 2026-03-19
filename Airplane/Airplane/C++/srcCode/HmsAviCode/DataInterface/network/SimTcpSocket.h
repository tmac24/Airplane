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


class CSimTcpSocket
{
public:
	CSimTcpSocket(void);
	~CSimTcpSocket(void);

	bool Create(const char * pServerIp, unsigned int nServerPort, unsigned int nSocketPort = 0, const char * pSocketAddress = NULL);

	bool Run();

	bool Close();

	void SetDataInterface(CSimDataInterface * pDataInterface);

	SOCKET GetSocket(){return m_socketListen;}

public:
	bool OnRecv(const char * pData, int nLen);

	bool SendData(const char * pData, int nLen);

	bool TestThread();

	bool CheckIP(const char * strIP);

    bool Connect();

	bool OnAccept();

	bool OnClose();

private:
	bool InitSocket();
	void CloseSocket();
	void CreateSocket();

private:
	std::thread						m_thread;
	sockaddr_in						m_addrLocal;
    sockaddr_in                     m_addrServer;
	SOCKET							m_socketListen;
	bool							m_bRunThread;

	CSimDataInterface	*			m_pDataInterface;

};

