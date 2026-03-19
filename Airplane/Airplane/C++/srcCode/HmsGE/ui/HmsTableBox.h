#pragma once

#include <string>
#include <vector>
#include "base/HmsClippingNode.h"
#include "ui/HmsUiImage2StateButton.h"
#include "ui/HmsUiImageButton.h"
#include "base/HmsTimer.h"
#include "ui/HmsUiImagePanel.h"
#include "base/HmsActionTimeRef.h"
#include "display/HmsLabel.h"
#include "ui/HmsUiStretchButton.h"

#include "HmsListBox.h"

using namespace HmsAviPf;

enum class TableItemAlign
{
	ALIGN_LEFT,
	ALIGN_CENTER,
	ALIGN_RIGHT
};

class CHmsTableBox : public CHmsNode, public HmsUiEventInterface
{
public:
	CHmsTableBox();
	~CHmsTableBox();

	static CHmsTableBox* CreateWithImage(const char * strBgImage, int labelFontSize = 26);

    bool InitWithImage(const char * strBgImage, int labelFontSize);

public:
	void SetColumnCount(int count);
	void SetColumnWidth(int index, int width);
    void SetHeaderLabel(int column, const char *text, bool leftBorder, int labelFontSize = 26);
	//返回添加的行索引
	int AppendRow();
	CHmsLabel * AppendTextItem(int row, int column, const char *text, TableItemAlign align = TableItemAlign::ALIGN_CENTER
        , int maxLineWidth = 0, int maxLineCount = 0);
	CHmsUiStretchButton * AppendButtonItem(int row, int column, TableItemAlign align = TableItemAlign::ALIGN_CENTER);
	CHmsImageNode * AppendImageItem(int row, int column, const Color3B &imageColor, TableItemAlign align = TableItemAlign::ALIGN_CENTER);
	void RemoveRow(int rowIndex);

	void ScrollToDefault();

	Size GetBoxSize();
	int GetCurSelectRowIndex();
	int GetRowCount(){ return m_rowCount; }
	void GetCellIndex(CHmsNode *pNode, int &row, int &column);
	void SelectRow(int rowIndex);

	CHmsNode* GetCell(int row, int column);

	void SetBoxSize(Size size);
	void SetItemHeight(float itemHeight);
    void SetHeaderHeight(float height);

	void ScrollUp(float distance);
	void ScrollDown(float distance);
	void ScrollTo(float y);
	float GetScrollPosition();
	void EdgeDetection();

	void Clear();

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

	void SetClickCellCallback(std::function<void(int,int)> func);

private:
	void CalScrollBarSize();
	void PushItemAndAlign(int row, int column, CHmsNode *pItem, TableItemAlign align);

private:
	RefPtr<CHmsStretchImageNode> m_pBoxBgNode;
	RefPtr<CHmsDrawNode> m_pItemSelectBgNode;

	Size m_boxSize;
	Size m_boxContainerSize;
	float m_itemHeight;

	std::vector< std::vector<CHmsNode*> > m_vec2Items;

	int m_curSelectRowIndex;

	RefPtr<CHmsEventClippingNode>	m_pNodeClip;
	RefPtr<CHmsDrawNode>	m_pNodeStencil;
	RefPtr<CHmsUiPanel> m_pItemsContainer;

	RefPtr<CHmsStretchImageNode>	m_pVBar;

	Vec2 m_cursorPoinOnPress;
	float m_scrollbarYOnPress;
	float m_ratioBarDivideItems;
	//按住滚动条会滚动更快
	bool m_bPressOnScrollbar;

	bool m_bHasScrollbar;
	float m_scrollbarWidth;
	//计算滚动条时的最小高度
	float m_scrollbarCalMinHeight;
	float m_scrollbarCalHeight;
	float m_scrollbarDragMinHeight;

	Color4B m_itemNormalTextColor;
	Color4B m_itemSelectTextColor;

	CHmsTime m_time;

	int m_headerHeight;
	RefPtr<CHmsDrawNode> m_pHeaderGridNode;
    RefPtr<CHmsDrawNode> m_pContentGridNode;
	int m_columnCount;
	std::vector<Rect> m_vecColumnRect;
	std::vector<Rect> m_vecRowRect;
	int m_rowCount;
	int m_itemSpace;
	int m_gridLineWidth;

	HmsAviPf::Vector<CHmsLabel*> m_vecLabelPool;
	int m_nCurPoolItemIndex;

	std::function<void(int,int)> m_callbackClickCell;

    int m_labelFontSize;
};

