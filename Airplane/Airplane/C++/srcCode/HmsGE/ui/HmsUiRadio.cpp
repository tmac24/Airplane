#include "HmsUiRadio.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"


NS_HMS_BEGIN

CHmsUiRadio::CHmsUiRadio()
: m_colorNormal(Color4B::WHITE)
, m_colorSelected(Color4B::WHITE)
, m_textAlign(TextAlign::ALIGN_CENTER)
{
}

CHmsUiRadio::~CHmsUiRadio()
{
}


CHmsUiRadio * CHmsUiRadio::CreateWithStretchImage(const Size & size, const char * normalImage, const char * selectedImage, const char * disableImage /*= nullptr*/)
{
	auto GetImageNode = [](const Size size, const char *  path)
	{
		CHmsStretchImageNode * ret = nullptr;
		if (path)
		{
			ret = CHmsStretchImageNode::Create(path);
			if (ret)
			{
				ret->SetContentSize(size);
				ret->SetAnchorPoint(Vec2(0, 0));
			}
		}
		return ret;
	};

	CHmsStretchImageNode * normal = GetImageNode(size, normalImage);
	CHmsStretchImageNode * select = GetImageNode(size, selectedImage);
	CHmsStretchImageNode * disable = GetImageNode(size, disableImage);

	CHmsUiRadio *ret = new CHmsUiRadio();
	if (ret && ret->Init(normal, select, disable))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsUiRadio* CHmsUiRadio::CreateWithImage(const char * normalImage, const char * selectedImage, const char * disableImage /*= nullptr*/)
{
	auto GetImageNode = [](const char *  path)
	{
		CHmsImageNode * ret = nullptr;
		if (path)
		{
			ret = CHmsImageNode::Create(path);
			if (ret)
			{
				ret->SetAnchorPoint(Vec2(0, 0));
			}
		}
		return ret;
	};

	CHmsImageNode * normal = GetImageNode(normalImage);
	CHmsImageNode * select = GetImageNode(selectedImage);
	CHmsImageNode * disable = GetImageNode(disableImage);

	CHmsUiRadio *ret = new CHmsUiRadio();
	if (ret && ret->Init(normal, select, disable))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}


bool CHmsUiRadio::Init(CHmsNode * normal, CHmsNode * select, CHmsNode * disable)
{
	if (normal == nullptr || select == nullptr)
	{
		return false;
	}

	normal->SetAnchorPoint(HmsAviPf::Vec2(0, 0));
	select->SetAnchorPoint(HmsAviPf::Vec2(0, 0));
	if (disable) disable->SetAnchorPoint(HmsAviPf::Vec2(0, 0));

	if (!CHmsUiButton::InitWithNode(normal, select, disable))
	{
		return false;
	}
	this->SetToggleBtn(true);
	return true;
}

bool CHmsUiRadio::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
	if (m_eBtnState == HmsAviPf::HMS_UI_BUTTON_STATE::btn_disable || m_eBtnState == HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected)
	{
		return false;
	}

	SetState(HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected);
	return true;
}

void CHmsUiRadio::SetState(HmsAviPf::HMS_UI_BUTTON_STATE eState)
{
	CHmsLabel *label = GetLabel();
	if (label)
	{
		if (eState == HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected)
		{
			label->SetTextColor(m_colorSelected);
		}
		else
		{
			label->SetTextColor(m_colorNormal);
		}
	}
	
	CHmsUiButton::SetState(eState);
}

void CHmsUiRadio::SetNormalTextColor(HmsAviPf::Color4B color)
{
	m_colorNormal = color;
	if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_normal)
	{
		CHmsLabel *label = GetLabel();
		if (label)
		{
			label->SetTextColor(m_colorNormal);
		}
	}
}

void CHmsUiRadio::SetSelectedTextColor(HmsAviPf::Color4B color)
{
	m_colorSelected = color;
	if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_selected)
	{
		CHmsLabel *label = GetLabel();
		if (label)
		{
			label->SetTextColor(m_colorSelected);
		}
	}
}

void CHmsUiRadio::SetDisableTextColor(HmsAviPf::Color4B color)
{
	m_colorDisable = color;
	if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_disable)
	{
		CHmsLabel *label = GetLabel();
		if (label)
		{
			label->SetTextColor(m_colorDisable);
		}
	}
}

void CHmsUiRadio::AddText(const char * strText, float size /*= 12.0f*/, const HmsAviPf::Color4B &color /*= HmsAviPf::Color4B::WHITE*/, bool bBorder /*= false*/)
{
	CHmsUiButton::AddText(strText, size, color, bBorder);
	SetTextAlign(m_textAlign);
}

void CHmsUiRadio::SetTextAlign(TextAlign align)
{
	m_textAlign = align;
	auto label = GetLabel();
	if (label != nullptr)
	{
		Vec2 anchorPoint(0.5, 0.5);
		Vec2 pos(m_pNodeNormal->GetContentSize().width / 2.0f, m_pNodeNormal->GetContentSize().height / 2.0f);
		switch (m_textAlign)
		{
		case HmsAviPf::CHmsUiRadio::ALIGN_CENTER:
			break;
		case HmsAviPf::CHmsUiRadio::ALIGN_RIGHT:
			anchorPoint = Vec2(0, 0.5);
			pos = Vec2(m_pNodeNormal->GetContentSize().width + 16, m_pNodeNormal->GetContentSize().height / 2.0f);
			break;
		default:
			break;
		}
		label->SetAnchorPoint(anchorPoint);
		label->SetPosition(pos);
	}
}

NS_HMS_END