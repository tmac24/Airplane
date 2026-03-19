#include "HmsIndexMgr32.h"

NS_HMS_BEGIN

CHmsIndexMgr32::CHmsIndexMgr32(unsigned int nMaxCnt /*= 10*/)
{
	if (nMaxCnt > 32)
	{
		nMaxCnt = 32;
	}

	m_nMaxIndexCnt = nMaxCnt;
	m_nUsedIndexBit = 0;
}


CHmsIndexMgr32::~CHmsIndexMgr32()
{
	m_nMaxIndexCnt = 0;
	m_nUsedIndexBit = 0;
}

int CHmsIndexMgr32::GetUnusedIndex()
{
	int i = 0;
	int tempBit = m_nUsedIndexBit;
	for (; i < (int)m_nMaxIndexCnt; i++)
	{
		if (!(tempBit&1))
		{
			m_nUsedIndexBit |= (1 << i);
			return i;
		}

		tempBit >>= 1;
	}
	return -1;
}

void CHmsIndexMgr32::RemoveUsedIndex(int index)
{
	if (index >= 0 && index < (int)m_nMaxIndexCnt)
	{
		m_nUsedIndexBit &= (~(1 << index));
	}
}

void CHmsIndexMgr32::MarkIndex(int index)
{
	if (index >= 0 && index < (int)m_nMaxIndexCnt)
	{
		m_nUsedIndexBit |= (1 << index);
	}
}

NS_HMS_END