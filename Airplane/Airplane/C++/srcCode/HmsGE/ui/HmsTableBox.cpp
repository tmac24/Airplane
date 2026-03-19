#include "HmsTableBox.h"

CHmsTableBox::CHmsTableBox()
:HmsUiEventInterface(this)
, m_itemHeight(40)
, m_boxSize(260, 500)
, m_curSelectRowIndex(-1)
, m_ratioBarDivideItems(2.0)
, m_itemNormalTextColor(Color4B::WHITE)
, m_itemSelectTextColor(Color4B::WHITE)
, m_headerHeight(30)
, m_itemSpace(10)
, m_gridLineWidth(2)
, m_rowCount(0)
, m_scrollbarWidth(10)
, m_scrollbarCalMinHeight(50)
, m_scrollbarDragMinHeight(5)
, m_callbackClickCell(nullptr)
, m_labelFontSize(32)
{
	m_scrollbarCalHeight = 0;
}

CHmsTableBox::~CHmsTableBox()
{
}

CHmsTableBox* CHmsTableBox::CreateWithImage(const char * strBgImage, int labelFontSize)
{
	CHmsTableBox *ret = new (std::nothrow) CHmsTableBox();
	if (ret && ret->InitWithImage(strBgImage, labelFontSize))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsTableBox::InitWithImage(const char * strBgImage, int labelFontSize)
{
	if (strBgImage)
	{
		m_pBoxBgNode = CHmsStretchImageNode::Create(strBgImage);
		this->AddChild(m_pBoxBgNode);
	}

    m_labelFontSize = labelFontSize;

	m_pNodeStencil = CHmsDrawNode::Create();
	m_pNodeClip = CHmsEventClippingNode::Create();
	m_pNodeClip->setStencil(m_pNodeStencil);
	this->AddChild(m_pNodeClip);

	m_pItemsContainer = CHmsUiPanel::Create(m_boxContainerSize);
	m_pNodeClip->AddChild(m_pItemsContainer);

    m_pHeaderGridNode = CHmsDrawNode::Create();
	this->AddChild(m_pHeaderGridNode);

    m_pContentGridNode = CHmsDrawNode::Create();
	m_pContentGridNode->SetContentSize(m_boxContainerSize);
	m_pContentGridNode->SetPosition(0, 0);
	m_pItemsContainer->AddChild(m_pContentGridNode);

	m_pItemSelectBgNode = CHmsDrawNode::Create();
	m_pItemsContainer->AddChild(m_pItemSelectBgNode);

	m_pVBar = CHmsStretchImageNode::Create("res/airport/key_char_normal.png");
	this->AddChild(m_pVBar);

	for (int i = 0; i < 12*15; ++i)
	{
        auto pItem = CHmsGlobal::CreateLabel("text", m_labelFontSize);
		pItem->SetTextColor(m_itemNormalTextColor); 
		m_vecLabelPool.pushBack(pItem);
	}
	m_nCurPoolItemIndex = 0;



	this->RegisterAllNodeChild();

	SetBoxSize(m_boxSize);

	return true;
}

void CHmsTableBox::CalScrollBarSize()
{
	m_ratioBarDivideItems = 2.0;
	if (m_rowCount*m_itemHeight > m_boxContainerSize.height)
	{
		m_pVBar->SetVisible(true);

		float barHeight = 0;
		while (barHeight < m_scrollbarCalMinHeight)
		{
			barHeight = m_boxContainerSize.height - (m_rowCount*m_itemHeight - m_boxContainerSize.height) / m_ratioBarDivideItems;
			if (barHeight < m_scrollbarCalMinHeight)
			{
				m_ratioBarDivideItems += 0.5;
			}
		}
		m_pVBar->SetContentSize(Size(m_scrollbarWidth, barHeight));
		m_pVBar->SetAnchorPoint(Vec2(0, 1));
		m_pVBar->SetPosition(m_boxContainerSize.width - m_scrollbarWidth, m_boxContainerSize.height);

		m_scrollbarCalHeight = barHeight;
		m_bHasScrollbar = true;
	}
	else
	{
		m_pVBar->SetVisible(false);
		m_bHasScrollbar = false;

		m_pVBar->SetContentSize(Size(m_scrollbarWidth, m_scrollbarCalMinHeight));
		m_pVBar->SetAnchorPoint(Vec2(0, 1));
		m_pVBar->SetPosition(m_boxContainerSize.width - m_scrollbarWidth, m_boxContainerSize.height);
		m_pItemsContainer->SetPosition(Vec2(0, m_boxContainerSize.height));
	}
}

void CHmsTableBox::Clear()
{
	for (int i = 0; i < m_vecLabelPool.size(); ++i)
	{
		auto pItem = m_vecLabelPool.at(i);
		pItem->RemoveFromParent();
	}
	m_nCurPoolItemIndex = 0;

	for (int row = 0; row < (int)m_vec2Items.size(); ++row)
	{
		for (int col = 0; col < (int)m_vec2Items[row].size(); ++col)
		{
			if (m_vec2Items[row][col])
			{
				m_vec2Items[row][col]->RemoveFromParent();
			}
		}
	}
	m_vec2Items.clear();
	m_vecRowRect.clear();
	m_rowCount = 0;

	m_pContentGridNode->clear();

	CalScrollBarSize();

	SelectRow(-1);
}

bool CHmsTableBox::OnPressed(const Vec2 & posOrigin)
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

void CHmsTableBox::OnMove(const Vec2 & posOrigin)
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

void CHmsTableBox::OnReleased(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_cursorPoinOnPress;
	auto deltaTime = m_time.GetElapsed();
	if (deltaTime < 2 && fabsf(delta.y) < 10)
	{
		//是点击事件
		HmsUiEventInterface::OnPressed(posOrigin);
		HmsUiEventInterface::OnReleased(posOrigin);

		if (m_callbackClickCell)
		{
			int selectRow = -1;
			int selectCol = -1;
			for (int row = 0; row < (int)m_vecRowRect.size(); ++row)
			{
				auto temp = m_pItemsContainer->GetPoistion();
				Rect r;
				r.origin = m_vecRowRect[row].origin + m_pItemsContainer->GetPoistion();
				r.size = m_vecRowRect[row].size;
				if (r.containsPoint(posOrigin))
				{
					selectRow = row;
					break;
				}
			}
            for (int col = (int)m_vecColumnRect.size() - 1; col >= 0; --col)
			{
				if (posOrigin.x >= m_vecColumnRect[col].origin.x)
				{
					selectCol = col;
					break;
				}
			}
			m_callbackClickCell(selectRow, selectCol);
		}
	}
	else
	{
		EdgeDetection();
	}
}

void CHmsTableBox::SetBoxSize(Size size)
{
	m_boxSize = size;
	m_boxContainerSize = m_boxSize - Size(0, m_headerHeight);

	this->SetContentSize(m_boxSize);
	if (m_pBoxBgNode)
	{
		m_pBoxBgNode->SetContentSize(m_boxSize);
	}
	m_pNodeStencil->clear();
	m_pNodeStencil->DrawSolidRect(0, 0, m_boxContainerSize.width, m_boxContainerSize.height);
	m_pNodeClip->SetContentSize(m_boxContainerSize);
	m_pItemsContainer->SetPosition(Vec2(0, m_boxContainerSize.height));
	m_pVBar->SetPosition(m_boxContainerSize.width - m_scrollbarWidth, m_boxContainerSize.height);

    m_pHeaderGridNode->clear();
	m_pHeaderGridNode->SetContentSize(Size(m_boxSize.width, m_headerHeight));
	m_pHeaderGridNode->SetPosition(Vec2(0, m_boxContainerSize.height));
	std::vector<Vec2> vec;
	vec.push_back(Vec2(0, 0));
	vec.push_back(Vec2(m_boxSize.width, 0));
	vec.push_back(Vec2(0, m_headerHeight));
	vec.push_back(Vec2(m_boxSize.width, m_headerHeight));
	m_pHeaderGridNode->DrawLines(vec, m_gridLineWidth, Color4B(0x0e, 0x19, 0x25, 0xff));

	m_pItemSelectBgNode->clear();
	m_pItemSelectBgNode->SetContentSize(Size(m_boxContainerSize.width, m_itemHeight));
	m_pItemSelectBgNode->SetPosition(0, 0 - m_itemHeight);
	m_pItemSelectBgNode->SetVisible(false);
	m_pItemSelectBgNode->DrawSolidRect(Rect(0, 0, m_boxContainerSize.width, m_itemHeight), Color4F(Color3B(0x0e, 0x19, 0x25)));
	//
	CalScrollBarSize();
}

Size CHmsTableBox::GetBoxSize()
{
	return m_boxSize;
}

void CHmsTableBox::SetItemHeight(float itemHeight)
{
	m_itemHeight = itemHeight;
}

void CHmsTableBox::SetHeaderHeight(float height)
{
    m_headerHeight = height;
}

int CHmsTableBox::GetCurSelectRowIndex()
{
	return m_curSelectRowIndex;
}

void CHmsTableBox::ScrollUp(float distance)
{
	float y = m_pVBar->GetPoistion().y + distance;
	ScrollTo(y);
}

void CHmsTableBox::ScrollDown(float distance)
{
	float y = m_pVBar->GetPoistion().y - distance;
	ScrollTo(y);
}

void CHmsTableBox::ScrollTo(float y)
{
	if (!m_bHasScrollbar && y < m_pVBar->GetPoistion().y)
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
	if (barHPos > m_boxContainerSize.height)
	{
		auto s = m_pVBar->GetContentSize();
		auto h = m_scrollbarCalHeight - (barHPos - m_boxContainerSize.height);
		if (h < m_scrollbarDragMinHeight) h = m_scrollbarDragMinHeight;
		m_pVBar->SetContentSize(Size(s.width, h));

		barHPos = m_boxContainerSize.height;
	}
	m_pVBar->SetPosition(m_boxContainerSize.width - m_scrollbarWidth, barHPos);

	float distance = (m_boxContainerSize.height - y)*m_ratioBarDivideItems;
	m_pItemsContainer->SetPosition(Vec2(0, m_boxContainerSize.height + distance));
}

float CHmsTableBox::GetScrollPosition()
{
	if (m_bHasScrollbar)
	{
		return m_pVBar->GetPoistion().y;
	}
	else
	{
		return 0;
	}
}

void CHmsTableBox::EdgeDetection()
{
	auto y = m_pVBar->GetPoistion().y;
	//判断边界
	if (y < m_scrollbarCalHeight)
	{
		y = m_scrollbarCalHeight;

		m_pVBar->SetContentSize(Size(m_pVBar->GetContentSize().width, m_scrollbarCalHeight));
		m_pVBar->SetPosition(m_boxContainerSize.width - m_scrollbarWidth, m_scrollbarCalHeight);
	}
	if (y >= m_boxContainerSize.height && m_pVBar->GetContentSize().height < m_scrollbarCalHeight)
	{
		y = m_boxContainerSize.height;

		m_pVBar->SetContentSize(Size(m_pVBar->GetContentSize().width, m_scrollbarCalHeight));
		m_pVBar->SetPosition(m_boxContainerSize.width - m_scrollbarWidth, m_boxContainerSize.height);
	}

	auto distance = (m_boxContainerSize.height - y)*m_ratioBarDivideItems;
	auto moveto = CHmsActionMoveTo::Create(0.3f, Vec2(0, m_boxContainerSize.height + distance));
	m_pItemsContainer->StopAllActions();
	m_pItemsContainer->RunAction(moveto);
}

void CHmsTableBox::SetHeaderLabel(int column, const char *text, bool leftBorder, int labelFontSize)
{
    auto pItem = CHmsGlobal::CreateLanguageLabelUtf8(text, labelFontSize)->GetLabel();
	pItem->SetTextColor(m_itemNormalTextColor);
	pItem->SetAnchorPoint(Vec2(0, 0.5));
	pItem->SetPosition(m_vecColumnRect[column].origin.x + 10, m_boxContainerSize.height + m_headerHeight/2);
	this->AddChild(pItem);
	if (leftBorder)
	{
		std::vector<Vec2> vec;
		vec.push_back(Vec2(m_vecColumnRect[column].origin.x, 0));
		vec.push_back(Vec2(m_vecColumnRect[column].origin.x, m_headerHeight));
		m_pHeaderGridNode->DrawLines(vec, m_gridLineWidth, Color4B(0x0e, 0x19, 0x25, 0xff));
	}
}

void CHmsTableBox::SetColumnCount(int count)
{
	m_columnCount = count;

	m_vecColumnRect.clear();
	auto tempWidth = (m_boxContainerSize.width - m_scrollbarWidth) / count;
	for (int i = 0; i < count; ++i)
	{
		Rect re;
		re.size.width = tempWidth;
		re.size.height = m_itemHeight;
		re.origin.x = i*re.size.width;
		re.origin.y = 0;

		m_vecColumnRect.push_back(re);
	}
}

void CHmsTableBox::SetColumnWidth(int index, int width)
{
	if (index < (int)m_vecColumnRect.size())
	{
		auto temp = m_vecColumnRect[index].size.width - width;
		m_vecColumnRect[index].size.width = width;

		temp = temp / (m_vecColumnRect.size() - index - 1);
		for (int i = index + 1; i< (int)m_vecColumnRect.size(); ++i)
		{
			m_vecColumnRect[i].size.width += temp;
		}
		for (int i = 1; i < (int)m_vecColumnRect.size(); ++i)
		{
			m_vecColumnRect[i].origin.x = m_vecColumnRect[i - 1].origin.x + m_vecColumnRect[i - 1].size.width;
		}
	}
}

int CHmsTableBox::AppendRow()
{
	++m_rowCount;

	std::vector<CHmsNode*> vecRow(m_columnCount, nullptr);
	m_vec2Items.push_back(vecRow);

	auto y = 0 - m_rowCount * m_itemHeight;
	m_pContentGridNode->SetContentSize(Size(m_boxContainerSize.width, y));
	m_pItemsContainer->SetContentSize(Size(m_boxContainerSize.width, y));
	CalScrollBarSize();

	std::vector<Vec2> vec;
	vec.push_back(Vec2(0, y));
	vec.push_back(Vec2(m_boxSize.width, y));
	for (int i = 3; i < (int)m_vecColumnRect.size(); ++i)
	{
		vec.push_back(Vec2(m_vecColumnRect[i].origin.x, y));
		vec.push_back(Vec2(m_vecColumnRect[i].origin.x, y + m_itemHeight));
	}
	m_pContentGridNode->DrawLines(vec, m_gridLineWidth, Color4B(0x0e, 0x19, 0x25, 0xff));

	m_vecRowRect.push_back(Rect(0, y, m_boxContainerSize.width, m_itemHeight));

	return m_rowCount -1;
}

CHmsLabel * CHmsTableBox::AppendTextItem(int row, int column, const char *text, TableItemAlign align /*= TableItemAlign::ALIGN_CENTER*/
    , int maxLineWidth /*= 0*/, int maxLineCount /*= 0*/)
{
	if (m_nCurPoolItemIndex >= m_vecLabelPool.size())
	{
        auto pItem = CHmsGlobal::CreateLabel("text", m_labelFontSize);
		pItem->SetTextColor(m_itemNormalTextColor);
		m_vecLabelPool.pushBack(pItem);
	}
	auto pItem = m_vecLabelPool.at(m_nCurPoolItemIndex);
	++m_nCurPoolItemIndex;

	m_pItemsContainer->AddChild(pItem);

    pItem->SetMaxLineWidth(maxLineWidth);
    pItem->SetMaxLineCount(maxLineCount);
	//pItem->SetContentSize(m_vecColumnRect[column].size);
	pItem->SetString(text);
	pItem->UpdateTextString();	

	auto temp = dynamic_cast<CHmsNode*>(pItem);
	if (temp)
	{
		PushItemAndAlign(row, column, temp, align);	
	}

	return pItem;
}

CHmsUiStretchButton * CHmsTableBox::AppendButtonItem(int row, int column, TableItemAlign align /*= TableItemAlign::ALIGN_CENTER*/)
{
	auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", 
		Size(75,50), Color3B(0x0e, 0x19, 0x25), Color3B(0x80, 0x4e, 0x9d));
	pItem->AddIcon("res/airport/arrow.png");
	auto temp = dynamic_cast<CHmsNode*>(pItem);
	if (temp)
	{
		PushItemAndAlign(row, column, temp, align);
	}
	m_pItemsContainer->AddChild(pItem);
	{
		auto interf = dynamic_cast<HmsUiEventInterface*>(pItem);
		if (interf)
		{
			m_pItemsContainer->AddChildInterface(interf);
		}
	}
	return pItem;
}

CHmsImageNode * CHmsTableBox::AppendImageItem(int row, int column, const Color3B &imageColor, TableItemAlign align /*= TableItemAlign::ALIGN_CENTER*/)
{
	auto pItem = CHmsImageNode::Create("res/airport/circle.png");
	pItem->SetColor(imageColor);
	auto temp = dynamic_cast<CHmsNode*>(pItem);
	if (temp)
	{
		PushItemAndAlign(row, column, temp, align);
	}
	m_pItemsContainer->AddChild(pItem);
	return pItem;
}

void CHmsTableBox::PushItemAndAlign(int row, int column, CHmsNode *pItem, TableItemAlign align)
{
	if (!pItem)
	{
		return;
	}
	switch (align)
	{
	case TableItemAlign::ALIGN_LEFT:
		pItem->SetAnchorPoint(Vec2(0, 0.5));
		pItem->SetPosition(
			m_vecColumnRect[column].origin.x + m_itemSpace
			, 0 - (row + 1) * m_itemHeight + m_itemHeight / 2);
		break;
	case TableItemAlign::ALIGN_CENTER:
		pItem->SetAnchorPoint(Vec2(0.5, 0.5));
		pItem->SetPosition(
			m_vecColumnRect[column].origin.x + m_vecColumnRect[column].size.width / 2
			, 0 - (row + 1) * m_itemHeight + m_itemHeight / 2);
		break;
	case TableItemAlign::ALIGN_RIGHT:
		pItem->SetAnchorPoint(Vec2(1, 0.5));
		pItem->SetPosition(
			m_vecColumnRect[column].origin.x + m_vecColumnRect[column].size.width - m_itemSpace
			, 0 - (row + 1) * m_itemHeight + m_itemHeight / 2);
		break;
	default:
		break;
	}
	if (row < (int)m_vec2Items.size() && column < (int)m_vec2Items[row].size())
	{
		m_vec2Items[row][column] = pItem;
	}
}

void CHmsTableBox::RemoveRow(int rowIndex)
{
#if 0
	if (rowIndex < m_vec2Items.size())
	{
		for (auto item : m_vec2Items[rowIndex])
		{
			if (item)
			{
				item->RemoveFromParent();
			}	
		}
		--m_rowCount;
		m_vec2Items.erase(m_vec2Items.begin() + rowIndex);

		m_pContentGridNode->clear();
		m_vecRowRect.clear();
		//重新调整剩余行位置
		for (int row = 0; row < m_vec2Items.size(); ++row)
		{
			for (auto item : m_vec2Items[row])
			{
				if (item)
				{
					auto s = item->GetPoistion();
					item->SetPosition(s.x, 0 - (row + 1) * m_itemHeight + m_itemHeight / 2);
				}
			}
			//画格子
			auto y = 0 - (row+1) * m_itemHeight;
			m_pContentGridNode->SetContentSize(Size(m_boxContainerSize.width, y));
			std::vector<Vec2> vec;
			vec.push_back(Vec2(0, y));
			vec.push_back(Vec2(m_boxSize.width, y));
			for (int i = 5; i < m_vecColumnRect.size(); ++i)
			{
				vec.push_back(Vec2(m_vecColumnRect[i].origin.x, y));
				vec.push_back(Vec2(m_vecColumnRect[i].origin.x, y + m_itemHeight));
			}
			m_pContentGridNode->DrawLines(vec, m_gridLineWidth, Color4B(0x0e, 0x19, 0x25, 0xff));

			m_vecRowRect.push_back(Rect(0, y, m_boxContainerSize.width, m_itemHeight));
		}
		if (rowIndex <= m_curSelectRowIndex)
		{
			SelectRow(-1);
		}

		CalScrollBarSize();
	}
#endif
}

void CHmsTableBox::ScrollToDefault()
{
	float y = m_pVBar->GetPoistion().y;
	ScrollTo(y);
}

void CHmsTableBox::SelectRow(int rowIndex)
{
	if (rowIndex < 0 || rowIndex >= (int)m_vecRowRect.size())
	{
		m_curSelectRowIndex = -1;
		m_pItemSelectBgNode->SetVisible(false);
	}
	else
	{
		m_curSelectRowIndex = rowIndex;
		m_pItemSelectBgNode->SetVisible(true);
		m_pItemSelectBgNode->SetPosition(m_vecRowRect[rowIndex].origin);
	}
}

void CHmsTableBox::GetCellIndex(CHmsNode *pNode, int &row, int &column)
{
	row = -1;
	column = -1;
	for (int r = 0; r < (int)m_vec2Items.size(); ++r)
	{
		for (int c = 0; c < (int)m_vec2Items[r].size(); ++c)
		{
			if (pNode == m_vec2Items[r][c])
			{
				row = r;
				column = c;
				return;
			}
		}
	}
}

CHmsNode* CHmsTableBox::GetCell(int row, int column)
{
	if (row >= 0 && row < (int)m_vec2Items.size() && column >= 0 && column < (int)m_vec2Items[row].size())
	{
		return m_vec2Items[row][column];
	}
	return nullptr;
}

void CHmsTableBox::SetClickCellCallback(std::function<void(int,int)> func)
{
	m_callbackClickCell = func;
}



