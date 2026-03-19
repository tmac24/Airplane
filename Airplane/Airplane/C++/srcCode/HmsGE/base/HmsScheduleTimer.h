#pragma once
#include "HmsAviMacros.h"
#include "base/Ref.h"
#include <functional>
#include <string>

NS_HMS_BEGIN
class CHmsScheduler;

class HMS_DLL ScheduleTimer : public Ref
{
protected:
	ScheduleTimer();
public:
	/** get interval in seconds */
	inline float getInterval() const { return _interval; };
	/** set interval in seconds */
	inline void setInterval(float interval) { _interval = interval; };

	void setupTimerWithInterval(float seconds, unsigned int repeat, float delay);

	virtual void trigger(float dt) = 0;
	virtual void cancel() = 0;

	/** triggers the timer */
	void update(float dt);

protected:

	CHmsScheduler * _scheduler; // weak ref
	float _elapsed;
	bool _runForever;
	bool _useDelay;
	unsigned int _timesExecuted;
	unsigned int _repeat; //0 = once, 1 is 2 x executed
	float _delay;
	float _interval;
};


class HMS_DLL TimerTargetSelector : public ScheduleTimer
{
public:
	TimerTargetSelector();

	/** Initializes a timer with a target, a selector and an interval in seconds, repeat in number of times to repeat, delay in seconds. */
	bool initWithSelector(CHmsScheduler * scheduler, SEL_SCHEDULE selector, Ref* target, float seconds, unsigned int repeat, float delay);

	inline SEL_SCHEDULE getSelector() const { return _selector; };

	inline Ref * getTarget() const { return _target; }

	virtual void trigger(float dt) override;
	virtual void cancel() override;

protected:
	Ref* _target;
	SEL_SCHEDULE _selector;
};


class HMS_DLL TimerTargetCallback : public ScheduleTimer
{
public:
	TimerTargetCallback();

	// Initializes a timer with a target, a lambda and an interval in seconds, repeat in number of times to repeat, delay in seconds.
	bool initWithCallback(CHmsScheduler * scheduler, const HmsSchedulerFunc& callback, void *target, const std::string& key, float seconds, unsigned int repeat, float delay);

	inline const HmsSchedulerFunc& getCallback() const { return _callback; };
	inline const std::string& getKey() const { return _key; };

	virtual void trigger(float dt) override;
	virtual void cancel() override;

	inline void * getTarget() const { return _target; }


protected:
	void* _target;
	HmsSchedulerFunc _callback;
	std::string _key;
};

NS_HMS_END
