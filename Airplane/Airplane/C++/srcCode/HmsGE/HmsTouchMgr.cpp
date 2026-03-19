#include "HmsTouchMgr.h"


CHmsTouchMgr::CHmsTouchMgr()
{
}


CHmsTouchMgr::~CHmsTouchMgr()
{
}

HmsAviPf::Touch * CHmsTouchMgr::GetOrCreateTouch(int nID)
{
	auto it = m_mapTouch.find(nID);
	if (it == m_mapTouch.end())
	{
		HmsAviPf::Touch * touch = new HmsAviPf::Touch(nID);
		m_mapTouch.insert(std::make_pair(nID, touch));
		return touch;
	}
	else
		return it->second;
}

void CHmsTouchMgr::ReleaseTouch(int nID)
{
	auto it = m_mapTouch.find(nID);
	if (it != m_mapTouch.end())
	{
		auto touch = it->second;
		if (touch)
		{
			touch->release();
		}
		m_mapTouch.erase(it);
	}
}

HmsAviPf::Touch * CHmsTouchMgr::GetAndRemoveTouch(int nID)
{
	HmsAviPf::Touch * touch = nullptr;
	auto it = m_mapTouch.find(nID);
	if (it != m_mapTouch.end())
	{
		touch = it->second;
		m_mapTouch.erase(it);
	}
	return touch;
}
