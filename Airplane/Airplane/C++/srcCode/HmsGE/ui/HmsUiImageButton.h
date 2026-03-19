#pragma once
#include "display/HmsImageNode.h"
#include "ui/HmsUiButtonAbstract.h"

NS_HMS_BEGIN

class CHmsUiImageButton : public CHmsImageNode, public CHmsUiButtonAbstract
{
public:
	CHmsUiImageButton();
	~CHmsUiImageButton();

	static CHmsUiImageButton* CreateWithImage(const char * strImagePath, const HmsUiBtnCallback& callback = nullptr);
    static CHmsUiImageButton* CreateWithMipMapTexture(Texture2D *texture, const HmsUiBtnCallback& callback = nullptr);

};

class CHmsFixSizeUiImageButton : public CHmsNode, public CHmsUiButtonAbstract
{
public:
	CHmsFixSizeUiImageButton();
	~CHmsFixSizeUiImageButton();

	static CHmsFixSizeUiImageButton* CreateWithImage(const char * strImagePath, const Size & size, const HmsUiBtnCallback& callback = nullptr);

	virtual void SetColor(const Color3B& color);

private:

	RefPtr<CHmsImageNode> m_imageNode;
};

NS_HMS_END