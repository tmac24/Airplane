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

#include "HmsKeyItem.h"

using namespace HmsAviPf;

class CHmsKeyboard : public CHmsStretchImageNode, public HmsUiEventInterface
{
public:
	CHmsKeyboard();
	~CHmsKeyboard();

	CREATE_FUNC(CHmsKeyboard);
	CREATE_FUNC_BY_SIZE(CHmsKeyboard);

	typedef std::function<void(char c)> HmsOnKeyPress;
	typedef std::function<void()>		HmsOnKbdHide;

	virtual void SetVisible(bool bVisible) override;

	void SetKeyPressCallback(const HmsOnKeyPress &func);
	void SetKeybdHideCallback(const HmsOnKbdHide &func);
	void ShowKeyboard(bool bShow);

protected:
	virtual bool Init() override;
	virtual bool Init(const HmsAviPf::Size & size);
	virtual bool OnPressed(const Vec2 & posOrigin) override;

	void CreateItems();

	struct KeyItemStu
	{
		Rect rect;
		const char *pNormalImg;
		const char *pSelectImg;
		char pressChar;

		KeyItemStu() :pressChar(0)
		{}
	};

    void AddKeyItem(char c, KeyItemStu &stu, Color4B color, std::string icon = "");
    void AddKeyItem(std::string str, KeyItemStu &stu, Color4B color, std::string icon = "");

private:
	std::map<char, KeyItemStu> m_mapRect;
	HmsOnKeyPress m_onKeyPressFunc;
	HmsOnKbdHide m_onKeybdHideFunc;
};

