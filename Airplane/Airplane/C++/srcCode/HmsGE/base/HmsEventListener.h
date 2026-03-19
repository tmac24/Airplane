#pragma once

#include "HmsAviMacros.h"
#include <stdint.h>
#include <string>
#include "base/Ref.h"
#include "base/RefPtr.h"
#include "HmsVector.h"
#include "HmsEvent.h"
#include "HmsNode.h"

NS_HMS_BEGIN

class HMS_DLL CHmsEventListener : public Ref
{
public:
	CHmsEventListener();
	virtual ~CHmsEventListener();

	enum class Type
	{
		UNKNOWN,
		TOUCH_ONE_BY_ONE,
		TOUCH_ALL,
		KEYBOARD,
		MOUSE,
		KEY_BTN,
		KEY_KNOB,
		CUSTOM
	};

	virtual bool Init(Type t, const std::function<void(CHmsEvent*)>& callback);

	/** Checks whether the listener is available.
	*
	* @return True if the listener is available.
	*/
	virtual bool CheckAvailable() = 0;

	/** Clones the listener, its subclasses have to override this method.
	*/
	virtual CHmsEventListener * Clone() = 0;

	inline Type GetType() const { return m_eType; };

	void BindNode(CHmsNode * node){ m_nodeBind = node; }
	CHmsNode * GetBindNode(){ return m_nodeBind.get(); }

	void SetEventOrderId(int nId){ m_nOrder = nId; }
	int32_t GetEventOrderId(){ return m_nOrder; }

protected:
	Type									m_eType;
	std::function<void(CHmsEvent*)>			m_callbackOnEvent;
	RefPtr<CHmsNode>						m_nodeBind;
	int32_t									m_nOrder;
};

NS_HMS_END;

