#pragma once

#include "HmsAviMacros.h"
#include <stdint.h>
#include <string>
#include "base/Ref.h"
#include "HmsVector.h"
#include "HmsEventListener.h"
#include "HmsEventTouch.h"
#include "HmsEventKey.h"
#include "HmsEventCustom.h"
#include "HmsNode.h"
#include <map>
#include <vector>


NS_HMS_BEGIN

class EventListenerCustom;

class HMS_DLL CHmsEventListenerGroup
{
public:
	Vector<CHmsEventListener*>			m_vEvtListener;
};

// class HMS_DLL CHmsEventListenerLater
// {
// public:
// 	CHmsEventListener * pListener;
// };

class HMS_DLL CHmsEventDispatcher : public Ref
{
public:
	CHmsEventDispatcher();
	~CHmsEventDispatcher();

	void AddEventListener(CHmsEventListener * pEvtListener, CHmsNode * node);
	void AddEventListener(CHmsEventListener * pEvtListener, CHmsNode * node, int32_t nOrderID);
	void RemoveEventListener(CHmsEventListener * pEvtListener);

	EventListenerCustom* AddCustomEventListener(const std::string &eventName, const std::function<void(CHmsEventCustom*)>& callback);
	void RemoveCustomEventListeners(const std::string& customEventName);

	void DispatchEvent(CHmsEvent * event);

	void UpdateEventListener();

	CHmsEventListenerGroup * GetEventListeners(CHmsEventListener::Type type);

protected:
	void DispatchTouchEvent(EventTouch * event);
	void DispatchKeyButtonEvent(EventKeyButton * event);
	void DispatchKeyKnobEvent(EventKeyKnob * event);

private:
	/**
	*
	*param   
	*returns true the msg need be swallowed
	*by [10/18/2017 song.zhang] 
	*/
	bool DealTouchEventOneByOne(Touch *touch, CHmsEventListener* listener, EventTouch * event);

	/**
	* get front CHmsNode and Swallow Node.
	*param
	*returns void.
	*by [01/24/2018 yonghong.xiao]
	*/
	void TestTouchEventOneByOne(Touch *touch, CHmsEventListener* listener, EventTouch * event);

private:
	std::map<CHmsEventListener::Type, CHmsEventListenerGroup*>	m_mapEvtListener;
	std::vector<CHmsEventListener*>								m_vEvtListenerTemp;		    //used for one by one test[01/24/2018 yonghong.xiao]
 	Vector<CHmsEventListener*>									m_vEvtListenerAdd;			//used for add listener safely
 	Vector<CHmsEventListener*>									m_vEvtListenerRemove;		//used for remove listener safely
	CHmsNode*													m_pTempNode;				//used for one by one test[01/24/2018 yonghong.xiao]
};


NS_HMS_END;

