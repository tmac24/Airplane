#pragma once
#include "base/HmsNode.h"
#include "ui/HmsUiButtonAbstract.h"

class CHmsToolBarModeBtn : public HmsAviPf::CHmsNode, public HmsAviPf::CHmsUiButtonAbstract
{
public:
	CHmsToolBarModeBtn();
	~CHmsToolBarModeBtn();

	static CHmsToolBarModeBtn* CreateWithImage(const char * strImagePath, const char * strTitle);
	static HmsAviPf::Size GetDesignBtnSize(){ return s_sizeBtnDesign; }
	static void SetDesignSize(const HmsAviPf::Size & size){ s_sizeBtnDesign = size; }

	virtual bool Init(const char * strImagePath, const char * strTitle);

	virtual void SetState(HmsAviPf::HMS_UI_BUTTON_STATE eState) override;

	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;

	void Selected() override;

protected:
	void SetButtonColor(const HmsAviPf::Color3B & color);

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsStretchImageNode>			m_bgNode;
	static HmsAviPf::Size s_sizeBtnDesign;
};