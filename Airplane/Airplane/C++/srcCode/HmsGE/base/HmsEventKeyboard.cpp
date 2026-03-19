
#include "base/HmsEventKeyboard.h"

NS_HMS_BEGIN

EventKeyboard::EventKeyboard(KeyCode keyCode, bool isPressed)
: CHmsEvent(Type::KEYBOARD)
, _keyCode(keyCode)
, _isPressed(isPressed)
{}

NS_HMS_END
