#pragma once
#include "base/HmsTouch.h"
#include <map>


class CHmsTouchMgr
{
public:
	CHmsTouchMgr();
	~CHmsTouchMgr();

	/**
	*Get Or Create the touch, and add to map
	*param   
	*returns 
	*by [10/17/2017 song.zhang] 
	*/
	HmsAviPf::Touch * GetOrCreateTouch(int nID);

	/**
	*Release By ID
	*param   
	*returns 
	*by [10/17/2017 song.zhang] 
	*/
	void ReleaseTouch(int nID);

	/**
	*Just Remove int the map, you map release it after use the pointer
	*param   
	*returns 
	*by [10/17/2017 song.zhang] 
	*/
	HmsAviPf::Touch * GetAndRemoveTouch(int nID);

private:
	std::map<int, HmsAviPf::Touch*>   m_mapTouch;
};

