#include "HmsSettingTrackDataApplyPage.h"
#include "ui/HmsUiImageButton.h"
#include "ui/HmsUiRadio.h"
#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiImagePanel.h"
#include "Language/HmsLanguageLabel.h"
#include "HmsLog.h"
#include "HmsGlobal.h"
#include "Nav/HmsFrame2DRoot.h"
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include "HmsAndroidFunc.h"
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX || HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#else
#include "io.h"
#endif

extern std::string g_stringRecordTrackPath;

USING_NS_HMS;

CHmsSettingTrackDataApplyPage::CHmsSettingTrackDataApplyPage()
: m_eAction(TrackApplyAction::NoAction)
{

}

CHmsSettingTrackDataApplyPage::~CHmsSettingTrackDataApplyPage()
{

}

CHmsSettingTrackDataApplyPage * CHmsSettingTrackDataApplyPage::Create(const HmsAviPf::Size & size)
{
    CHmsSettingTrackDataApplyPage *ret = new CHmsSettingTrackDataApplyPage();
    if (ret && ret->init(size))
    {
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

#define XXXINFO(x) (x*CHmsGlobal::GetDesignScale())

bool CHmsSettingTrackDataApplyPage::init(const HmsAviPf::Size & size)
{
    if (!CHmsSettingSubPage::init(size))
    {
        return false;
    }
    float m_fContentBeginY = size.height - 100;
    auto posPanel = Vec2(0, m_fContentBeginY);

    auto sizeBoder = Size(10, 0);
    auto sizeContentShow = Size(size.width, m_fContentBeginY - 20 * 2 - 90);
    sizeContentShow = sizeContentShow - (sizeBoder * 2);

    auto window = CHmsUiVerticalScrollView::Create(sizeContentShow);
    window->SetPosition(posPanel + Vec2(sizeBoder.width, -sizeBoder.height));
    window->SetAnchorPoint(Vec2(0, 1.0f));
    window->SetItemVSpace(10);
    this->AddChild(window);

    m_viewWindow = window;

    Size btnSize(250, 90);
    float fFontSize = 34;

    do
    {
        auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
        btn->SetAnchorPoint(Vec2(1.0, 0));
        btn->SetPosition(Vec2((size.width - btnSize.width) * 0.5 - 10, 20));
        btn->AddText("To Route", fFontSize, Color4B::WHITE, true);
        btn->SetOnClickedFunc(
            [=](CHmsUiButtonAbstract *btn){
            OnClickToRoute();
        });

        this->AddChild(btn);
        this->AddChildInterface(btn);
    } while (0);

    do
    {
        auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
        btn->SetAnchorPoint(Vec2(0.5f, 0.0f));
        btn->SetPosition(Vec2(size.width / 2.0, 20));
        btn->AddText("Reverse Route", fFontSize, Color4B::WHITE, true);
        btn->SetOnClickedFunc(
            [=](CHmsUiButtonAbstract *btn){
            OnClickToReverseRoute();
        });

        this->AddChild(btn);
        this->AddChildInterface(btn);
    } while (0);

    do
    {
        auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
        btn->SetAnchorPoint(Vec2(0, 0));
        btn->SetPosition(Vec2((size.width + btnSize.width) * 0.5 + 10, 20));
        btn->AddText("Clean Selected", fFontSize, Color4B::WHITE, true);
        btn->SetOnClickedFunc(
            [=](CHmsUiButtonAbstract *btn){
            OnClickCleanSelected();
        });

        this->AddChild(btn);
        this->AddChildInterface(btn);
    } while (0);


    InitComfirmDlg();

    this->SetClickCallback(HMS_CALLBACK_2(CHmsSettingTrackDataApplyPage::OnItemSelected, this));

    RegisterAllNodeChild();
    return true;
}

void CHmsSettingTrackDataApplyPage::InitComfirmDlg()
{
    Size bgSize(GetContentSize().width - 100, 280);

    auto panel = CHmsUiStretchImagePanel::CreateWithImage("res/BasicFrame/Round4SolidWhite.png", bgSize);
    panel->SetColor(Color3B(0xDE, 0xDE, 0xDE));
    panel->SetPosition(GetContentSize() / 2.0f);
    panel->SetLocalZOrder(10);
    this->AddChild(panel);

    Size btnSize(220, 90);

    m_labelConfirmTitle = CHmsGlobal::CreateLanguageLabel("Is nothing!", 40, Color4F::BLACK, true);
    m_labelConfirmTitle->SetAnchorPoint(0.5f, 0.5f);
    m_labelConfirmTitle->SetPosition(bgSize.width / 2.0f, 20 + 90 + (bgSize.height - 20 - 90) * 0.5f);
    panel->AddChild(m_labelConfirmTitle);

    do
    {
        auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
        btn->SetAnchorPoint(Vec2(1.0, 0));
        btn->SetPosition(Vec2(bgSize.width / 2.0 - 10, 20));
        btn->AddText("Cancel", 38, Color4B::WHITE, true);
        btn->SetOnClickedFunc(
            [=](CHmsUiButtonAbstract *btn){
            OnClickConfirmDlgCancel();
        });

        panel->AddChild(btn);
        panel->AddChildInterface(btn);
    } while (0);

    do
    {
        auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
        btn->SetAnchorPoint(Vec2(0, 0));
        btn->SetPosition(Vec2(bgSize.width / 2.0 + 10, 20));
        btn->AddText("Confirm", 38, Color4B::WHITE, true);
        btn->SetOnClickedFunc(
            [=](CHmsUiButtonAbstract *btn){
            OnClickConfirmDlgConfirm();
        });

        panel->AddChild(btn);
        panel->AddChildInterface(btn);
    } while (0);

    this->AddChild(panel);
    this->AddChildInterface(panel);


    m_nodeConfirmDlg = panel;
    m_nodeConfirmDlg->SetVisible(false);
}



void CHmsSettingTrackDataApplyPage::SetContentCheckBox(const std::vector<TrackItemInfo> & vItemContent)
{
    float fContentBoder = 10;
    float fPageWidth = GetContentSize().width;
    float fContentWidth = fPageWidth - 2 * fContentBoder;
    Size sizeItem(fContentWidth, 90);

    m_mapCheckboxGroup.clear();
    m_nodeDraw->clear();
    this->RemoveAllContentNode();

    auto getSelectedBtn = [=](const TrackItemInfo & item)
    {
        auto btn = CHmsUiImage2StateButton::CreateWithImage("res/Btns/checkRightRU.png", "res/Btns/checkRightRS.png");
        btn->SetToggleBtn(true);
        btn->SetSelected(item.bSelected);
        btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsSettingTrackDataApplyPage::OnCheckBoxClick, this, item.strName, item.nParam));
        return btn;
    };

    auto getRadioBtn = [=](const TrackItemInfo & item)
    {
        //auto radio = CHmsUiRadio::CreateWithImage("res/Btns/checkRightCU.png", "res/Btns/checkRightCS.png");
        auto radio = CHmsUiImage2StateButton::CreateWithImage("res/Btns/checkRightCU.png", "res/Btns/checkRightCS.png");
        radio->SetToggleBtn(true);
        radio->SetSelected(item.bSelected);
        m_mapCheckboxGroup.insert(item.strName, radio);
        radio->SetCallbackFunc(HMS_CALLBACK_2(CHmsSettingTrackDataApplyPage::OnCheckBoxClick, this, item.strName, item.nParam));
        return radio;
    };

    for (auto c : vItemContent)
    {
        auto panel = CHmsUiStretchImagePanel::CreateWithImage("res/BasicFrame/SolidFrame.png", sizeItem);
        panel->SetColor(Color3B(0xC9, 0xC9, 0xC9));
        panel->SetAnchorPoint(Vec2(0, 1.0));

        auto labelContent = CHmsLanguageLabel::CreateLabel(c.strName.c_str(), (38), true)->GetLabel();
        labelContent->SetTextColor(Color4B::BLACK);
        labelContent->SetAnchorPoint(Vec2(0, 0.5f));
        labelContent->SetTag(88);
        labelContent->SetPosition(64, sizeItem.height*0.5f);
        panel->AddChild(labelContent);

        auto btn = getRadioBtn(c);
        btn->SetAnchorPoint(Vec2(1.0f, 0.5f));
        btn->SetPosition(fContentWidth - 32, sizeItem.height*0.5f);
        panel->AddChild(btn);

        panel->RegisterAllNodeChild();
        m_viewWindow->AddWindowItem(panel);
    }
}

void CHmsSettingTrackDataApplyPage::OnEnter()
{
    CHmsSettingSubPage::OnEnter();

    UpdateItem();
}


void CHmsSettingTrackDataApplyPage::AddContentNode(HmsAviPf::CHmsNode * node)
{
    if (node)
    {
        m_vContectNode.pushBack(node);
        m_viewWindow->AddWindowItem(node);
    }
}

void CHmsSettingTrackDataApplyPage::RemoveAllContentNode()
{
    m_viewWindow->RemoveAllItem();
    CHmsSettingSubPage::RemoveAllContentNode();
}

void CHmsSettingTrackDataApplyPage::UpdateItem()
{
    m_vItemInfo.clear();
    GetRecordItemInfo(m_vItemInfo);
    if (!m_strCurItemName.empty())
    {
        for (auto & c : m_vItemInfo)
        {
            if (c.strName == m_strCurItemName)
            {
                c.bSelected = true;
            }
        }
    }
    SetContentCheckBox(m_vItemInfo);
}

bool CHmsSettingTrackDataApplyPage::GetRecordItemInfo(std::vector<TrackItemInfo> & vItemInfo)
{
    TrackItemInfo info;
    info.nParam = 0;
    info.nSize = 0;
    info.bSelected = false;

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
    DIR *dir = opendir(g_stringRecordTrackPath.c_str());
    if (nullptr == dir)
    {
        HMSLOG("Can not open dir. Check path or permission!");
    }

    struct dirent *file;
    // read all the files in dir
    while ((file = readdir(dir)) != NULL)
    {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        {
            HMSLOG("ignore . and ..");
            continue;
        }
        if (file->d_type == DT_DIR) {
            //string filePath = g_stringRecordPath + file->d_name;
            HMSLOG("ignore Folder");
            continue;
        }
        else {
            if (nullptr != strstr(file->d_name, ".trk"));
            {
                struct stat statbuff;	
                info.strName = file->d_name;
                std::string filePath = g_stringRecordTrackPath + file->d_name;
                if(stat(filePath.c_str(), &statbuff) < 0){
                    info.nSize = 0;
                }
                else
                {
                    info.nSize = statbuff.st_size;
                }
                vItemInfo.push_back(info);
                info.nParam++;
            }
        }
    }
    closedir(dir);
#else
    std::string strPath = g_stringRecordTrackPath + "*.trk";

    intptr_t handle;
    _finddata_t findData;
    handle = _findfirst(strPath.c_str(), &findData);    // find the fist file

    if (handle == -1)
    {
        HMSLOG("Failed to find first file!\n");
        return false;
    }

    do
    {
        if (findData.attrib & _A_SUBDIR
            && strcmp(findData.name, ".") == 0
            && strcmp(findData.name, "..") == 0)
        {
            HMSLOG("%s\t<dir>\n", findData.name);
        }
        else
        {
            info.strName = findData.name;
            info.nSize = findData.size;
            vItemInfo.push_back(info);
            info.nParam++;

            HMSLOG("%s size=%d\n", findData.name, findData.size);
        }
    } while (_findnext(handle, &findData) == 0);    // find the next file

    HMSLOG("Done!\n");
    _findclose(handle); //close the find handle
#endif

    return true;
}

void CHmsSettingTrackDataApplyPage::OnCheckBoxClick(CHmsUiImage2StateButton * sender, HmsAviPf::HMS_UI_BUTTON_STATE state, const std::string & strName, int nParam)
{
    for (auto & c : m_vItemInfo)
    {
        if (c.strName == strName)
        {
            if (state == HMS_UI_BUTTON_STATE::btn_selected)
            {
                c.bSelected = true;
                OnItemSelected(strName, nParam);
                m_strCurItemName = strName;
            }
            else
            {
                c.bSelected = false;
                if (strName == m_strCurItemName)
                {
                    OnItemUnSelected(strName, nParam);
                    m_strCurItemName = "";
                }
            }
        }
        else
        {
            c.bSelected = false;
        }
    }

    if (state == HMS_UI_BUTTON_STATE::btn_selected)
    {
        for (auto c : m_mapCheckboxGroup)
        {
            if (nullptr == c.second)
                continue;

            if (sender != c.second)
            {
                if (c.second->IsSelected())
                {
                    c.second->SetSelected(false);
                }
            }
        }
    }
}
#include "DataInterface/HmsTrackDataServer.h"
#include "ToolBar/HmsAvionicsDisplayMgr.h"
#include "Nav/Frame2DLayer/HmsFrame2DAftTrackLayer.h"
void CHmsSettingTrackDataApplyPage::OnItemSelected(const std::string & strName, int nParam)
{
    CHmsFrame2DAftTrackLayer * player = dynamic_cast<CHmsFrame2DAftTrackLayer*>(CHmsAvionicsDisplayMgr::GetInstance()->GetTrackLayer());
    if (player)
    {
        auto info = CHmsGlobal::GetInstance()->GetTrackDataServer()->GetHistoryTrackPos(strName);
        if (info.size() > 0)
        {
            std::vector<Vec3> vPos;
            Vec3 pos;
            for (auto & c : info)
            {
                pos = CHmsTrackDataServer::Get3DPostion(c.pos);
                vPos.push_back(pos);
            }
            player->DisplayHistoryTrack(vPos);
        }
        else
        {
            player->CleanHistoryTrack();
        }
    }
}

void CHmsSettingTrackDataApplyPage::OnItemUnSelected(const std::string & strName, int nParam)
{
    CHmsFrame2DAftTrackLayer * player = dynamic_cast<CHmsFrame2DAftTrackLayer*>(CHmsAvionicsDisplayMgr::GetInstance()->GetTrackLayer());
    if (player)
    {
        player->CleanHistoryTrack();
    }
}

void CHmsSettingTrackDataApplyPage::OnClickToRoute()
{
    if (m_nodeConfirmDlg->IsVisible())
    {
        m_nodeConfirmDlg->SetVisible(false);
    }
    else
    {
        m_nodeConfirmDlg->SetVisible(true);

        CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is use the track data to create \n route!");
        m_eAction = TrackApplyAction::ToRoute;
    }
}

void CHmsSettingTrackDataApplyPage::OnClickCleanSelected()
{
    if (m_nodeConfirmDlg->IsVisible())
    {
        m_nodeConfirmDlg->SetVisible(false);
    }
    else
    {
        m_nodeConfirmDlg->SetVisible(true);
        CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is Clean the selected files!");
        m_eAction = TrackApplyAction::CleanSelected;
    }
}

void CHmsSettingTrackDataApplyPage::OnClickToReverseRoute()
{
    if (m_nodeConfirmDlg->IsVisible())
    {
        m_nodeConfirmDlg->SetVisible(false);
    }
    else
    {
        m_nodeConfirmDlg->SetVisible(true);
        CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is use the track data to create \n reverse route!");
        m_eAction = TrackApplyAction::ToReverseRoute;
    }
}

void CHmsSettingTrackDataApplyPage::OnClickConfirmDlgCancel()
{
    m_nodeConfirmDlg->SetVisible(false);
    m_eAction = TrackApplyAction::NoAction;
}

void CHmsSettingTrackDataApplyPage::OnClickConfirmDlgConfirm()
{
    m_nodeConfirmDlg->SetVisible(false);
    switch (m_eAction)
    {
    case TrackApplyAction::NoAction:
        break;
    case TrackApplyAction::ToRoute:
        DoToRoute();
        break;
    case TrackApplyAction::CleanSelected:
        DoCleanSelected();
        break;
    case TrackApplyAction::ToReverseRoute:
        DoToReverseRoute();
        break;
    default:
        break;
    }
    m_eAction = TrackApplyAction::NoAction;
}
#include "Calculate/CalculateZs.h"
void CHmsSettingTrackDataApplyPage::DoToRoute()
{
    if (m_strCurItemName.empty())
    {
        return;
    }

    auto info = CHmsGlobal::GetInstance()->GetTrackDataServer()->GetHistoryTrackPos(m_strCurItemName);
    if (info.size() <= 5)
    {
        return;
    }

    CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ClearAll();
    WptNodeBrief brief;
    brief.wType = WptPointType::WPT_WPT;
    brief.attr.altStu[0].Clean();
    brief.attr.altStu[1].Clean();
    int i = 0;

    auto PutWpt = [=, &i, &brief](const HmsTrackPosition & htp)
    {
        i++;
        brief.lon = htp.pos.dLongitude;
        brief.lat = htp.pos.dLatitude;
        brief.attr.altStu[0].uLimit.bitType.bAt = 1;
        brief.attr.altStu[0].nAltitude = htp.altFt;
        brief.ident = "T-" + std::to_string(i);

        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertWptNode(brief, FPLNodeIndex(), false);
    };

    double dDistance = 0.0f;

    PutWpt(info.at(0));

    for (unsigned int i = 1; i < info.size(); i++)
    {
        auto & curPos = info.at(i);
        auto & prePos = info.at(i - 1);
        dDistance += curPos.pos.GetDistanceKm(prePos.pos);
        if (dDistance >= 5.0f)
        {
            PutWpt(curPos);
            dDistance = 0.0f;
        }
    }

    if (dDistance > 0.0f)
    {
        PutWpt(info.at(info.size() - 1));
    }
}

void CHmsSettingTrackDataApplyPage::DoCleanSelected()
{
    int nRemove = 0;
    int nRemoveError = 0;

    for (auto & c : m_vItemInfo)
    {
        if (c.bSelected)
        {
            std::string filePath = g_stringRecordTrackPath + c.strName;
            if (0 == remove(filePath.c_str()))
            {
                HMSLOG("delete file %s success!", filePath.c_str());
                nRemove++;
            }
            else
            {
                HMSLOG("delete file %s Error!", filePath.c_str());
                nRemoveError++;
            }
        }
    }

    UpdateItem();
}

void CHmsSettingTrackDataApplyPage::DoToReverseRoute()
{
    if (m_strCurItemName.empty())
    {
        return;
    }

    auto info = CHmsGlobal::GetInstance()->GetTrackDataServer()->GetHistoryTrackPos(m_strCurItemName);
    if (info.size() <= 5)
    {
        return;
    }

    CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ClearAll();
    WptNodeBrief brief;
    brief.wType = WptPointType::WPT_WPT;
    brief.attr.altStu[0].Clean();
    brief.attr.altStu[1].Clean();
    int i = 0;

    auto PutWpt = [=, &i, &brief](const HmsTrackPosition & htp)
    {
        i++;
        brief.lon = htp.pos.dLongitude;
        brief.lat = htp.pos.dLatitude;
        brief.attr.altStu[0].uLimit.bitType.bAt = 1;
        brief.attr.altStu[0].nAltitude = htp.altFt;
        brief.ident = "RT-" + std::to_string(i);

        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertWptNode(brief, FPLNodeIndex(), false);
    };

    double dDistance = 0.0f;

    PutWpt(info.at(info.size() - 1));

    for (int i = info.size() - 1; i > 0; i--)
    {
        auto & curPos = info.at(i - 1);
        auto & prePos = info.at(i);
        dDistance += curPos.pos.GetDistanceKm(prePos.pos);
        if (dDistance >= 5.0f)
        {
            PutWpt(curPos);
            dDistance = 0.0f;
        }
    }

    if (dDistance > 0.0f)
    {
        PutWpt(info.at(0));
    }
}
