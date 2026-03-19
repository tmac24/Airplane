#include "HmsButton.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "HmsImageNode.h"

NS_HMS_BEGIN

CHmsButton::CHmsButton()
{

}

CHmsButton::~CHmsButton()
{
}

CHmsButton* CHmsButton::Create()
{
	CHmsButton *ret = new CHmsButton();
	if (ret && ret->Init())
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsButton* CHmsButton::Create(const std::string &normalImage, const std::string &selectedImage)
{
	return Create(normalImage, selectedImage, "", (const onClickCallback&)nullptr);
}

CHmsButton* CHmsButton::Create(const std::string &normalImage, const std::string &selectedImage, const onClickCallback& callback)
{
	return Create(normalImage, selectedImage, "", (const onClickCallback&)nullptr);
}

CHmsButton* CHmsButton::Create(const std::string &normalImage, const std::string &selectedImage, const std::string &disableImage, const onClickCallback& callback)
{
	CHmsButton *ret = new STD_NOTHROW CHmsButton();
	if (ret && ret->InitWithNormalImage(normalImage, selectedImage, disableImage, callback))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsButton::Init()
{
	return InitWithNormalImage("", "", "", (const onClickCallback&)nullptr);
}


void CHmsButton::SetIcon(CHmsImageNode *icon)
{
	if (icon != m_pNodeIcon)
	{
		if (icon)
		{
			this->AddChild(icon);
		}

		if (m_pNodeIcon)
		{
			this->RemoveChild(m_pNodeIcon, true);
		}

		m_pNodeIcon = icon;

		if (!m_enabled) m_pNodeIcon->SetColor(Color3B::GRAY);
	}

	ContentLayout();
}

void CHmsButton::SetIcon(const std::string iconPath, const Size &iconSize /*= Size(16, 16)*/)
{
	auto icon = CHmsImageNode::Create(iconPath);
	if (icon)
	{
		float width = icon->GetContentSize().width;
		float height = icon->GetContentSize().height;
		icon->SetScale(iconSize.width / width, iconSize.height / height);
		auto size = icon->GetContentSize();
		//icon->SetContentSize(iconSize);
		SetIcon(icon);
	}
}

void CHmsButton::Selected()
{
	m_selected = true;

	SetPressed(true);
}

void CHmsButton::UnSelected()
{
	m_selected = false;

	SetPressed(false);
}

bool CHmsButton::IsSelected()
{
	return m_selected;
}

void CHmsButton::SetPressed(bool selected)
{
	if (!selected)
	{
		UpdateImagesVisibility();
	}
	else
	{
		if (m_pImageNormal)
		{
			if (m_pImageDisable)
			{
				m_pImageDisable->SetVisible(false);
			}

			if (m_pImageSelected)
			{
				m_pImageNormal->SetVisible(false);
				m_pImageSelected->SetVisible(true);
			}
			else
			{
				m_pImageNormal->SetVisible(true);
			}
		}
	}
	
}

void CHmsButton::SetSize(const Size &size)
{
	CHmsWidget::SetSize(size);
	if (m_pImageNormal)
	{
		m_pImageNormal->SetContentSize(size);
	}
	if (m_pImageSelected)
	{
		m_pImageSelected->SetContentSize(size);
	}
	if (m_pImageDisable)
	{
		m_pImageDisable->SetContentSize(size);
	}

	ContentLayout();
}

void CHmsButton::SetText(const std::string text, const float &size /*= 12.0f*/, const Color4B &color /*= Color4B::BLACK*/)
{
	auto label = CHmsLabel::createWithTTF(text, "fonts/msyh.ttc", size);
	if (label)
	{
		label->SetTextColor(color);
		SetTextLabel(label);
	}
}

std::string CHmsButton::GetText()
{
	return m_pLabelText->GetString();
}

void CHmsButton::SetTextColor(const Color4B &color)
{
	m_textColor = color;

	m_pLabelText->SetTextColor(color);
}

void CHmsButton::SetTextLabel(CHmsLabel *label)
{
	if (label != m_pLabelText)
	{
		if (label)
		{
			this->AddChild(label);
		}

		if (m_pLabelText)
		{
			this->RemoveChild(m_pLabelText, true);
		}

		m_pLabelText = label;

		m_textColor = Color4B(label->GetColor());

		if (!m_enabled) m_pLabelText->SetTextColor(Color4B::GRAY);

	}

	ContentLayout();
}

void CHmsButton::SetNormalImage(CHmsStretchImageNode* image)
{
	if (image != m_pImageNormal)
	{
		if (image)
		{
			AddChild(image);
			image->SetAnchorPoint(Vec2(0, 0));
		}

		if (m_pImageNormal)
		{
			RemoveChild(m_pImageNormal, true);
		}

		m_pImageNormal = image;
		if (m_pImageNormal != nullptr)
		{
			this->SetSize(m_pImageNormal->GetContentSize());
		}
		this->UpdateImagesVisibility();
	}
}

void CHmsButton::SetSelectedImage(CHmsStretchImageNode* image)
{
	if (image != m_pImageNormal)
	{
		if (image)
		{
			AddChild(image);
			image->SetAnchorPoint(Vec2(0, 0));
		}

		if (m_pImageSelected)
		{
			RemoveChild(m_pImageSelected, true);
		}

		m_pImageSelected = image;
		this->UpdateImagesVisibility();
	}
}

void CHmsButton::SetDisabledImage(CHmsStretchImageNode* image)
{
	if (image != m_pImageNormal)
	{
		if (image)
		{
			AddChild(image);
			image->SetAnchorPoint(Vec2(0, 0));
		}

		if (m_pImageDisable)
		{
			RemoveChild(m_pImageDisable, true);
		}

		m_pImageDisable = image;
		this->UpdateImagesVisibility();
	}
}


bool CHmsButton::InitWithNormalImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const onClickCallback& callback)
{
	if (!CHmsWidget::Init())
	{
		return false;
	}

	CHmsStretchImageNode *normalSprite = nullptr;
	CHmsStretchImageNode *selectedSprite = nullptr;
	CHmsStretchImageNode *disabledSprite = nullptr;

	if (normalImage.size() > 0)
	{
		normalSprite = CHmsStretchImageNode::Create(normalImage);
	}

	if (selectedImage.size() > 0)
	{
		selectedSprite = CHmsStretchImageNode::Create(selectedImage);
	}

	if (disabledImage.size() > 0)
	{
		disabledSprite = CHmsStretchImageNode::Create(disabledImage);
	}

	InitWithCallback(callback);
	if (normalSprite)
	{
		SetNormalImage(normalSprite);
	}
	if (selectedSprite)
	{
		SetSelectedImage(selectedSprite);
	}
	if (disabledSprite)
	{
		SetDisabledImage(disabledSprite);
	}

	if (m_pImageNormal)
	{
		this->SetSize(m_pImageNormal->GetContentSize());
	}

	return true;
}

bool CHmsButton::InitWithCallback(const onClickCallback& callback)
{
	SetAnchorPoint(Vec2(0.5f, 0.5f));
	onClicked = callback;
	m_enabled = true;
	m_selected = false;
	return true;
}

bool CHmsButton::OnPressed(const Vec2 & posParent)
{
	if (m_enabled)
	{
		if (!m_selected)
		{
			SetPressed(true);
		}
	}

	return true;
}

void CHmsButton::OnReleased(const Vec2 & posParent)
{
	if (m_enabled)
	{
		if (!m_selected)
		{
			SetPressed(false);
		}

		if (onClicked)
		{
			onClicked(this);
		}
	}
}

void CHmsButton::UpdateImagesVisibility()
{
	if (m_enabled)
	{
		if (m_pImageNormal)   m_pImageNormal->SetVisible(true);
		if (m_pImageSelected) m_pImageSelected->SetVisible(false);
		if (m_pImageDisable) m_pImageDisable->SetVisible(false);
	}
	else
	{
		if (m_selected && m_pImageSelected)
		{
			if (m_pImageNormal) m_pImageNormal->SetVisible(false);
			if (m_pImageSelected) m_pImageSelected->SetVisible(true);
			if (m_pImageDisable) m_pImageDisable->SetVisible(false);
		}
		else if (m_pImageDisable)
		{
			if (m_pImageNormal)   m_pImageNormal->SetVisible(false);
			if (m_pImageSelected) m_pImageSelected->SetVisible(false);
			if (m_pImageDisable) m_pImageDisable->SetVisible(true);
		}
		else
		{
			if (m_pImageNormal) m_pImageNormal->SetVisible(true);
			if (m_pImageSelected) m_pImageSelected->SetVisible(false);
			if (m_pImageDisable) m_pImageDisable->SetVisible(false);
		}
	}
}

void CHmsButton::UpdateContentColor()
{
	if (m_enabled)
	{
		if (m_pImageNormal)   m_pImageNormal->SetColor(Color3B::WHITE);
		if (m_pImageSelected) m_pImageSelected->SetColor(Color3B::WHITE);
		if (m_pImageDisable) m_pImageDisable->SetColor(Color3B::WHITE);
		if (m_pNodeIcon) m_pNodeIcon->SetColor(Color3B::WHITE);
		if (m_pLabelText) m_pLabelText->SetTextColor(Color4B(m_textColor));
	}
	else
	{
		if (m_pImageNormal)   m_pImageNormal->SetColor(Color3B::GRAY);
		if (m_pImageSelected) m_pImageSelected->SetColor(Color3B::GRAY);
		if (m_pImageDisable) m_pImageDisable->SetColor(Color3B::GRAY);
		if (m_pNodeIcon) m_pNodeIcon->SetColor(Color3B::GRAY);
		if (m_pLabelText) m_pLabelText->SetTextColor(Color4B::GRAY);
	}
}

void CHmsButton::ContentLayout()
{
	auto height = this->GetContentSize().height;
	auto width = this->GetContentSize().width;
	if (m_pNodeIcon != nullptr)
	{
		m_pNodeIcon->SetAnchorPoint(Vec2(0, 0.5));
		m_pNodeIcon->SetPosition(6, height / 2.0);
	}

	if (m_pLabelText != nullptr)
	{
		if (m_pNodeIcon == nullptr)
		{
			m_pLabelText->SetAnchorPoint(0.5, 0.5);
			m_pLabelText->SetPosition(width / 2.0, height / 2.0);
		}
		else
		{
			m_pLabelText->SetAnchorPoint(0, 0.5);
			m_pLabelText->SetPosition(6 + m_pNodeIcon->GetContentSize().width * m_pNodeIcon->GetScaleX() + 5, height / 2.0);
		}
	}
}

void CHmsButton::EnableChanged()
{
	UpdateContentColor();
	UpdateImagesVisibility();
}

NS_HMS_END