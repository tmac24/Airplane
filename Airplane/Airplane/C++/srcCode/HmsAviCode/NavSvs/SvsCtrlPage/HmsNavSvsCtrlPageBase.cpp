#include "HmsNavSvsCtrlPageBase.h"
#include "NavSvs/HmsNavSvsModuleMgr.h"
#include "HmsGlobal.h"

CHmsNavSvsCtrlPageBase::CHmsNavSvsCtrlPageBase()
: HmsUiEventInterface(this)
{
    m_type = NavSvsCtrlPageType::CTRLPAGE_BASE;
}

CHmsNavSvsCtrlPageBase::~CHmsNavSvsCtrlPageBase()
{

}

bool CHmsNavSvsCtrlPageBase::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(size);

    m_pLabel = CHmsGlobal::CreateUTF8Label("", 32);
    m_pLabel->SetPosition(Vec2(300, 300));
    m_pLabel->SetVisible(false);
    this->AddChild(m_pLabel);

    m_pTitleLabel = CHmsGlobal::CreateLanguageLabelUtf8("", 32, true)->GetLabel();
    m_pTitleLabel->SetPosition(Vec2(50, size.height - 50));
    m_pTitleLabel->SetAnchorPoint(Vec2(0, 0));
    this->AddChild(m_pTitleLabel);

    return true;
}

void CHmsNavSvsCtrlPageBase::Update(float delta)
{
#if 0
    auto pCamera = CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl();
    if (pCamera)
    {
        if (pCamera->GetInTouchEventCtrlTime() > 0)
        {
            char tmp[32] = { 0 };
            sprintf(tmp, "%.3f", pCamera->GetInTouchEventCtrlTime());
            m_pLabel->SetString(tmp);
            m_pLabel->SetVisible(true);
        }
        else
        {
            m_pLabel->SetVisible(false);
        }
    }
#endif
}

bool CHmsNavSvsCtrlPageBase::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
    auto pCamera = CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl();
    if (pCamera)
    {
        pCamera->OnPressed(posOrigin);
    }
    HmsUiEventInterface::OnPressed(posOrigin);

    return true;
}

void CHmsNavSvsCtrlPageBase::OnMove(const HmsAviPf::Vec2 & posOrigin)
{
    auto pCamera = CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl();
    if (pCamera)
    {
        pCamera->OnMove(posOrigin);
    }
    HmsUiEventInterface::OnMove(posOrigin);
}

void CHmsNavSvsCtrlPageBase::OnReleased(const HmsAviPf::Vec2 & posOrigin)
{
    auto pCamera = CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl();
    if (pCamera)
    {
        pCamera->OnReleased(posOrigin);
    }
    HmsUiEventInterface::OnReleased(posOrigin);
}

bool CHmsNavSvsCtrlPageBase::OnMutiPressed(const std::vector<UiEventPoint> &pos)
{
    return false;
}

void CHmsNavSvsCtrlPageBase::OnMutiMove(const std::vector<UiEventPoint> &pos)
{

}

void CHmsNavSvsCtrlPageBase::OnMutiReleased(const std::vector<UiEventPoint> &pos)
{

}

void CHmsNavSvsCtrlPageBase::OnEnter()
{
    CHmsNode::OnEnter();
    SetScheduleUpdate();
}

void CHmsNavSvsCtrlPageBase::OnExit()
{
    CHmsNode::OnExit();
    SetUnSchedule();
}

void CHmsNavSvsCtrlPageBase::SetTitle(const char *str)
{
    if (m_pTitleLabel && str)
    {
        CHmsLanguageLabel::SetText(m_pTitleLabel, str);
    }
}
