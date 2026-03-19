#include "HmsScheduleTimer.h"
#include "base/HmsScheduler.h"

NS_HMS_BEGIN
// data structures



// implementation Timer

ScheduleTimer::ScheduleTimer()
	: _scheduler(nullptr)
	, _elapsed(-1)
	, _runForever(false)
	, _useDelay(false)
	, _timesExecuted(0)
	, _repeat(0)
	, _delay(0.0f)
	, _interval(0.0f)
{

}

void ScheduleTimer::setupTimerWithInterval(float seconds, unsigned int repeat, float delay)
{
	_elapsed = -1;
	_interval = seconds;
	_delay = delay;
	_useDelay = (_delay > 0.0f) ? true : false;
	_repeat = repeat;
	_runForever = (_repeat == HMS_REPEAT_FOREVER) ? true : false;
}

void ScheduleTimer::update(float dt)
{
	if (_elapsed == -1)
	{
		_elapsed = 0;
		_timesExecuted = 0;
		return;
	}

	// accumulate elapsed time
	_elapsed += dt;

	// deal with delay
	if (_useDelay)
	{
		if (_elapsed < _delay)
		{
			return;
		}
		trigger(_delay);
		_elapsed = _elapsed - _delay;
		_timesExecuted += 1;
		_useDelay = false;
		// after delay, the rest time should compare with interval
		if (!_runForever && _timesExecuted > _repeat)
		{    //unschedule timer
			cancel();
			return;
		}
	}

	// if _interval == 0, should trigger once every frame
	float interval = (_interval > 0) ? _interval : _elapsed;
	while (_elapsed >= interval)
	{
		trigger(interval);
		_elapsed -= interval;
		_timesExecuted += 1;

		if (!_runForever && _timesExecuted > _repeat)
		{
			cancel();
			break;
		}

		if (_elapsed <= 0.f)
		{
			break;
		}
	}
}

// TimerTargetSelector

TimerTargetSelector::TimerTargetSelector()
	: _target(nullptr)
	, _selector(nullptr)
{
}

bool TimerTargetSelector::initWithSelector(CHmsScheduler* scheduler, SEL_SCHEDULE selector, Ref* target, float seconds, unsigned int repeat, float delay)
{
	_scheduler = scheduler;
	_target = target;
	_selector = selector;
	setupTimerWithInterval(seconds, repeat, delay);
	return true;
}

void TimerTargetSelector::trigger(float dt)
{
	if (_target && _selector)
	{
		(_target->*_selector)(dt);
	}
}

void TimerTargetSelector::cancel()
{
	_scheduler->RemoveSchedule(_selector, _target);
}

// TimerTargetCallback

TimerTargetCallback::TimerTargetCallback()
	: _target(nullptr)
	, _callback(nullptr)
{
}

bool TimerTargetCallback::initWithCallback(CHmsScheduler* scheduler, const HmsSchedulerFunc& callback, void *target, const std::string& key, float seconds, unsigned int repeat, float delay)
{
	_scheduler = scheduler;
	_target = target;
	_callback = callback;
	_key = key;
	setupTimerWithInterval(seconds, repeat, delay);
	return true;
}

void TimerTargetCallback::trigger(float dt)
{
	if (_callback)
	{
		_callback(dt);
	}
}

void TimerTargetCallback::cancel()
{
	_scheduler->RemoveSchedule(_key, _target);
}


NS_HMS_END