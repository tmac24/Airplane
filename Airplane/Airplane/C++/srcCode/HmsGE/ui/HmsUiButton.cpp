#include "HmsUiButton.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"

NS_HMS_BEGIN

CHmsUiButton* CHmsUiButton::CreateWithImage(const char * normalImage, const char * selectedImage, const char * disableImage /*= nullptr*/, const HmsUiBtnCallback& callback /*= nullptr*/)
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

	return CreateWithNode(normal, select, disable, callback);
}

CHmsUiButton* CHmsUiButton::CreateWithStretchImage(const Size & size, const char * normalImage, const char * selectedImage, const char * disableImage /*= nullptr*/, const HmsUiBtnCallback& callback /*= nullptr*/)
{
	auto GetStretchImageNode = [size](const char *  path)
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

	CHmsStretchImageNode * normal = GetStretchImageNode(normalImage);
	CHmsStretchImageNode * select = GetStretchImageNode(selectedImage);
	CHmsStretchImageNode * disable = GetStretchImageNode(disableImage);

	return CreateWithNode(normal, select, disable, callback);
}

CHmsUiButton* CHmsUiButton::CreateWithNode(CHmsNode * normalImage, CHmsNode * selectedImage, CHmsNode * disableImage /*= nullptr*/, const HmsUiBtnCallback& callback /*= nullptr*/)
{
	CHmsUiButton *ret = new CHmsUiButton();
	if (ret && ret->InitWithNode(normalImage, selectedImage, disableImage))
	{
		ret->SetCallbackFunc(callback);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsUiButton::CHmsUiButton()
: CHmsUiButtonAbstract(this)
{

}

CHmsUiButton::~CHmsUiButton()
{
}


bool CHmsUiButton::InitWithNode(CHmsNode * normalImage, CHmsNode * selectedImage, CHmsNode * disableImage)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_pNodeNormal = normalImage;
	m_pNodeSelected = selectedImage;
	m_pNodeDisable = disableImage;

	if (m_pNodeNormal)
	{
		m_pNodeNormal->SetLocalZOrder(0);
		auto size = m_pNodeNormal->GetContentSize();
		this->SetContentSize(size);
		this->AddChild(m_pNodeNormal);
	}

	if (m_pNodeSelected)
	{
		m_pNodeSelected->SetLocalZOrder(0);
	}

	if (m_pNodeDisable)
	{
		m_pNodeDisable->SetLocalZOrder(0);
	}

	return true;
}


void CHmsUiButton::SetState(HMS_UI_BUTTON_STATE eState)
{
	if (m_eBtnState != eState)
	{
		auto nodeCurrent = GetStateNode(m_eBtnState);
		auto nodeSet = GetStateNode(eState);
		if (nodeSet)
		{
			if (nodeCurrent)
			{
				nodeCurrent->RemoveFromParent();
			}
			this->AddChild(nodeSet, -1);
			m_eBtnState = eState;
			if (m_funcCallback)
			{
				m_funcCallback(this, m_eBtnState);
			}
		}
	}
}

CHmsNode * CHmsUiButton::GetStateNode(HMS_UI_BUTTON_STATE eState)
{
	switch (eState)
	{
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_normal:
		return m_pNodeNormal;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected:
		return m_pNodeSelected;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_disable:
		return m_pNodeDisable;
	default:
		break;
	}

	return nullptr;
}

void CHmsUiButton::ResetTouchSize(const Size & size)
{
	this->SetContentSize(size);
//#if 0
	Vec2 pos = size / 2.0f;

	if (m_pNodeNormal)
		m_pNodeNormal->SetPosition(pos);

	if (m_pNodeSelected)
		m_pNodeSelected->SetPosition(pos);

	if (m_pNodeDisable)
		m_pNodeDisable->SetPosition(pos);
//#else
	Vec2 ap(0.5, 0.5);

	if (m_pNodeNormal)
		m_pNodeNormal->SetAnchorPoint(ap);

	if (m_pNodeSelected)
		m_pNodeSelected->SetAnchorPoint(ap);

	if (m_pNodeDisable)
		m_pNodeDisable->SetAnchorPoint(ap);
//#endif
}


NS_HMS_END


