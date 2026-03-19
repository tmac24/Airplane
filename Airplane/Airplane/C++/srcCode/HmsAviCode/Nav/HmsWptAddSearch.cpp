#include "HmsWptAddSearch.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsFrame2DRoot.h"
#include "HmsNavFplLayer.h"
#include "HmsNavComm.h"
#include "Database/Hms424Database.h"
#include "Database/HmsHistoryRouteDatabase.h"

CHmsWptAddSearch::CHmsWptAddSearch()
: HmsUiEventInterface(this)
{

}

CHmsWptAddSearch::~CHmsWptAddSearch()
{

}

bool CHmsWptAddSearch::Init(const HmsAviPf::Size & windowSize)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(windowSize);

    auto pBgNode = CHmsDrawNode::Create();
    pBgNode->SetAnchorPoint(Vec2(0, 0));
    pBgNode->SetPosition(0, 0);
    this->AddChild(pBgNode);
    pBgNode->DrawSolidRect(Rect(Vec2(0, 0), windowSize), Color4F(Color3B(0x0e, 0x19, 0x25)));
    //画边框
    {
        std::vector<Vec2> vec;
        vec.push_back(Vec2(0, 0));
        vec.push_back(Vec2(windowSize.width, 0));
        vec.push_back(Vec2(windowSize.width, windowSize.height));
        vec.push_back(Vec2(0, windowSize.height));
        vec.push_back(Vec2(0, 0));
        pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
    }

    auto CreateButton = [=](Size s, Vec2 pos, const char * text)
    {
        auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
        pItem->AddText(text, 32, Color4B::WHITE, true);
        pItem->SetAnchorPoint(Vec2(0, 0));
        pItem->SetPosition(pos);
        this->AddChild(pItem);
        return pItem;
    };

    auto btnSize = Size(400, 95);
    float topPos = windowSize.height - btnSize.height - 10;
    CHmsUiStretchButton *pItem = nullptr;
    pItem = CreateButton(btnSize, Vec2(10, topPos - (btnSize.height + 10) * 1), "Longitude Latitude");
    pItem->SetOnClickedFunc(std::bind(&CHmsWptAddSearch::OnBtnClick, this, ButtonType::BUTTON_LonLat));
    m_mapLeftBtn[ButtonType::BUTTON_LonLat] = pItem;

    pItem = CreateButton(btnSize, Vec2(10, topPos - (btnSize.height + 10) * 2), "Search Waypoint");
    pItem->SetOnClickedFunc(std::bind(&CHmsWptAddSearch::OnBtnClick, this, ButtonType::BUTTON_UserWpt));
    m_mapLeftBtn[ButtonType::BUTTON_UserWpt] = pItem;
    
    {
        m_pLonLatEditWidget = CHmsLonLatEditWidget::Create();
        m_pLonLatEditWidget->SetAnchorPoint(Vec2(0, 0));
        m_pLonLatEditWidget->SetPosition(Vec2(10 + btnSize.width + 10, topPos - (btnSize.height + 10) * 2));
        this->AddChild(m_pLonLatEditWidget);

        auto pos = Vec2(10 + btnSize.width + 10 + 810, topPos - (btnSize.height + 10) - btnSize.height * 0.5);
        m_pLonLatButton = CreateButton(btnSize, pos, "Add To Edit");
        m_pLonLatButton->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
        {
            if (m_pLonLatEditWidget->IsRightResult())
            {
                std::string strLonLat = m_pLonLatEditWidget->GetFormatString();
                m_pLonLatEditWidget->ClearText();

                std::string ret = m_pLineEditTop->GetText();
                ret += strLonLat + " ";
                m_pLineEditTop->SetText(ret);
            }
            else
            {

            }
        });
    }
    {
        m_pSearchEdit = CHmsUiSystemEditBox::Create("", 32, Size(800, 95), "res/airport/search_bg.png");
        m_pSearchEdit->SetAnchorPoint(0, 0);
        m_pSearchEdit->SetPosition(Vec2(10 + btnSize.width + 10, topPos - (btnSize.height + 10) * 1));
        m_pSearchEdit->SetTextColor(Color4B::WHITE);
        this->AddChild(m_pSearchEdit);

        m_pSearchButton = CreateButton(btnSize, Vec2(10 + btnSize.width + 10 + 810, topPos - (btnSize.height + 10)), "Search");
        m_pSearchButton->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
        {
            UpdateSearchList();
            CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false,
                [=](char c){


            }, HMS_CALLBACK_0(CHmsWptAddSearch::LineEditBlur, this));
        });

        m_pSearchAddToEditButton = CreateButton(btnSize, Vec2(10 + btnSize.width + 10, 10), "Add To Edit");
        m_pSearchAddToEditButton->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
        {
            int index = m_pListBox->GetCurSelectIndex();
            auto pItem = m_pListBox->GetItem(index);
            if (pItem)
            {
                std::string str = pItem->GetText();
                std::string ret = m_pLineEditTop->GetText();
                ret += str + " ";
                m_pLineEditTop->SetText(ret);
            }
        });

        m_pListBox = CHmsListBox::CreateWithImage(NULL, NULL, "res/airport/lightblue_selbox.png", NULL, NULL);
        m_pListBox->SetBoxSize(Size(800, m_pSearchEdit->GetPoistion().y - 30 - btnSize.height));
        m_pListBox->SetItemTextAlignLeft(false);
        m_pListBox->SetItemNormalTextColor(Color4B::WHITE);
        m_pListBox->SetAnchorPoint(Vec2(0, 0));
        m_pListBox->SetPosition(Vec2(10 + btnSize.width + 10, 20 + btnSize.height));
        m_pListBox->SetItemHeight(70);
        m_pListBox->SetBacktopButtonVisible(false);
        this->AddChild(m_pListBox);

        m_pListBox->SetItemSelectChangeCallBack([=](CHmsListBoxItem *pItem, int index, bool selected){

            if (selected)
            {
                
            }
        });

        for (int i = 0; i < 50; ++i)
        {
            CHmsListBoxItem *pItem = CHmsListBoxItem::CreateWithImage(NULL, "res/airport/lightblue_selbox.png", NULL, NULL);
            pItem->SetText("text", 32, Color4B(0x19, 0x29, 0x3f, 0xff), true);
            m_vecListBoxItemPool.pushBack(pItem);
        }
    }


    btnSize = Size(260, 95);
    m_pLineEditTop = CHmsLineEidt::Create("res/airport/search_bg.png");
    m_pLineEditTop->SetAnchorPoint(0, 0);
    m_pLineEditTop->SetSize(Size(windowSize.width - btnSize.width - 30, btnSize.height));
    m_pLineEditTop->SetPosition(10, topPos);
    m_pLineEditTop->SetTextColor(Color4B::WHITE);
    this->AddChild(m_pLineEditTop);
    {
        auto pLabel = CHmsGlobal::CreateUTF8Label("", 32, true);
        m_pLineEditTop->SetTextLabel(pLabel);
    }
    pItem = CreateButton(btnSize, Vec2(windowSize.width - btnSize.width - 10, topPos), "Add To Route");
    pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
    {
        if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertFPLNodeByInputRouteString(FPLNodeIndex(-1, -1)))
        {
            ClearInputRouteString();
        }
    });

    btnSize = Size(260, 95);
    pItem = CreateButton(btnSize, Vec2(windowSize.width - btnSize.width - 10, 10), "Close");
    pItem->SetTextColor(Color4B::RED);
    pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
    {
        CloseDlg();
    });

    this->RegisterAllNodeChild();

    SelectBtn(ButtonType::BUTTON_LonLat);

    return true;
}

void CHmsWptAddSearch::LineEditBlur()
{
    if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
    {
        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false, nullptr, nullptr);
    }
    if (m_pLineEditTop)
    {
        m_pLineEditTop->Blur();
    }
    if (m_pLonLatEditWidget)
    {
        m_pLonLatEditWidget->Blur();
    }
    if (m_pSearchEdit)
    {
        m_pSearchEdit->CloseLastEditBoxKeyboard();
    }
}

void CHmsWptAddSearch::ShowDlg()
{
#if 0
    if (m_pLineEditTop)
    {
        m_pLineEditTop->SetText("");
    }
#endif
    if (m_pLonLatEditWidget)
    {
        m_pLonLatEditWidget->ClearText();
    }
    if (m_pSearchEdit)
    {
        m_pSearchEdit->SetText("");
    }

    this->SetVisible(true);
}

void CHmsWptAddSearch::CloseDlg()
{
    LineEditBlur();
    this->SetVisible(false);
    if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
    {
        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false,
            [=](char c){


        }, HMS_CALLBACK_0(CHmsWptAddSearch::LineEditBlur, this));
    }
}

std::string CHmsWptAddSearch::GetInputRouteString()
{
    return m_pLineEditTop->GetText();
}

void CHmsWptAddSearch::ClearInputRouteString()
{
    m_pLineEditTop->SetText("");
}

void CHmsWptAddSearch::UpdateSearchList()
{
    std::vector<std::string> vecRet;
    CHmsHistoryRouteDatabase::GetInstance()->GetFuzzyIdent(m_pSearchEdit->GetText(), vecRet);
    CHms424Database::GetInstance()->GetFuzzyIdent(m_pSearchEdit->GetText(), vecRet);
    
    int len = vecRet.size();
    if (len > (int)m_vecListBoxItemPool.size())
    {
        len = m_vecListBoxItemPool.size();
    }
    m_pListBox->Clear();
    for (int i = 0; i < len; ++i)
    {
        m_pListBox->AppendItem(m_vecListBoxItemPool.at(i), vecRet.at(i).c_str(), false, 32, true);
    }
}

bool CHmsWptAddSearch::OnPressed(const Vec2 & posOrigin)
{
    LineEditBlur();

    auto rect = m_pLineEditTop->GetRectFromParent();
    if (m_pLineEditTop->IsVisible() && rect.containsPoint(posOrigin))
    {
        m_pLineEditTop->Focus();

        if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
        {
            CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(true,
                [=](char c){

                auto str = m_pLineEditTop->GetText();
                if ('\b' == c)
                {
                    if (!str.empty())
                    {
                        str.pop_back();
                    }
                }
                else
                {
                    str.push_back(c);
                }
                m_pLineEditTop->SetText(str);

            }, HMS_CALLBACK_0(CHmsWptAddSearch::LineEditBlur, this));
        }
    }
    else
    {
        HmsUiEventInterface::OnPressed(posOrigin);
    }

    return true;
}

void CHmsWptAddSearch::OnBtnClick(ButtonType type)
{
    SelectBtn(type);
}

void CHmsWptAddSearch::SelectBtn(ButtonType type)
{
    for (auto &c : m_mapLeftBtn)
    {
        if (c.first == type)
        {
            c.second->Selected();
        }
        else
        {
            c.second->UnSelected();
        }
    }

    m_pLonLatEditWidget->SetVisible(false);
    m_pLonLatButton->SetVisible(false);

    m_pSearchEdit->SetVisible(false);
    m_pSearchButton->SetVisible(false);
    m_pListBox->SetVisible(false);
    m_pSearchAddToEditButton->SetVisible(false);

    if (type == ButtonType::BUTTON_LonLat)
    {
        m_pLonLatEditWidget->SetVisible(true);
        m_pLonLatButton->SetVisible(true);
    }
    else if (type == ButtonType::BUTTON_UserWpt)
    {
        m_pSearchEdit->SetVisible(true);
        m_pSearchButton->SetVisible(true);
        m_pListBox->SetVisible(true);
        m_pSearchAddToEditButton->SetVisible(true);
    }
}
