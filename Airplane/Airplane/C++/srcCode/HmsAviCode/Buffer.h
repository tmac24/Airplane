// Buffer.h: interface for the CBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFER_H__71077DD9_D5DB_473F_AF8F_9DDE5D8D43B5__INCLUDED_)
#define AFX_BUFFER_H__71077DD9_D5DB_473F_AF8F_9DDE5D8D43B5__INCLUDED_


class CBuffer;

CBuffer operator+(const CBuffer& buffer1, const CBuffer& buffer2);

class CBuffer  
{
public:
	CBuffer();
	CBuffer(const char * str);
	CBuffer(const char * buff, int len);
	virtual ~CBuffer();
	CBuffer(const CBuffer & buffer);

	const CBuffer& operator=(CBuffer buffer);
	const CBuffer& operator+=(CBuffer buffer);

	friend CBuffer operator+(const CBuffer& buffer1, const CBuffer& buffer2);

	int GetLength() const;
	char * GetBuffer() const;
	char * GetNewBuffer(int buffLen);

	CBuffer Left(int nLen) const;
	CBuffer Right(int nLen) const;

	bool SetData(const char * buff, int len);
	void Empty();

private:
	void Release();
private:
	int			m_nLen;
	char *		m_pBuffer;
};

#if ZS_NOT_USE
class CConvert
{
public:
	static int ConvertStrHexToBit(const TCHAR * strHex, char * bHex, int nMaxConvert);	
	static int ConvertBitToStrHex(const char * bHex, TCHAR * strHex, int nConvertLen);
	static int CharToHex(TCHAR c);
};
#endif

#endif // !defined(AFX_BUFFER_H__71077DD9_D5DB_473F_AF8F_9DDE5D8D43B5__INCLUDED_)
