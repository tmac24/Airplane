#include "HmsNavMenuButton.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "../HmsGlobal.h"

USING_NS_HMS;


CHmsNavMenuButton* CHmsNavMenuButton::CreateWithImage(const char * strImagePath, const char * strTitle)
{
	CHmsNavMenuButton *ret = new CHmsNavMenuButton();
	if (ret && ret->Init(strImagePath, strTitle))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsNavMenuButton::CHmsNavMenuButton()
	: CHmsUiButtonAbstract(this)
{

}

CHmsNavMenuButton::~CHmsNavMenuButton()
{
}

bool CHmsNavMenuButton::Init(const char * strImagePath, const char * strTitle)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetAnchorPoint(0.5, 0.5);
	//SetContentSize(Size(64, 60));
	SetContentSize(Size(120, 60));

	auto icon = CHmsImageNode::Create(strImagePath);
#if _NAV_SVS_LOGIC_JUDGE
	if (!icon)
		return false;
#endif
	icon->SetAnchorPoint(Vec2(0.5, 0.5));
	icon->SetPosition(GetUiCenter() + Vec2(0, 6));
	auto title = CHmsGlobal::CreateLanguageLabel(strTitle, 12, true);
#if _NAV_SVS_LOGIC_JUDGE
	if (!title)
		return false;
#endif
	AddIcon(icon);
	AddLabel(title);

	auto pos = GetUiBottomCenter();
	title->SetPosition(pos + Vec2(0, 10));
	title->SetAnchorPoint(Vec2(0.5f, 0.5f));

	return true;
}

void CHmsNavMenuButton::SetButtonColor(const HmsAviPf::Color3B & color)
{
	auto label = GetLabel();
	auto icon = GetIconNode();
	if (label)
	{
		label->SetColor(color);
	}
	if (icon)
	{
		icon->SetColor(color);
	}
}

void CHmsNavMenuButton::SetState(HMS_UI_BUTTON_STATE eState)
{
	switch (eState)
	{
	case HMS_UI_BUTTON_STATE::btn_normal:
	{
		SetButtonColor(Color3B::WHITE);
		break;
	}
	case HMS_UI_BUTTON_STATE::btn_selected:
	{
		SetButtonColor(Color3B(0, 71, 157));
		break;
	}
	}
	CHmsUiButtonAbstract::SetState(eState);
}



