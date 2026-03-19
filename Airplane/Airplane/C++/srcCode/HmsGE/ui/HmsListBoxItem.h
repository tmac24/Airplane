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

class CHmsListBoxItem : public CHmsNode, public HmsUiEventInterface
{
public:
	CHmsListBoxItem();
	~CHmsListBoxItem();

	static CHmsListBoxItem* CreateWithImage(
		const char * strBgNormalImage, const char * strBgSelectImage,
		const char * strBtnNormalImage, const char * strBtnSelectImage);

public:
	bool InitWithImage(const char * strBgNormalImage, const char * strBgSelectImage,
		const char * strBtnNormalImage, const char * strBtnSelectImage);

	std::string GetText();

	void SetText(const char * strText, float size = 12.0f, const Color4B &color = Color4B::WHITE, bool bBorder = false);
	void SetTextColor(Color4B color);
	void SetTextAlignLeft(bool b);

	void SetItemSize(Size itemSize);
	void SetBorderVisiable(bool bBorder);
	void SetNormalTextColor(Color4B color);
	void SetSelectTextColor(Color4B color);

	void SetChecked(bool bEnableCallback = true);
	void SetUnChecked(bool bEnableCallback = true);
	void SetCheckable(bool b){ m_bCheckable = b; }

	void SetSelect(bool bEnableCallback = true);
	void SetUnSelect(bool bEnableCallback = true);
	void SetSelectable(bool b);
	bool IsSelectable();
	bool IsSelected();
	bool IsChecked();

	typedef std::function<void(CHmsListBoxItem*, bool)> HmsOnListItemSelect;

	void SetOnSelectChangeFunc(const HmsOnListItemSelect & callback);
	void SetOnCheckChangeFunc(const HmsOnListItemSelect & callback);

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

private:
	RefPtr<CHmsStretchImageNode> m_pBgNode;
	RefPtr<CHmsImageNode> m_pBtnNode;
	RefPtr<CHmsImageNode> m_pBorderNode;
	RefPtr<CHmsLabel> m_pTextLabel;

	HmsOnListItemSelect m_OnSelectChangeFunc;
	HmsOnListItemSelect m_OnCheckChangeFunc;

	std::string m_strBgNormal;
	std::string m_strBgSelect;
	std::string m_strBtnNormal;
	std::string m_strBtnSelect;

	Size m_itemSize;

	float m_fTextXPos;
	bool m_bSelected;
	bool m_bChecked;

	bool m_bSelectable;
	bool m_bCheckable;

	bool m_bTextAlignLeft;

	bool m_bBorder;

	Color4B m_normalTextColor;
	Color4B m_selectTextColor;

	float m_fontSize;
};

