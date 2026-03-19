

#include "base/HmsEventListenerKey.h"
#include "base/HmsEventDispatcher.h"
#include "base/HmsEventTouch.h"
#include "base/HmsTouch.h"

#include <algorithm>

NS_HMS_BEGIN

const std::string EventListenerKeyButton::LISTENER_ID = "__HMS_key_btn";

EventListenerKeyButton::EventListenerKeyButton()
:  m_callbackKeyBtn(nullptr)
{
}

EventListenerKeyButton::~EventListenerKeyButton()
{
    
}

bool EventListenerKeyButton::init()
{
	if (CHmsEventListener::Init(Type::KEY_BTN, nullptr))
    {
        return true;
    }
    
    return false;
}


EventListenerKeyButton* EventListenerKeyButton::create()
{
    auto ret = new STD_NOTHROW EventListenerKeyButton();
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

bool EventListenerKeyButton::CheckAvailable()
{
    if (m_callbackKeyBtn == nullptr)
    {
        return false;
    }
    
    return true;
}

EventListenerKeyButton* EventListenerKeyButton::Clone()
{
    auto ret = new STD_NOTHROW EventListenerKeyButton();
    if (ret && ret->init())
    {
        ret->autorelease();
        
        ret->m_callbackKeyBtn = m_callbackKeyBtn;
    }
    else
    {
        HMS_SAFE_DELETE(ret);
    }
    return ret;
}




const std::string EventListenerKeyKnob::LISTENER_ID = "__HMS_key_knob";


EventListenerKeyKnob* EventListenerKeyKnob::create()
{
	auto ret = new STD_NOTHROW EventListenerKeyKnob();
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

EventListenerKeyKnob::~EventListenerKeyKnob()
{

}

EventListenerKeyKnob* EventListenerKeyKnob::Clone()
{
	auto ret = new STD_NOTHROW EventListenerKeyKnob();
	if (ret && ret->init())
	{
		ret->autorelease();

		ret->m_callbackKeyKnob = m_callbackKeyKnob;
	}
	else
	{
		HMS_SAFE_DELETE(ret);
	}
	return ret;
}

bool EventListenerKeyKnob::CheckAvailable()
{
	if (m_callbackKeyKnob == nullptr)
	{
		return false;
	}

	return true;
}

EventListenerKeyKnob::EventListenerKeyKnob()
	: m_callbackKeyKnob(nullptr)
{

}

bool EventListenerKeyKnob::init()
{
	if (CHmsEventListener::Init(Type::KEY_KNOB, nullptr))
	{
		return true;
	}

	return false;
}

NS_HMS_END
