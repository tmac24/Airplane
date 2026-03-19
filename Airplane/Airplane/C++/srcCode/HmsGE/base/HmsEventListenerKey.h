#ifndef __HMS_BtnKeyEventListener1__
#define __HMS_BtnKeyEventListener1__

#include "base/HmsEventListener.h"
#include <vector>


NS_HMS_BEGIN


class HMS_DLL EventListenerKeyButton : public CHmsEventListener
{
public:
    static const std::string LISTENER_ID;
    
    static EventListenerKeyButton* create();
    

    virtual ~EventListenerKeyButton();
    
    /// Overrides
    virtual EventListenerKeyButton* Clone() override;
    virtual bool CheckAvailable() override;

public:
    typedef std::function<bool(int, int,CHmsEvent*)> HmsKeyBtnCallback;

	HmsKeyBtnCallback	m_callbackKeyBtn;
    
    EventListenerKeyButton();
    bool init();
    
private:
    
	friend class CHmsEventDispatcher;
};

class HMS_DLL EventListenerKeyKnob : public CHmsEventListener
{
public:
	static const std::string LISTENER_ID;

	static EventListenerKeyKnob* create();


	virtual ~EventListenerKeyKnob();

	/// Overrides
	virtual EventListenerKeyKnob* Clone() override;
	virtual bool CheckAvailable() override;

public:
	typedef std::function<bool(int, float, CHmsEvent*)> HmsKeyKnobCallback;

	HmsKeyKnobCallback	m_callbackKeyKnob;

	EventListenerKeyKnob();
	bool init();

private:

	friend class CHmsEventDispatcher;
};

NS_HMS_END

#endif 
