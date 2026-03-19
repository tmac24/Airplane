#include "HmsCheckbox.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "HmsImageNode.h"

NS_HMS_BEGIN

CHmsCheckBox::CHmsCheckBox()
: OnStateChanged(nullptr)
{
}

CHmsCheckBox::~CHmsCheckBox()
{
}

CHmsCheckBox* CHmsCheckBox::Create()
{
	CHmsCheckBox *ret = new CHmsCheckBox();
	if (ret && ret->Init())
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsCheckBox* CHmsCheckBox::Create(const std::string &normalImage, const std::string &selectedImage)
{
	return Create(normalImage, selectedImage, "", (const onClickCallback&)nullptr);
}

CHmsCheckBox* CHmsCheckBox::Create(const std::string &normalImage, const std::string &selectedImage, const onClickCallback& callback)
{
	return Create(normalImage, selectedImage, "", (const onClickCallback&)nullptr);
}

CHmsCheckBox* CHmsCheckBox::Create(const std::string &normalImage, const std::string &selectedImage, const std::string &disableImage, const onClickCallback& callback)
{
	CHmsCheckBox *ret = new STD_NOTHROW CHmsCheckBox();
	if (ret && ret->InitWithNormalImage(normalImage, selectedImage, disableImage, callback))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsCheckBox::Init()
{
	return InitWithNormalImage("res/display/checkbox_normal.png", "res/display/checkbox_selected.png", "", (const onClickCallback&)nullptr);
}

void CHmsCheckBox::ContentLayout()
{
	auto height = this->m_pImageNormal->GetContentSize().height;
	auto width = this->m_pImageNormal->GetContentSize().width;
	if (m_pNodeIcon != nullptr)
	{
		m_pNodeIcon->SetAnchorPoint(Vec2(0, 0.5));
		m_pNodeIcon->SetPosition(width + 6, height / 2.0);
	}

	if (m_pLabelText != nullptr)
	{
		if (m_pNodeIcon == nullptr)
		{
			m_pLabelText->SetAnchorPoint(0, 0.5);
			m_pLabelText->SetPosition(width + 6, height / 2.0);
		}
		else
		{
			m_pLabelText->SetAnchorPoint(0, 0.5);
			m_pLabelText->SetPosition(6 + width + m_pNodeIcon->GetContentSize().width * m_pNodeIcon->GetScaleX() + 5, height / 2.0);
		}
	}
}

void CHmsCheckBox::Selected()
{
	if (!m_selected)
	{
		CHmsButton::Selected();

		if (OnStateChanged)
		{
			OnStateChanged(this, true);
		}
	}
}

void CHmsCheckBox::UnSelected()
{
	if (m_selected)
	{

		CHmsButton::UnSelected();

		if (OnStateChanged)
		{
			OnStateChanged(this, false);
		}
	}
}

void CHmsCheckBox::OnReleased(const Vec2 & posParent)
{
	CHmsButton::OnReleased(posParent);

	if (m_enabled)
	{
		if (!m_selected)
		{
			Selected();
		}
		else
		{
			UnSelected();
		}
	}
}

void CHmsCheckBox::SetStateChangedCallback(onStateChangedCallback stateChanged)
{
	OnStateChanged = stateChanged;
}

void CHmsCheckBox::SetSize(const Size &size)
{
	this->SetContentSize(size);
	ContentLayout();
}


NS_HMS_END