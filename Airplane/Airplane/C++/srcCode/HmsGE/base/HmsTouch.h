
#ifndef __HMS_TOUCH_H__
#define __HMS_TOUCH_H__

#include "base/Ref.h"
#include "math/HmsGeometry.h"

NS_HMS_BEGIN


/** @class Touch
 * @brief Encapsulates the Touch information, such as touch point, id and so on,
 and provides the methods that commonly used.
 */
class HMS_DLL Touch : public Ref
{
public:
    /** 
     * Dispatch mode, how the touches are dispatched.
     */
    enum class DispatchMode {
        ALL_AT_ONCE, /** All at once. */
        ONE_BY_ONE,  /** One by one. */
    };

    /** Constructor.
     */
    Touch() 
        : m_nId(0),
        m_bStartPointCaptured(false)
    {}

	Touch(int id)
		: m_nId(id),
		m_bStartPointCaptured(false)
	{
	}

    /** Returns the current touch location in OpenGL coordinates.
     *
     * @return The current touch location in OpenGL coordinates.
     */
    Vec2 GetLocation() const;
    /** Returns the previous touch location in OpenGL coordinates.
     *
     * @return The previous touch location in OpenGL coordinates.
     */
    Vec2 GetPreviousLocation() const;
    /** Returns the start touch location in OpenGL coordinates.
     *
     * @return The start touch location in OpenGL coordinates.
     */
    Vec2 GetStartLocation() const;
    /** Returns the delta of 2 current touches locations in screen coordinates.
     *
     * @return The delta of 2 current touches locations in screen coordinates.
     */
    Vec2 GetDelta() const;
    /** Returns the current touch location in screen coordinates.
     *
     * @return The current touch location in screen coordinates.
     */
    Vec2 GetLocationInView() const;
    /** Returns the previous touch location in screen coordinates. 
     *
     * @return The previous touch location in screen coordinates.
     */
    Vec2 GetPreviousLocationInView() const;
    /** Returns the start touch location in screen coordinates.
     *
     * @return The start touch location in screen coordinates.
     */
    Vec2 GetStartLocationInView() const;
    
    /** Set the touch information. It always used to monitor touch event.
     *
     * @param id A given id
     * @param x A given x coordinate.
     * @param y A given y coordinate.
     */
    void SetTouchInfo(int id, float x, float y)
    {
        m_nId = id;
        m_pointPrev = m_point;
        m_point.x   = x;
        m_point.y   = y;
        if (!m_bStartPointCaptured)
        {
            m_pointStart = m_point;
            m_bStartPointCaptured = true;
            m_pointPrev = m_point;
        }
    }

	void SetTouchInfo(float x, float y)
	{
		m_pointPrev = m_point;
		m_point.x = x;
		m_point.y = y;
		if (!m_bStartPointCaptured)
		{
			m_pointStart = m_point;
			m_bStartPointCaptured = true;
			m_pointPrev = m_point;
		}
	}

	void ReSetStartCapture(){ m_bStartPointCaptured = false; }

	void SetTouchID(int id)
	{
		m_nId = id;
	}

    /** Get touch id.
     * @return The id of touch.
     */
    int GetID() const
    {
        return m_nId;
    }

private:
    int			m_nId;
    bool		m_bStartPointCaptured;
    Vec2		m_pointStart;
    Vec2		m_point;
    Vec2		m_pointPrev;
};

NS_HMS_END

#endif  // __PLATFORM_TOUCH_H__
