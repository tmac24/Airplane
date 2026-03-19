#ifndef __HMS__TouchEvent__
#define __HMS__TouchEvent__

#include "base/HmsEvent.h"
#include <vector>


NS_HMS_BEGIN

class Touch;

#define TOUCH_PERF_DEBUG 1

/** @class EventTouch
 * @brief Touch event.
 */
class HMS_DLL EventTouch : public CHmsEvent
{
public:
    static const int MAX_TOUCHES = 15;
    
    /** EventCode Touch event code.*/
    enum class EventCode
    {
        BEGAN,
        MOVED,
        ENDED,
        CANCELLED
    };

    /** 
     * Constructor.
     */
    EventTouch();

    /** Get event code.
     *
     * @return The code of the event.
     */
    inline EventCode getEventCode() const { return _eventCode; };
    
    /** Get the touches.
     *
     * @return The touches of the event.
     */
    inline const std::vector<Touch*>& getTouches() const { return _touches; };

#if TOUCH_PERF_DEBUG
    /** Set the event code.
     * 
     * @param eventCode A given EventCode.
     */
    void setEventCode(EventCode eventCode) { _eventCode = eventCode; };
    /** Set the touches
     *
     * @param touches A given touches vector.
     */
    void setTouches(const std::vector<Touch*>& touches) { _touches = touches; };
#endif
    
public:
    EventCode _eventCode;
    std::vector<Touch*> _touches;
};


NS_HMS_END


#endif
