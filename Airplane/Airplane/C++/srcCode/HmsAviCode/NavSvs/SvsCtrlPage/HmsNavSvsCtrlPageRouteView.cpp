#include "HmsNavSvsCtrlPageRouteView.h"
#include "NavSvs/HmsNavSvsModuleMgr.h"
#include "NavSvs/PositionMgr/HmsNavSvsPositionRouteView.h"
#include "HmsGlobal.h"
#include "NavSvs/HmsNavSvsMathHelper.h"

CHmsNavSvsCtrlPageRouteView::CHmsNavSvsCtrlPageRouteView()
{
    m_type = NavSvsCtrlPageType::CTRLPAGE_ROUTEVIEW;
    m_bPressOnProcessBar = false;
    m_space = 0;
}

CHmsNavSvsCtrlPageRouteView::~CHmsNavSvsCtrlPageRouteView()
{

}

bool CHmsNavSvsCtrlPageRouteView::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNavSvsCtrlPageBase::Init(size))
    {
        return false;
    }
    SetContentSize(size);

    m_toolBtnSize = Size(160, 100);
    m_space = 100;

    auto pImage = CHmsStretchImageNode::Create("res/NavSvs/solidWhite.png");
    pImage->SetContentSize(Size(size.width - m_space * 2 - m_toolBtnSize.width * 2, m_toolBtnSize.height));
    pImage->SetAnchorPoint(Vec2(0, 0));
    pImage->SetPosition(m_space + m_toolBtnSize.width * 2, m_toolBtnSize.height);
    pImage->SetOpacity(100);
    pImage->SetColor(Color3B::BLACK);
    this->AddChild(pImage);
    m_pProcessBgImageNode = pImage;

    m_pProcessVLineDrawNode = CHmsDrawNode::Create();
    m_pProcessBgImageNode->AddChild(m_pProcessVLineDrawNode);

    m_pProcessDrawNode = CHmsDrawNode::Create();
    m_pProcessDrawNode->SetPosition(Vec2(m_space + m_toolBtnSize.width * 2, 100));
    m_pProcessDrawNode->SetContentSize(Size(20, m_toolBtnSize.height));
    m_pProcessDrawNode->DrawSolidRect(Rect(0, 0, 20, m_toolBtnSize.height), Color4F::ORANGE);
    this->AddChild(m_pProcessDrawNode);

    InitPlayButton();
    InitSpeedModeButton();

    this->RemoveAllChildInterface();
    this->RegisterAllNodeChild();

    return true;
}

void CHmsNavSvsCtrlPageRouteView::Update(float delta)
{
    CHmsNavSvsCtrlPageBase::Update(delta);

    auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRouteView*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
    if (pPositionMgr && pPositionMgr->m_vecData.size() > 1 && pPositionMgr->m_bRun)
    {
        float allSpace = m_space * 2 + m_toolBtnSize.width * 2 + 20;
        float step = (this->GetContentSize().width - allSpace) / (pPositionMgr->m_vecData.size() - 1);
        float width = step * pPositionMgr->m_curIndex;
        m_pProcessDrawNode->SetPosition(Vec2(m_space + m_toolBtnSize.width * 2 + width, 100));
    } 
}

bool CHmsNavSvsCtrlPageRouteView::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
    auto rect = m_pProcessDrawNode->GetRectFromParent();
    rect.origin.x -= 30;
    rect.size.width += 60;
    if (rect.containsPoint(posOrigin))
    {
        m_bPressOnProcessBar = true;
        m_pPlayButton->UnSelected();
    }
    else
    {
        m_bPressOnProcessBar = false;
        CHmsNavSvsCtrlPageBase::OnPressed(posOrigin);
    }

    return true;
}

void CHmsNavSvsCtrlPageRouteView::OnMove(const HmsAviPf::Vec2 & posOrigin)
{
    if (m_bPressOnProcessBar)
    {
        float x = posOrigin.x;
        x = SVS_BETWEEN_VALUE(x, m_space + m_toolBtnSize.width * 2, this->GetContentSize().width - m_space - 20);
        m_pProcessDrawNode->SetPosition(Vec2(x, 100));
    }
    else
    {
        CHmsNavSvsCtrlPageBase::OnMove(posOrigin);
    }
}

void CHmsNavSvsCtrlPageRouteView::OnReleased(const HmsAviPf::Vec2 & posOrigin)
{
    if (m_bPressOnProcessBar)
    {
        auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRouteView*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
        if (pPositionMgr && pPositionMgr->m_vecData.size() > 1)
        {
            float allSpace = m_space * 2 + m_toolBtnSize.width * 2 + 20;
            float step = (this->GetContentSize().width - allSpace) / ((int)pPositionMgr->m_vecData.size() - 1);
            int index = (m_pProcessDrawNode->GetPoistionX() - m_space - m_toolBtnSize.width * 2) / step;
            if (index >= 0 && index < (int)pPositionMgr->m_vecData.size())
            {
                pPositionMgr->m_curIndex = index;
            }
        }
    }
    else
    {
        CHmsNavSvsCtrlPageBase::OnReleased(posOrigin);
    }
}

void CHmsNavSvsCtrlPageRouteView::OnEnter()
{
    CHmsNavSvsCtrlPageBase::OnEnter();
    SetScheduleUpdate();

    UpdateIdentLabel();
}

void CHmsNavSvsCtrlPageRouteView::OnExit()
{
    CHmsNavSvsCtrlPageBase::OnExit();
    SetUnSchedule();

    m_pPlayButton->UnSelected();
}

void CHmsNavSvsCtrlPageRouteView::InitPlayButton()
{
    m_pPlayButton = CHmsUiStretchButton::Create("res/NavSvs/solidWhite.png", m_toolBtnSize, Color3B(0x0e, 0x19, 0x25), Color3B::GREEN);
    m_pPlayButton->SetAnchorPoint(Vec2(0, 0));
    m_pPlayButton->SetPosition(Vec2(m_space, 100));
    m_pPlayButton->SetToggleBtn(true);
    m_pPlayButton->SetCallbackFunc([=](CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE eState){

        auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRouteView*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
        
        if (eState == HMS_UI_BUTTON_STATE::btn_normal)
        {
            m_pPlayButton->GetChildByTag(1001)->SetVisible(true);
            m_pPlayButton->GetChildByTag(1002)->SetVisible(false);
            if (pPositionMgr)
            {
                pPositionMgr->Pause();
            }
        }
        else
        {
            m_pPlayButton->GetChildByTag(1001)->SetVisible(false);
            m_pPlayButton->GetChildByTag(1002)->SetVisible(true);
            if (pPositionMgr)
            {
                pPositionMgr->Play();
            }
        }
    });
    this->AddChild(m_pPlayButton);

    auto pImage1 = CHmsImageNode::Create("res/NavSvs/play.png");
    pImage1->SetPosition(m_toolBtnSize * 0.5);
    pImage1->SetTag(1001);
    m_pPlayButton->AddChild(pImage1);

    auto pImage2 = CHmsImageNode::Create("res/NavSvs/pause.png");
    pImage2->SetPosition(m_toolBtnSize * 0.5);
    pImage2->SetTag(1002);
    pImage2->SetVisible(false);
    m_pPlayButton->AddChild(pImage2);

    auto pDrawNode = CHmsDrawNode::Create();
    pDrawNode->DrawSolidRect(Rect(m_toolBtnSize.width - 3, 0, 3, 100), Color4F::BLACK);
    m_pPlayButton->AddChild(pDrawNode);
}

void CHmsNavSvsCtrlPageRouteView::InitSpeedModeButton()
{
    m_pSpeedButton = CHmsUiStretchButton::Create("res/NavSvs/solidWhite.png", m_toolBtnSize, Color3B(0x0e, 0x19, 0x25), Color3B::GREEN);
    m_pSpeedButton->SetAnchorPoint(Vec2(0, 0));
    m_pSpeedButton->SetPosition(Vec2(m_space + m_toolBtnSize.width, 100));
    m_pSpeedButton->AddText("1x", 32);
    m_pSpeedButton->SetOnClickedFunc([=](CHmsUiButtonAbstract *pBtn){

        auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRouteView*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
        if (pPositionMgr)
        {
            auto speed = pPositionMgr->ChangeSpeed();
            char tmp[32];
            sprintf(tmp, "%dx", speed);
            m_pSpeedButton->SetText(tmp);
        }

    });
    this->AddChild(m_pSpeedButton);
}

void CHmsNavSvsCtrlPageRouteView::UpdateIdentLabel()
{
    m_pProcessBgImageNode->RemoveAllChildren();
    m_pProcessVLineDrawNode->clear();

    m_pProcessBgImageNode->AddChild(m_pProcessVLineDrawNode);

    auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRouteView*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
    if (pPositionMgr)
    {
        int lastPos = -1;
        int count = pPositionMgr->m_vecData.size();
        for (int i = 0; i < count; ++i)
        {
            auto &stu = pPositionMgr->m_vecData[i];
            if (!stu.ident.empty())
            {
                int pos = (this->GetContentSize().width - m_space * 2 - m_toolBtnSize.width * 2) / count * i;
                if (lastPos < 0 || abs(pos - lastPos) > 150)
                {
                    lastPos = pos;

                    auto pLabel = CHmsGlobal::CreateUTF8Label(stu.ident.c_str(), 26, Color4F::WHITE, true);
                    pLabel->SetAnchorPoint(Vec2(0.5, 1));
                    pLabel->SetPosition(Vec2(pos, 0));
                    pLabel->SetMaxLineWidth(150);
                    m_pProcessBgImageNode->AddChild(pLabel);                 
                }            
                m_pProcessVLineDrawNode->DrawSolidRect(Rect(pos, 0, 3, 100), Color4F::WHITE);
            }
        }  
    }
}
