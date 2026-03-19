#include "HmsListBox.h"

CHmsListBox::CHmsListBox()
:HmsUiEventInterface(this)
, m_itemSize(260,70)
, m_boxSize(260,500)
, m_curSelectItem(-1)
, m_pCurSelectItem(nullptr)
, m_bItemTextAlignLeft(true)
, m_fMarginLeft(0)
, m_fMarginRight(0)
, m_fMarginTop(0)
, m_fMarginBottom(0)
, m_ratioBarDivideItems(2.0)
, m_itemNormalTextColor(Color4B::WHITE)
, m_itemSelectTextColor(Color4B::WHITE)
, m_itemSelectChangeFunc(nullptr)
, m_itemCheckChangeFunc(nullptr)
, m_bBacktopBtnVisible(true)
, m_bSelectWithCheck(false)
, m_scrollbarWidth(10)
, m_scrollbarCalMinHeight(50)
, m_scrollbarDragMinHeight(5)
, m_bHasScrollbar(false)
{
	m_scrollbarCalHeight = 0;
}


CHmsListBox::~CHmsListBox()
{
}

CHmsListBox* CHmsListBox::CreateWithImage(
	const char * strBgImage,
	const char * strItemBgNormalImage, const char * strItemBgSelectImage,
	const char * strBtnNormalImage, const char * strBtnSelectImage)
{
	CHmsListBox *ret = new (std::nothrow) CHmsListBox();
	if (ret && ret->InitWithImage(strBgImage, strItemBgNormalImage, strItemBgSelectImage, strBtnNormalImage, strBtnSelectImage))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsListBox::InitWithImage(
	const char * strBgImage,
	const char * strItemBgNormalImage, const char * strItemBgSelectImage,
	const char * strBtnNormalImage, const char * strBtnSelectImage)
{
	this->SetContentSize(m_boxSize);

	if (strBgImage)
	{
		m_strBg = strBgImage;
		m_pBoxBgNode = CHmsStretchImageNode::Create(strBgImage);
		m_pBoxBgNode->SetAnchorPoint(Vec2(0, 0));
		m_pBoxBgNode->SetContentSize(m_boxSize);
		this->AddChild(m_pBoxBgNode);
	}

	if (strItemBgNormalImage) m_strItemBgNormal = strItemBgNormalImage;
	if (strItemBgSelectImage) m_strItemBgSelect = strItemBgSelectImage;
	if (strBtnNormalImage) m_strBtnNormal = strBtnNormalImage;
	if (strBtnSelectImage) m_strBtnSelect = strBtnSelectImage;

	m_pNodeStencil = CHmsDrawNode::Create();
	m_pNodeStencil->clear();
	m_pNodeStencil->DrawSolidRect(0, 0, m_boxSize.width, m_boxSize.height);
	m_pNodeClip = CHmsEventClippingNode::Create();
	m_pNodeClip->setStencil(m_pNodeStencil);
	m_pNodeClip->SetContentSize(Size(m_boxSize.width, m_boxSize.height));
	this->AddChild(m_pNodeClip);

	m_pItemsContainer = CHmsUiPanel::Create(m_boxSize);
	//m_pItemsContainer->SetAnchorPoint(0, 1);
	m_pItemsContainer->SetPosition(Vec2(0, m_boxSize.height));
	m_pNodeClip->AddChild(m_pItemsContainer);

	m_pVBar = CHmsStretchImageNode::Create("res/airport/scrollbar.png");
	m_pVBar->SetContentSize(Size(m_scrollbarWidth, m_scrollbarCalMinHeight));
	m_pVBar->SetAnchorPoint(Vec2(0, 1));
	m_pVBar->SetPosition(m_boxSize.width - m_scrollbarWidth, m_boxSize.height);
	this->AddChild(m_pVBar);

	m_pBackToTopBtn = CHmsUiImageButton::CreateWithImage("res/airport/back_to_top.png");
	m_pBackToTopBtn->SetColor(Color3B(0xd0, 0xd0, 0xd0));
	m_pBackToTopBtn->SetAnchorPoint(Vec2(1,0));
	m_pBackToTopBtn->SetPosition(m_boxSize.width*0.95, m_boxSize.height*0.2);
	m_pBackToTopBtn->SetVisible(false);
	m_pBackToTopBtn->SetCallbackFunc([=](CHmsUiButtonAbstract * btn, HMS_UI_BUTTON_STATE eState){
		auto pNode = dynamic_cast<CHmsImageNode*>(btn);
		switch (eState)
		{
		case HmsAviPf::HMS_UI_BUTTON_STATE::btn_normal:
			pNode->SetColor(Color3B(0xd0, 0xd0, 0xd0));
			pNode->SetScale(1.0);
			break;
		case HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected:
			pNode->SetColor(Color3B::WHITE);
			pNode->SetScale(1.1F);
			ScrollTo(m_boxSize.height);
			break;
		case HmsAviPf::HMS_UI_BUTTON_STATE::btn_disable:
			break;
		default:
			break;
		}
	});
	this->AddChild(m_pBackToTopBtn);

	this->RegisterAllNodeChild();
	
	return true;
}

void CHmsListBox::AppendItem(const std::vector<std::string> &vec, 
	bool bBtn /*= true*/, 
	bool bBorder /*= true*/, 
	float fontSize /*= 16*/, 
	bool bFontBorder /*= false*/)
{
    for (int i = 0; i < (int)vec.size(); ++i)
	{
		AppendItem(vec.at(i).c_str(), bBtn, bBorder, fontSize, bFontBorder);
	}
}

CHmsListBoxItem* CHmsListBox::AppendItem(const char *text,
	bool bBtn /*= true*/, 
	bool bBorder /*= true*/, 
	float fontSize /*= 16*/,
	bool bFontBorder /*= false*/)
{
	CHmsListBoxItem *pItem = CHmsListBoxItem::CreateWithImage(
		m_strItemBgNormal.empty() ? (NULL) : m_strItemBgNormal.c_str(),
		m_strItemBgSelect.empty() ? (NULL) : m_strItemBgSelect.c_str(),
		m_strBtnNormal.empty() ? (NULL) : (bBtn ? m_strBtnNormal.c_str() : NULL),
		m_strBtnSelect.empty() ? (NULL) : (bBtn ? m_strBtnSelect.c_str() : NULL));

	return AppendItem(pItem, text, bBorder, fontSize, bFontBorder);
}

CHmsListBoxItem* CHmsListBox::AppendItem(CHmsListBoxItem *pItem, const char *text, bool bBorder, float fontSize, bool bFontBorder)
{
	return AppendItemToIndex(m_pItemsContainer->GetChildrenCount(), pItem,
		text,
		bBorder,
		fontSize,
		bFontBorder);
}


CHmsListBoxItem* CHmsListBox::AppendItemToIndex(int index, const std::string &text, bool bBtn /*= true*/, bool bBorder /*= true*/, float fontSize /*= 16*/, bool bFontBorder /*= false*/)
{
	CHmsListBoxItem *pItem = CHmsListBoxItem::CreateWithImage(
		m_strItemBgNormal.empty() ? (NULL) : m_strItemBgNormal.c_str(),
		m_strItemBgSelect.empty() ? (NULL) : m_strItemBgSelect.c_str(),
		m_strBtnNormal.empty() ? (NULL) : (bBtn ? m_strBtnNormal.c_str() : NULL),
		m_strBtnSelect.empty() ? (NULL) : (bBtn ? m_strBtnSelect.c_str() : NULL));

	return AppendItemToIndex(index, pItem,
		text.c_str(),
		bBorder,
		fontSize,
		bFontBorder);
}

CHmsListBoxItem* CHmsListBox::AppendItemToIndex(int index, CHmsListBoxItem *pItem,
	const char *text, 
	bool bBorder /*= true*/, 
	float fontSize /*= 16*/,
	bool bFontBorder /*= false*/)
{
	int itemCount = m_vecItem.size();

	if (index < 0)
	{
		index = 0;
	}
	if (index > itemCount)
	{
		index = itemCount;
	}

	m_vecItem.insert(m_vecItem.begin() + index, pItem);

	m_itemSize.width = m_boxSize.width - m_fMarginLeft - m_fMarginRight;

    for (int i = index; i < (int)m_vecItem.size(); i++)
	{
		m_vecItem[i]->SetPositionY(-(i + 1)* (m_itemSize.height + m_fMarginTop + m_fMarginBottom));
	}

	pItem->SetItemSize(m_itemSize);
	pItem->SetAnchorPoint(Vec2(0, 0));
	pItem->SetPosition(m_fMarginLeft, 0 - (index + 1) * (m_itemSize.height + m_fMarginTop + m_fMarginBottom));
	pItem->SetNormalTextColor(m_itemNormalTextColor);
	pItem->SetSelectTextColor(m_itemSelectTextColor);
	pItem->SetBorderVisiable(bBorder);
	pItem->SetTextAlignLeft(m_bItemTextAlignLeft);
	pItem->SetText(text, fontSize, m_itemNormalTextColor, bFontBorder);
	if (m_bSelectWithCheck)
	{
		pItem->SetCheckable(false);
	}

	pItem->SetOnSelectChangeFunc([=](CHmsListBoxItem *pItem, bool selected){
		if (m_pCurSelectItem && m_pCurSelectItem != pItem)
		{
			m_pCurSelectItem->SetUnSelect();
			if (m_bSelectWithCheck)
			{
				m_pCurSelectItem->SetUnChecked();
			}
		}
		m_pCurSelectItem = pItem;
		if (m_bSelectWithCheck)
		{
			m_pCurSelectItem->SetChecked();
		}

		if (m_itemSelectChangeFunc)
		{
			int curIndex = GetCurSelectIndex();
			m_itemSelectChangeFunc(pItem, curIndex, selected);
		}
	});
	pItem->SetOnCheckChangeFunc([=](CHmsListBoxItem *pItem, bool checked){
		if (m_itemCheckChangeFunc)
		{
			auto iter = std::find(m_vecItem.begin(), m_vecItem.end(), pItem);
			if (iter != m_vecItem.end())
			{
				int intex = std::distance(m_vecItem.begin(), iter);
				m_itemCheckChangeFunc(pItem, intex, checked);
			}
		}
	});

	m_pItemsContainer->AddChild(pItem);

	auto intf = dynamic_cast<HmsUiEventInterface*>(pItem);
	if (intf)
	{
		m_pItemsContainer->AddChildInterface(intf);
	}

	m_pItemsContainer->SetContentSize(Size(m_itemSize.width, 0 - m_vecItem.size() * (m_itemSize.height + m_fMarginTop + m_fMarginBottom)));

	CalScrollBarSize();

	return pItem;
}

void CHmsListBox::CalScrollBarSize()
{
	m_ratioBarDivideItems = 2.0;
	if (m_vecItem.size()*(m_itemSize.height + m_fMarginTop + m_fMarginBottom) > m_boxSize.height)
	{
		m_pVBar->SetVisible(true);
		if (m_bBacktopBtnVisible)
		{
			m_pBackToTopBtn->SetVisible(true);
		}
		else
		{
			m_pBackToTopBtn->SetVisible(false);
		}

		float barHeight = 0;
		while (barHeight < m_scrollbarCalMinHeight)
		{
			barHeight = m_boxSize.height - (m_vecItem.size()*(m_itemSize.height + m_fMarginTop + m_fMarginBottom) - m_boxSize.height) / m_ratioBarDivideItems;
			if (barHeight < m_scrollbarCalMinHeight)
			{
				m_ratioBarDivideItems += 0.5;
			}
		}
		m_pVBar->SetContentSize(Size(m_scrollbarWidth, barHeight));
		m_pVBar->SetAnchorPoint(Vec2(0, 1));
		m_pVBar->SetPosition(m_boxSize.width - m_scrollbarWidth, m_boxSize.height);

		m_scrollbarCalHeight = barHeight;
		m_bHasScrollbar = true;
	}
	else
	{
		m_pVBar->SetVisible(false);
		m_pBackToTopBtn->SetVisible(false);
		m_bHasScrollbar = false;

		m_pVBar->SetContentSize(Size(m_scrollbarWidth, m_scrollbarCalMinHeight));
		m_pVBar->SetAnchorPoint(Vec2(0, 1));
		m_pVBar->SetPosition(m_boxSize.width - m_scrollbarWidth, m_boxSize.height);
		m_pItemsContainer->SetPosition(Vec2(0, m_boxSize.height));
	}
}

void CHmsListBox::SelectItem(int index, bool bEnableCallback /*= true*/)
{
	if (index < 0)
	{
		if (m_pCurSelectItem)
		{
			m_pCurSelectItem->SetUnSelect(bEnableCallback);
		}
		m_pCurSelectItem = nullptr;
	}
	else
	{
        if (index < (int)m_vecItem.size())
		{
			m_vecItem[index]->SetSelect(bEnableCallback);

			if (m_pCurSelectItem && m_pCurSelectItem != m_vecItem[index])
			{
				m_pCurSelectItem->SetUnSelect(bEnableCallback);
			}
			m_pCurSelectItem = m_vecItem[index];
		}
	}
}

void CHmsListBox::Clear()
{
    for (int i = 0; i < (int)m_vecItem.size(); ++i)
	{
		m_vecItem[i]->SetUnSelect();
		m_vecItem[i]->SetUnChecked(false);
		m_vecItem[i]->RemoveFromParent();

		auto intf = dynamic_cast<HmsUiEventInterface*>(m_vecItem[i]);
		if (intf)
		{
			intf->RemoveFromParentInterface();
		}
	}
	m_vecItem.clear();
	m_pCurSelectItem = nullptr;

	CalScrollBarSize();
}

void CHmsListBox::RemoveItemByIndex(int index)
{
    if (index <0 || (int)m_vecItem.size() <= index)
	{
		return;
	}

	m_vecItem[index]->SetUnSelect();
	m_vecItem[index]->SetUnChecked(false);
	m_vecItem[index]->RemoveFromParent();

	auto intf = dynamic_cast<HmsUiEventInterface*>(m_vecItem[index]);
	if (intf)
	{
		intf->RemoveFromParentInterface();
	}

	std::vector<CHmsListBoxItem*>::iterator   iter = m_vecItem.begin() + index;
	m_vecItem.erase(iter);

    for (int i = index; i < (int)m_vecItem.size(); i++)
	{
		m_vecItem[i]->SetPositionY(-(i + 1) * (m_itemSize.height + m_fMarginTop + m_fMarginBottom));
	}
	m_pCurSelectItem = nullptr;
	CalScrollBarSize();
}

bool CHmsListBox::OnPressed(const Vec2 & posOrigin)
{
	m_cursorPoinOnPress = posOrigin;
	m_scrollbarYOnPress = m_pVBar->GetPoistion().y;

	auto rect = m_pVBar->GetRectFromParent();
	if (rect.containsPoint(posOrigin))
	{
		m_bPressOnScrollbar = true;
	}
	else
	{
		m_bPressOnScrollbar = false;
	}
	m_time.RecordCurrentTime();
	
	return true;
}

void CHmsListBox::OnMove(const Vec2 & posOrigin)
{
	if (m_bPressOnScrollbar)
	{
		float y = (posOrigin.y - m_cursorPoinOnPress.y) + m_scrollbarYOnPress;
		ScrollTo(y);
	}
	else
	{
		float y = -(posOrigin.y - m_cursorPoinOnPress.y) / m_ratioBarDivideItems + m_scrollbarYOnPress;
		ScrollTo(y);
	}
}

void CHmsListBox::OnReleased(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_cursorPoinOnPress;
	auto deltaTime = m_time.GetElapsed();
	if (deltaTime < 2 && fabsf(delta.y) < 10)
	{
		//是点击事件
		HmsUiEventInterface::OnPressed(posOrigin);
		HmsUiEventInterface::OnReleased(posOrigin);
	}
	else
	{
		auto y = m_pVBar->GetPoistion().y;
		//判断边界
		if (y < m_scrollbarCalHeight)
		{
			y = m_scrollbarCalHeight;

			m_pVBar->SetContentSize(Size(m_pVBar->GetContentSize().width, m_scrollbarCalHeight));
			m_pVBar->SetPosition(m_boxSize.width - m_scrollbarWidth, m_scrollbarCalHeight);
		}
		if (y >= m_boxSize.height && m_pVBar->GetContentSize().height<m_scrollbarCalHeight)
		{
			y = m_boxSize.height;

			m_pVBar->SetContentSize(Size(m_pVBar->GetContentSize().width, m_scrollbarCalHeight));
			m_pVBar->SetPosition(m_boxSize.width - m_scrollbarWidth, m_boxSize.height);
		}
		
		auto distance = (m_boxSize.height - y)*m_ratioBarDivideItems;
		auto moveto = CHmsActionMoveTo::Create(0.3f, Vec2(0, m_boxSize.height + distance));
		m_pItemsContainer->RunAction(moveto);
	}
}

void CHmsListBox::SetBoxSize(Size size)
{
	m_boxSize = size;

	this->SetContentSize(m_boxSize);
	if (m_pBoxBgNode)
	{
		m_pBoxBgNode->SetContentSize(m_boxSize);
	}
	m_pNodeStencil->clear();
	m_pNodeStencil->DrawSolidRect(0, 0, m_boxSize.width, m_boxSize.height);
	m_pNodeClip->SetContentSize(m_boxSize);

	m_pItemsContainer->SetPosition(Vec2(0, m_boxSize.height));

	m_pVBar->SetPosition(m_boxSize.width - m_scrollbarWidth, m_boxSize.height);

	m_pBackToTopBtn->SetPosition(m_boxSize.width*0.95, m_boxSize.height*0.2);
	//
	m_itemSize.width = m_boxSize.width - m_fMarginLeft - m_fMarginRight;
    for (int row = 0; row < (int)m_vecItem.size(); ++row)
	{
		CHmsListBoxItem *pItem = m_vecItem[row];
		pItem->SetItemSize(m_itemSize);
	}

	CalScrollBarSize();
}

Size CHmsListBox::GetBoxSize()
{
	return m_boxSize;
}

void CHmsListBox::SetItemHeight(float itemHeight)
{
	m_itemSize.height = itemHeight;
}

Size CHmsListBox::GetItemSize()
{
	return m_itemSize;
}

void CHmsListBox::SetItemNormalTextColor(Color4B color)
{
	m_itemNormalTextColor = color;
}

void CHmsListBox::SetItemSelectTextColor(Color4B color)
{
	m_itemSelectTextColor = color;
}

void CHmsListBox::SetItemTextAlignLeft(bool b)
{
	m_bItemTextAlignLeft = b;
}

void CHmsListBox::SetItemMarginLeft(float margin)
{
	m_fMarginLeft = margin;
}

void CHmsListBox::SetItemMarginRight(float margin)
{
	m_fMarginRight = margin;
}

void CHmsListBox::SetItemMarginTop(float margin)
{
	m_fMarginTop = margin;
}

void CHmsListBox::SetItemMarginBottom(float margin)
{
	m_fMarginBottom = margin;
}

void CHmsListBox::SetItemMargin(float left, float right, float top, float bottom)
{
	m_fMarginLeft = left;
	m_fMarginRight = right;
	m_fMarginTop = top;
	m_fMarginBottom = bottom;
}

int CHmsListBox::GetItemCount()
{
	return m_vecItem.size();
}

void CHmsListBox::SelectNext(bool bEnableCallback /*= true*/)
{
	if (m_vecItem.empty())
	{
		return;
	}
	int curIndex = GetCurSelectIndex();
	++curIndex;
    if (curIndex >= (int)m_vecItem.size())
	{
		curIndex = 0;
		ScrollTo(m_boxSize.height);
	}
	SelectItem(curIndex, bEnableCallback);

	float y = m_pCurSelectItem->GetPoistion().y + m_pItemsContainer->GetPoistion().y;
	if (y < 0)
	{
		ScrollDown(-y / m_ratioBarDivideItems);
	}
	if (!m_pCurSelectItem->IsSelectable())
	{
		SelectNext(bEnableCallback);
	}
}

void CHmsListBox::SelectPrevious(bool bEnableCallback /*= true*/)
{
	if (m_vecItem.empty())
	{
		return;
	}
	int curIndex = GetCurSelectIndex();
	--curIndex;
	if (curIndex < 0)
	{
		curIndex = m_vecItem.size() - 1;
		ScrollTo(m_scrollbarCalHeight);
	}
	SelectItem(curIndex, bEnableCallback);

	float y = m_pCurSelectItem->GetPoistion().y + m_pItemsContainer->GetPoistion().y + m_itemSize.height;
	if (y > m_boxSize.height)
	{
		ScrollUp((y - m_boxSize.height)/ m_ratioBarDivideItems);
	}
	if (!m_pCurSelectItem->IsSelectable())
	{
		SelectPrevious(bEnableCallback);
	}
}

int CHmsListBox::GetCurSelectIndex()
{
	if (!m_pCurSelectItem)
	{
		return -1;
	}
	auto iter = std::find(m_vecItem.begin(), m_vecItem.end(), m_pCurSelectItem);
	if (iter != m_vecItem.end())
	{
		return std::distance(m_vecItem.begin(), iter);
	}
	else
	{
		return -1;
	}
}

void CHmsListBox::ScrollUp(float distance)
{
	float y = m_pVBar->GetPoistion().y + distance;
	ScrollTo(y);
}

void CHmsListBox::ScrollDown(float distance)
{
	float y = m_pVBar->GetPoistion().y - distance;
	ScrollTo(y);
}

void CHmsListBox::ScrollTo(float y)
{
	if (!m_bHasScrollbar && y<m_pVBar->GetPoistion().y)
	{
		return;
	}
	auto barHPos = y;
	if (barHPos < m_pVBar->GetContentSize().height)
	{
		auto s = m_pVBar->GetContentSize();
		auto h = barHPos;
		if (h<m_scrollbarDragMinHeight) h = m_scrollbarDragMinHeight;
		m_pVBar->SetContentSize(Size(s.width, h));

		barHPos = m_pVBar->GetContentSize().height;
	}
	if (barHPos > m_boxSize.height)
	{
		auto s = m_pVBar->GetContentSize();
		auto h = m_scrollbarCalHeight - (barHPos - m_boxSize.height);
		if (h < m_scrollbarDragMinHeight) h = m_scrollbarDragMinHeight;
		m_pVBar->SetContentSize(Size(s.width, h));

		barHPos = m_boxSize.height;
	}
	m_pVBar->SetPosition(m_boxSize.width - m_scrollbarWidth, barHPos);

	float distance = (m_boxSize.height - y)*m_ratioBarDivideItems;
	m_pItemsContainer->SetPosition(Vec2(0, m_boxSize.height + distance));
}

CHmsListBoxItem* CHmsListBox::GetItem(int index)
{
    if (index < 0 || index >= (int)m_vecItem.size())
	{
		return nullptr;
	}
	return m_vecItem[index];
}
