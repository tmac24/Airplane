#pragma once
#include "base/HmsNode.h"
#include "ui/HmsUiButtonAbstract.h"

class CHmsToolBarFuncBtn : public HmsAviPf::CHmsNode, public HmsAviPf::CHmsUiButtonAbstract
{
public:
	CHmsToolBarFuncBtn();
	~CHmsToolBarFuncBtn();

	static CHmsToolBarFuncBtn* CreateWithImage(const char * strImagePath, const char * strTitle);
	static HmsAviPf::Size GetDesignBtnSize(){ return s_sizeBtnDesign; }
	static void SetDesignSize(const HmsAviPf::Size & size){ s_sizeBtnDesign = size; }

	virtual bool Init(const char * strImagePath, const char * strTitle);

	virtual void SetState(HmsAviPf::HMS_UI_BUTTON_STATE eState) override;

protected:
	void SetButtonColor(const HmsAviPf::Color3B & color);

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsStretchImageNode>			m_bgNode;
	static HmsAviPf::Size s_sizeBtnDesign;
};