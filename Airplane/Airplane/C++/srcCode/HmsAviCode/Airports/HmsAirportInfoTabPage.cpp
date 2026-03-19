#include "HmsAirportInfoTabPage.h"

CHmsAirportInfoTabPage::CHmsAirportInfoTabPage()
: HmsUiEventInterface(this)
{

}

CHmsAirportInfoTabPage::~CHmsAirportInfoTabPage()
{

}

bool CHmsAirportInfoTabPage::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(size);

    auto pBgDrawNode = CHmsDrawNode::Create();
	if (NULL != pBgDrawNode)
	{
		pBgDrawNode->DrawSolidRect(Rect(0, 0, size.width, size.height - CHmsAirportConstant::s_buttonSize.height),
			Color4F(CHmsAirportConstant::s_airportBlue));
		pBgDrawNode->DrawSolidRect(Rect(50, 0, size.width - 100, size.height - CHmsAirportConstant::s_buttonSize.height - 30),
			Color4F(CHmsAirportConstant::s_airportWhite));
		pBgDrawNode->SetPosition(Vec2(0, 0));
		this->AddChild(pBgDrawNode);
	}    

    this->RegisterAllNodeChild();
    return true;
}

CHmsAirportInfoTabPage::TabPageStu* CHmsAirportInfoTabPage::AddPage(string str)
{
    auto CreateButton = [=](const char *text, Size buttonSize, Vec2 pos, const string& str)
    {
        auto pItem = CHmsUiStretchButton::Create("res/AirportInfo/Round4SolidWhite.png",
            buttonSize, CHmsAirportConstant::s_airportGray, CHmsAirportConstant::s_airportBlue);
		if (NULL != pItem)
		{
			pItem->AddText(text, CHmsAirportConstant::s_fontSize, Color4B::BLACK, true);
			pItem->SetAnchorPoint(Vec2(0, 1));
			pItem->SetPosition(pos);
			pItem->SetToggleBtn(true);

			m_map[str].title = str;
			m_map[str].pBtn = pItem;

			pItem->SetCallbackFunc(std::bind(&CHmsAirportInfoTabPage::OnBtnClick, this,
				std::placeholders::_1, std::placeholders::_2, m_map[str].title));
			this->AddChild(pItem);
		}

        return pItem;
    };
    auto buttonSize = CHmsAirportConstant::s_buttonSize;
    auto size = GetContentSize();
    int index = m_map.size();
    CHmsUiStretchButton *pButton = nullptr;
    pButton = CreateButton(str.c_str(), buttonSize, Vec2((buttonSize.width + 5)*index++, size.height), str);

    auto CreateListFunc = [=](Size size, Vec2 pos){

        auto temp = CHmsUiVerticalScrollView::Create(size);
		if (NULL != temp)
		{
			this->AddChild(temp);
			temp->SetAnchorPoint(Vec2(0, 0));
			temp->SetPosition(pos);
			temp->SetItemVSpace(3);
		}

        return temp;
    };
    m_map[str].pListLeft = CreateListFunc(
        Size(size.width*0.5 - 53, size.height - CHmsAirportConstant::s_buttonSize.height - 30), Vec2(50, 0));
    m_map[str].pListRight = CreateListFunc(
        Size(size.width*0.5 - 53, size.height - CHmsAirportConstant::s_buttonSize.height - 30), Vec2(size.width*0.5 + 3, 0));

    this->RemoveAllChildInterface();
    this->RegisterAllNodeChild();

    return &m_map[str];
}

CHmsAirportInfoTabPage::TabPageStu* CHmsAirportInfoTabPage::GetPageByTitle(const string& str)
{
    if (m_map.find(str) != m_map.end())
    {
        return &m_map[str];
    }
    else
    {
        return nullptr;
    }
}

void CHmsAirportInfoTabPage::SetOnTabSelectCallback(string title, std::function<void(string title)> func)
{
    auto iter = m_map.find(title);
    if (iter != m_map.end())
    {
        iter->second.tabSelectCallback = func;
    }
}

void CHmsAirportInfoTabPage::OnBtnClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state, const string &str)
{
    if (pBtn->IsSelected())
    {
        pBtn->SetTextColor(Color4B::WHITE);
    }
    else
    {
        pBtn->SetTextColor(Color4B::BLACK);
        pBtn->Selected();
        return;
    }
    for (auto &c : m_map)
    {
        if (c.second.pBtn != pBtn)
        {
            c.second.pBtn->SetEnableCallback(false);
            c.second.pBtn->SetTextColor(Color4B::BLACK);
            c.second.pBtn->UnSelected();
            c.second.pBtn->SetEnableCallback(true);

            c.second.pListLeft->SetVisible(false);
            c.second.pListRight->SetVisible(false);
        }
        else
        {
            c.second.pListLeft->SetVisible(true);
            c.second.pListRight->SetVisible(true);
            if (c.second.tabSelectCallback)
            {
                c.second.tabSelectCallback(str);
            }
        }
    }
}
