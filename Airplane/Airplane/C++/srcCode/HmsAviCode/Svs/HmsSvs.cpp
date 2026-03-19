#include "HmsSvs.h"
#include "HmsSVSFrame.h"
#include "HmsConfigXMLReader.h"
#include "HVTMgr.h"
#include "Module.h"


#define SVS_VIEWER_ID 870

CHmsSvs::CHmsSvs()
: m_pCameraControler(nullptr)
, HmsUiEventInterface(this)
, m_pSvsFrame(nullptr)
, m_bChildTouch(false)
{

}

CHmsSvs::~CHmsSvs()
{

}

void CHmsSvs::OnEnter()
{
    CHmsNode::OnEnter();
    SetScheduleUpdate();
}

void CHmsSvs::OnExit()
{
    CHmsNode::OnExit();
    this->SetUnSchedule();
}

void CHmsSvs::Update(float delta)
{
    CHmsNode::Update(delta);
}

void CHmsSvs::ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos)
{
    CHmsResizeableLayer::ResetLayerSize(size, pos);

    auto svsViewer = dynamic_cast<HmsSVSViewer*>(this->GetChildByTag(SVS_VIEWER_ID));
    if (svsViewer)
    {
#if 0
        Rect rectTexture(Vec2(), m_sizeLayout);
        svsViewer->SetTextureRect(rectTexture, false, size);

        //svsViewer->SetScaleX(size.width/m_sizeLayout.width);
#else
        Rect rectTexture(Vec2(), size);
        svsViewer->SetTextureRect(rectTexture, false, size);
#endif		
    }

    if (m_nodeCameraInd)
    {
        auto size = this->GetContentSize();
        auto offsetY = 450;
        auto offsetX = 450;
        m_nodeCameraInd->SetPosition(Vec2(size.width / 2 - offsetX, size.height / 2 - offsetY));
    }

    if (m_pSvsFrame)
    {
        m_pSvsFrame->ResetFrameSize(size);
    }
}

void CHmsSvs::Set1stPM(void)
{
    if (m_pCameraControler)
    {
        m_pCameraControler->Set1stPM();
    }
}

void CHmsSvs::Set3rdPM(void)
{
    if (m_pCameraControler)
    {
        m_pCameraControler->Set3rdPM();
    }
}

void CHmsSvs::SetAircraftModel(const std::string &name)
{
    bool res = CHmodule_SetAircraftModel(name.c_str());
    if (false == res)
    {
        std::cout << "set aircraft model failed" << std::endl;
    }
}

void CHmsSvs::SetWarnningDistance(float fDistanceMeter)
{
    HVTMgr::getInstance()->SetAlarmDistance(fDistanceMeter);
}

void CHmsSvs::SetFlyPathDisplay(bool bShow)
{
    if (bShow)
    {
        GetGlobal()->m_waypointRender = 1;
    }
    else
    {
        GetGlobal()->m_waypointRender = 0;
    }
}

bool CHmsSvs::Init(const HmsAviPf::Size & size)
{
    if (!CHmsResizeableLayer::Init())
    {
        return false;
    }
    m_sizeLayout = size;
    SetContentSize(this->m_sizeLayout);

    auto offsetY = 420;
    auto offsetX = 450;
    m_nodeCameraInd = CHmsCameraIndicator::Create();
    m_nodeCameraInd->SetPosition(Vec2(m_sizeLayout.width / 2 - offsetX, m_sizeLayout.height / 2 - offsetY));
    m_nodeCameraInd->SetLocalZOrder(1);
    AddChild(m_nodeCameraInd);

    int border = 4;
    auto sf = HmsSVSFrame::Create(Size(m_sizeLayout.width - border * 2, m_sizeLayout.height - border * 2));
    auto svsViewerNode = sf->CreateSVSViewer();
    svsViewerNode->SetAnchorPoint(Vec2(0, 0));
    svsViewerNode->SetPosition(0, 0);
    svsViewerNode->SetFlippedY(true);
    svsViewerNode->SetTag(SVS_VIEWER_ID);
    auto config = CHmsConfigXMLReader::GetSingleConfig();
    auto strSvsEnable = config->GetConfigInfo("SvsEnable", "true");
    if (strSvsEnable == std::string("true"))
    {
        this->AddChild(sf);
        this->AddChild(svsViewerNode);
    }

    m_pCameraControler = new CameraController(m_nodeCameraInd);
    this->AddChild(m_pCameraControler);

    m_pSvsFrame = sf;

    InitFlowBtns();

    return true;
}

bool CHmsSvs::OnPressed(const Vec2 & posOrigin)
{
    m_bChildTouch = HmsUiEventInterface::OnPressed(posOrigin);
    if (!m_bChildTouch)
    {
        m_posLast = posOrigin;
        m_pCameraControler->onPress(posOrigin);
    }
    return true;
}

void CHmsSvs::OnReleased(const Vec2 & posOrigin)
{
    if (m_bChildTouch)
    {
        HmsUiEventInterface::OnReleased(posOrigin);
    }
    else
    {
        m_pCameraControler->onRelease(posOrigin);
    }
}

void CHmsSvs::OnMove(const Vec2 & posOrigin)
{
    auto posOri = posOrigin;

    if (posOri.y<0 || posOri.x> CHmsGlobal::GetDesignWidth())
    {   //for windows, 
        //if mouse cursor move out of the left border of the window form, x suddenly turn into a very large positive number
        //if mouse cursor move out of the upper border of the window form, y suddenly turn into a very small negative number.
        //here is the solusion: force it equal to last positon.
        posOri = m_posLast;
    }

    if (m_bChildTouch)
    {
        HmsUiEventInterface::OnMove(posOri);
    }
    else
    {
        Vec2 deltaPos = posOri - m_posLast;
        m_pCameraControler->onMove(-deltaPos);
        m_posLast = posOri;
    }
}

#include "ui/HmsUiImagePanel.h"
#include "ToolBar/HmsToolBarFlowBtn.h"
void CHmsSvs::InitFlowBtns()
{
    auto rootModeMenu = CHmsUiPanel::Create();
    float fMargin = 10.0f;
    Size sizeShow(fMargin * 2 + CHmsToolBarFlowBtn::GetDesignBtnSize().width, fMargin);
    Vec2 pos(fMargin, fMargin + sizeShow.height / 2.0f);
    float fMoveY = CHmsToolBarFlowBtn::GetDesignBtnSize().height + fMargin;

    auto CreateBtns = [=, &pos, &sizeShow](const char * strModeName, const char * strIconPath, const CHmsUiButtonAbstract::HmsUiBtnCallback & callback)
    {
        auto btn = CHmsToolBarFlowBtn::CreateWithImage(strIconPath, strModeName);
        btn->SetToggleBtn(true);
        btn->SetClick2Normal(true);
        btn->SetAnchorPoint(0, 0.0f);
        btn->SetPosition(pos);
        btn->SetCallbackFunc(callback);

        pos.y += fMoveY;
        sizeShow.height += fMoveY;
        rootModeMenu->AddChild(btn);
        return btn;
    };

    auto btnFlyPath = CreateBtns("FlyPath", "res/ToolBarIcon/flyPathIcon.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
        bool bDisplay = false;
        if (eState == HMS_UI_BUTTON_STATE::btn_selected)
            bDisplay = true;

        SetFlyPathDisplay(bDisplay);
    });

    auto btnViewMode = CreateBtns("NavMode", "res/ToolBarIcon/3rdView.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
        if (eState == HMS_UI_BUTTON_STATE::btn_selected)
        {
            Set3rdPM();
        }
        else
        {
            Set1stPM();
        }
    });

    rootModeMenu->SetContentSize(sizeShow);
    rootModeMenu->RegisterAllNodeChild();
    this->AddChild(rootModeMenu, 10);
    this->AddChildInterface(rootModeMenu);

    rootModeMenu->SetAnchorPoint(Vec2(0.0f, 1.0f));
    Vec2 posMenu(0, GetContentSize().height);
    posMenu += Vec2(40 - fMargin, -40 + fMargin);
    rootModeMenu->SetPosition(posMenu);

    Set1stPM();
}

