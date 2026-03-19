#pragma once

#include "ui/HmsUiStretchButton.h"
#include "display/HmsLineEdit.h"
#include "ui/HmsKeyboard.h"
#include "RoutePlanData/mcduDef.h"
#include <map>
#include "HmsLonLatEditWidget.h"
#include "ui/HmsListBox.h"
#include "ui/HmsUiSystemEditBox.h"

using namespace HmsAviPf;

class CHmsWptAddSearch : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
    enum class ButtonType
    {
        BUTTON_LonLat = 0,
        BUTTON_UserWpt
    };
public:
    CHmsWptAddSearch();
    ~CHmsWptAddSearch();

    CREATE_FUNC_BY_SIZE(CHmsWptAddSearch)
    virtual bool Init(const HmsAviPf::Size & windowSize);

    void LineEditBlur();
    void ShowDlg();
    void CloseDlg();

    std::string GetInputRouteString();
    void ClearInputRouteString();

    void UpdateSearchList();

protected:
    virtual bool OnPressed(const Vec2 & posOrigin) override;

    void OnBtnClick(ButtonType type);
    void SelectBtn(ButtonType type);

private:
    RefPtr<CHmsLineEidt> m_pLineEditTop;

    RefPtr<CHmsLonLatEditWidget> m_pLonLatEditWidget;
    RefPtr<CHmsUiStretchButton> m_pLonLatButton;

    RefPtr<CHmsUiSystemEditBox> m_pSearchEdit;
    RefPtr<CHmsUiStretchButton> m_pSearchButton;
    RefPtr<CHmsListBox> m_pListBox;
    RefPtr<CHmsUiStretchButton> m_pSearchAddToEditButton;

    HmsAviPf::Vector<CHmsListBoxItem*> m_vecListBoxItemPool;

    std::map<ButtonType, CHmsUiStretchButton*> m_mapLeftBtn;
};

