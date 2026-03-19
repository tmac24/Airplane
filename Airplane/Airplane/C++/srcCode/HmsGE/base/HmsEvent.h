#pragma once

#include "HmsAviMacros.h"
#include "base/Ref.h"

NS_HMS_BEGIN

class CHmsNode;

class HMS_DLL CHmsEvent : public Ref
{
public:
	enum class Type
	{
		TOUCH,
		KEYBOARD,
		MOUSE,
		KEYBTN,
		KEYKNOB,
		CUSTOM
	};

	CHmsEvent(Type type);
	virtual ~CHmsEvent();

	inline Type GetType(){ return m_eType; }
	inline void StopPropagation(){ m_bIsStopped = true; }
	inline bool IsStoped(){ return m_bIsStopped; }

	inline CHmsNode * GetCurrentNode(){ return m_pNodeCurrent; }
protected:
	inline void SetCurrentNode(CHmsNode * node){ m_pNodeCurrent = node; }

protected:
	Type		m_eType;
	bool		m_bIsStopped;				//whether the event was stopped
	CHmsNode    * m_pNodeCurrent;			//recode the current node
};
NS_HMS_END
