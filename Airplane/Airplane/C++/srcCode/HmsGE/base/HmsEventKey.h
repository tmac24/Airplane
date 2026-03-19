#ifndef __HMS__KEY_BTN_Event__
#define __HMS__KEY_BTN_Event__

#include "base/HmsEvent.h"
#include <vector>


NS_HMS_BEGIN


class HMS_DLL EventKeyButton : public CHmsEvent
{
public:
    EventKeyButton();
	EventKeyButton(int nBtnID, int nBtnState);

	void SetData(int nBtnID, int nBtnState);

	int m_nBtnID;
	int m_nBtnState;
};

class HMS_DLL EventKeyKnob : public CHmsEvent
{
public:
	EventKeyKnob();
	EventKeyKnob(int nKnobID, float fRotateRatio);

	void SetData(int nKnobID, float fRotateRatio);

	int m_nKnobID;
	float m_fRotateRatio;
};

NS_HMS_END


#endif
