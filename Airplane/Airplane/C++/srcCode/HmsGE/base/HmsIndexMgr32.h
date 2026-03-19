#pragma once

#include "base/Ref.h"
NS_HMS_BEGIN

/*
*用于最多32个id的管理
*/
class CHmsIndexMgr32
{
public:
	/**
	* the id is in the range of 0 - nMaxCnt, and the nMaxCnt is less than 32
	*param   
	*returns 
	*by [10/17/2017 song.zhang] 
	*/
	CHmsIndexMgr32(unsigned int nMaxCnt = 10);
	~CHmsIndexMgr32();


	//this begin is used for used mode
public:
	/**
	* Get the unused index
	*param   
	*returns the usable index
	*by [10/17/2017 song.zhang] 
	*/
	int GetUnusedIndex();

	/**
	* remove the used index 
	*param   
	*returns 
	*by [10/17/2017 song.zhang] 
	*/
	void RemoveUsedIndex(int index);

	//this begin is used for mark
public:
	void MarkIndex(int index);

	unsigned int GetMarkBit(){ return m_nUsedIndexBit; }


private:
	unsigned int m_nUsedIndexBit;
	unsigned int m_nMaxIndexCnt;
};

NS_HMS_END