

#include "base/HmsEventListenerCustom.h"
#include "base/HmsEventDispatcher.h"
#include "base/HmsEventCustom.h"
#include "HmsEventListener.h"
#include "HmsEventListenerCustom.h"

#include <algorithm>
#include <HmsGE/HmsAviMacros.h>

NS_HMS_BEGIN



HmsAviPf::EventListenerCustom* EventListenerCustom::Create(const std::string& eventName, const std::function<void(CHmsEventCustom*)>& callback)
{
	EventListenerCustom* ret = new (std::nothrow) EventListenerCustom();
	if (ret && ret->Init(eventName, callback))
	{
		ret->autorelease();
	}
	else
	{
		HMS_SAFE_DELETE(ret);
	}
	return ret;
}


bool HmsAviPf::EventListenerCustom::Init(const std::string & listenerId, const std::function<void(CHmsEventCustom*)>& callback)
{
	bool ret = false;

	m_onCustomEvent = callback;
	m_listenerID = listenerId;

	auto listener = [this](CHmsEvent* event){
		if (m_onCustomEvent != nullptr)
		{
			m_onCustomEvent(static_cast<CHmsEventCustom*>(event));
		}
	};

	if (CHmsEventListener::Init(CHmsEventListener::Type::CUSTOM, /*listenerId,*/ listener))
	{
		ret = true;
	}
	return ret;
}

HmsAviPf::EventListenerCustom::EventListenerCustom()
	: m_onCustomEvent(nullptr)
{

}

HmsAviPf::EventListenerCustom* HmsAviPf::EventListenerCustom::Clone()
{
	EventListenerCustom* ret = new (std::nothrow) EventListenerCustom();
	if (ret && ret->Init(m_listenerID, m_onCustomEvent))
	{
		ret->autorelease();
	}
	else
	{
		HMS_SAFE_DELETE(ret);
	}
	return ret;
}

bool HmsAviPf::EventListenerCustom::CheckAvailable()
{
	bool ret = false;
	if (/*CHmsEventListener::CheckAvailable() &&*/ m_onCustomEvent != nullptr)
	{
		ret = true;
	}
	return ret;
}

NS_HMS_END


