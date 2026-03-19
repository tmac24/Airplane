#ifndef __HMS_CustomEventListener1__
#define __HMS_CustomEventListener1__

#include "base/HmsEventListener.h"
#include <vector>


NS_HMS_BEGIN

class CHmsEventCustom;

class HMS_DLL EventListenerCustom : public CHmsEventListener
{
public:
	static EventListenerCustom* Create(const std::string& eventName, const std::function<void(CHmsEventCustom*)>& callback);
    
	virtual bool CheckAvailable() override;
	virtual EventListenerCustom* Clone() override;

protected:
	EventListenerCustom();

	/** Initializes event with type and callback function */
	bool Init(const std::string & listenerId, const std::function<void(CHmsEventCustom*)>& callback);

protected:
	std::function<void(CHmsEventCustom*)> m_onCustomEvent;
	std::string							  m_listenerID;

	friend class CHmsEventDispatcher;
};


NS_HMS_END

#endif 
