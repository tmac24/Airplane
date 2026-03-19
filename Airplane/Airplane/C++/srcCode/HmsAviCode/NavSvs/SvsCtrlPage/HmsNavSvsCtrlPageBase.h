#pragma once

#include "base/HmsNode.h"
#include "ui/HmsUiEventInterface.h"
#include "display/HmsLabel.h"
#include "NavSvs/HmsNavSvsCommDef.h"

using namespace HmsAviPf;

class CHmsNavSvsCtrlPageBase : public CHmsNode, public HmsUiEventInterface
{
public:
    CHmsNavSvsCtrlPageBase();
    virtual ~CHmsNavSvsCtrlPageBase();

    CREATE_FUNC_BY_SIZE(CHmsNavSvsCtrlPageBase);
    virtual bool Init(const HmsAviPf::Size & size);
    virtual void Update(float delta) override;
    virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;
    virtual bool OnMutiPressed(const std::vector<UiEventPoint> &pos) override;
    virtual void OnMutiMove(const std::vector<UiEventPoint> &pos) override;
    virtual void OnMutiReleased(const std::vector<UiEventPoint> &pos) override;
    virtual void OnEnter() override;
    virtual void OnExit() override;
    virtual NavSvsCtrlPageType GetCtrlPageType(){ return m_type; }
    virtual void SetTitle(const char *str);

protected:
    NavSvsCtrlPageType m_type;
    CHmsLabel *m_pLabel;
    RefPtr<CHmsLabel> m_pTitleLabel;
};
