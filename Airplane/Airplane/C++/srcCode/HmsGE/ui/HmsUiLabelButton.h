#pragma once
#include "display/HmsLabel.h"
#include "ui/HmsUiButtonAbstract.h"

NS_HMS_BEGIN

class CHmsUiLabelButton : public CHmsNode, public CHmsUiButtonAbstract
{
public:
	CHmsUiLabelButton();
	~CHmsUiLabelButton();

	static CHmsUiLabelButton* CreateWithString(const char * strTextUtf8, float fSize, bool bold = false, const HmsUiBtnCallback& callback = nullptr);

	virtual Size GetContentSize() const override;

	void SetText(const char * str);

	void setTextColor(const HmsAviPf::Color4B &color);

	void UpdateContentSize();
private:
	CHmsLabel *	m_pLabel;
};

NS_HMS_END