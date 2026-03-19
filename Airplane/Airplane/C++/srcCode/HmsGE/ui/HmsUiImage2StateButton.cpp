#include "HmsUiImage2StateButton.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"

NS_HMS_BEGIN

#define HMS_UI_BTN_ICON_ID				0x86
#define HMS_UI_BTN_STATE_LABEL_ID		0x87

CHmsUiImage2StateButton* CHmsUiImage2StateButton::CreateWithImage(const char * strImageNormal, const char * strImageSelected, const HmsUiBtnCallback& callback /*= nullptr*/)
{
	Texture2D * textureNormal = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(strImageNormal);
	Texture2D * textureSelected = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(strImageSelected);
	CHmsUiImage2StateButton *ret = new CHmsUiImage2StateButton();
	if (ret && ret->InitWithTexture(textureNormal, textureSelected))
	{
		ret->SetCallbackFunc(callback);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsUiImage2StateButton::CHmsUiImage2StateButton()
	: HmsUiEventInterface(this)
	, m_eBtnState(HMS_UI_BUTTON_STATE::btn_normal)
	, m_bToggleBtn(false)
{

}

CHmsUiImage2StateButton::~CHmsUiImage2StateButton()
{
}

void CHmsUiImage2StateButton::SetCallbackFunc(const HmsUiBtnCallback & callback)
{
	m_funcCallback = callback;
}

bool CHmsUiImage2StateButton::Init()
{
	if (!CHmsImageNode::Init())
	{
		return false;
	}
	return true;
}

bool CHmsUiImage2StateButton::InitWithTexture(Texture2D * textureNormal, Texture2D * textureSelected)
{
	bool bRet = false;
	if (textureNormal && textureSelected)
	{
		m_textureNormal = textureNormal;
		m_textureSelected = textureSelected;

		Rect rect = Rect::ZERO;
		rect.size = textureNormal->getContentSize();
		bRet =  CHmsImageNode::InitWithTexture(textureNormal, rect);
	}

	return bRet;
}

void CHmsUiImage2StateButton::AddText(const char * strText, float size /*= 12.0f*/, const Color4B &color /*= Color4B::BLACK*/, bool bBorder /*= false*/)
{
	auto labelOld = GetLabel();
	if (labelOld)
	{
		labelOld->RemoveFromParent();
	}

	auto labelText = CreateLabel(strText, size, bBorder);
	if (labelText)
	{
		labelText->SetTextColor(color);
		labelText->SetPosition(GetUiCenter());
		labelText->SetLocalZOrder(1);
		labelText->SetTag(HMS_UI_BTN_STATE_LABEL_ID);
		this->AddChild(labelText);
	}
}

void CHmsUiImage2StateButton::AddLabel(CHmsLabel *label)
{
	auto labelOld = GetLabel();
	if (labelOld)
	{
		labelOld->RemoveFromParent();
	}

	if (label)
	{
		label->SetTag(HMS_UI_BTN_STATE_LABEL_ID);
		this->AddChild(label);
	}
}

std::string CHmsUiImage2StateButton::GetText()
{
	auto label = GetLabel();
	if (label)
	{
		return label->GetString();
	}

	return "";
}


void CHmsUiImage2StateButton::SetText(const char * strText)
{
	auto label = GetLabel();
	if (label)
	{
		label->SetString(strText);
	}
}

void CHmsUiImage2StateButton::SetTextColor(const Color4B &color)
{
	auto label = GetLabel();
	if (label)
	{
		label->SetTextColor(color);
	}
}

CHmsLabel * CHmsUiImage2StateButton::GetLabel()
{
	auto label = dynamic_cast<CHmsLabel*>(this->GetChildByTag(HMS_UI_BTN_STATE_LABEL_ID));
	return label;
}

void CHmsUiImage2StateButton::Selected()
{
	if (m_eBtnState != HMS_UI_BUTTON_STATE::btn_selected)
	{
		this->SetTexture(m_textureSelected);
	}
	m_eBtnState = HMS_UI_BUTTON_STATE::btn_selected;
}

void CHmsUiImage2StateButton::UnSelected()
{
	if (m_eBtnState != HMS_UI_BUTTON_STATE::btn_normal)
	{
		this->SetTexture(m_textureNormal);
	}
	m_eBtnState = HMS_UI_BUTTON_STATE::btn_normal;
}

bool CHmsUiImage2StateButton::IsSelected()
{
	return m_eBtnState == HMS_UI_BUTTON_STATE::btn_selected;
}

void CHmsUiImage2StateButton::SetToggleBtn(bool bToggle)
{
	m_bToggleBtn = bToggle;
}

void CHmsUiImage2StateButton::SetSelected(bool bSelected)
{
	if (bSelected)
	{
		SetState(HMS_UI_BUTTON_STATE::btn_selected);
	}
	else
	{
		SetState(HMS_UI_BUTTON_STATE::btn_normal);
	}
}

bool CHmsUiImage2StateButton::OnPressed(const Vec2 & posOrigin)
{
	if (m_bToggleBtn)
	{
		if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			SetState(HMS_UI_BUTTON_STATE::btn_selected);
		}
		else if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_selected)
		{
			SetState(HMS_UI_BUTTON_STATE::btn_normal);
		}
	}
	else
	{
		SetState(HMS_UI_BUTTON_STATE::btn_selected);
	}

	return true;
}

void CHmsUiImage2StateButton::OnMove(const Vec2 & posOrigin)
{
	
}

void CHmsUiImage2StateButton::OnReleased(const Vec2 & posOrigin)
{
	if (!m_bToggleBtn)
	{
		SetState(HMS_UI_BUTTON_STATE::btn_normal);
	}
}

void CHmsUiImage2StateButton::SetState(HMS_UI_BUTTON_STATE eState)
{
	if (m_eBtnState != eState)
	{
		switch (eState)
		{
		case HMS_UI_BUTTON_STATE::btn_normal:
			this->SetTexture(m_textureNormal);
			break;
		case HMS_UI_BUTTON_STATE::btn_selected:
			this->SetTexture(m_textureSelected);
			break;
		default:
			return;
		}

		m_eBtnState = eState;
		if (m_funcCallback)
		{
			m_funcCallback(this, m_eBtnState);
		}
	}
}

void CHmsUiImage2StateButton::EnableUi(bool bEnable)
{
	HmsUiEventInterface::EnableUi(bEnable);

	if (bEnable)
	{
		SetState(HMS_UI_BUTTON_STATE::btn_normal);
	}
	else
	{
		SetState(HMS_UI_BUTTON_STATE::btn_disable);
	}
}



NS_HMS_END


