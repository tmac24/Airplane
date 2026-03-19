#include "HmsAbstractButton.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "HmsImageNode.h"

NS_HMS_BEGIN

CHmsAbstractButton::CHmsAbstractButton()
{

}

CHmsAbstractButton::~CHmsAbstractButton()
{
}

CHmsAbstractButton* CHmsAbstractButton::Create()
{
	CHmsAbstractButton *ret = new CHmsAbstractButton();
	if (ret && ret->Init())
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsAbstractButton* CHmsAbstractButton::Create(const std::string &normalImage, const std::string &selectedImage)
{
	return Create(normalImage, selectedImage, "", (const onClickCallback&)nullptr);
}

CHmsAbstractButton* CHmsAbstractButton::Create(const std::string &normalImage, const std::string &selectedImage, const onClickCallback& callback)
{
	return Create(normalImage, selectedImage, "", (const onClickCallback&)nullptr);
}

CHmsAbstractButton* CHmsAbstractButton::Create(const std::string &normalImage, const std::string &selectedImage, const std::string &disableImage, const onClickCallback& callback)
{
	CHmsAbstractButton *ret = new STD_NOTHROW CHmsAbstractButton();
	if (ret && ret->InitWithNormalImage(normalImage, selectedImage, disableImage, callback))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsAbstractButton::Init()
{
	return InitWithNormalImage("", "", "", (const onClickCallback&)nullptr);
}

void CHmsAbstractButton::SetPressed(bool isPressed)
{
	if (!isPressed)
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

void CHmsAbstractButton::SetSize(const Size &size)
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
}

void CHmsAbstractButton::SetNormalImage(CHmsStretchImageNode* image)
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

void CHmsAbstractButton::SetSelectedImage(CHmsStretchImageNode* image)
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

void CHmsAbstractButton::SetDisabledImage(CHmsStretchImageNode* image)
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


bool CHmsAbstractButton::InitWithNormalImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const onClickCallback& callback)
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

bool CHmsAbstractButton::InitWithCallback(const onClickCallback& callback)
{
	SetAnchorPoint(Vec2(0.5f, 0.5f));
	onClicked = callback;
	m_enabled = true;
	return true;
}

bool CHmsAbstractButton::OnPressed(const Vec2 & posParent)
{
	if (m_enabled)
	{
		SetPressed(true);
	}

	return true;
}

void CHmsAbstractButton::OnReleased(const Vec2 & posParent)
{
	if (m_enabled)
	{
		SetPressed(false);

		if (onClicked)
		{
			onClicked(this);
		}
	}
}

void CHmsAbstractButton::UpdateImagesVisibility()
{
	if (m_enabled)
	{
		if (m_pImageNormal)   m_pImageNormal->SetVisible(true);
		if (m_pImageSelected) m_pImageSelected->SetVisible(false);
		if (m_pImageDisable) m_pImageDisable->SetVisible(false);
	}
	else
	{
		if (m_pImageDisable)
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

void CHmsAbstractButton::UpdateContentColor()
{
	if (m_enabled)
	{
		if (m_pImageNormal)   m_pImageNormal->SetColor(Color3B::WHITE);
		if (m_pImageSelected) m_pImageSelected->SetColor(Color3B::WHITE);
		if (m_pImageDisable) m_pImageDisable->SetColor(Color3B::WHITE);
	}
	else
	{
		if (m_pImageNormal)   m_pImageNormal->SetColor(Color3B::GRAY);
		if (m_pImageSelected) m_pImageSelected->SetColor(Color3B::GRAY);
		if (m_pImageDisable) m_pImageDisable->SetColor(Color3B::GRAY);
	}
}

void CHmsAbstractButton::EnableChanged()
{
	UpdateContentColor();
	UpdateImagesVisibility();
}

NS_HMS_END