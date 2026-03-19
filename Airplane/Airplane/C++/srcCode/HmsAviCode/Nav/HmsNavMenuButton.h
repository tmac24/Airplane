#pragma once
#include "base/HmsNode.h"
#include "ui/HmsUiButtonAbstract.h"

class CHmsNavMenuButton : public HmsAviPf::CHmsNode, public HmsAviPf::CHmsUiButtonAbstract
{
public:
	CHmsNavMenuButton();
	~CHmsNavMenuButton();

	static CHmsNavMenuButton* CreateWithImage(const char * strImagePath, const char * strTitle);

	virtual bool Init(const char * strImagePath, const char * strTitle);

	virtual void SetState(HmsAviPf::HMS_UI_BUTTON_STATE eState);

protected:
	void SetButtonColor(const HmsAviPf::Color3B & color);

};