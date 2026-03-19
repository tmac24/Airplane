#pragma once
#include "base/HmsNode.h"
#include "ui/HmsUiButtonAbstract.h"

class CHmsToolBarFlowBtn : public HmsAviPf::CHmsNode, public HmsAviPf::CHmsUiButtonAbstract
{
public:
	CHmsToolBarFlowBtn();
	~CHmsToolBarFlowBtn();

	static CHmsToolBarFlowBtn* CreateWithImage(const char * strImagePath, const char * strTitle);
	static HmsAviPf::Size GetDesignBtnSize(){ return s_sizeBtnDesign; }
	static void SetDesignSize(const HmsAviPf::Size & size){ s_sizeBtnDesign = size; }

	virtual bool Init(const char * strImagePath, const char * strTitle);

	virtual void SetState(HmsAviPf::HMS_UI_BUTTON_STATE eState) override;

	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;

	virtual void Selected() override;

	void SetClick2Normal(bool bOn){ m_bClick2Normal = bOn; }

protected:
	void SetButtonColor(const HmsAviPf::Color3B & color);

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsStretchImageNode>			m_bgNode;
	bool														m_bClick2Normal;

	static HmsAviPf::Size s_sizeBtnDesign;
	
};