#include "HmsUiImageButton.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"

NS_HMS_BEGIN

#define HMS_UI_BTN_ICON_ID				0x86
#define HMS_UI_BTN_STATE_LABEL_ID		0x87

CHmsUiImageButton* CHmsUiImageButton::CreateWithImage(const char * strImagePath, const HmsUiBtnCallback& callback /*= nullptr*/)
{
	CHmsUiImageButton *ret = new CHmsUiImageButton();
	if (ret && ret->InitWithFile(strImagePath))
	{
		ret->SetCallbackFunc(callback);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsUiImageButton* CHmsUiImageButton::CreateWithMipMapTexture(Texture2D *texture, const HmsUiBtnCallback& callback /*= nullptr*/)
{
    CHmsUiImageButton *ret = new CHmsUiImageButton();
    if (ret && ret->InitWithTexture(texture))
    {
        ret->SetCallbackFunc(callback);
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

CHmsUiImageButton::CHmsUiImageButton()
	: CHmsUiButtonAbstract(this)
{

}

CHmsUiImageButton::~CHmsUiImageButton()
{
}






CHmsFixSizeUiImageButton* CHmsFixSizeUiImageButton::CreateWithImage(const char * strImagePath, const Size & size, const HmsUiBtnCallback& callback /*= nullptr*/)
{
	CHmsFixSizeUiImageButton *ret = new CHmsFixSizeUiImageButton();
	
	if (ret && ret->Init())
	{
		ret->SetContentSize(size);
		auto image = CHmsImageNode::Create(strImagePath);
		image->SetAnchorPoint(Vec2(0.5, 0.5));
		image->SetPosition(size / 2.0f);
		ret->AddChild(image);
		ret->m_imageNode = image;
		ret->SetCallbackFunc(callback);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsFixSizeUiImageButton::CHmsFixSizeUiImageButton()
: CHmsUiButtonAbstract(this)
{

}

CHmsFixSizeUiImageButton::~CHmsFixSizeUiImageButton()
{
}

void CHmsFixSizeUiImageButton::SetColor(const Color3B& color)
{
	CHmsNode::SetColor(color);

	if (m_imageNode)
	{
		m_imageNode->SetColor(color);
	}
}

NS_HMS_END


