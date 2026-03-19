

#include "base/HmsEventListenerTouch.h"
#include "base/HmsEventDispatcher.h"
#include "base/HmsEventTouch.h"
#include "base/HmsTouch.h"

#include <algorithm>

NS_HMS_BEGIN

const std::string EventListenerTouchOneByOne::LISTENER_ID = "__HMS_touch_one_by_one";

EventListenerTouchOneByOne::EventListenerTouchOneByOne()
: onTouchBegan(nullptr)
, onTouchMoved(nullptr)
, onTouchEnded(nullptr)
, onTouchCancelled(nullptr)
, _needSwallow(false)
{
}

EventListenerTouchOneByOne::~EventListenerTouchOneByOne()
{
    CCLOGINFO("In the destructor of EventListenerTouchOneByOne, %p", this);
}

bool EventListenerTouchOneByOne::init()
{
    if (CHmsEventListener::Init(Type::TOUCH_ONE_BY_ONE, nullptr))
    {
        return true;
    }
    
    return false;
}

void EventListenerTouchOneByOne::setSwallowTouches(bool needSwallow)
{
    _needSwallow = needSwallow;
}

bool EventListenerTouchOneByOne::isSwallowTouches()
{
    return _needSwallow;
}

EventListenerTouchOneByOne* EventListenerTouchOneByOne::create()
{
    auto ret = new STD_NOTHROW EventListenerTouchOneByOne();
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        HMS_SAFE_DELETE(ret);
    }
    return ret;
}

bool EventListenerTouchOneByOne::CheckAvailable()
{
    // EventDispatcher will use the return value of 'onTouchBegan' to determine whether to pass following 'move', 'end'
    // message to 'EventListenerTouchOneByOne' or not. So 'onTouchBegan' needs to be set.
    if (onTouchBegan == nullptr)
    {
        HMSASSERT(false, "Invalid EventListenerTouchOneByOne!");
        return false;
    }
    
    return true;
}

EventListenerTouchOneByOne* EventListenerTouchOneByOne::Clone()
{
    auto ret = new STD_NOTHROW EventListenerTouchOneByOne();
    if (ret && ret->init())
    {
        ret->autorelease();
        
        ret->onTouchBegan = onTouchBegan;
        ret->onTouchMoved = onTouchMoved;
        ret->onTouchEnded = onTouchEnded;
        ret->onTouchCancelled = onTouchCancelled;
        
        ret->_claimedTouches = _claimedTouches;
        ret->_needSwallow = _needSwallow;
    }
    else
    {
        HMS_SAFE_DELETE(ret);
    }
    return ret;
}

/////////

const std::string EventListenerTouchAllAtOnce::LISTENER_ID = "__HMS_touch_all_at_once";

EventListenerTouchAllAtOnce::EventListenerTouchAllAtOnce()
: onTouchesBegan(nullptr)
, onTouchesMoved(nullptr)
, onTouchesEnded(nullptr)
, onTouchesCancelled(nullptr)
{
}

EventListenerTouchAllAtOnce::~EventListenerTouchAllAtOnce()
{
    CCLOGINFO("In the destructor of EventListenerTouchAllAtOnce, %p", this);
}

bool EventListenerTouchAllAtOnce::init()
{
	if (CHmsEventListener::Init(Type::TOUCH_ALL, nullptr))
    {
        return true;
    }
    
    return false;
}

EventListenerTouchAllAtOnce* EventListenerTouchAllAtOnce::create()
{
    auto ret = new STD_NOTHROW EventListenerTouchAllAtOnce();
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        HMS_SAFE_DELETE(ret);
    }
    return ret;
}

bool EventListenerTouchAllAtOnce::CheckAvailable()
{
    if (onTouchesBegan == nullptr && onTouchesMoved == nullptr
        && onTouchesEnded == nullptr && onTouchesCancelled == nullptr)
    {
        HMSASSERT(false, "Invalid EventListenerTouchAllAtOnce!");
        return false;
    }
    
    return true;
}

EventListenerTouchAllAtOnce* EventListenerTouchAllAtOnce::Clone()
{
    auto ret = new STD_NOTHROW EventListenerTouchAllAtOnce();
    if (ret && ret->init())
    {
        ret->autorelease();
        
        ret->onTouchesBegan = onTouchesBegan;
        ret->onTouchesMoved = onTouchesMoved;
        ret->onTouchesEnded = onTouchesEnded;
        ret->onTouchesCancelled = onTouchesCancelled;
    }
    else
    {
        HMS_SAFE_DELETE(ret);
    }
    return ret;
}

NS_HMS_END
