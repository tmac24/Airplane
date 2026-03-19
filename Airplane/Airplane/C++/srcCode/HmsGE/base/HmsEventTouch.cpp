

#include "base/HmsEventTouch.h"
#include "base/HmsTouch.h"

NS_HMS_BEGIN

EventTouch::EventTouch()
: CHmsEvent(Type::TOUCH)
{
    _touches.reserve(MAX_TOUCHES);
}

NS_HMS_END
