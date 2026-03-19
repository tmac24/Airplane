#include <stdio.h>
#include "HmsComReader.h"
#include "HmsPlatformConfig.h"
#include <stdarg.h>
#ifdef __vxworks
#include <tasklib.h>
#include <iolib.h>
#include <sioLib.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>

#endif

int CHmsComReader::TaskRunReader(void * pParam)
{
	int nRet = 0;

	CHmsComReader * pComReader = (CHmsComReader *)pParam;
	if (pComReader)
	{
		nRet = pComReader->RunReader();
	}
	return nRet;
}


CHmsComReader::CHmsComReader()
{
	m_nDataLen = 128;
	m_nBaudRate = 9600;
	m_bRun = true;
#ifdef __vxworks
	m_fdCom = -1;
#endif

#ifdef __WIN32
	m_fdCom = INVALID_HANDLE_VALUE;
#endif
	
#ifdef __linux

	m_fdCom = -1;

#endif

	m_funcCheck = CHmsComReader::CRC_check;
}

CHmsComReader::~CHmsComReader()
{
	m_bRun = false;
#ifdef __vxworks

#else
	if (m_thread.joinable())
	{
		m_thread.join();
	}
#endif
}

void CHmsComReader::InitCom(const char * strComName, const char * strTaskName /*= nullptr*/)
{
	m_strComName = strComName;
	m_strTaskName = strTaskName;
}

unsigned char CHmsComReader::CRC_check(unsigned char *buf, int length)
{
	unsigned char result = 0;
	unsigned char sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum += buf[i];
	}
	result = 0x100 - sum;
	return result;
	//return sum;
}
unsigned char CHmsComReader::CRC_check_touch(unsigned char *buf, int length)
{
	//unsigned char result = 0;
	unsigned char sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum += buf[i];
	}
	//result = 0x100 - sum;
	//return result;
	return sum;
}

void CHmsComReader::SetMsgHead(int prefixion1, ...)
{
	int c;
	va_list params;
	va_start(params, prefixion1);
	m_strMsgHead.push_back((unsigned char)prefixion1);
	while (true)
	{
		c = va_arg(params, int);
		if (c != 0)
		{
			m_strMsgHead.push_back((unsigned char)c);
		}
		else
		{
			break;
		}
	}
	va_end(params);
}

void CHmsComReader::SetDataCallback(int nDataLen, const ComDataCallBack & callback)
{
	m_nDataLen = nDataLen;
	m_callback = callback;
}

void CHmsComReader::Run()
{
#ifdef __vxworks
#define TASK_STACK_SIZE 0x500000
	taskSpawn(m_strTaskName.c_str(), 100, VX_FP_TASK, TASK_STACK_SIZE, (FUNCPTR)TaskRunReader, (int)this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#else
	m_thread = std::thread(TaskRunReader, this);
#endif
}


#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX)

void CHmsComReader::SendComData(const char* strData, int nLen) {
	if(m_fdCom != -1)
	{
		write(m_fdCom, strData, nLen);
	}
}

int CHmsComReader::RunReader()
{
	printf("Task:%s Com:%s Reader\n", m_strTaskName.c_str(), m_strComName.c_str());

	//?????
	int	com_Fd = open(m_strComName.c_str(), O_RDWR, 0);/*??232????*/
	if(com_Fd < 0)
	{
		return -1;
	}
	m_fdCom = com_Fd;

	struct termios oldtio, newtio;
	tcgetattr(com_Fd, &oldtio);

	long BAUD;
	switch (m_nBaudRate)
	{
	case 38400:
	default:
		BAUD = B38400;
		break;
	case 19200:
		BAUD  = B19200;
		break;
	case 9600:
		BAUD  = B9600;
		break;
	}  //end of switch baud_rate

	newtio.c_cflag = BAUD | CS8 | PARENB | PARODD | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;       //ICANON;
	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;
	tcflush(com_Fd, TCIFLUSH);
	tcsetattr(com_Fd,TCSANOW,&newtio);

	{
#define MAX_COM_BUFFER_READ_SIZE  512

		unsigned char bufferRead[MAX_COM_BUFFER_READ_SIZE] = { 0 };
		int		nReadCnt = 0;				//?????????
		int		nComReadAbleCnt = 0;		//Com?п?????????
		int		nBufferRemainCnt = 0;		//?????п?????????
		int		nBufferRemainCheakAbleCnt = 0;
		int		nCrcLen = m_nDataLen - 1;
		int		nPosMsgStart = 0;
		int		nPosMsgEnd = 0;
		int i;
		//int		n
		while (m_bRun)
		{
			//taskDelay(0);
			//ioctl(com_Fd, FIONREAD, &nComReadAbleCnt);/*????????????ж??????????????????????????readCnt1???档*/

			nComReadAbleCnt = m_nDataLen;
			if (nComReadAbleCnt)
			{
				if (nBufferRemainCnt + nComReadAbleCnt > MAX_COM_BUFFER_READ_SIZE)
				{
					nComReadAbleCnt = MAX_COM_BUFFER_READ_SIZE - nBufferRemainCnt;
				}

				nReadCnt = read(com_Fd, bufferRead + nBufferRemainCnt, nComReadAbleCnt);
				nBufferRemainCnt += nReadCnt;
				if (nBufferRemainCnt >= m_nDataLen)
				{
					//?ж???????????淶
					if (m_strMsgHead.size())
					{
						nPosMsgStart = 0;
						nBufferRemainCheakAbleCnt = nBufferRemainCnt - m_strMsgHead.size();

						//???????
						while ((nPosMsgStart < nBufferRemainCheakAbleCnt) &&
							(memcmp(m_strMsgHead.data(), bufferRead + nPosMsgStart, m_strMsgHead.size()) != 0))
						{
							nPosMsgStart++;
						}
					}

					//??????????????
					nPosMsgEnd = nPosMsgStart + m_nDataLen;

					if (nPosMsgEnd <= nBufferRemainCnt)
					{
						unsigned char crc = 0;
						if(m_funcCheck)
							crc = m_funcCheck(bufferRead + nPosMsgStart, nCrcLen);
						else
							crc = CRC_check(bufferRead + nPosMsgStart, nCrcLen);

						if (crc == bufferRead[nPosMsgStart + nCrcLen])
						{

							if (m_callback)
							{
								m_callback(bufferRead + nPosMsgStart, m_nDataLen);
							}
						}
						else
						{
							printf("Task:%s,Failed to CRC CHECK!\n", m_strTaskName.c_str());
						}
					}
					else
					{
						//?????????????????????????
						nPosMsgEnd = nPosMsgStart;
					}
					nBufferRemainCnt -= nPosMsgEnd;

					if (nPosMsgEnd && (nBufferRemainCnt > 0))
					{
						memmove(bufferRead, bufferRead + nPosMsgEnd, nBufferRemainCnt);
					}
				}
			}
		}
	};

	return 0;
}

#endif


#ifdef __vxworks

void CHmsComReader::SendComData(const char* strData, int nLen) {
	if(m_fdCom != -1)
	{
		write(m_fdCom, strData, nLen);
	}
}

int CHmsComReader::RunReader()
{
	printf("Task:%s Com:%s Reader\n", m_strTaskName.c_str(), m_strComName.c_str());

	//初始化
	int	com_Fd = open(m_strComName.c_str(), O_RDWR, 0);/*打开232串口*/
	m_fdCom = com_Fd;
	if (ERROR == ioctl(com_Fd, FIOBAUDRATE, m_nBaudRate))
	{
		printf("Init_232_VIDEO_CPU:can not set BAUDRATE!/n");
		return ERROR;
	}

	ioctl(com_Fd, FIOSETOPTIONS, OPT_RAW);
	if (ERROR == ioctl(com_Fd, SIO_HW_OPTS_SET, CLOCAL | CREAD | CS8 | PARENB | PARODD)) /*奇校验*/
	{
		printf("Init_232_VIDEO_CPU:can not set OPT!/n");
		return ERROR;
	}

	ioctl(com_Fd, FIOFLUSH, 0);/*清空输入输出缓冲区*/

	//数据处理部分
	{
#define MAX_COM_BUFFER_READ_SIZE  512

		unsigned char bufferRead[MAX_COM_BUFFER_READ_SIZE] = { 0 };
		int		nReadCnt = 0;				//读取的数量
		int		nComReadAbleCnt = 0;		//Com中可读取的数量
		int		nBufferRemainCnt = 0;		//缓存中可使用的数量
		int		nBufferRemainCheakAbleCnt = 0;
		int		nCrcLen = m_nDataLen - 1;
		int		nPosMsgStart = 0;
		int		nPosMsgEnd = 0;
		int i;
		//int		n
		while (m_bRun)
		{
			//taskDelay(0);
			//ioctl(com_Fd, FIONREAD, &nComReadAbleCnt);/*得到缓冲区里有多少字节要被读取，然后将字节数放入readCnt1里面。*/

			nComReadAbleCnt = m_nDataLen;
			if (nComReadAbleCnt)
			{
				if (nBufferRemainCnt + nComReadAbleCnt > MAX_COM_BUFFER_READ_SIZE)
				{
					nComReadAbleCnt = MAX_COM_BUFFER_READ_SIZE - nBufferRemainCnt;
				}

				nReadCnt = read(com_Fd, bufferRead + nBufferRemainCnt, nComReadAbleCnt);
				nBufferRemainCnt += nReadCnt;
				if (nBufferRemainCnt >= m_nDataLen)
				{
					//判断数据是否符合规范
					if (m_strMsgHead.size())
					{
						nPosMsgStart = 0;
						nBufferRemainCheakAbleCnt = nBufferRemainCnt - m_strMsgHead.size();

						//找到消息头
						while ((nPosMsgStart < nBufferRemainCheakAbleCnt) &&
							(memcmp(m_strMsgHead.data(), bufferRead + nPosMsgStart, m_strMsgHead.size()) != 0))
						{
							nPosMsgStart++;
						}
					}

					//找到消息结束长度
					nPosMsgEnd = nPosMsgStart + m_nDataLen;

					if (nPosMsgEnd <= nBufferRemainCnt)
					{
						unsigned char crc = 0;
						if(m_funcCheck)
							crc = m_funcCheck(bufferRead + nPosMsgStart, nCrcLen);
						else
							crc = CRC_check(bufferRead + nPosMsgStart, nCrcLen);
						
						if (crc == bufferRead[nPosMsgStart + nCrcLen])
						{

							if (m_callback)
							{
								m_callback(bufferRead + nPosMsgStart, m_nDataLen);
							}
						}
						else
						{
							printf("Task:%s,Failed to CRC CHECK!\n", m_strTaskName.c_str());
						}
					}
					else
					{
						//数据长度不够，放到下帧数据处理
						nPosMsgEnd = nPosMsgStart;
					}
					nBufferRemainCnt -= nPosMsgEnd;

					if (nPosMsgEnd && (nBufferRemainCnt > 0))
					{
						memmove(bufferRead, bufferRead + nPosMsgEnd, nBufferRemainCnt);
					}
				}
			}
		}
	};

	return 0;
}
#endif

#ifdef _WIN32
int CHmsComReader::RunReader()
{
	printf("Task:%s Com:%s Reader\n", m_strTaskName.c_str(), m_strComName.c_str());

	m_fdCom = CreateFileA(m_strComName.c_str(),  /** 设备名,COM1,COM2等 */
		GENERIC_READ | GENERIC_WRITE, /** 访问模式,可同时读写 */
		0,                            /** 共享模式,0表示不共享 */
		NULL,                         /** 安全性设置,一般使用NULL */
		OPEN_EXISTING,                /** 该参数表示设备必须存在,否则创建失败 */
		0,
		0);

		/** 如果打开失败，释放资源并返回 */
	if (m_fdCom == INVALID_HANDLE_VALUE)
		return 0;

	/** 设置串口的超时时间,均设为0,表示不使用超时限制 */
	 COMMTIMEOUTS  CommTimeouts;
	 CommTimeouts.ReadIntervalTimeout = 0;
	 CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	 CommTimeouts.ReadTotalTimeoutConstant = 0;
	 CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	 CommTimeouts.WriteTotalTimeoutConstant = 0;
	 
	 SetCommTimeouts(m_fdCom, &CommTimeouts);

	DCB dcb;
	/*  先获取dcb信息    */
	GetCommState(m_fdCom, &dcb);
	/*  设置串口信息  */
	dcb.BaudRate = m_nBaudRate;
	dcb.Parity = ODDPARITY;
	dcb.fParity = 0;

	dcb.StopBits = 0;
	dcb.ByteSize = 8;
	
	BOOL b = SetCommState(m_fdCom, &dcb);
	if (!b)
	{
		DWORD nError = GetLastError();
		int zz = 0;
	}

	/**  清空串口缓冲区 */
	PurgeComm(m_fdCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);


	//数据处理部分
	if (m_fdCom != INVALID_HANDLE_VALUE)
	{
#define MAX_COM_BUFFER_READ_SIZE  512

		unsigned char bufferRead[MAX_COM_BUFFER_READ_SIZE] = { 0 };
		DWORD 		nReadCnt = 0;				//读取的数量
		int		nComReadAbleCnt = 0;		//Com中可读取的数量
		int		nBufferRemainCnt = 0;		//缓存中可使用的数量
		int		nBufferRemainCheakAbleCnt = 0;
		int		nCrcLen = m_nDataLen - 1;
		int		nPosMsgStart = 0;
		int		nPosMsgEnd = 0;
		//int		n
		while (m_bRun)
		{
			std::this_thread::sleep_for(std::chrono::seconds(0));
			//taskDelay(0);
			//ioctl(com_Fd, FIONREAD, &nComReadAbleCnt);/*得到缓冲区里有多少字节要被读取，然后将字节数放入readCnt1里面。*/
			nComReadAbleCnt = m_nDataLen;

			if (nComReadAbleCnt)
			{
				if (nBufferRemainCnt && nBufferRemainCnt < m_nDataLen)
				{
					nComReadAbleCnt = m_nDataLen - nBufferRemainCnt;
				}
				else if (nBufferRemainCnt + nComReadAbleCnt > MAX_COM_BUFFER_READ_SIZE)
				{
					nComReadAbleCnt = MAX_COM_BUFFER_READ_SIZE - nBufferRemainCnt;
				}

				//nReadCnt = read(com_Fd, bufferRead + nBufferRemainCnt, nComReadAbleCnt);
				if (!ReadFile(m_fdCom, bufferRead + nBufferRemainCnt, nComReadAbleCnt, &nReadCnt, NULL))
				{
					/** 获取错误码,可以根据该错误码查出错误原因 */
					DWORD dwError = GetLastError();
					
					/** 清空串口缓冲区 */
					PurgeComm(m_fdCom, PURGE_RXCLEAR | PURGE_RXABORT);
				}
	
				nBufferRemainCnt += nReadCnt;
				if (nBufferRemainCnt >= m_nDataLen)
				{
					//判断数据是否符合规范
					if (m_strMsgHead.size())
					{
						nPosMsgStart = 0;
						nBufferRemainCheakAbleCnt = nBufferRemainCnt - m_strMsgHead.size();

						//找到消息头
						while ((nPosMsgStart < nBufferRemainCheakAbleCnt) &&
							(memcmp(m_strMsgHead.data(), bufferRead + nPosMsgStart, m_strMsgHead.size()) != 0))
						{
							nPosMsgStart++;
						}
					}

					//找到消息结束长度
					nPosMsgEnd = nPosMsgStart + m_nDataLen;

					if (nPosMsgEnd <= nBufferRemainCnt)
					{
						unsigned char crc = 0;
						if (m_funcCheck)
							crc = m_funcCheck(bufferRead + nPosMsgStart, nCrcLen);
						else
							crc = CRC_check(bufferRead + nPosMsgStart, nCrcLen);

						if (crc == bufferRead[nPosMsgStart + nCrcLen])
						{

							if (m_callback)
							{
								m_callback(bufferRead + nPosMsgStart, m_nDataLen);
							}
						}
						else
						{
							unsigned char crc_rev = bufferRead[nPosMsgStart + nCrcLen];
							unsigned char crc_100 = CRC_check(bufferRead + nPosMsgStart, nCrcLen);
							unsigned char crc_0 = CRC_check_touch(bufferRead + nPosMsgStart, nCrcLen);
							printf("Task:%s Failed to CRC CHECK crc_rev=%d crc_100=%d crc=%d\n", m_strTaskName.c_str(),crc_rev, crc_100, crc_0);
							//printf("Task:%s,Failed to CRC CHECK!\n", m_strTaskName.c_str());
						}
					}
					else
					{
						//数据长度不够，放到下帧数据处理
						nPosMsgEnd = nPosMsgStart;
					}
					nBufferRemainCnt -= nPosMsgEnd;

					if (nPosMsgEnd && (nBufferRemainCnt > 0))
					{
						memmove(bufferRead, bufferRead + nPosMsgEnd, nBufferRemainCnt);
					}
				}
			}
		}
	};

	if (m_fdCom != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_fdCom);
		m_fdCom = INVALID_HANDLE_VALUE;
	}


	printf("Task:%s Ended\n", m_strTaskName.c_str());
	return 0;
}

void CHmsComReader::SendComData(const char* strData, int nLen) {
	if (m_fdCom != INVALID_HANDLE_VALUE)
	{
		WriteFile(m_fdCom, strData, nLen, nullptr, nullptr);
	}
}
#endif

#if defined(__APPLE__) && !defined(__ANDROID__)

int CHmsComReader::RunReader()
{
    // iOS / macOS 不支持串口 COM
    // Stub implementation
    return 0;
}

void CHmsComReader::SendComData(const char* strData, int nLen)
{
    // do nothing
}

#endif

