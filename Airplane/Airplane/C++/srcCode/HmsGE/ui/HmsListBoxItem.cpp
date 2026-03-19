#include "HmsListBoxItem.h"


CHmsListBoxItem::CHmsListBoxItem()
: HmsUiEventInterface(this)
, m_fTextXPos(5.0)
, m_bSelected(false)
, m_bChecked(false)
, m_bTextAlignLeft(true)
, m_bSelectable(true)
, m_bCheckable(true)
, m_normalTextColor(Color4B::WHITE)
, m_selectTextColor(Color4B::WHITE)
, m_fontSize(0.0f)
, m_bBorder(true)
{
	
}


CHmsListBoxItem::~CHmsListBoxItem()
{
}

CHmsListBoxItem* CHmsListBoxItem::CreateWithImage(
	const char * strBgNormalImage, const char * strBgSelectImage,
	const char * strBtnNormalImage, const char * strBtnSelectImage)
{
	CHmsListBoxItem *ret = new (std::nothrow) CHmsListBoxItem();
	if (ret && ret->InitWithImage(strBgNormalImage, strBgSelectImage, strBtnNormalImage, strBtnSelectImage))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsListBoxItem::InitWithImage(
	const char * strBgNormalImage, const char * strBgSelectImage,
	const char * strBtnNormalImage, const char * strBtnSelectImage)
{
	this->SetContentSize(m_itemSize);

	if (strBgNormalImage)
	{
		m_strBgNormal = strBgNormalImage;

		m_pBgNode = CHmsStretchImageNode::Create(m_strBgNormal);
		m_pBgNode->SetAnchorPoint(Vec2(0, 0));
		m_pBgNode->SetPosition(Vec2(0, 0));
		m_pBgNode->SetContentSize(m_itemSize);
		m_pBgNode->SetVisible(true);
		this->AddChild(m_pBgNode);
	}
	else
	{
		if (strBgSelectImage)
		{
			m_strBgSelect = strBgSelectImage;

			m_pBgNode = CHmsStretchImageNode::Create(m_strBgSelect);
			m_pBgNode->SetAnchorPoint(Vec2(0, 0));
			m_pBgNode->SetPosition(Vec2(0, 0));
			m_pBgNode->SetContentSize(m_itemSize);
			m_pBgNode->SetVisible(false);
			this->AddChild(m_pBgNode);
		}
	}
	if (strBtnNormalImage && strBtnSelectImage)
	{
		m_strBtnNormal = strBtnNormalImage;
		m_strBtnSelect = strBtnSelectImage;

		m_pBtnNode = CHmsImageNode::Create(strBtnNormalImage);
		m_pBtnNode->SetAnchorPoint(Vec2(0, 0.5));
		m_pBtnNode->SetPosition(m_itemSize.height / 1.8, m_itemSize.height / 2);
		m_pBtnNode->SetScale(m_itemSize.height / 1.8 / m_pBtnNode->GetContentSize().width);

		this->AddChild(m_pBtnNode);

		m_fTextXPos = m_itemSize.height/1.8 + m_itemSize.height / 1.8 + 20;
	}
	if (m_bBorder)
	{
		m_pBorderNode = CHmsImageNode::Create("res/airport/line.png");
		m_pBorderNode->SetTextureRect(Rect(0, 0, m_itemSize.width, 1));
		m_pBorderNode->SetAnchorPoint(Vec2(0, 0));
		m_pBorderNode->SetPosition(0, 0);
        m_pBorderNode->SetColor(Color3B(0xd7, 0xd8, 0xdc));
		this->AddChild(m_pBorderNode);
	}
	
	return true;
}

void CHmsListBoxItem::SetText(const char * strText, float size, const Color4B &color, bool bBorder)
{
	if (!m_pTextLabel)
	{
		m_pTextLabel = CHmsGlobal::CreateLabel(strText, size, bBorder);
		this->AddChild(m_pTextLabel);
	}
	else
	{
		if (m_fontSize != size)
		{
			m_pTextLabel->RemoveFromParent();
			m_pTextLabel = nullptr;
			m_pTextLabel = CHmsGlobal::CreateLabel(strText, size, bBorder);
			this->AddChild(m_pTextLabel);
		}
	}
	m_fontSize = size;

	m_pTextLabel->SetTextColor(color);
	m_pTextLabel->SetString(strText);
	m_pTextLabel->UpdateTextString();

	SetTextAlignLeft(m_bTextAlignLeft);
}

void CHmsListBoxItem::SetTextColor(Color4B color)
{
	if (m_pTextLabel)
	{
		m_pTextLabel->SetTextColor(color);
	}
}

void CHmsListBoxItem::SetChecked(bool bEnableCallback /*= true*/)
{
	if (m_pBtnNode)
	{
		m_bChecked = true;

		m_pBtnNode->SetTexture(m_strBtnSelect.c_str());

		if (bEnableCallback && m_OnCheckChangeFunc)
		{
			m_OnCheckChangeFunc(this, m_bChecked);
		}
	}
}

void CHmsListBoxItem::SetUnChecked(bool bEnableCallback /*= true*/)
{
	if (m_pBtnNode)
	{
		m_bChecked = false;

		m_pBtnNode->SetTexture(m_strBtnNormal.c_str());

		if (bEnableCallback && m_OnCheckChangeFunc)
		{
			m_OnCheckChangeFunc(this, m_bChecked);
		}
	}
}

void CHmsListBoxItem::SetSelect(bool bEnableCallback /*= true*/)
{
	if (!m_bSelectable)
	{
		return;
	}
	m_bSelected = true;
	SetTextColor(m_selectTextColor);
	if (!m_strBgSelect.empty())
	{
		m_pBgNode->SetTexture(m_strBgSelect.c_str());
		m_pBgNode->SetContentSize(m_itemSize);
		m_pBgNode->SetVisible(true);
	}
	if (bEnableCallback && m_OnSelectChangeFunc)
	{
		m_OnSelectChangeFunc(this, m_bSelected);
	}
}

void CHmsListBoxItem::SetUnSelect(bool bEnableCallback /*= true*/)
{
	if (!m_bSelectable)
	{
		return;
	}
	m_bSelected = false;
	SetTextColor(m_normalTextColor);
	if (!m_strBgNormal.empty())
	{
		m_pBgNode->SetTexture(m_strBgNormal.c_str());
		m_pBgNode->SetContentSize(m_itemSize);
		m_pBgNode->SetVisible(true);
	}
	else
	{
		if (m_pBgNode != nullptr) m_pBgNode->SetVisible(false);
	}

	if (bEnableCallback && m_OnSelectChangeFunc)
	{
		m_OnSelectChangeFunc(this, m_bSelected);
	}
}

void CHmsListBoxItem::SetOnSelectChangeFunc(const HmsOnListItemSelect & callback)
{
	m_OnSelectChangeFunc = callback;
}

void CHmsListBoxItem::SetOnCheckChangeFunc(const HmsOnListItemSelect & callback)
{
	m_OnCheckChangeFunc = callback;
}

bool CHmsListBoxItem::OnPressed(const Vec2 & posOrigin)
{
	if (m_pBtnNode && m_bCheckable)
	{
		auto rect = m_pBtnNode->GetRectFromParent();
		if (rect.containsPoint(posOrigin))
		{
			if (m_bChecked)
			{
				SetUnChecked();
			}
			else
			{
				SetChecked();
			}
			return true;
		}
	}

	//if (!m_bSelected)
	{
		SetSelect();
	}

	return true;
}

void CHmsListBoxItem::OnMove(const Vec2 & posOrigin)
{

}

void CHmsListBoxItem::OnReleased(const Vec2 & posOrigin)
{
	
}

void CHmsListBoxItem::SetItemSize(Size itemSize)
{
	m_itemSize = itemSize;
	this->SetContentSize(m_itemSize);
	if (m_pBgNode)
	{
		m_pBgNode->SetContentSize(m_itemSize);
	}
	if (m_pBtnNode)
	{
		m_pBtnNode->SetAnchorPoint(Vec2(0, 0.5));
		m_pBtnNode->SetPosition(m_itemSize.height / 1.8, m_itemSize.height / 2);
		m_pBtnNode->SetScale(m_itemSize.height / 1.8 / m_pBtnNode->GetContentSize().width);

		m_fTextXPos = m_itemSize.height / 1.8 + m_itemSize.height / 1.8 + 20;
	}
	if (m_bBorder && m_pBorderNode)
	{
		m_pBorderNode->SetTextureRect(Rect(0, 0, m_itemSize.width, 1));
	}

	SetTextAlignLeft(m_bTextAlignLeft);
}

void CHmsListBoxItem::SetBorderVisiable(bool bBorder)
{
	m_bBorder = bBorder;
	if (m_pBorderNode)
	{
		m_pBorderNode->SetVisible(m_bBorder);
	}
}

void CHmsListBoxItem::SetNormalTextColor(Color4B color)
{
	m_normalTextColor = color;
}

void CHmsListBoxItem::SetSelectTextColor(Color4B color)
{
	m_selectTextColor = color;
}

void CHmsListBoxItem::SetTextAlignLeft(bool b)
{
	m_bTextAlignLeft = b;

	if (m_pTextLabel)
	{
		if (m_bTextAlignLeft)
		{
			m_pTextLabel->SetAnchorPoint(0, 0.5);
			m_pTextLabel->SetPosition(m_fTextXPos, m_itemSize.height / 2.0);
		}
		else
		{
			m_pTextLabel->SetAnchorPoint(0.5, 0.5);
			m_pTextLabel->SetPosition(m_itemSize.width / 2, m_itemSize.height / 2.0);
		}
	}
}

void CHmsListBoxItem::SetSelectable(bool b)
{
	m_bSelectable = b;
}

bool CHmsListBoxItem::IsSelectable()
{
	return m_bSelectable;
}

std::string CHmsListBoxItem::GetText()
{
	return m_pTextLabel->GetString();
}

bool CHmsListBoxItem::IsSelected()
{
	return m_bSelected;
}

bool CHmsListBoxItem::IsChecked()
{
	return m_bChecked;
}




