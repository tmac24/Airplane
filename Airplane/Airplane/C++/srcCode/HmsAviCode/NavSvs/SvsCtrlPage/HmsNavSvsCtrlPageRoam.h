#pragma once

#include "HmsNavSvsCtrlPageBase.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiStretchButton.h"
#include "display/HmsStretchImageNode.h"
#include "ui/HmsVSlideBar.h"

class CHmsNavSvsCtrlPageRoam : public CHmsNavSvsCtrlPageBase
{
public:
    CHmsNavSvsCtrlPageRoam();
    virtual ~CHmsNavSvsCtrlPageRoam();

    CREATE_FUNC_BY_SIZE(CHmsNavSvsCtrlPageRoam);
    virtual bool Init(const HmsAviPf::Size & size);
    virtual void Update(float delta) override;
    virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;

    virtual void OnEnter() override;
    virtual void OnExit() override;

private:
    void UpdateCtrlData();

private:
    RefPtr<CHmsImageNode> m_pCtrlPointBgImageNode;
    RefPtr<CHmsImageNode> m_pCtrlPointImageNode;
    RefPtr<CHmsLabel> m_pInfoLabel;
    RefPtr<CHmsVSlideBar> m_pVSlideBar;

    bool m_bPressOnCtrlPoint;
    bool m_bPressChangeAttitude;
    Vec2 m_pressPos;
};

