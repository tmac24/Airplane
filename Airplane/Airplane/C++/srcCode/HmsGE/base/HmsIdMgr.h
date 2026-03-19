#pragma once

#include "base/Ref.h"
#include <queue>
NS_HMS_BEGIN

/*
*用于id的管理
*/
class CHmsIdMgr
{
public:
	/**
	*
	*param   
	*returns 
	*by [12/14/2017 song.zhang] 
	*/
	CHmsIdMgr(unsigned int nIdStart = 1);
	~CHmsIdMgr();

	unsigned int GetID();

	void RemoveID(unsigned int id);

private:
	unsigned int					m_nCurID;
	std::queue<unsigned int>		m_queueUsedID;
};

NS_HMS_END