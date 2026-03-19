#include "HmsEvent.h"

NS_HMS_BEGIN


CHmsEvent::CHmsEvent(Type type)
: m_eType(type)
, m_bIsStopped(false)
, m_pNodeCurrent(nullptr)
{

}

CHmsEvent::~CHmsEvent()
{

}

NS_HMS_END

