#pragma once

#include "HmsGlobal.h"
#include "HmsScreenLayout.h"
#include "display/HmsDrawNode.h"
#include "display/HmsLabel.h"
//#include "display/HmsFixedLengthLineEidt.h"
#include "base/HmsTypes.h"

#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "ui/HmsUiEventInterface.h"

using namespace HmsAviPf;

class CHmsKeyItem : public CHmsStretchImageNode, public HmsUiEventInterface
{
public:
	CHmsKeyItem();
	~CHmsKeyItem();

	typedef std::function<void(char c)> HmsOnKeyPress;
	static CHmsKeyItem* CreateWithImage(const char * strImageNormal, const char * strImageSelected, 
		Size btnSize, const HmsOnKeyPress& callback = nullptr);

	void SetCallbackFunc(const HmsOnKeyPress & callback){ m_funcCallback = callback; }
	//void SetItemSize(Size itemSize){ m_itemSize = itemSize; }
	void SetPressChar(char c){ m_pressChar = c; }

	void SetText(const char * strText, float size = 12.0f, const Color4B &color = Color4B::BLACK, bool bBorder = false);
	void SetTextColor(Color4B color);

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

protected:
	virtual bool Init() override;
	bool InitWithTexture(Texture2D * textureNormal, Texture2D * textureSelected, Size btnSize);

private:
	HmsOnKeyPress m_funcCallback;
	RefPtr<Texture2D> m_textureNormal;
	RefPtr<Texture2D> m_textureSelected;
	RefPtr<CHmsLabel> m_pTextLabel;

	Size m_itemSize;

	char m_pressChar;
};

