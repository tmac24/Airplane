#include "HmsUiLabelButton.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "Language/HmsLanguageLabel.h"

NS_HMS_BEGIN

#define HMS_UI_BTN_ICON_ID				0x86
#define HMS_UI_BTN_STATE_LABEL_ID		0x87

CHmsUiLabelButton* CHmsUiLabelButton::CreateWithString(const char * strTextUtf8, float fSize, bool bold, const HmsUiBtnCallback& callback /*= nullptr*/)
{
	CHmsUiLabelButton *ret = new CHmsUiLabelButton();
	if (ret && ret->Init())
	{
		auto labelObj = CHmsLanguageLabel::CreateLabel(strTextUtf8, fSize, bold);
		labelObj->SetTextChangeCallback([](CHmsLanguageObject * obj, bool bUpdate){
			auto btn = dynamic_cast<CHmsUiLabelButton*>(obj);
			if (btn)
			{
				btn->UpdateContentSize();
			}
		});
		auto label = labelObj->GetLabel();;
		ret->m_pLabel = label;
		label->SetAnchorPoint(Vec2(0, 0));
		ret->AddChild(label);
		ret->SetContentSize(label->GetContentSize());
		ret->SetCallbackFunc(callback);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

void HmsAviPf::CHmsUiLabelButton::UpdateContentSize()
{
	SetContentSize(m_pLabel->GetContentSize());
}

void HmsAviPf::CHmsUiLabelButton::setTextColor(const HmsAviPf::Color4B &color)
{
	if (m_pLabel)
	{
		m_pLabel->SetTextColor(color);
	}
}

void HmsAviPf::CHmsUiLabelButton::SetText(const char * str)
{
	CHmsLanguageLabel::SetText(m_pLabel, str);
	UpdateContentSize();
}

Size HmsAviPf::CHmsUiLabelButton::GetContentSize() const
{
	return m_pLabel->GetContentSize();
}

CHmsUiLabelButton::CHmsUiLabelButton()
	: CHmsUiButtonAbstract(this)
	, m_pLabel(nullptr)
{

}

CHmsUiLabelButton::~CHmsUiLabelButton()
{
}

NS_HMS_END


