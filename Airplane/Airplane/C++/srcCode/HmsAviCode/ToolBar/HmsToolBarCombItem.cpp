#include "HmsToolBarCombItem.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "../HmsGlobal.h"

USING_NS_HMS;

CHmsToolBarCombItem* CHmsToolBarCombItem::CreateWithImage(const char * strImagePath, const char * strTitle, const HmsAviPf::Size & size, const char * strBgPath)
{
	CHmsToolBarCombItem *ret = new CHmsToolBarCombItem();
	if (ret && ret->Init(strImagePath, strTitle, size, strBgPath))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsToolBarCombItem* CHmsToolBarCombItem::CreateWithString(const char * strTitle, const HmsAviPf::Size & size, const char * strBgPath)
{
	CHmsToolBarCombItem *ret = new CHmsToolBarCombItem();
	if (ret && ret->Init(strTitle, size, strBgPath))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsToolBarCombItem::CHmsToolBarCombItem()
	: CHmsUiButtonAbstract(this)
	, m_bClick2Normal(true)
{

}

CHmsToolBarCombItem::~CHmsToolBarCombItem()
{
}

bool CHmsToolBarCombItem::Init(const char * strImagePath, const char * strTitle, const HmsAviPf::Size & size, const char * strBgPath)
{
	const float c_fMargin = 4;

	if (!CHmsNode::Init())
	{
		return false;
	}

	SetAnchorPoint(0.0f, 0.0f);
	SetContentSize(size);


	m_bgNode = CHmsStretchImageNode::Create((strBgPath!=nullptr)?strBgPath:"res/BasicFrame/Round4SolidWhite.png", size);
	if (m_bgNode)
	{
		m_bgNode->SetColor(Color3B::BLACK);
		m_bgNode->SetAnchorPoint(Vec2(0, 0));
		this->AddChild(m_bgNode);
	}

	auto icon = CHmsImageNode::Create(strImagePath);

	icon->SetAnchorPoint(Vec2(0.5f, 0.5f));
	icon->SetPosition(Vec2(c_fMargin + size.height / 2.0f, size.height / 2.0f));
	this->AddIcon(icon);

	auto title = CHmsGlobal::CreateLanguageLabel(strTitle, 28, true);
	if (title)
	{
		title->SetAnchorPoint(Vec2(0.0f, 0.5f));
		title->SetPosition(Vec2(size.height + c_fMargin * 2, size.height / 2.0f));
		this->AddChild(title);

		m_labelText = title;
	}
	m_strText = strTitle;

	return true;
}

bool CHmsToolBarCombItem::Init(const char * strTitle, const HmsAviPf::Size & size, const char * strBgPath)
{
	const float c_fMargin = 8;

	if (!CHmsNode::Init())
	{
		return false;
	}

	SetAnchorPoint(0.0f, 0.0f);
	SetContentSize(size);

	m_bgNode = CHmsStretchImageNode::Create((strBgPath != nullptr) ? strBgPath : "res/BasicFrame/Round4SolidWhite.png", size);
	if (m_bgNode)
	{
		m_bgNode->SetColor(Color3B::BLACK);
		m_bgNode->SetAnchorPoint(Vec2(0, 0));
		this->AddChild(m_bgNode);
	}

	auto title = CHmsGlobal::CreateLanguageLabel(strTitle, 28, true);
	if (title)
	{
		title->SetAnchorPoint(Vec2(0.5f, 0.5f));
		title->SetPosition(Vec2(size.width / 2.0f, size.height / 2.0f));
		this->AddChild(title);

		m_labelText = title;
	}
	m_strText = strTitle;

	return true;
}

void CHmsToolBarCombItem::SetButtonColor(const HmsAviPf::Color3B & color)
{
	auto label = GetLabel();
	auto icon = GetIconNode();
	if (label)
	{
		label->SetColorWithChild(color);
	}
	if (icon)
	{
		icon->SetColor(color);
	}
}

void CHmsToolBarCombItem::SetState(HMS_UI_BUTTON_STATE eState)
{
	switch (eState)
	{
	case HMS_UI_BUTTON_STATE::btn_normal:
	{
		SetButtonColor(Color3B::WHITE);
		if (m_bgNode)
		{
			m_bgNode->SetColor(Color3B::BLACK);
		}
		break;
	}
	case HMS_UI_BUTTON_STATE::btn_selected:
	{
		//SetButtonColor(Color3B::GREEN);
		SetButtonColor(Color3B::WHITE);//Color3B(0, 71, 157)
		if (m_bgNode)
		{
			m_bgNode->SetColor(Color3B::GREEN);
		}
		break;
	}
	}
	CHmsUiButtonAbstract::SetState(eState);
}

bool CHmsToolBarCombItem::OnPressed(const Vec2 & posOrigin)
{
	if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_disable)
	{
		return false;
	}

	if (m_bToggleBtn)
	{
		if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			SetState(HMS_UI_BUTTON_STATE::btn_selected);
		}
		else if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_selected)
		{
			if (m_bClick2Normal)
			{
				SetState(HMS_UI_BUTTON_STATE::btn_normal);
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		SetState(HMS_UI_BUTTON_STATE::btn_selected);
	}

	return true;
}

void CHmsToolBarCombItem::Selected()
{
	if (m_bToggleBtn)
	{
		if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			SetState(HMS_UI_BUTTON_STATE::btn_selected);
		}
		else if (m_eBtnState == HMS_UI_BUTTON_STATE::btn_selected)
		{
			return;
		}
	}
	else
	{
		SetState(HMS_UI_BUTTON_STATE::btn_selected);
	}
}

void CHmsToolBarCombItem::SetText(const char * strText)
{
	if (m_labelText)
	{
		CHmsLanguageLabel::SetText(m_labelText, strText);
		m_strText = strText;
	}
}

void CHmsToolBarCombItem::SetBgOpacity(GLubyte opacity)
{
	if (m_bgNode)
	{
		m_bgNode->SetOpacity(opacity);
	}
}

