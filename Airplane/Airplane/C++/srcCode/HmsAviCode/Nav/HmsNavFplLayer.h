#pragma once

#include "ui/HmsUiEventInterface.h"
#include "base/HmsNode.h"
#include "ui/HmsKeyboard.h"
#include "HmsRouteNodeWin.h"
#include "Procedure/HmsProcedureDlg.h"
#include "HmsHistoryRouteDlg.h"
#include "HmsHistoryRouteInputName.h"
#include "HmsInputAltitude.h"
#include "HmsSearchShape.h"
#include "HmsEditAirspace.h"
#include "HmsUserWptEditDlg.h"
#include "HmsNavComm.h"
#include "HmsWptAddSearch.h"

using namespace HmsAviPf;

class CHmsNavFplLayer : public CHmsNode, public HmsAviPf::HmsUiEventInterface
{
public:
    CHmsNavFplLayer();
    ~CHmsNavFplLayer();
    CREATE_FUNC_BY_SIZE(CHmsNavFplLayer);
    bool Init(const HmsAviPf::Size & size);

    virtual void Update(float delta) override;

    void ShowOrHideKeyboard(bool bShow, CHmsKeyboard::HmsOnKeyPress func, CHmsKeyboard::HmsOnKbdHide funcWhenHide);
    void ShowRouteNodeWin(WptNodeBrief stu, int stuIndex, Vec2 worldPos);
    void HideRouteNodeWin();
    void ShowWptAddSearch(bool bShow);
    void ToggleWptAddSearch();

private:
    RefPtr<CHmsNavInfoWindow> m_pNavInfoWin;
    RefPtr<CHmsRouteNodeWin> m_pRouteNodeWin;
    RefPtr<CHmsProcedureDlg> m_pProcedure;
    RefPtr<CHmsHistoryRouteDlg> m_pHistoryRouteDlg;
    RefPtr<CHmsHistoryRouteInputName> m_pHistoryRouteInputName;
    RefPtr<CHmsInputAltitude> m_pInputAltitude;
    RefPtr<CHmsSearchShape> m_pSearchShape;
    RefPtr<CHmsEditAirspace> m_pEditAirspace;
    RefPtr<CHmsUserWptEditDlg> m_pUserWptEditDlg;
    RefPtr<CHmsWptAddSearch> m_pWptAddSearch;

    RefPtr<CHmsKeyboard> m_pKeyBoard;

    friend class CHmsFrame2DRoot;
};

