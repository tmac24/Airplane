#pragma once
#include "HmsUiButton.h"
#include "base/RefPtr.h"
#include "../display/HmsImageNode.h"

NS_HMS_BEGIN

class CHmsStretchImageNode;

class CHmsUiRadio : public CHmsUiButton
{
public:
	enum TextAlign{ALIGN_CENTER, ALIGN_RIGHT};

	CHmsUiRadio();
	~CHmsUiRadio();

	static CHmsUiRadio* CreateWithImage(const char * normalImage,
		const char * selectedImage,
		const char * disableImage = nullptr);
	
	static CHmsUiRadio * CreateWithStretchImage(const HmsAviPf::Size & size, const char * normalImage, const char * selectedImage, const char * disableImage = nullptr);

	virtual bool Init(CHmsNode * normal, CHmsNode * select, CHmsNode * disable);

	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;

	virtual void AddText(const char * strText, float size = 12.0f, const HmsAviPf::Color4B &color = HmsAviPf::Color4B::WHITE, bool bBorder = false) override;

	void SetTextAlign(TextAlign align);

	void SetNormalTextColor(HmsAviPf::Color4B color);

	void SetSelectedTextColor(HmsAviPf::Color4B color);

	void SetDisableTextColor(HmsAviPf::Color4B color);

	virtual void Selected() override
	{
		SetState(HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected);
	}

	virtual void UnSelected() override
	{
		SetState(HmsAviPf::HMS_UI_BUTTON_STATE::btn_normal);
	}
protected:
	virtual void SetState(HmsAviPf::HMS_UI_BUTTON_STATE eState) override;
	TextAlign m_textAlign;
	HmsAviPf::Color4B m_colorNormal;
	HmsAviPf::Color4B m_colorSelected;
	HmsAviPf::Color4B m_colorDisable;
};

NS_HMS_END