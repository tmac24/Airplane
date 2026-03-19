#include "HmsUiStretchButton.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"

NS_HMS_BEGIN

#define HMS_UI_BTN_ICON_ID				0x86
#define HMS_UI_BTN_STATE_LABEL_ID		0x87

CHmsUiStretchButton* CHmsUiStretchButton::Create(const char * strImageStretch, const Size & size, 
							const Color3B & colorNormal, const Color3B & colorSelected, const Color3B & colorDisable, 
							const HmsUiBtnCallback& callback /*= nullptr*/)
{
	CHmsUiStretchButton *ret = new CHmsUiStretchButton();
	if (ret && ret->InitWithFile(strImageStretch))
	{
		ret->SetContentSize(size);
		ret->SetCallbackFunc(callback);
		ret->m_colorNormal = colorNormal;
		ret->m_colorSelected = colorSelected;
		ret->m_colorDisable = colorDisable;
		ret->SetColor(ret->m_colorNormal);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsUiStretchButton::CHmsUiStretchButton()
	: CHmsUiButtonAbstract(this)
{

}

CHmsUiStretchButton::~CHmsUiStretchButton()
{
}

bool CHmsUiStretchButton::Init()
{
	if (!CHmsStretchImageNode::Init())
	{
		return false;
	}
	return true;
}

void CHmsUiStretchButton::SetState(HMS_UI_BUTTON_STATE eState)
{
	if (m_eBtnState != eState)
	{
		switch (eState)
		{
		case HMS_UI_BUTTON_STATE::btn_normal:
			this->SetColor(m_colorNormal);
			break;
		case HMS_UI_BUTTON_STATE::btn_selected:
			this->SetColor(m_colorSelected);
			break;
		case HMS_UI_BUTTON_STATE::btn_disable:
		default:
			this->SetColor(m_colorDisable);
			break;
		}

		m_eBtnState = eState;
		DispatchCallbackFunc();
	}
}

NS_HMS_END


