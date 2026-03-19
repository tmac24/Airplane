#pragma once
#include "../HmsGlobal.h"
#include "display/HmsDrawNode.h"
#include "base/HmsTimer.h"
#include "ui/HmsUiEventInterface.h"

class CHmsSampleBox : public HmsAviPf::CHmsDrawNode, public HmsAviPf::HmsUiEventInterface
{
public:
    typedef std::function<void(bool)> OnSelectedChangedCallback;
    CHmsSampleBox();
    ~CHmsSampleBox();

    CREATE_FUNC(CHmsSampleBox);

    void SetText(const std::string &text);
    void SetToggle(const bool &bToggle);

    virtual bool Init() override;

    virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;

    OnSelectedChangedCallback OnSelectedChanged;
private:
    float m_unitBoxWidth;
    float m_unitBoxHeight;
    HmsAviPf::Color4B m_defBorderColor;//默认边框颜色
    HmsAviPf::Color4B m_selBorderColor;//选中时的边框颜色
    bool m_isSelected;
    bool m_isToggle;	//是否为开关按钮

    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pNodeLabel;

    void UpdateBackground();
};

