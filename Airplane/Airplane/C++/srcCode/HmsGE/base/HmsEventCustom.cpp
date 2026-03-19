#include "HmsEventCustom.h"

NS_HMS_BEGIN


CHmsEventCustom::CHmsEventCustom(const std::string& eventName)
: CHmsEvent(Type::CUSTOM)
, _userData(nullptr)
, _eventName(eventName)
{
}

NS_HMS_END

