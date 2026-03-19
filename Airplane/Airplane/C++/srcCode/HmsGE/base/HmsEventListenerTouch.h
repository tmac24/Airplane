#ifndef __HMS_TouchEventListener1__
#define __HMS_TouchEventListener1__

#include "base/HmsEventListener.h"
#include <vector>


NS_HMS_BEGIN

class HMS_DLL Touch;

/** @class EventListenerTouchOneByOne
 * @brief Single touch event listener.
 */
class HMS_DLL EventListenerTouchOneByOne : public CHmsEventListener
{
public:
    static const std::string LISTENER_ID;
    
    /** Create a one by one touch event listener.
     */
    static EventListenerTouchOneByOne* create();
    
    /**
     * Destructor.
     */
    virtual ~EventListenerTouchOneByOne();
    
    /** Whether or not to swall touches.
     *
     * @param needSwallow True if needs to swall touches.
     */
    void setSwallowTouches(bool needSwallow);

    /** Is swall touches or not.
     *
     * @return True if needs to swall touches.
     */
    bool isSwallowTouches();
    
    /// Overrides
    virtual EventListenerTouchOneByOne* Clone() override;
    virtual bool CheckAvailable() override;
    //

public:

    typedef std::function<bool(Touch*, CHmsEvent*)> ccTouchBeganCallback;
	typedef std::function<void(Touch*, CHmsEvent*)> ccTouchCallback;

    ccTouchBeganCallback onTouchBegan;
    ccTouchCallback onTouchMoved;
    ccTouchCallback onTouchEnded;
    ccTouchCallback onTouchCancelled;
    
    EventListenerTouchOneByOne();
    bool init();
    
private:
    std::vector<Touch*> _claimedTouches;
    bool _needSwallow;
    
	friend class CHmsEventDispatcher;
};

/** @class EventListenerTouchAllAtOnce
 * @brief Multiple touches event listener.
 */
class HMS_DLL EventListenerTouchAllAtOnce : public CHmsEventListener
{
public:
    static const std::string LISTENER_ID;
    
    /** Create a all at once event listener.
     *
     * @return An autoreleased EventListenerTouchAllAtOnce object.
     */
    static EventListenerTouchAllAtOnce* create();
    /** Destructor.
     * @js NA
     */
    virtual ~EventListenerTouchAllAtOnce();
    
    /// Overrides
    virtual EventListenerTouchAllAtOnce* Clone() override;
    virtual bool CheckAvailable() override;
    //
public:

	typedef std::function<void(const std::vector<Touch*>&, CHmsEvent*)> ccTouchesCallback;

    ccTouchesCallback onTouchesBegan;
    ccTouchesCallback onTouchesMoved;
    ccTouchesCallback onTouchesEnded;
    ccTouchesCallback onTouchesCancelled;
    
    EventListenerTouchAllAtOnce();
    bool init();
private:
    
	friend class CHmsEventDispatcher;
};

NS_HMS_END

#endif 
