
#include "base/HmsTouch.h"
#include "HmsAviDisplayUnit.h"

NS_HMS_BEGIN

// returns the current touch location in screen coordinates
Vec2 Touch::GetLocationInView() const 
{ 
    return m_point; 
}

// returns the previous touch location in screen coordinates
Vec2 Touch::GetPreviousLocationInView() const 
{ 
    return m_pointPrev; 
}

// returns the start touch location in screen coordinates
Vec2 Touch::GetStartLocationInView() const 
{ 
    return m_pointStart; 
}

// returns the current touch location in OpenGL coordinates
Vec2 Touch::GetLocation() const
{ 
	return CHmsAviDisplayUnit::GetInstance()->ConvertToGL(m_point);
}

// returns the previous touch location in OpenGL coordinates
Vec2 Touch::GetPreviousLocation() const
{ 
	return CHmsAviDisplayUnit::GetInstance()->ConvertToGL(m_pointPrev);
}

// returns the start touch location in OpenGL coordinates
Vec2 Touch::GetStartLocation() const
{ 
	return CHmsAviDisplayUnit::GetInstance()->ConvertToGL(m_pointStart);
}

// returns the delta position between the current location and the previous location in OpenGL coordinates
Vec2 Touch::GetDelta() const
{     
    return GetLocation() - GetPreviousLocation();
}

NS_HMS_END
