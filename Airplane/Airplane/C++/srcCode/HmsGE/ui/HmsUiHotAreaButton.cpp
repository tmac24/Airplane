#include "HmsUiHotAreaButton.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"

NS_HMS_BEGIN

CHmsUiHotAreaButton* CHmsUiHotAreaButton::Create(const Size & size , const HmsUiBtnCallback& callback /*= nullptr*/)
{
	CHmsUiHotAreaButton *ret = new CHmsUiHotAreaButton();
	if (ret && ret->Init())
	{
		ret->SetContentSize(size);
		ret->SetCallbackFunc(callback);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsUiHotAreaButton::CHmsUiHotAreaButton()
: CHmsUiButtonAbstract(this)
{

}

CHmsUiHotAreaButton::~CHmsUiHotAreaButton()
{
}

void HmsAviPf::CHmsUiHotAreaButton::AddContentAlignRightCenter(CHmsNode * node, const Vec2 & offset)
{
	if (node)
	{
		node->SetAnchorPoint(Vec2(1.0f, 0.5f));
		node->SetPosition(m_sizeContent.width + offset.x, m_sizeContent.height*0.5f + offset.y);
		this->AddChild(node);
	}
}

void HmsAviPf::CHmsUiHotAreaButton::AddContentAlignLeftCenter(CHmsNode * node, const Vec2 & offset)
{
	if (node)
	{
		node->SetAnchorPoint(Vec2(0.0f, 0.5f));
		node->SetPosition(0 + offset.x, m_sizeContent.height*0.5f + offset.y);
		this->AddChild(node);
	}
}

void HmsAviPf::CHmsUiHotAreaButton::AddContentAlignCenter(CHmsNode * node, const Vec2 & offset)
{
	if (node)
	{
		node->SetAnchorPoint(Vec2(0.5f, 0.5f));
		node->SetPosition(m_sizeContent.width*0.5f + offset.x, m_sizeContent.height*0.5f + offset.y);
		this->AddChild(node);
	}
}

NS_HMS_END


