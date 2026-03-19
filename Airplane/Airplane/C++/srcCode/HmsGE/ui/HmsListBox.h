#pragma once

#include <string>
#include "HmsListBoxItem.h"
#include "base/HmsClippingNode.h"
#include "ui/HmsUiImage2StateButton.h"
#include "ui/HmsUiImageButton.h"
#include "base/HmsTimer.h"
#include "ui/HmsUiImagePanel.h"
#include "base/HmsActionTimeRef.h"

class CHmsEventClippingNode : public CHmsClippingNode, public HmsUiEventInterface
{
public:
	CREATE_FUNC(CHmsEventClippingNode);

	CHmsEventClippingNode() :HmsUiEventInterface(this){}

	virtual void AddChild(CHmsNode * child)
	{
		CHmsClippingNode::AddChild(child);
		auto tempChild = dynamic_cast<HmsUiEventInterface*>(child);
		if (tempChild)
		{
			this->AddChildInterface(tempChild);
		}
	}
};

class CHmsListBox : public CHmsNode, public HmsUiEventInterface
{
public:
	CHmsListBox();
	~CHmsListBox();

	static CHmsListBox* CreateWithImage(
		const char * strBgImage,
		const char * strItemBgNormalImage, const char * strItemBgSelectImage,
		const char * strBtnNormalImage, const char * strBtnSelectImage);

	bool InitWithImage(
		const char * strBgImage,
		const char * strItemBgNormalImage, const char * strItemBgSelectImage,
		const char * strBtnNormalImage, const char * strBtnSelectImage);

public:

	void AppendItem(const std::vector<std::string> &vec, bool bBtn = true, bool bBorder = true, float fontSize = 16, bool bFontBorder = false);
	CHmsListBoxItem* AppendItem(const char *text, bool bBtn = true, bool bBorder = true, float fontSize = 16, bool bFontBorder = false);
	CHmsListBoxItem* AppendItem(CHmsListBoxItem *pItem, const char *text, bool bBorder = true, float fontSize = 16, bool bFontBorder = false);
	CHmsListBoxItem* AppendItemToIndex(int index, const std::string &text, bool bBtn = true, bool bBorder = true, float fontSize = 16, bool bFontBorder = false);
	CHmsListBoxItem* AppendItemToIndex(int index, CHmsListBoxItem *pItem, const char *text, bool bBorder = true, float fontSize = 16, bool bFontBorder = false);

	Size GetBoxSize();
	Size GetItemSize();
	int GetItemCount();
	int GetCurSelectIndex();
	CHmsListBoxItem* GetItem(int index);

	typedef std::function<void(CHmsListBoxItem*, int, bool)> HmsOnListItemSelect;
	void SetItemSelectChangeCallBack(HmsOnListItemSelect func){ m_itemSelectChangeFunc = func; }
	void SetItemCheckChangeCallBack(HmsOnListItemSelect func){ m_itemCheckChangeFunc = func; }

	void SetBoxSize(Size size);
	void SetItemHeight(float itemHeight);
	void SetItemNormalTextColor(Color4B color);
	void SetItemSelectTextColor(Color4B color);
	void SetItemTextAlignLeft(bool b);
	void SelectItem(int index, bool bEnableCallback = true);
	void SelectNext(bool bEnableCallback = true);
	void SelectPrevious(bool bEnableCallback = true);

	void SetItemMarginLeft(float margin);
	void SetItemMarginRight(float margin);
	void SetItemMarginTop(float margin);
	void SetItemMarginBottom(float margin);
	void SetItemMargin(float left, float right, float top, float bottom);

	void SetScrollbarWidth(float width){ m_scrollbarWidth = width; }
	void SetBacktopButtonVisible(bool b){ m_bBacktopBtnVisible = b; }
	void SetSelectWithCheck(bool b){ m_bSelectWithCheck = b; }

	void ScrollUp(float distance);
	void ScrollDown(float distance);
	void ScrollTo(float y);

	void Clear();

	void RemoveItemByIndex(int index);

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

private:
	void CalScrollBarSize();

private:
	RefPtr<CHmsStretchImageNode> m_pBoxBgNode;

	std::string m_strBg;
	std::string m_strItemBgNormal;
	std::string m_strItemBgSelect;
	std::string m_strBtnNormal;
	std::string m_strBtnSelect;

	Size m_boxSize;
	Size m_itemSize;
	bool m_bItemTextAlignLeft;
	std::vector<CHmsListBoxItem*> m_vecItem;	

	float m_fMarginLeft;
	float m_fMarginRight;
	float m_fMarginTop;
	float m_fMarginBottom;

	int m_curSelectItem;
	RefPtr<CHmsListBoxItem> m_pCurSelectItem;

	RefPtr<CHmsEventClippingNode>	m_pNodeClip;
	RefPtr<CHmsDrawNode>	m_pNodeStencil;

	RefPtr<CHmsUiPanel> m_pItemsContainer;

	RefPtr<CHmsStretchImageNode>	m_pVBar;
	RefPtr<CHmsUiImageButton>	m_pBackToTopBtn;

	Vec2 m_cursorPoinOnPress;
	float m_scrollbarYOnPress;
	float m_ratioBarDivideItems;
	//按住滚动条会滚动更快
	bool m_bPressOnScrollbar;
	
	bool m_bHasScrollbar;
	float m_scrollbarWidth;
	bool m_bBacktopBtnVisible;
	//计算滚动条时的最小高度
	float m_scrollbarCalMinHeight;
	float m_scrollbarCalHeight;
	float m_scrollbarDragMinHeight;

	Color4B m_itemNormalTextColor;
	Color4B m_itemSelectTextColor;

	CHmsTime m_time;

	HmsOnListItemSelect m_itemSelectChangeFunc;
	HmsOnListItemSelect m_itemCheckChangeFunc;

	bool m_bSelectWithCheck;
};

