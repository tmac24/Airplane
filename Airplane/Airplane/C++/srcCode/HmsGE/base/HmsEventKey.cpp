

#include "base/HmsEventKey.h"

NS_HMS_BEGIN




EventKeyButton::EventKeyButton()
	: CHmsEvent(Type::KEYBTN)
	, m_nBtnID(0)
	, m_nBtnState(0)
{

}

EventKeyButton::EventKeyButton(int nBtnID, int nBtnState)
	: CHmsEvent(Type::KEYBTN)
	, m_nBtnID(nBtnID)
	, m_nBtnState(nBtnState)
{

}

void EventKeyButton::SetData(int nBtnID, int nBtnState)
{
	m_nBtnID = nBtnID;
	m_nBtnState = nBtnState;
}


EventKeyKnob::EventKeyKnob()
	: CHmsEvent(Type::KEYKNOB)
	, m_nKnobID(0)
	, m_fRotateRatio(0.0f)
{

}

EventKeyKnob::EventKeyKnob(int nKnobID, float fRotateRatio)
	: CHmsEvent(Type::KEYKNOB)
	, m_nKnobID(nKnobID)
	, m_fRotateRatio(fRotateRatio)
{

}

void EventKeyKnob::SetData(int nKnobID, float fRotateRatio)
{
	m_nKnobID = nKnobID;
	m_fRotateRatio = fRotateRatio;
}

NS_HMS_END
