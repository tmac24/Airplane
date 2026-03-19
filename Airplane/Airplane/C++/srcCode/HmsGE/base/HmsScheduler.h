#pragma once

#include "HmsAviMacros.h"
#include <stdint.h>
#include <string>
#include "base/Ref.h"
#include "HmsVector.h"
#include "HmsScheduleTimer.h"
#include <map>
#include <mutex>

NS_HMS_BEGIN

class CHmsNode;

class HMS_DLL CHmsScheduler : public Ref
{
public:
	CHmsScheduler();
	virtual ~CHmsScheduler();

	/**
	* update the schedule
	*param   deltaTime		unit:second
	*returns 
	*by [4/28/2017 song.zhang079] 
	*/
	void Update(float deltaTime);

	void AddSchedule(CHmsNode * node);
	void AddSchedule(const HmsSchedulerFunc & callback, void *target, float interval, unsigned int repeat, float delay, bool paused, const std::string& key);
	void AddSchedule(const HmsSchedulerFunc& callback, void *target, float interval, bool paused, const std::string& key);
	void AddSchedule(SEL_SCHEDULE selector, Ref *target, float interval, unsigned int repeat, float delay, bool paused);
	void AddSchedule(SEL_SCHEDULE selector, Ref *target, float interval, bool paused);

	void RemoveSchedule(CHmsNode * node);
	void RemoveSchedule(const std::string& key, void *target);
	void RemoveSchedule(SEL_SCHEDULE selector, Ref *target);

	void AddScheduleOnce(const HmsSchedulerFunc & callback);

	void PerformFunctionInAviThread(std::function<void()> function);

	void RemoveAllFunctionsToBePerformedInAviThread();

private:
	Vector<CHmsNode*>										m_vSchedule;
	Vector<CHmsNode*>										m_vScheduleAdd;				//used for dynamic add schedule
	Vector<CHmsNode*>										m_vScheduleRemove;			//used for dynamic remove schedule

	std::vector<HmsSchedulerFunc>							m_vScheduleOnce;			//once update vector
	std::multimap<unsigned int, TimerTargetSelector*>		m_mapTargetSelector;
	std::multimap<std::string, TimerTargetCallback*>		m_mapTargetCallback;

	std::vector<std::function<void()>>						m_removeTargetLater;

	std::vector<std::function<void()>>						m_vFunctionsToPerform;
	std::mutex												m_mutexPerform;
};

NS_HMS_END