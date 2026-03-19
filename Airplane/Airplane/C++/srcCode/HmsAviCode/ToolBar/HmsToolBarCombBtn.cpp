#include "HmsToolBarCombBtn.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "../HmsGlobal.h"

USING_NS_HMS;

CHmsToolBarCombBtn * CHmsToolBarCombBtn::CreateBtns(const char * strIcon, CombBtnItem * pCombBtnItem, int nCnt, const  HmsAviPf::Size & size)
{
	CHmsToolBarCombBtn *ret = new CHmsToolBarCombBtn();
	if (ret && ret->Init(strIcon))
	{
		if (size.equals(Size::ZERO))
		{
			ret->SetContentSize(ret->m_sizeBtnDesign);
		}
		else
		{
			ret->SetContentSize(size);
		}
		ret->AddItems(pCombBtnItem, nCnt);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsToolBarCombBtn::CHmsToolBarCombBtn()
	: HmsUiEventInterface(this)
	, m_sizeBtnDesign(264,90)
	, m_bPopMenus(false)
{
	m_strCurText = " ";
}

CHmsToolBarCombBtn::~CHmsToolBarCombBtn()
{

}

bool CHmsToolBarCombBtn::Init(const char * strIcon)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_itemShow = CHmsToolBarCombItem::CreateWithImage(strIcon, m_strCurText.c_str(), m_sizeBtnDesign);
	if (m_itemShow)
	{
		m_itemShow->SetAnchorPoint(0.0f, 0.0f);
		m_itemShow->SetCallbackFunc(HMS_CALLBACK_2(CHmsToolBarCombBtn::OnCombBtnClick, this));
		m_itemShow->SetPosition(Vec2(0, 0));
		m_itemShow->SetBgOpacity(127);
		this->AddChild(m_itemShow);
		this->AddChildInterface(m_itemShow);
	}

	return true;
}

bool CHmsToolBarCombBtn::AddItem(const CombBtnItem & item)
{
	bool bRet = true;
	CHmsToolBarCombItem * combBox = nullptr;
	Size sizeBtn(m_sizeBtnDesign.width - 40, m_sizeBtnDesign.height);
	if (item.strIcon.empty())
	{
		combBox = CHmsToolBarCombItem::CreateWithString(item.strTitle.c_str(), sizeBtn);
	}
	else
	{
		combBox = CHmsToolBarCombItem::CreateWithImage(item.strIcon.c_str(), item.strTitle.c_str(), sizeBtn);
	}
		
	if (combBox)
	{
		combBox->SetAnchorPoint(0.0f, 0.0f);
		combBox->SetBgOpacity(180);
		combBox->SetCallbackFunc(HMS_CALLBACK_2(CHmsToolBarCombBtn::OnBtnItemClick, this));


		CombBtnItemInfo info;
		info.info = item;
		info.item = combBox;
		m_vCombItems.push_back(info);
	}

	return bRet;
}

bool CHmsToolBarCombBtn::AddItems(CombBtnItem * pCombBtnItem, int nCnt)
{
	bool bRet = false;
	if (pCombBtnItem)
	{
		for (int i = 0; i < nCnt; i++)
		{
			bRet = AddItem(pCombBtnItem[i]);
		}
	}
	return bRet;
}

void CHmsToolBarCombBtn::SetSelect(int nIndex)
{
	if ((nIndex >= 0) && ((int)m_vCombItems.size() > nIndex))
	{
		auto & item = m_vCombItems.at(nIndex);
		if (m_itemCurrent == item.item)
		{
			return;
		}
		
		m_itemCurrent = item.item;
		m_strCurText = item.info.strTitle;

		if (m_itemShow)
		{
			m_itemShow->SetText(m_strCurText.c_str());
			m_itemShow->SetPosition(Vec2(0, 0));
		}

		if (item.info.callback)
		{
			item.info.callback();
		}

		UnpopItems();
	}
}

void CHmsToolBarCombBtn::SetSelect(const std::string & strTitle)
{
	for (int i = 0; i < (int)m_vCombItems.size(); i++)
	{
		auto & item = m_vCombItems.at(i);
		if (strTitle == item.info.strTitle)
		{
			SetSelect(i);
		}
	}
}

void CHmsToolBarCombBtn::OnSelectItem(HmsAviPf::CHmsUiButtonAbstract * btn)
{
	for (int i = 0; i < (int)m_vCombItems.size(); i++)
	{
		auto item = m_vCombItems.at(i).item;
		if (btn == item)
		{
			SetSelect(i);
		}
	}
}

void CHmsToolBarCombBtn::OnTogglePopItems()
{
	if (m_bPopMenus)
	{
		UnpopItems();
	}
	else
	{
		PopItems();	
	}
}
#include "base/HmsActionTimeRef.h"
void CHmsToolBarCombBtn::PopItems()
{
	if (!m_bPopMenus)
	{
		float fOffsetX = 90;
		Size sizeDisplay = m_sizeBtnDesign;
		sizeDisplay.width += fOffsetX;

		Vec2 posSet(fOffsetX, 0);
		for (int i = 0; i < (int)m_vCombItems.size(); i++)
		{
			auto item = m_vCombItems.at(i).item;
			if (m_itemCurrent != item && item)
			{
				auto moveto = CHmsActionMoveTo::Create(0.3f, posSet);
				item->RunAction(moveto);
				//item->SetPosition(posSet);
				this->AddChild(item);
				this->AddChildInterface(item);
				auto tempSize = item->GetContentSize();
				posSet.y += tempSize.height + 2;

				sizeDisplay.height += tempSize.height + 2;
			}
		}

		if (m_itemShow)
		{
			m_itemShow->SetPosition(0, posSet.y);
		}

		posSet.y -= m_sizeBtnDesign.height;
		for (int i = 0; i < (int)m_vCombItems.size(); i++)
		{
			auto item = m_vCombItems.at(i).item;
			if (item)
			{
				item->SetPosition(posSet);
			}
		}

		SetContentSize(sizeDisplay);
		m_bPopMenus = true;
	}
}

void CHmsToolBarCombBtn::UnpopItems()
{
	if (m_bPopMenus)
	{
		for (int i = 0; i < (int)m_vCombItems.size(); i++)
		{
			auto item = m_vCombItems.at(i).item;
			if (item)
			{
				item->RemoveFromParentAndCleanup(true);
				item->RemoveFromParentInterface();
			}
		}
		if (m_itemShow)
		{
			m_itemShow->SetPosition(Vec2(0,0));
		}
		SetContentSize(m_sizeBtnDesign);
		m_bPopMenus = false;
	}
}

void CHmsToolBarCombBtn::OnBtnItemClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState)
{
	if (eState == HMS_UI_BUTTON_STATE::btn_normal)
	{
		OnSelectItem(btn);
	}
}

void CHmsToolBarCombBtn::OnCombBtnClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState)
{
	if (eState == HMS_UI_BUTTON_STATE::btn_normal)
	{
		OnTogglePopItems();
	}
}


