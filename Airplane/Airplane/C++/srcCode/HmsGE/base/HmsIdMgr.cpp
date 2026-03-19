#include "HmsIdMgr.h"

NS_HMS_BEGIN


CHmsIdMgr::CHmsIdMgr(unsigned int nIdStart)
{
	m_nCurID = nIdStart;
}

CHmsIdMgr::~CHmsIdMgr()
{

}

unsigned int CHmsIdMgr::GetID()
{
	unsigned int nRet = 0;
	if (m_queueUsedID.size())
	{
		nRet = m_queueUsedID.front();
		m_queueUsedID.pop();
	}
	else
	{
		nRet = m_nCurID++;
	}
	return nRet;
}

void CHmsIdMgr::RemoveID(unsigned int id)
{
	m_queueUsedID.push(id);
}

NS_HMS_END