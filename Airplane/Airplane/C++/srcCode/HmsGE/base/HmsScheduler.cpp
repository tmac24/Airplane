#include "HmsScheduler.h"
#include "base/HmsNode.h"
#include "base/uthash.h"
NS_HMS_BEGIN

CHmsScheduler::CHmsScheduler()
{

}

CHmsScheduler::~CHmsScheduler()
{

}

void CHmsScheduler::Update(float deltaTime)
{
	if (m_vScheduleAdd.size())
	{
		m_vSchedule.pushBack(m_vScheduleAdd);
		m_vScheduleAdd.clear();
	}

	for (auto c:m_vSchedule)
	{
		if (c && c->IsUpdateAble())
		{
			c->Update(deltaTime);
		}
	}

	if (m_vScheduleRemove.size())
	{
		for (auto c:m_vScheduleRemove)
		{
			m_vSchedule.eraseObject(c, false);
		}

		m_vScheduleRemove.clear();
	}

	if (m_vScheduleOnce.size())
	{
		for (auto & c:m_vScheduleOnce)
		{
			if (c)
			{
				c(deltaTime);
			}
		}
		m_vScheduleOnce.clear();
	}

	if (m_mapTargetSelector.size())
	{
		for (auto & c:m_mapTargetSelector)
		{
			if (c.second)
			{
				c.second->update(deltaTime);
			}
		}
	}

	if (m_mapTargetCallback.size())
	{
		for (auto & c : m_mapTargetCallback)
		{
			if (c.second)
			{
				c.second->update(deltaTime);
			}
		}
	}

	if (m_removeTargetLater.size())
	{
		for (auto & c:m_removeTargetLater)
		{
			if (c)
			{
				c();
			}
		}
		m_removeTargetLater.clear();
	}

	if (!m_vFunctionsToPerform.empty()) {
		m_mutexPerform.lock();
		auto temp = std::move(m_vFunctionsToPerform);
		m_mutexPerform.unlock();

		for (const auto &function : temp) {
			function();
		}
	}
}

void CHmsScheduler::AddSchedule(CHmsNode * node)
{
	m_vScheduleAdd.pushBack(node);
}

void CHmsScheduler::AddSchedule(const HmsSchedulerFunc & callback, void *target, float interval, unsigned int repeat, float delay, bool paused, const std::string& key)
{

	TimerTargetCallback *timer = new TimerTargetCallback();
	timer->initWithCallback(this, callback, target, key, interval, repeat, delay);
	timer->autorelease();
	timer->retain();
	m_mapTargetCallback.insert(std::pair<std::string, TimerTargetCallback*>(key, timer));
}

void CHmsScheduler::AddSchedule(const HmsSchedulerFunc& callback, void *target, float interval, bool paused, const std::string& key)
{
	this->AddSchedule(callback, target, interval, HMS_REPEAT_FOREVER, 0.0f, paused, key);
}

#include "external/xxhash/xxhash.h"
unsigned int GetHashData(SEL_SCHEDULE selector, Ref *target)
{
#pragma pack(push, 1)
	struct HashScheduleInfo
	{
		SEL_SCHEDULE selector;
		Ref *target;
	};
#pragma  pack(pop)

	HashScheduleInfo hsi;
	hsi.selector = selector;
	hsi.target = target;
	auto hash = XXH32((const void*)&hsi, sizeof(hsi), 0);
	return hash;
}


void CHmsScheduler::AddSchedule(SEL_SCHEDULE selector, Ref *target, float interval, unsigned int repeat, float delay, bool paused)
{
	TimerTargetSelector *timer = new STD_NOTHROW TimerTargetSelector();
	timer->initWithSelector(this, selector, target, interval, repeat, delay);
	timer->autorelease();

	auto hKey = GetHashData(selector, target);
	timer->retain();
	m_mapTargetSelector.insert(std::pair<unsigned int, TimerTargetSelector*>(hKey, timer));
}

void CHmsScheduler::AddSchedule(SEL_SCHEDULE selector, Ref *target, float interval, bool paused)
{
	AddSchedule(selector, target, interval, HMS_REPEAT_FOREVER, 0.0f, paused);
}

void CHmsScheduler::RemoveSchedule(CHmsNode * node)
{
	m_vScheduleRemove.pushBack(node);
}

void CHmsScheduler::RemoveSchedule(const std::string& key, void *target)
{
	auto doIt = [&]()
	{
		auto itFind = m_mapTargetCallback.find(key);

		while (itFind != m_mapTargetCallback.end())
		{
			if (itFind->first != key)
			{
				break;
			}

			if (itFind->second->getTarget() == target)
			{
				itFind->second->autorelease();
				m_mapTargetCallback.erase(itFind);
				break;
			}

			itFind++;
		}
	};

	m_removeTargetLater.push_back(doIt);
}

void CHmsScheduler::RemoveSchedule(SEL_SCHEDULE selector, Ref *target)
{
	auto doIt = [&]()
	{
		auto hKey = GetHashData(selector, target);

		auto itFind = m_mapTargetSelector.find(hKey);

		while (itFind != m_mapTargetSelector.end())
		{
			if (itFind->first != hKey)
			{
				break;
			}

			if (itFind->second->getSelector() == selector && itFind->second->getTarget() == target)
			{
				itFind->second->autorelease();
				m_mapTargetSelector.erase(itFind);
				break;
			}

			itFind++;
		}
	};

	m_removeTargetLater.push_back(doIt);
}

void CHmsScheduler::AddScheduleOnce(const HmsSchedulerFunc & callback)
{
	m_vScheduleOnce.push_back(callback);
}

void HmsAviPf::CHmsScheduler::PerformFunctionInAviThread(std::function<void()> function)
{
	std::lock_guard<std::mutex> lock(m_mutexPerform);
	m_vFunctionsToPerform.push_back(std::move(function));
}

void HmsAviPf::CHmsScheduler::RemoveAllFunctionsToBePerformedInAviThread()
{
	std::unique_lock<std::mutex> lock(m_mutexPerform);
	m_vFunctionsToPerform.clear();
}

NS_HMS_END



