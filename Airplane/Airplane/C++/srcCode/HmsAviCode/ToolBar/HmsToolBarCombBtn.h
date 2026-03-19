#pragma once
#include "base/HmsNode.h"
#include "ui/HmsUiButtonAbstract.h"
#include "HmsToolBarCombItem.h"
#include <map>
#include <vector>

struct CombBtnItem
{
	std::string		strIcon;
	std::string		strTitle;
	std::function<void()> callback;
};

class CHmsToolBarCombBtn : public HmsAviPf::CHmsNode, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsToolBarCombBtn();
	~CHmsToolBarCombBtn();

	static CHmsToolBarCombBtn * CreateBtns(const char * strIcon, CombBtnItem * pCombBtnItem, int nCnt, const  HmsAviPf::Size & size = HmsAviPf::Size::ZERO);

	virtual bool Init(const char * strIcon);

	bool AddItem(const CombBtnItem & item);
	bool AddItems(CombBtnItem * pCombBtnItem, int nCnt);
	void SetSelect(int nIndex);
	void SetSelect(const std::string & strTitle);


	//virtual bool OnPressed(const Vec2 & posOrigin) override;

protected:
	void OnBtnItemClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState);
	void OnCombBtnClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState);

	void OnSelectItem(HmsAviPf::CHmsUiButtonAbstract * btn);
	void OnTogglePopItems();
	void PopItems();
	void UnpopItems();

private:
	struct CombBtnItemInfo
	{
		CombBtnItem								info;
		HmsAviPf::RefPtr<CHmsToolBarCombItem>	item;
	};

	std::vector<CombBtnItemInfo>				m_vCombItems;
	HmsAviPf::Size								m_sizeBtnDesign;
	HmsAviPf::RefPtr<CHmsToolBarCombItem>		m_itemShow;
	HmsAviPf::RefPtr<CHmsToolBarCombItem>		m_itemCurrent;
	std::string									m_strCurText;
	bool										m_bPopMenus;
};