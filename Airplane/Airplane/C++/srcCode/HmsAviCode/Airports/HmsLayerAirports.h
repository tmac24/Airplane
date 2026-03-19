#pragma once

#include "../HmsGlobal.h"
#include "../HmsScreenLayout.h"
#include "display/HmsDrawNode.h"
#include "display/HmsLabel.h"
#include "display/HmsLineEdit.h"
#include "base/HmsTypes.h"
#include "ui/HmsUiImage2StateButton.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiImageButton.h"

#include "ui/HmsListBox.h"
#include "ui/HmsKeyboard.h"
#include "HmsMutexButton.h"
#include "HmsNavImageSql.h"

#include "HmsAirportInfo.h"

#include <map>

using namespace HmsAviPf;

class CHmsLayerAirportProgram;

class CHmsLayerAirports : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsLayerAirports();
	~CHmsLayerAirports();

	CREATE_FUNC_BY_SIZE(CHmsLayerAirports)

	virtual bool Init(const HmsAviPf::Size & size);
	virtual void Update(float delta) override;
	virtual bool OnPressed(const Vec2 & posOrigin);

private:
	void InitEvent();

	void InitLeft();
	void InitRight();
	void InitBottom();

	void ToolBtnMapClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);
	void ToolBtnXTabClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);
	void ToolBtnFavoriteClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);
	void ToolBtnArrowLeftClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);
	void ToolBtnArrowRightClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);
	void ToolBtnArrowUpClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);
	void ToolBtnArrowDownClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);
	void ToolBtnEnterClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);

	void ChangeBtnColor(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);

	void AirportSelectChanged(CHmsListBoxItem *pItem, int index, bool selected);
	void RefreshAirportList();
	void RefreshLineEditData(std::string curText, std::string needSearchText, bool bUp);

	int GetCurOperateArea();

	CHmsListBoxItem* CreateRightListItem();

private:

	RefPtr<CHmsMutexButton> m_pBtnAll;
	RefPtr<CHmsMutexButton> m_pBtnFavorite;

	RefPtr<CHmsLineEidt> m_pLineEdit;
	RefPtr<CHmsKeyboard> m_pKeyBoard;

	RefPtr<CHmsListBox> m_pListBoxLeft;
	RefPtr<CHmsListBox> m_pListBoxRight;

    CHmsAirportInfo *m_pAirportInfo;

	float m_toolHeight;//底部工具栏
	float m_itemHeight;//列表item
	float m_bottomSpace;//左侧列表下部空余空间
	float m_margin;

	CHmsNavImageSql &m_navDb;

	std::vector<StarSidStu> m_vecStarSidStu;
	bool m_bIsFavorite;

	HmsAviPf::Vector<CHmsListBoxItem*> m_vecLeftListItemPool;
	HmsAviPf::Vector<CHmsListBoxItem*> m_vecRightListItemPool;
};

