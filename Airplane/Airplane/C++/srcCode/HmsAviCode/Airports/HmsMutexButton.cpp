#include "HmsMutexButton.h"


CHmsMutexButton::CHmsMutexButton()
: HmsUiEventInterface(this)
, m_funcCallback(nullptr)
, m_bIsSelected(false)
, m_bAutoNormal(false)
{
}


CHmsMutexButton::~CHmsMutexButton()
{
}

CHmsMutexButton* CHmsMutexButton::CreateWithImage(const char * strImageNormal, const char * strImageSelected,
	const HmsOnButtonSelect& callback)
{
	Texture2D * textureNormal = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(strImageNormal);
	Texture2D * textureSelected = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(strImageSelected);

	if (textureNormal && textureSelected)
	{
		CHmsMutexButton *ret = new (std::nothrow) CHmsMutexButton();
		if (ret && ret->InitWithTexture(textureNormal, textureSelected))
		{
			ret->SetOnBtnSelectFunc(callback);
			ret->autorelease();
			return ret;
		}
		HMS_SAFE_DELETE(ret);
	}
	
	return nullptr;
}

bool CHmsMutexButton::Init()
{
	if (!CHmsStretchImageNode::Init())
	{
		return false;
	}
	return true;
}

bool CHmsMutexButton::InitWithTexture(Texture2D * textureNormal, Texture2D * textureSelected)
{
	bool bRet = false;
	if (textureNormal && textureSelected)
	{
		m_textureNormal = textureNormal;
		m_textureSelected = textureSelected;

		Rect rect = Rect::ZERO;
		rect.size = textureNormal->getContentSize();;
		bRet = CHmsStretchImageNode::InitWithTexture(textureNormal, rect);
	}
	return bRet;
}

void CHmsMutexButton::SetText(const char * strText, float size /*= 12.0f*/, const Color4B &color /*= Color4B::BLACK*/, bool bBorder /*= false*/)
{
	if (m_pTextLabel)
	{
		m_pTextLabel->RemoveFromParent();
		m_pTextLabel = nullptr;
	}
// 	auto fontPath = "fonts/msyh.ttc";
// 	if (bBorder)
// 	{
// 		fontPath = "fonts/msyhbd.ttc";
// 	}
    m_pTextLabel = CHmsGlobal::CreateLanguageLabel(strText, size, bBorder);
	if (m_pTextLabel.get())
	{
		m_pTextLabel->SetAnchorPoint(0.5, 0.5);
		Size btnSize = this->GetContentSize();
		m_pTextLabel->SetPosition(btnSize.width / 2, btnSize.height / 2.0);
		this->AddChild(m_pTextLabel);

		m_pTextLabel->SetTextColor(color);
	}	
//	m_pTextLabel->SetString(strText);
}

void CHmsMutexButton::SetTextColor(Color4B color)
{
	if (m_pTextLabel)
	{
		m_pTextLabel->SetTextColor(color);
	}
}

bool CHmsMutexButton::OnPressed(const Vec2 & posOrigin)
{
	if (!m_bIsSelected)
	{
		SetSelect();
	}
	return true;
}

void CHmsMutexButton::OnMove(const Vec2 & posOrigin)
{

}

void CHmsMutexButton::OnReleased(const Vec2 & posOrigin)
{
	if (m_bAutoNormal)
	{
		SetUnSelect();
	}
}

void CHmsMutexButton::SetSelect()
{
	if (m_textureSelected.get())
	{
		this->SetTexture(m_textureSelected);
		m_bIsSelected = true;
		if (m_funcCallback)
		{
			m_funcCallback(this);
		}
	}	
}

void CHmsMutexButton::SetUnSelect()
{
	if (m_textureNormal)
	{
		this->SetTexture(m_textureNormal);
		m_bIsSelected = false;
	}	
}

bool CHmsMutexButton::IsSelected()
{
	return m_bIsSelected;
}

void CHmsMutexButton::SetAutoNormal(bool b)
{
	m_bAutoNormal = b;
}


