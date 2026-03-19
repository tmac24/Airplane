#pragma once
#include "display/HmsStretchImageNode.h"
#include "ui/HmsUiButtonAbstract.h"

NS_HMS_BEGIN

class CHmsUiStretchButton : public CHmsStretchImageNode, public CHmsUiButtonAbstract
{
public:
	CHmsUiStretchButton();
	~CHmsUiStretchButton();

	static CHmsUiStretchButton* Create(const char * strImageStretch, const Size & size,
		const Color3B & colorNormal = Color3B::WHITE, const Color3B & colorSelected = Color3B::WHITE, const Color3B & colorDisable = Color3B::WHITE,
		const HmsUiBtnCallback& callback = nullptr);

protected:
	virtual bool Init() override;

	virtual void SetState(HMS_UI_BUTTON_STATE eState) override;

	Color3B							m_colorNormal;
	Color3B							m_colorSelected;
	Color3B							m_colorDisable;
};

NS_HMS_END