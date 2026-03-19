// Buffer.cpp: implementation of the CBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "Buffer.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuffer::CBuffer()
{
	m_nLen = 0;
	m_pBuffer = nullptr;
}

CBuffer::~CBuffer()
{
	Release();
}

void CBuffer::Empty()
{
	Release();
}

char * CBuffer::GetNewBuffer(int buffLen)
{
	Release();
	if (0 == buffLen)
	{
		return nullptr;
	}

	m_nLen = buffLen;
	m_pBuffer = new char[buffLen];
	memset(m_pBuffer, 0, buffLen);

	return m_pBuffer;
}

CBuffer::CBuffer(const char * buff, int len)
{
	if (nullptr == buff)
	{
		m_nLen = 0;
		m_pBuffer = nullptr;
	}
	else
	{
		m_nLen = len;
		m_pBuffer = new char[len];
		memcpy(m_pBuffer, buff, len);
	}
}

CBuffer::CBuffer(const CBuffer & buffer)
{
	m_nLen = buffer.GetLength();
	if (buffer.GetBuffer())
	{
		m_pBuffer = new char[m_nLen];
		memcpy(m_pBuffer, buffer.GetBuffer(), m_nLen);
	}
	else
	{
		m_pBuffer = nullptr;
	}
}

CBuffer::CBuffer( const char * str )
{
	if (str)
	{
		m_nLen = strlen(str) + 1;
		m_pBuffer = new char[m_nLen];
		strcpy(m_pBuffer, str);
	}
	else
	{
		m_nLen = 0;
		m_pBuffer = nullptr;
	}
}

void CBuffer::Release()
{
	m_nLen = 0;
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = 0;
	}
}

int CBuffer::GetLength() const
{
	return m_nLen;
}

char * CBuffer::GetBuffer() const
{
	return m_pBuffer;
}

const CBuffer& CBuffer::operator =(CBuffer buffer)
{
	Release();
	if (buffer.GetBuffer())
	{
		m_nLen = buffer.GetLength();
		m_pBuffer = new char[m_nLen];
		memcpy(m_pBuffer, buffer.GetBuffer(), m_nLen);
	}
	
	return *this;
}

const CBuffer& CBuffer::operator +=(CBuffer buffer)
{
	if (nullptr != m_pBuffer && 0 != m_nLen)
	{
		int nLen = m_nLen;
		char * pTemp = m_pBuffer;

		m_nLen += buffer.GetLength();
		m_pBuffer = new char[m_nLen];
		memcpy(m_pBuffer, pTemp, nLen);
		memcpy(m_pBuffer+nLen, buffer.GetBuffer(), buffer.GetLength());
		delete pTemp;
	}
	else
	{
		Release();
		m_nLen = buffer.GetLength();
		m_pBuffer = new char[m_nLen];
		memcpy(m_pBuffer, buffer.GetBuffer(), m_nLen);		
	}

	return *this;
}

CBuffer operator+(const CBuffer& buffer1, const CBuffer& buffer2)
{
	CBuffer buf;
	buf = buffer1;
	buf += buffer2;
	return buf;
}

bool CBuffer::SetData(const char * buff, int len)
{
	Release();
	if (nullptr == buff)
	{
		m_nLen = 0;
		m_pBuffer = nullptr;
	}
	else
	{
		m_nLen = len;
		m_pBuffer = new char[len];
		memcpy(m_pBuffer, buff, len);
	}
	return true;
}

CBuffer CBuffer::Right(int nLen) const
{
	CBuffer buffer;
	if (nLen > m_nLen)
	{
		buffer = *this;
	}
	else if (nLen > 0)
	{
		buffer.SetData(m_pBuffer + m_nLen - nLen, nLen);
	}
	else if (0 < nLen)
	{
		buffer = *this;
	}
	
	return buffer;
}

CBuffer CBuffer::Left(int nLen) const
{
	CBuffer buffer;

	if (nLen > m_nLen || nLen < 0)
	{
		nLen = m_nLen;
	}

	buffer.SetData(m_pBuffer, nLen);

	return buffer;
}


// CString CBuffer::ToString()
// {
// 	CString strOut;
// 	TCHAR * pStr = strOut.GetBuffer(2*this->m_nLen + 2);
// 	CConvert::ConvertBitToStrHex((char*)m_pBuffer, pStr, 2*this->m_nLen);
// 	strOut.ReleaseBuffer();
// 	return strOut;
// }

#if ZS_NOT_USE
int CConvert::ConvertStrHexToBit(const TCHAR * strHex, char * bHex, int nMaxConvert)
{
	int len = (int)_tcslen(strHex) * sizeof(TCHAR);

	if (len > nMaxConvert)
	{
		len = nMaxConvert;
	}
	
	char nTemp = -1;
	for (int i=0; i<len; i++)
	{
		nTemp = CharToHex(strHex[i]);
		if (0xff == nTemp)
		{
			return i;
		}
		
		if (i%2 == 0)
		{
			bHex[i/2] = nTemp<<4;
		}
		else
		{
			bHex[i/2] |= nTemp;
		}
	}
	return len;
}

int CConvert::ConvertBitToStrHex(const char * bHex, TCHAR * strHex, int nConvertLen)
{
	if (nullptr == bHex || nullptr == strHex)
	{
		return 0;
	}
	
	char nTemp = 0;
	for (int i=0; i<nConvertLen; i++)
	{
		if (i%2 == 0)
		{
			nTemp = (bHex[i/2] >> 4) & 0xf;
		}
		else
		{
			nTemp = bHex[i/2] & 0xf;
		}
		
		if (nTemp >= 10)
		{
			strHex[i] = nTemp + 'A' - 10;
		}
		else
		{
			strHex[i] = nTemp + '0';
		}
	}
	
	strHex[nConvertLen] = 0;
	return nConvertLen;
}

int CConvert::CharToHex(TCHAR c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	
	if (c >= 'A' && c <= 'F')
	{
		return c - 'A' + 10;
	}
	
	if (c >= 'a' && c <= 'f')
	{
		return c - 'a' + 10;
	}
	
	return -1;
}
#endif
