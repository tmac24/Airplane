#include "HmsEventDispatcher.h"
#include "base/HmsEventListenerTouch.h"
#include "base/HmsEventListenerKey.h"
#include "base/HmsIndexMgr32.h"
#include "base/HmsEventListenerCustom.h"

NS_HMS_BEGIN

CHmsEventDispatcher::CHmsEventDispatcher()
{

}

CHmsEventDispatcher::~CHmsEventDispatcher()
{

}
#define DEAL_EVE_DIRECT 0
void CHmsEventDispatcher::AddEventListener(CHmsEventListener * pEvtListener, CHmsNode * node)
{
#if ADD_EVE_DIRECT
	CHmsEventListenerGroup * group = nullptr;

	auto itFind = m_mapEvtListener.find(pEvtListener->GetType());
	if (itFind == m_mapEvtListener.end())
	{
		group = new CHmsEventListenerGroup;
		m_mapEvtListener.insert(std::make_pair(pEvtListener->GetType(), group));
	}
	else
		group = itFind->second;

	group->m_vEvtListener.pushBack(pEvtListener);
#else
	if (pEvtListener)
	{
		pEvtListener->BindNode(node);
	}
	m_vEvtListenerAdd.pushBack(pEvtListener);
#endif
	
}

void CHmsEventDispatcher::AddEventListener(CHmsEventListener * pEvtListener, CHmsNode * node, int32_t nOrderID)
{
	if (pEvtListener)
	{
		AddEventListener(pEvtListener, node);
		pEvtListener->SetEventOrderId(nOrderID);
	}
}

void CHmsEventDispatcher::RemoveEventListener(CHmsEventListener * pEvtListener)
{
#if DEAL_EVE_DIRECT
	auto itFind = m_mapEvtListener.find(pEvtListener->GetType());
	if (itFind != m_mapEvtListener.end())
	{
		CHmsEventListenerGroup * group = itFind->second;
		if (group)
		{
			group->m_vEvtListener.eraseObject(pEvtListener, false);
		}
	}
#else
	m_vEvtListenerRemove.pushBack(pEvtListener);
#endif
}

HmsAviPf::EventListenerCustom* HmsAviPf::CHmsEventDispatcher::AddCustomEventListener(const std::string &eventName, const std::function<void(CHmsEventCustom*)>& callback)
{
	EventListenerCustom *listener = EventListenerCustom::Create(eventName, callback);
	//AddEventListenerWithFixedPriority(listener, 1);
	return listener;
}

void HmsAviPf::CHmsEventDispatcher::RemoveCustomEventListeners(const std::string& customEventName)
{
	//removeEventListenersForListenerID(customEventName);
}

void CHmsEventDispatcher::DispatchEvent(CHmsEvent * event)
{
	auto eType = event->GetType();
	switch (eType)
	{
	case HmsAviPf::CHmsEvent::Type::TOUCH:
		DispatchTouchEvent(dynamic_cast<EventTouch*>(event));
		break;
	case HmsAviPf::CHmsEvent::Type::KEYBOARD:
		break;
	case HmsAviPf::CHmsEvent::Type::MOUSE:
		break;
	case HmsAviPf::CHmsEvent::Type::KEYBTN:
		DispatchKeyButtonEvent(dynamic_cast<EventKeyButton*>(event));
		break;
	case HmsAviPf::CHmsEvent::Type::KEYKNOB:
		DispatchKeyKnobEvent(dynamic_cast<EventKeyKnob*>(event));
		break;
	default:
		break;
	}
}

CHmsEventListenerGroup * CHmsEventDispatcher::GetEventListeners(CHmsEventListener::Type type)
{
	auto itFind = m_mapEvtListener.find(type);
	if (itFind != m_mapEvtListener.end())
	{
		return itFind->second;
	}
	return nullptr;
}

bool CHmsEventDispatcher::DealTouchEventOneByOne(Touch *touch, CHmsEventListener* listener, EventTouch * event)
{
	bool bSwallow = false;
	EventListenerTouchOneByOne * obo = static_cast<EventListenerTouchOneByOne*>(listener);
	
	if (obo)
	{
		auto eType = event->getEventCode();
		bool bActiveTouch = false;

		CHmsNode * bindNode = obo->GetBindNode();
		if (bindNode && !bindNode->IsUpdateAble())
		{
			return bSwallow;
		}

		switch (eType)
		{
		case EventTouch::EventCode::BEGAN:
		{
			if (obo->onTouchBegan)
			{
				bActiveTouch = obo->onTouchBegan(touch, event);

				if (bActiveTouch)
				{
					obo->_claimedTouches.push_back(touch);
				}
			}
		}
		break;
		case EventTouch::EventCode::MOVED:
		{
			auto itFind = std::find(obo->_claimedTouches.begin(), obo->_claimedTouches.end(), touch);
			if (obo->onTouchMoved && obo->_claimedTouches.size() && 
				(itFind != obo->_claimedTouches.end()))
			{
				obo->onTouchMoved(touch, event);
				bActiveTouch = true;
			}
		}
		break;
		case EventTouch::EventCode::ENDED:
		{
			auto itFind = std::find(obo->_claimedTouches.begin(), obo->_claimedTouches.end(), touch);
			if (obo->onTouchEnded && obo->_claimedTouches.size() &&
				(itFind != obo->_claimedTouches.end()))
			{
				obo->onTouchEnded(touch, event);
				obo->_claimedTouches.erase(itFind);
				bActiveTouch = true;
			}
		}
		break;
		case EventTouch::EventCode::CANCELLED:
			break;
		default:
			break;
		}

		if (bActiveTouch && obo->isSwallowTouches())
		{
			//this msg have been used,
			bSwallow = true;
		}
	}

	return bSwallow;
};

void CHmsEventDispatcher::TestTouchEventOneByOne(Touch *touch, CHmsEventListener* listener, EventTouch * event)
{
	EventListenerTouchOneByOne * obo = static_cast<EventListenerTouchOneByOne*>(listener);

	if (obo && event)
	{
		CHmsNode * bindNode = obo->GetBindNode();
		if (bindNode && !bindNode->IsUpdateAble())
		{
			return;
		}

		if (obo->isSwallowTouches())
		{
			m_vEvtListenerTemp.front() = listener;
			return;
		}

		auto frontNode = [=](CHmsNode *first, CHmsNode *second)
		{
			if (first)
			{
				if (!(first->GetLocalZOrder() > second->GetLocalZOrder() ||
					(first->GetLocalZOrder() == second->GetLocalZOrder() && first->GetOrderOfArrival() > second->GetOrderOfArrival())))
				{
					m_vEvtListenerTemp.back() = listener;
					return second;
				}
			}
			else
			{
				first = second;
				m_vEvtListenerTemp.back() = listener;
			}
			
			return first;
		};

		auto posOrg = bindNode->ConvertTouchToNodeSpace(touch);
		auto rect = bindNode->GetRectByOrigin();
		if ((event->getEventCode() == EventTouch::EventCode::BEGAN) && rect.containsPoint(posOrg))
		{
			m_pTempNode = frontNode(m_pTempNode, bindNode);
		}
	}
}

void CHmsEventDispatcher::DispatchTouchEvent(EventTouch * event)
{
	auto oneByOneListeners = GetEventListeners(CHmsEventListener::Type::TOUCH_ONE_BY_ONE);
	auto allOnceListeners = GetEventListeners(CHmsEventListener::Type::TOUCH_ALL);

	if (nullptr == oneByOneListeners && nullptr == allOnceListeners)
	{
		return;
	}

	auto touches = event->getTouches();
	std::vector<Touch*> touchAll;

	//Touch * tt;

	if (oneByOneListeners)
	{
		auto DispatchTouchToListener = [=](Touch *touch, CHmsEventListenerGroup * listenerGroup, EventTouch * event)
		{
			for (auto listener : listenerGroup->m_vEvtListener)
			{
				if (DealTouchEventOneByOne(touch, listener, event))
				{
					return true;
				}
			}

			//this means msg to the after
			return false;
		};


		for (Touch * touch:touches)
		{
			if (!DispatchTouchToListener(touch, oneByOneListeners, event))//original content.
			{
				touchAll.push_back(touch);
			}

			if (event->IsStoped())
			{
				return;
			}
		}
	}
	else
	{
		touchAll = touches;
	}

	if (allOnceListeners)
	{
		auto DealTouchEventAllAtOnce = [=](const std::vector<Touch*> & touches, CHmsEventListener* listener, EventTouch * event)
		{
			EventListenerTouchAllAtOnce * aao = static_cast<EventListenerTouchAllAtOnce*>(listener);
			if (aao)
			{
				auto eType = event->getEventCode();
				bool bActiveTouch = false;

				CHmsNode * bindNode = aao->GetBindNode();
				if (bindNode && !bindNode->IsUpdateAble())
				{
					return;
				}

				switch (eType)
				{
				case EventTouch::EventCode::BEGAN:
				{
					if (aao->onTouchesBegan)
					{
						aao->onTouchesBegan(touches, event);
					}
				}
				break;
				case EventTouch::EventCode::MOVED:
				{
					if (aao->onTouchesMoved)
					{
						aao->onTouchesMoved(touches, event);
					}
				}
				break;
				case EventTouch::EventCode::ENDED:
				{
					if (aao->onTouchesEnded)
					{
						aao->onTouchesEnded(touches, event);
					}
				}
				break;
				case EventTouch::EventCode::CANCELLED:
				{
					if (aao->onTouchesCancelled)
					{
						aao->onTouchesCancelled(touches, event);
					}
				}
				break;
				default:
					break;
				}

			}
		};

		if (touchAll.size())
		{
			for (auto listener : allOnceListeners->m_vEvtListener)
			{
				//DealTouchEventAllAtOnce(touches, listener, event);
				DealTouchEventAllAtOnce(touchAll, listener, event);

				if (event->IsStoped())
				{
					return;
				}
			}
		}
	}

}

void CHmsEventDispatcher::UpdateEventListener()
{
	if (m_vEvtListenerAdd.size())
	{
		for (auto c : m_vEvtListenerAdd)
		{
			CHmsEventListenerGroup * group = nullptr;

			auto itFind = m_mapEvtListener.find(c->GetType());
			if (itFind == m_mapEvtListener.end())
			{
				group = new CHmsEventListenerGroup;
				m_mapEvtListener.insert(std::make_pair(c->GetType(), group));
			}
			else
				group = itFind->second;

			group->m_vEvtListener.pushBack(c);
		}

		for (auto c : m_mapEvtListener)
		{
			CHmsEventListenerGroup * group = c.second;
			if (group)
			{
				std::sort(group->m_vEvtListener.begin(), group->m_vEvtListener.end(), [](CHmsEventListener* n1, CHmsEventListener* n2){
					return n1->GetEventOrderId() < n2->GetEventOrderId();
				});
			}
		}
		
		m_vEvtListenerAdd.clear();
	}

	for (auto c : m_vEvtListenerRemove)
	{
		auto itFind = m_mapEvtListener.find(c->GetType());
		if (itFind != m_mapEvtListener.end())
		{
			CHmsEventListenerGroup * group = itFind->second;
			if (group)
			{
				group->m_vEvtListener.eraseObject(c, false);
			}
		}
	}
	m_vEvtListenerRemove.clear();
	
}

void CHmsEventDispatcher::DispatchKeyButtonEvent(EventKeyButton * event)
{
	auto keyBtnListeners = GetEventListeners(CHmsEventListener::Type::KEY_BTN);
	
	if (nullptr == keyBtnListeners)
	{
		return;
	}

	for (auto listener : keyBtnListeners->m_vEvtListener)
	{
		EventListenerKeyButton * kb = static_cast<EventListenerKeyButton*>(listener);
		if (kb->m_callbackKeyBtn && kb->m_callbackKeyBtn(event->m_nBtnID, event->m_nBtnState, event))
		{
			return;
		}
	}
}

void CHmsEventDispatcher::DispatchKeyKnobEvent(EventKeyKnob * event)
{
	auto keyKnobListeners = GetEventListeners(CHmsEventListener::Type::KEY_KNOB);

	if (nullptr == keyKnobListeners)
	{
		return;
	}

	for (auto listener : keyKnobListeners->m_vEvtListener)
	{
		EventListenerKeyKnob * kb = static_cast<EventListenerKeyKnob*>(listener);
		if (kb->m_callbackKeyKnob && kb->m_callbackKeyKnob(event->m_nKnobID, event->m_fRotateRatio, event))
		{
			return;
		}
	}
}

NS_HMS_END;

