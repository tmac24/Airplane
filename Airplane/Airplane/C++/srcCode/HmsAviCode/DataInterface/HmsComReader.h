#pragma once
#include <vector>
#include <functional>
#include <thread>

typedef  unsigned char (*LP_CHECK_FUNC)(unsigned char *buf, int length);

class CHmsComReader
{
public:
	CHmsComReader();
	~CHmsComReader();

	typedef std::function<void(unsigned char * pData, int nLen)> ComDataCallBack;

	void InitCom(const char * strComName, const char * strTaskName = nullptr);

	void SetMsgHead(int prefixion1, ...);

	void SetDataCallback(int nDataLen, const ComDataCallBack & callback);

	void SetBaudRate(int nBaudRate){ m_nBaudRate = nBaudRate; }

	void SetCheckFunc(LP_CHECK_FUNC func){m_funcCheck = func;}

	void Run();

	/**************************************************************************************
	*  int CRC_check(char *buf,int length)
	* 函数说明：校验和运算
	* 参数：无
	* 返回值：返回校验结果
	* ************************************************************************************/
	static unsigned char CRC_check(unsigned char *buf, int length);
	static unsigned char CRC_check_touch(unsigned char *buf, int length);

	static int TaskRunReader(void * pParam);
	
	void SendComData(const char * strData, int nLen);
protected:
	int	 RunReader();

private:
	std::string						m_strComName;
	std::string						m_strTaskName;
	std::vector<unsigned char>		m_strMsgHead;
	ComDataCallBack					m_callback;
	int								m_nDataLen;
	int								m_nBaudRate;
	bool							m_bRun;

#ifdef _WIN32
	std::thread						m_thread;
	HANDLE							m_fdCom;
#else
	int 							m_fdCom;
	std::thread						m_thread;
#endif
	LP_CHECK_FUNC					m_funcCheck;
};
