#include "HmsEventListener.h"

NS_HMS_BEGIN




CHmsEventListener::CHmsEventListener()
	:m_nOrder(0)
{

}

CHmsEventListener::~CHmsEventListener()
{

}

bool CHmsEventListener::Init(Type t, const std::function<void(CHmsEvent*)>& callback)
{
	m_eType = t;
	m_callbackOnEvent = callback;
	return true;
}

NS_HMS_END;

