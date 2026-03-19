#pragma once

#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiButton.h"
#include "display/HmsLineEdit.h"
#include "ui/HmsKeyboard.h"
#include "RoutePlanData/mcduDef.h"
#include "HmsNavDataStruct.h"
#include "ui/HmsTableBox.h"
#include "HmsLonLatEditWidget.h"
#include "ui/HmsUiSystemEditBox.h"

using namespace HmsAviPf;

class CHmsUserWptEditDlg : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsUserWptEditDlg();
	~CHmsUserWptEditDlg();

	CREATE_FUNC_BY_SIZE(CHmsUserWptEditDlg)
	virtual bool Init(const HmsAviPf::Size & windowSize);

	void LineEditBlur();
	//传入长度为2的数组
	void ShowDlg();
	void CloseDlg();
	void OnDragButtonCallback(float lon, float lat);
	
protected:
	virtual bool OnPressed(const Vec2 & posOrigin) override;

private:
	void AddUserWpt();
	void DeleteUserWpt();
	void UpdateDataFromDb();
	void UpdateList();
	void ShowOrHideDragButton(bool bShow);
	void ClearLineEdit();

private:
	RefPtr<CHmsTableBox> m_pTableBox;
	std::vector<UserWptStu> m_vecUserWpt;
    RefPtr<CHmsUiSystemEditBox> m_pLineEidtName;
    RefPtr<CHmsLonLatEditWidget> m_pLonLatEditWidget;
};

