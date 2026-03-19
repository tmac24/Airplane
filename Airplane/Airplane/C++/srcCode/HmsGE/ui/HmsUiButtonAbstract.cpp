#include "HmsUiButtonAbstract.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"

NS_HMS_BEGIN

#define HMS_UI_BTN_ICON_ID				0x86
#define HMS_UI_BTN_STATE_LABEL_ID		0x87
#define HMS_UI_BTN_STATE_NORMAL			0x91
#define HMS_UI_BTN_STATE_SELECTED		0x92
#define HMS_UI_BTN_STATE_DISABLE		0x93


CHmsUiButtonAbstract::CHmsUiButtonAbstract(CHmsNode * node)
	: HmsUiEventInterface(node)
	, m_eBtnState(HMS_UI_BUTTON_STATE::btn_normal)
	, m_bToggleBtn(false)
	, m_funcCallback(nullptr)
	, m_onClickedCallback(nullptr)
    , m_bEnableCallback(true)
{

}

CHmsUiButtonAbstract::~CHmsUiButtonAbstract()
{
}

void CHmsUiButtonAbstract::SetCallbackFunc(const HmsUiBtnCallback & callback)
{
	m_funcCallback = callback;
}

void CHmsUiButtonAbstract::SetOnClickedFunc(const OnClickedCallback & callback)
{
	m_onClickedCallback = callback;
}

void CHmsUiButtonAbstract::AddIcon(CHmsImageNode *icon)
{
	if (m_node)
	{
		m_node->RemoveChildByTag(HMS_UI_BTN_ICON_ID);
		if (icon)
		{
			icon->SetTag(HMS_UI_BTN_ICON_ID);
			icon->SetLocalZOrder(1);
			m_node->AddChild(icon);
		}
	}

}

void CHmsUiButtonAbstract::AddIcon(const std::string & iconPath, const Size &iconSize /*= Size(16, 16)*/)
{
	auto node = CHmsImageNode::Create(iconPath);
	if (node)
	{
		node->SetContentSize(iconSize);
		node->SetAnchorPoint(Vec2(0.5, 0.5));
		node->SetPosition(GetUiCenter());
		AddIcon(node);
	}
}

void CHmsUiButtonAbstract::AddIcon(const std::string & iconPath)
{
	auto node = CHmsImageNode::Create(iconPath);
	if (node)
	{
		node->SetAnchorPoint(Vec2(0.5, 0.5));
		node->SetPosition(GetUiCenter());
		AddIcon(node);
	}
}

void CHmsUiButtonAbstract::AddText(const char * strText, float size /*= 12.0f*/, const Color4B &color /*= Color4B::BLACK*/, bool bBorder /*= false*/)
{
	auto labelText = CreateLabel(strText, size, bBorder);
	if (labelText)
	{
		labelText->SetTextColor(color);
		labelText->SetPosition(GetUiCenter());
		labelText->SetLocalZOrder(1);
		AddLabel(labelText);
	}
}

void CHmsUiButtonAbstract::AddLabel(CHmsLabel *label)
{
	if (m_node && label)
	{
		m_node->RemoveChildByTag(HMS_UI_BTN_STATE_LABEL_ID);

		if (label)
		{
			m_node->AddChild(label);
			label->SetTag(HMS_UI_BTN_STATE_LABEL_ID);
		}
	}
}

void CHmsUiButtonAbstract::AttachLabelToState(CHmsLabel *label, HMS_UI_BUTTON_STATE eState)
{
	CHmsNode * nodeState = GetStateNode(eState);

	if (nodeState)
	{
		m_node->RemoveChildByTag(HMS_UI_BTN_STATE_LABEL_ID);

		if (label)
		{
			label->SetTag(HMS_UI_BTN_STATE_LABEL_ID);
			nodeState->AddChild(label);
		}
	}
}

void CHmsUiButtonAbstract::AttachIconToState(CHmsImageNode *icon, HMS_UI_BUTTON_STATE eState)
{
	CHmsNode * nodeState = GetStateNode(eState);

	if (nodeState && icon)
	{
		nodeState->RemoveChildByTag(HMS_UI_BTN_ICON_ID);
		if (icon)
		{
			icon->SetTag(HMS_UI_BTN_ICON_ID);
			nodeState->AddChild(icon);
		}
	}
}

std::string CHmsUiButtonAbstract::GetText()
{
	auto label = GetLabel();
	if (label)
	{
		return label->GetString();
	}
	else
	{
		return GetTextByState(m_eBtnState);
	}
}

std::string CHmsUiButtonAbstract::GetTextByState(HMS_UI_BUTTON_STATE eState)
{
	auto label = GetLabelByState(eState);
	if (label)
	{
		return label->GetString();
	}

	return "";
}

void CHmsUiButtonAbstract::SetText(const char * strText)
{
	auto label = GetLabel();
	if (label)
	{
		label->SetString(strText);
	}
	else
	{
		SetTextToState(strText, m_eBtnState);
	}
}

void CHmsUiButtonAbstract::SetTextToState(const char * strText, HMS_UI_BUTTON_STATE eState)
{
	auto label = GetLabelByState(eState);
	if (label)
	{
		label->SetString(strText);
	}
}

void CHmsUiButtonAbstract::SetTextColor(const Color4B &color)
{
	auto label = GetLabel();
	if (label)
	{
		label->SetTextColor(color);
	}
	else
	{
		SetTextColorToState(color, m_eBtnState);
	}
}

void CHmsUiButtonAbstract::SetTextColorToState(const Color4B &color, HMS_UI_BUTTON_STATE eState)
{
	auto label = GetLabelByState(eState);
	if (label)
	{
		label->SetTextColor(color);
	}
}

CHmsLabel * CHmsUiButtonAbstract::GetLabel()
{
	CHmsLabel * label = nullptr;
	if (m_node)
	{
		label = dynamic_cast<CHmsLabel*>(m_node->GetChildByTag(HMS_UI_BTN_STATE_LABEL_ID));
	}

	return label;
}

CHmsNode * CHmsUiButtonAbstract::GetStateNode(HMS_UI_BUTTON_STATE eState)
{
	CHmsNode * nodeState = nullptr;
	if (m_node)
	{
		switch (eState)
		{
		case HMS_UI_BUTTON_STATE::btn_normal:
			nodeState = m_node->GetChildByTag(HMS_UI_BTN_STATE_NORMAL);
			break;
		case HMS_UI_BUTTON_STATE::btn_selected:
			nodeState = m_node->GetChildByTag(HMS_UI_BTN_STATE_SELECTED);
			break;
		case HMS_UI_BUTTON_STATE::btn_disable:
			nodeState = m_node->GetChildByTag(HMS_UI_BTN_STATE_DISABLE);;
			break;
		}
	}
	return nodeState;
}

void CHmsUiButtonAbstract::RegisterStateNode(CHmsNode * node, HMS_UI_BUTTON_STATE eState)
{
	if (node)
	{
		int nNodeID = 0;
		switch (eState)
		{
		case HMS_UI_BUTTON_STATE::btn_normal:
			nNodeID = HMS_UI_BTN_STATE_NORMAL;
			break;
		case HMS_UI_BUTTON_STATE::btn_selected:
			nNodeID = HMS_UI_BTN_STATE_SELECTED;
			break;
		case HMS_UI_BUTTON_STATE::btn_disable:
			nNodeID = HMS_UI_BTN_STATE_DISABLE;
			break;
		}
		node->SetTag(nNodeID);
	}
}

CHmsLabel * CHmsUiButtonAbstract::GetLabelByState(HMS_UI_BUTTON_STATE eState)
{
	CHmsLabel * label = nullptr;
	if (m_node)
	{
		CHmsNode * nodeState = GetStateNode(eState);

		if (nodeState)
		{
			label = dynamic_cast<CHmsLabel*>(nodeState->GetChildByTag(HMS_UI_BTN_STATE_LABEL_ID));
		}
	}

	return label;
}

CHmsImageNode * CHmsUiButtonAbstract::GetIconNode()
{
	CHmsImageNode * pRet = nullptr;
	if (m_node)
	{
		pRet = dynamic_cast<CHmsImageNode*>(m_node->GetChildByTag(HMS_UI_BTN_ICON_ID));
	}
	return pRet;
}

void CHmsUiButtonAbstract::SetSelected(bool isSelected)
{
	if (isSelected)
	{
		Selected();
	}
	else
	{
		UnSelected();
	}
}

void CHmsUiButtonAbstract::Selected()
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
}

void CHmsUiButtonAbstract::UnSelected()
{
	//if (!m_bToggleBtn)
	{
		SetState(HMS_UI_BUTTON_STATE::btn_normal);
	}
}

bool CHmsUiButtonAbstract::IsSelected()
{
	return m_eBtnState == HMS_UI_BUTTON_STATE::btn_selected;
}

void CHmsUiButtonAbstract::SetToggleBtn(bool bToggle)
{
	m_bToggleBtn = bToggle;
}

bool CHmsUiButtonAbstract::OnPressed(const Vec2 & posOrigin)
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
			SetState(HMS_UI_BUTTON_STATE::btn_normal);
		}
	}
	else
	{
		SetState(HMS_UI_BUTTON_STATE::btn_selected);
	}

	return true;
}

void CHmsUiButtonAbstract::OnMove(const Vec2 & posOrigin)
{
	
}

void CHmsUiButtonAbstract::OnReleased(const Vec2 & posOrigin)
{
	if (!m_bToggleBtn)
	{
		SetState(HMS_UI_BUTTON_STATE::btn_normal);
	}

    if (m_onClickedCallback && m_bEnableCallback)
	{
		m_onClickedCallback(this);
	}
}

void CHmsUiButtonAbstract::SetState(HMS_UI_BUTTON_STATE eState)
{
	if (m_eBtnState != eState)
	{
		m_eBtnState = eState;
		DispatchCallbackFunc();
	}
}

void CHmsUiButtonAbstract::EnableUi(bool bEnable)
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

void HmsAviPf::CHmsUiButtonAbstract::SetEnableCallback(bool b)
{
    m_bEnableCallback = b;
}

void CHmsUiButtonAbstract::DispatchCallbackFunc()
{
    if (m_funcCallback && m_bEnableCallback)
	{
		m_funcCallback(this, m_eBtnState);
	}
}

NS_HMS_END


