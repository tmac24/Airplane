#include "HmsKeyItem.h"


CHmsKeyItem::CHmsKeyItem()
: HmsUiEventInterface(this)
, m_funcCallback(nullptr)
{
}


CHmsKeyItem::~CHmsKeyItem()
{
}

CHmsKeyItem* CHmsKeyItem::CreateWithImage(const char * strImageNormal, const char * strImageSelected, 
	Size btnSize, const HmsOnKeyPress& callback)
{
	Texture2D * textureNormal = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(strImageNormal);
	Texture2D * textureSelected = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(strImageSelected);
	CHmsKeyItem *ret = new (std::nothrow) CHmsKeyItem();
	if (ret && ret->InitWithTexture(textureNormal, textureSelected, btnSize))
	{
		ret->SetCallbackFunc(callback);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsKeyItem::Init()
{
	if (!CHmsStretchImageNode::Init())
	{
		return false;
	}
	return true;
}

bool CHmsKeyItem::InitWithTexture(Texture2D * textureNormal, Texture2D * textureSelected, Size btnSize)
{
	bool bRet = false;

	m_itemSize = btnSize;
	if (textureNormal && textureSelected)
	{
		m_textureNormal = textureNormal;
		m_textureSelected = textureSelected;

		Rect rect = Rect::ZERO;
		if (m_itemSize.width == 0 && m_itemSize.height == 0)
		{
			m_itemSize = textureNormal->getContentSize();
		}
		rect.size = m_itemSize;
		bRet = CHmsStretchImageNode::InitWithTexture(textureNormal, rect);
	}

	return bRet;
}

void CHmsKeyItem::SetText(const char * strText, float size /*= 12.0f*/, const Color4B &color /*= Color4B::BLACK*/, bool bBorder /*= false*/)
{
	if (!m_pTextLabel)
	{
		auto fontPath = "fonts/msyh.ttc";
		if (bBorder)
		{
			fontPath = "fonts/msyhbd.ttc";
		}
		m_pTextLabel = CHmsLabel::createWithTTF(strText, fontPath, size);

		m_pTextLabel->SetAnchorPoint(0.5, 0.5);
		m_pTextLabel->SetPosition(m_itemSize.width / 2, m_itemSize.height / 2.0);
		this->AddChild(m_pTextLabel);
	}
	m_pTextLabel->SetTextColor(color);
	m_pTextLabel->SetString(strText);
	m_pTextLabel->UpdateTextString();
}

void CHmsKeyItem::SetTextColor(Color4B color)
{
	if (m_pTextLabel)
	{
		m_pTextLabel->SetTextColor(color);
	}
}

bool CHmsKeyItem::OnPressed(const Vec2 & posOrigin)
{
	this->SetTexture(m_textureSelected);
	this->SetContentSize(m_itemSize);
	this->SetScale(1.05F);

	return true;
}

void CHmsKeyItem::OnMove(const Vec2 & posOrigin)
{

}

void CHmsKeyItem::OnReleased(const Vec2 & posOrigin)
{
	this->SetTexture(m_textureNormal);
	this->SetContentSize(m_itemSize);
	this->SetScale(1.0);

	if (m_funcCallback)
	{
		m_funcCallback(m_pressChar);
	}
}


