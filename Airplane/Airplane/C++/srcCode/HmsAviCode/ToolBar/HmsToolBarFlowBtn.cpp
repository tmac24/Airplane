#include "HmsToolBarFlowBtn.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "../HmsGlobal.h"

USING_NS_HMS;

HmsAviPf::Size CHmsToolBarFlowBtn::s_sizeBtnDesign = Size(90, 90);

CHmsToolBarFlowBtn* CHmsToolBarFlowBtn::CreateWithImage(const char * strImagePath, const char * strTitle)
{
	CHmsToolBarFlowBtn *ret = new CHmsToolBarFlowBtn();
	if (ret && ret->Init(strImagePath, strTitle))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsToolBarFlowBtn::CHmsToolBarFlowBtn()
	: CHmsUiButtonAbstract(this)
	, m_bClick2Normal(true)
{

}

CHmsToolBarFlowBtn::~CHmsToolBarFlowBtn()
{
}

bool CHmsToolBarFlowBtn::Init(const char * strImagePath, const char * strTitle)
{
	const float c_fMargin = 8;

	if (!CHmsNode::Init())
	{
		return false;
	}

	SetAnchorPoint(0.0f, 0.0f);
	SetContentSize(s_sizeBtnDesign);

	m_bgNode = CHmsStretchImageNode::Create("res/ToolBarIcon/FlowBtnBg.png", s_sizeBtnDesign);
	if (m_bgNode)
	{
		m_bgNode->SetColor(Color3B::BLACK);
		m_bgNode->SetAnchorPoint(Vec2(0, 0));
		this->AddChild(m_bgNode);
	}

	auto icon = CHmsImageNode::Create(strImagePath);

	icon->SetAnchorPoint(Vec2(0.5f, 0.5f));
	icon->SetPosition(s_sizeBtnDesign / 2.0f);
	this->AddIcon(icon);

	return true;
}

void CHmsToolBarFlowBtn::SetButtonColor(const HmsAviPf::Color3B & color)
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

void CHmsToolBarFlowBtn::SetState(HMS_UI_BUTTON_STATE eState)
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
		SetButtonColor(Color3B(0, 71, 157));//Color3B::BLUE
		if (m_bgNode)
		{
			m_bgNode->SetColor(Color3B::GREEN);
		}
		break;
	}
	}
	CHmsUiButtonAbstract::SetState(eState);
}

bool CHmsToolBarFlowBtn::OnPressed(const Vec2 & posOrigin)
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

void CHmsToolBarFlowBtn::Selected()
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


