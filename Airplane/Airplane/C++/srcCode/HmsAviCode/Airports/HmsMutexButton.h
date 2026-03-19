#pragma once

#include "../HmsGlobal.h"
#include "../HmsScreenLayout.h"
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

class CHmsMutexButton : public CHmsStretchImageNode, public HmsUiEventInterface
{
public:
	CHmsMutexButton();
	~CHmsMutexButton();

	typedef std::function<void(CHmsMutexButton*)> HmsOnButtonSelect;
	static CHmsMutexButton* CreateWithImage(const char * strImageNormal, const char * strImageSelected,
		const HmsOnButtonSelect& callback = nullptr);

	void SetOnBtnSelectFunc(const HmsOnButtonSelect & callback){ m_funcCallback = callback; }

	void SetText(const char * strText, float size = 12.0f, const Color4B &color = Color4B::WHITE, bool bBorder = false);
	void SetTextColor(Color4B color);

	void SetSelect();
	void SetUnSelect();
	bool IsSelected();

	void SetAutoNormal(bool b);

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

protected:
	virtual bool Init() override;
	bool InitWithTexture(Texture2D * textureNormal, Texture2D * textureSelected);

private:
	HmsOnButtonSelect m_funcCallback;
	RefPtr<Texture2D> m_textureNormal;
	RefPtr<Texture2D> m_textureSelected;
	RefPtr<CHmsLabel> m_pTextLabel;

	bool m_bIsSelected;
	bool m_bAutoNormal;
};

