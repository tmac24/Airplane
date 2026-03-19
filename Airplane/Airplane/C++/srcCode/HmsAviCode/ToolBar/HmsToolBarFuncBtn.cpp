#include "HmsToolBarFuncBtn.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "../HmsGlobal.h"

USING_NS_HMS;

HmsAviPf::Size CHmsToolBarFuncBtn::s_sizeBtnDesign = Size(260, 95);

CHmsToolBarFuncBtn* CHmsToolBarFuncBtn::CreateWithImage(const char * strImagePath, const char * strTitle)
{
	CHmsToolBarFuncBtn *ret = new CHmsToolBarFuncBtn();
	if (ret && ret->Init(strImagePath, strTitle))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsToolBarFuncBtn::CHmsToolBarFuncBtn()
	: CHmsUiButtonAbstract(this)
{

}

CHmsToolBarFuncBtn::~CHmsToolBarFuncBtn()
{
}

bool CHmsToolBarFuncBtn::Init(const char * strImagePath, const char * strTitle)
{
	const float c_fMargin = 8;

	if (!CHmsNode::Init())
	{
		return false;
	}

	SetAnchorPoint(0.0f, 0.0f);
	SetContentSize(s_sizeBtnDesign);

	m_bgNode = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png", s_sizeBtnDesign);
	if (m_bgNode)
	{
		m_bgNode->SetColor(Color3B::BLACK);
		m_bgNode->SetAnchorPoint(Vec2(0, 0));
		this->AddChild(m_bgNode);
	}

	auto icon = CHmsImageNode::Create(strImagePath);
	auto title = CHmsGlobal::CreateLanguageLabel(strTitle, 32, true);

	if (title)
	{
		icon->SetAnchorPoint(Vec2(1.0f, 0.5f));
		title->SetAnchorPoint(Vec2(0.5f, 0.5f));

		auto pos = GetUiCenter();
		pos.x += (c_fMargin + icon->GetContentSize().width) / 2.0f;
		title->SetPosition(pos);
		icon->SetPosition(Vec2(-c_fMargin, title->GetContentSize().height / 2.0f));

		title->AddChild(icon);
		AddLabel(title);
	}

	return true;
}

void CHmsToolBarFuncBtn::SetButtonColor(const HmsAviPf::Color3B & color)
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

void CHmsToolBarFuncBtn::SetState(HMS_UI_BUTTON_STATE eState)
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
		SetButtonColor(Color3B::WHITE);
		if (m_bgNode)
		{
			m_bgNode->SetColor(Color3B(0, 71, 157));
		}
		break;
	}
	}
	CHmsUiButtonAbstract::SetState(eState);
}




