#include "HmsNavSvsControlPage.h"
#include "ui/HmsUiStretchButton.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsNavSvsMgr.h"
#include "HmsNavSvsLayer.h"
#include "HmsNavSvsModuleMgr.h"
#include "NavSvsObject/HmsNavSvsObjectMgr.h"

CHmsNavSvsControlPage::CHmsNavSvsControlPage()
: HmsUiEventInterface(this)
{

}

CHmsNavSvsControlPage::~CHmsNavSvsControlPage()
{

}

bool CHmsNavSvsControlPage::Init(const HmsAviPf::Size & size)
{
    SetContentSize(size);

    auto CreateButton = [=](const char *text, Size buttonSize, int index)
    {
        int xIndex = index % 11;
        int yIndex = index / 11;
        auto pos = Vec2(250 + (buttonSize.width + 10) * xIndex, size.height - 10 - (buttonSize.height + 10) * yIndex);

        auto pItem = CHmsUiStretchButton::Create("res/display/btn_pressed.png",
            buttonSize, Color3B(0x0e, 0x19, 0x25), Color3B::GREEN);
        pItem->AddText(text, 20, Color4B::WHITE);
        pItem->SetAnchorPoint(Vec2(0, 1));
        pItem->SetPosition(pos);
        this->AddChild(pItem);
        return pItem;
    };

    auto buttonSize = Size(150, 70);
    int index = 0;
    CHmsUiStretchButton *pButton = nullptr;
    pButton = CreateButton("First View", buttonSize, index++);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        auto parent = dynamic_cast<CHmsNavSvsLayer*>(GetParent());
        if (parent)
        {
            parent->SetSvsView(NavSvsCameraType::SVSCAMERATYPE_FIRST);
        }
    });
    pButton = CreateButton("Third View", buttonSize, index++);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        auto parent = dynamic_cast<CHmsNavSvsLayer*>(GetParent());
        if (parent)
        {
            parent->SetSvsView(NavSvsCameraType::SVSCAMERATYPE_THIRD);
        }
    });
    pButton = CreateButton("Overlook View", buttonSize, index++);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        auto parent = dynamic_cast<CHmsNavSvsLayer*>(GetParent());
        if (parent)
        {
            parent->SetSvsView(NavSvsCameraType::SVSCAMERATYPE_OVERLOOK);
        }
    });
    pButton = CreateButton("Plane", buttonSize, index++);
    pButton->SetToggleBtn(true);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        CHmsNavSvsObjectMgr::GetInstance()->SetNavSvsObjectEnable(NavSvsObjectType::OBJECTTYPE_PLANE, pButton->IsSelected());
    });
    pButton = CreateButton("Runway", buttonSize, index++);
    pButton->SetToggleBtn(true);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        CHmsNavSvsObjectMgr::GetInstance()->SetNavSvsObjectEnable(NavSvsObjectType::OBJECTTYPE_RUNWAY, pButton->IsSelected());
    });
    pButton = CreateButton("Flight Pipeline", buttonSize, index++);
    pButton->SetToggleBtn(true);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        CHmsNavSvsObjectMgr::GetInstance()->SetNavSvsObjectEnable(NavSvsObjectType::OBJECTTYPE_FLYPIPE, pButton->IsSelected());
    });
    pButton = CreateButton("ADS-B", buttonSize, index++);
    pButton->SetToggleBtn(true);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        CHmsNavSvsObjectMgr::GetInstance()->SetNavSvsObjectEnable(NavSvsObjectType::OBJECTTYPE_ADSB, pButton->IsSelected());
    });
    pButton = CreateButton("Map Type", buttonSize, index++);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        CHmsNavSvsMgr::GetInstance()->SetSatelliteModel(!CHmsNavSvsMgr::GetInstance()->IsEnableSatelliteModel());
    });
    pButton = CreateButton("Terrain Alarm", buttonSize, index++);
    pButton->SetToggleBtn(true);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        CHmsNavSvsMgr::GetInstance()->SetTerrainAlarm(!CHmsNavSvsMgr::GetInstance()->IsEnableTerrainAlarm());
    });
    pButton = CreateButton("Databus", buttonSize, index++);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        auto parent = dynamic_cast<CHmsNavSvsLayer*>(GetParent());
        if (parent)
        {
            CHmsNavSvsModuleMgr::GetInstance()->SetCameraCtrl(NavSvsCameraType::SVSCAMERATYPE_FIRST);
            CHmsNavSvsModuleMgr::GetInstance()->SetPositionMgr(NavSvsPositionMgrType::POSMGR_DATABUS);
            parent->SetSvsCtrlPage(NavSvsCtrlPageType::CTRLPAGE_BASE);
        }
    });
    pButton = CreateButton("Roam ", buttonSize, index++);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        auto parent = dynamic_cast<CHmsNavSvsLayer*>(GetParent());
        if (parent)
        {
            auto pPositionMgr = CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr();
            if (pPositionMgr)
            {
                parent->SetRoamMode(pPositionMgr->GetLon(), pPositionMgr->GetLat());
            }
        }
    });
    pButton = CreateButton("Move Near", buttonSize, index++);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        auto parent = dynamic_cast<CHmsNavSvsLayer*>(GetParent());
        if (parent)
        {
            parent->MoveNear(300);
        }
    });
    pButton = CreateButton("Move Far", buttonSize, index++);
    pButton->SetOnClickedFunc(
        [=](CHmsUiButtonAbstract*){

        auto parent = dynamic_cast<CHmsNavSvsLayer*>(GetParent());
        if (parent)
        {
            parent->MoveFar(300);
        }
    });


    RegisterAllNodeChild();

    return true;
}
