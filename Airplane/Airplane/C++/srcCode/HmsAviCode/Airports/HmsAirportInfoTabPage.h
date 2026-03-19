#pragma once

#include "base/HmsNode.h"
#include "ui/HmsUiEventInterface.h"
#include "HmsAirportsStuDef.h"
#include "display/HmsLabel.h"
#include "base/RefPtr.h"
#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiVerticalScrollView.h"
#include <vector>
#include <map>

using namespace HmsAviPf;

class CHmsAirportInfoTabPage : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
    struct TabPageStu
    {
        string title;
        CHmsUiButtonAbstract *pBtn;
        std::function<void(string title)> tabSelectCallback;

        //content
        CHmsUiVerticalScrollView *pListLeft;
        CHmsUiVerticalScrollView *pListRight;

        TabPageStu()
        {
            pBtn = nullptr;
            tabSelectCallback = nullptr;
            pListLeft = nullptr;
            pListRight = nullptr;
        }
    };
public:
    CHmsAirportInfoTabPage();
    ~CHmsAirportInfoTabPage();
    CREATE_FUNC_BY_SIZE(CHmsAirportInfoTabPage)
    virtual bool Init(const HmsAviPf::Size & size);

    TabPageStu* AddPage(string str);
    TabPageStu* GetPageByTitle(const string& str);
    void SetOnTabSelectCallback(string title, std::function<void(string title)> func);

private:
    void OnBtnClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state, const string &str);

private:
    map<string, TabPageStu> m_map;
};

