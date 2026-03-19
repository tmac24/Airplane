#pragma once

#include "HmsNavSvsCtrlPageBase.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiStretchButton.h"
#include "display/HmsStretchImageNode.h"

class CHmsNavSvsCtrlPageRouteView : public CHmsNavSvsCtrlPageBase
{
public:
    CHmsNavSvsCtrlPageRouteView();
    virtual ~CHmsNavSvsCtrlPageRouteView();

    CREATE_FUNC_BY_SIZE(CHmsNavSvsCtrlPageRouteView);
    virtual bool Init(const HmsAviPf::Size & size);
    virtual void Update(float delta) override;
    virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;

    virtual void OnEnter() override;
    virtual void OnExit() override;

private:
    void InitPlayButton();
    void InitSpeedModeButton();
    void UpdateIdentLabel();

private:
    RefPtr<CHmsDrawNode> m_pProcessDrawNode;

    RefPtr<CHmsStretchImageNode> m_pProcessBgImageNode;
    RefPtr<CHmsDrawNode> m_pProcessVLineDrawNode;

    RefPtr<CHmsUiStretchButton> m_pPlayButton;
    RefPtr<CHmsUiStretchButton> m_pSpeedButton;

    Size m_toolBtnSize;
    float m_space;

    bool m_bPressOnProcessBar;
};

