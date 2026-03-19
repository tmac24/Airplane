#include "HmsNavSvsLayer.h"
#include "render/HmsFrameBuffer.h"
#include "base/HmsCamera.h"
#include "HmsNavSvsLoadCountMgr.h"  
#include "HmsNavSvsControlPage.h"
#include "HmsNavSvsModuleMgr.h"
#include "Language/HmsLanguageMananger.h"

#include "CameraCtrl/HmsNavSvsCameraThirdAirport.h"
#include "PositionMgr/HmsNavSvsPositionFixPos.h"
#include "PositionMgr/HmsNavSvsPositionRouteView.h"
#include "PositionMgr/HmsNavSvsPositionRoam.h"

CHmsNavSvsLayer* CHmsNavSvsLayer::GetInstance()
{
    static CHmsNavSvsLayer* s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = CHmsNavSvsLayer::Create(Size(2560, 1440));
        s_pIns->retain();
    }
    return s_pIns;
}

CHmsNavSvsLayer::CHmsNavSvsLayer()
: HmsUiEventInterface(this)
{

}

CHmsNavSvsLayer::~CHmsNavSvsLayer()
{

}

bool CHmsNavSvsLayer::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNode::Init())
    {
        return false;
    }

    SetContentSize(size);

    m_fCameraNear = 0.000001f;
    m_fCameraFar = 0.1f;
    m_fFieldOfView = 60.0f;
    m_fFieldOfViewRadian = MATH_DEG_TO_RAD(m_fFieldOfView);
    m_fAspectRatio = 1.0f;

    Size fboSize = size;
    auto fbo = FrameBuffer::create(1, fboSize.width, fboSize.height);
    auto rt = RenderTarget::create(fboSize.width, fboSize.height);
    auto rtDS = RenderTargetDepthStencil::create(fboSize.width, fboSize.height);
    fbo->attachRenderTarget(rt);
    fbo->attachDepthStencilTarget(rtDS);
    fbo->setClearColor(Color4F(0, 0, 0, 1.0));

    m_fAspectRatio = (GLfloat)fboSize.width / fboSize.height;
    m_fFieldOfViewRadian = MATH_DEG_TO_RAD(m_fFieldOfView);

    auto pCamera = Camera::createPerspective(m_fFieldOfView, m_fAspectRatio, m_fCameraNear, m_fCameraFar);
    pCamera->setFrameBufferObject(fbo);
    pCamera->setDepth(-1);
    m_pCamera = pCamera;

    m_pSvsMgr = CHmsNavSvsMgr::GetInstance();
    m_pSvsMgr->InitContent(size);
    pCamera->setSpecificRenderRoot(m_pSvsMgr);

    CHmsImageNode *pImageNode = CHmsImageNode::CreateWithTexture(rt->getTexture());
    pImageNode->SetAnchorPoint(Vec2(0, 0));
    pImageNode->SetPosition(0, 0);
    pImageNode->SetFlippedY(true);
    pImageNode->SetFlippedX(true);
    m_pSvsImageNode = pImageNode;

    this->AddChild(pCamera);
    this->AddChild(pImageNode);

    CHmsDrawNode *pDrawNode = CHmsDrawNode::Create();
    this->AddChild(pDrawNode);
    pDrawNode->clear();
    pDrawNode->DrawSolidRect(0, 0, size.width, size.height, Color4F(27.0f / 255.0f, 20.0f / 255.0f, 100.0f / 255.0f, 1.0f));
    m_pBgDrawNode = pDrawNode;

    CHmsLabel *pLabel = CHmsGlobal::CreateLabel("0", 32, true);
    pLabel->SetAnchorPoint(Vec2(0.5, 0.5));
    pLabel->SetPosition(size.width * 0.5, size.height * 0.5);
    pLabel->SetVisible(false);
    pLabel->SetTextColor(Color4B::GREEN);
    m_pLoadLabel = pLabel;
    this->AddChild(pLabel);

#if 0
    auto pControlPage = CHmsNavSvsControlPage::Create(size);
    this->AddChild(pControlPage, 999);
#endif

    Mat4d::createPerspective(m_fFieldOfView, m_fAspectRatio, m_fCameraNear, m_fCameraFar, &m_pSvsMgr->m_mat4dP);

    RegisterMutiTouchEvent();
    RegisterAllNodeChild();

    auto pModeMgr = CHmsNavSvsModuleMgr::GetInstance();
    pModeMgr->InitContent(size);
    SetRoamMode(999, 999);

    return true;
}

void CHmsNavSvsLayer::Update(float delta)
{
    auto pPositionMgr = CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr();
    if (pPositionMgr)
    {
        pPositionMgr->Update(delta);
    }
    else
    {
        return;
    }

    Vec2d lonlat;
    lonlat.x = pPositionMgr->GetLon();
    lonlat.y = pPositionMgr->GetLat();

    if (fabs(lonlat.x) <= 180 && fabs(lonlat.y) <= 90)
    {
        auto xy = CHmsNavSvsMathHelper::GetTileXYByLonLat(13, lonlat);
        auto ll = CHmsNavSvsMathHelper::GetTileLonLatByXY(13, xy);
        Vec3d origin = CHmsNavSvsMathHelper::LonLatToEarthPoint_Double(&ll);
        if (m_pSvsMgr->m_relativeOrigin != origin)
        {
            m_pSvsMgr->m_relativeOrigin = origin;
            origin.negate();
            Mat4d::createTranslation(origin, &m_pSvsMgr->m_mat4dTrans);
        }       
    }
    m_pSvsMgr->m_attitudeStu.LngLat.x = lonlat.x;
    m_pSvsMgr->m_attitudeStu.LngLat.y = lonlat.y;
    m_pSvsMgr->m_attitudeStu.Height = pPositionMgr->GetHeight();
    m_pSvsMgr->m_attitudeStu.yaw = pPositionMgr->GetYaw();
    m_pSvsMgr->m_attitudeStu.pitch = pPositionMgr->GetPitch();
    m_pSvsMgr->m_attitudeStu.roll = pPositionMgr->GetRoll();
    m_pSvsMgr->m_attitudeStu.speed = pPositionMgr->GetSpeed();

    if (m_pSvsMgr->IsLonLatInValidRange(m_pSvsMgr->m_attitudeStu.LngLat.x, m_pSvsMgr->m_attitudeStu.LngLat.y))
    {
        m_pSvsMgr->Update(delta);
    }   
}

static char* StringConverter_GBKToUTF8(const char* chGBK)
{
#if HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32
    static char pUTF8Buf[1024] = { 0 };
    DWORD dWideBufSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)chGBK, -1, NULL, 0);
    wchar_t * pWideBuf = (wchar_t*)malloc(dWideBufSize * sizeof(wchar_t));
    //wchar_t * pWideBuf = new wchar_t[dWideBufSize];
    memset(pWideBuf, 0, dWideBufSize);
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)chGBK, -1, pWideBuf, dWideBufSize);

    DWORD dUTF8BufSize = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pWideBuf, -1, NULL, 0, NULL, NULL);
    //char * pUTF8Buf = new char[dUTF8BufSize];
    memset(pUTF8Buf, 0, dUTF8BufSize);
    WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pWideBuf, -1, pUTF8Buf, dUTF8BufSize, NULL, NULL);

    free(pWideBuf);
    return pUTF8Buf;
#else
    return (char*)chGBK;
#endif
}

void CHmsNavSvsLayer::Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags)
{
    if (m_pSvsMgr->IsLonLatInValidRange(m_pSvsMgr->m_attitudeStu.LngLat.x, m_pSvsMgr->m_attitudeStu.LngLat.y))
    {
        CHmsNavSvsLoadCountMgr *pIns = CHmsNavSvsLoadCountMgr::GetInstance();
        if (pIns->m_bIsLoadFinished)
        {
            m_pBgDrawNode->SetVisible(false);
            m_pLoadLabel->SetVisible(false);
            m_pSvsImageNode->SetVisible(true);
        }
        else
        {
            m_pBgDrawNode->SetVisible(true);
            m_pLoadLabel->SetVisible(true);
            m_pSvsImageNode->SetVisible(false);

            char str[64] = { 0 };
            sprintf(str, "%d%%", (int)(pIns->m_percent * 100));
            m_pLoadLabel->SetString(StringConverter_GBKToUTF8(str));
        }    
    }
    else
    {
        m_pBgDrawNode->SetVisible(true);
        m_pLoadLabel->SetVisible(true);
        m_pSvsImageNode->SetVisible(false);

        if (CHmsLanguageMananger::GetInstance()->GetLanguageName() == "CHN")
        {
            m_pLoadLabel->SetString("\xe6\xb2\xa1\xe6\x9c\x89\xe5\x9c\xb0\xe5\x9b\xbe\xe6\x95\xb0\xe6\x8d\xae");//没有地图数据
        }
        else
        {
            m_pLoadLabel->SetString("No Map Data");
        }
        
    }
    CHmsNode::Visit(renderer, parentTransform, parentFlags);
}

void CHmsNavSvsLayer::OnEnter()
{
    SetScheduleUpdate();

    CHmsNode::OnEnter();
}

void CHmsNavSvsLayer::OnExit()
{
    SetUnSchedule();

    CHmsNode::OnExit();
}

void CHmsNavSvsLayer::ChangeSize(Size size)
{
    auto s = GetContentSize();
    if (s.equals(size))
    {
        return;
    }

    SetContentSize(size);

    Size fboSize = size;
    auto fbo = FrameBuffer::create(1, fboSize.width, fboSize.height);
    auto rt = RenderTarget::create(fboSize.width, fboSize.height);
    auto rtDS = RenderTargetDepthStencil::create(fboSize.width, fboSize.height);
    fbo->attachRenderTarget(rt);
    fbo->attachDepthStencilTarget(rtDS);
    fbo->setClearColor(Color4F(0, 0, 0, 1.0));

    m_fAspectRatio = (GLfloat)fboSize.width / fboSize.height;
    m_fFieldOfViewRadian = MATH_DEG_TO_RAD(m_fFieldOfView);

    m_pCamera->initPerspective(m_fFieldOfView, m_fAspectRatio, m_fCameraNear, m_fCameraFar);
    m_pCamera->setFrameBufferObject(fbo);
    m_pCamera->setDepth(-1);

    m_pSvsImageNode->InitWithTexture(rt->getTexture());
    m_pSvsImageNode->SetAnchorPoint(Vec2(0, 0));
    m_pSvsImageNode->SetPosition(0, 0);
    m_pSvsImageNode->SetFlippedY(true);
    m_pSvsImageNode->SetFlippedX(true);

    m_pBgDrawNode->clear();
    m_pBgDrawNode->DrawSolidRect(0, 0, size.width, size.height, Color4F(27.0f / 255.0f, 20.0f / 255.0f, 100.0f / 255.0f, 1.0f));

    m_pLoadLabel->SetPosition(size.width * 0.5, size.height * 0.5);

    Mat4d::createPerspective(m_fFieldOfView, m_fAspectRatio, m_fCameraNear, m_fCameraFar, &m_pSvsMgr->m_mat4dP);
}

void CHmsNavSvsLayer::SetSvsView(NavSvsCameraType view)
{
    CHmsNavSvsModuleMgr::GetInstance()->SetCameraCtrl(view);
}

void CHmsNavSvsLayer::SetSvsCtrlPage(NavSvsCtrlPageType type)
{
    CHmsNavSvsModuleMgr::GetInstance()->SetCtrlPage(type);

    auto pCtrlPage = CHmsNavSvsModuleMgr::GetInstance()->GetCurCtrlPage();
    if (pCtrlPage)
    {
        this->AddChild(pCtrlPage);
        this->AddChildInterface(pCtrlPage);
    }
}

void CHmsNavSvsLayer::SetAirportViewMode(double lon, double lat, double height)
{
    CHmsNavSvsModuleMgr::GetInstance()->SetCameraCtrl(NavSvsCameraType::SVSCAMERATYPE_THIRD_AIRPORT);
    CHmsNavSvsModuleMgr::GetInstance()->SetPositionMgr(NavSvsPositionMgrType::POSMGR_FIX);
    SetSvsCtrlPage(NavSvsCtrlPageType::CTRLPAGE_BASE);

    auto pCamera = dynamic_cast<CHmsNavSvsCameraThirdAirport*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl());
    if (pCamera)
    {
        pCamera->SetEnableVAngleLimit(true);
        pCamera->SetDistance(1500);
        pCamera->SetExecuteOnceInUpdate(true);
    }

    auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionFixPos*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
    if (pPositionMgr)
    {
        pPositionMgr->SetLonLatHeight(lon, lat, height);       
    }

    auto pCtrl = CHmsNavSvsModuleMgr::GetInstance()->GetCurCtrlPage();
    if (pCtrl)
    {
        pCtrl->SetTitle("Airport View");
    }
}

void CHmsNavSvsLayer::SetRouteViewMode(const std::vector<FPL2DNodeInfoStu> &vec)
{
    CHmsNavSvsModuleMgr::GetInstance()->SetCameraCtrl(NavSvsCameraType::SVSCAMERATYPE_THIRD);
    CHmsNavSvsModuleMgr::GetInstance()->SetPositionMgr(NavSvsPositionMgrType::POSMGR_ROUTE);
    SetSvsCtrlPage(NavSvsCtrlPageType::CTRLPAGE_ROUTEVIEW);

    auto pCamera = dynamic_cast<CHmsNavSvsCameraThirdAirport*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl());
    if (pCamera)
    {
        pCamera->SetEnableVAngleLimit(false);
        pCamera->SetDistance(100);
        pCamera->SetExecuteOnceInUpdate(true);
    }

    auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRouteView*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
    if (pPositionMgr)
    {
        pPositionMgr->SetRouteData(vec);
    }

    auto pCtrl = CHmsNavSvsModuleMgr::GetInstance()->GetCurCtrlPage();
    if (pCtrl)
    {
        pCtrl->SetTitle("Route View");
    }
}

void CHmsNavSvsLayer::SetRoamMode(double lon, double lat)
{
    CHmsNavSvsModuleMgr::GetInstance()->SetCameraCtrl(NavSvsCameraType::SVSCAMERATYPE_FIRST);
    CHmsNavSvsModuleMgr::GetInstance()->SetPositionMgr(NavSvsPositionMgrType::POSMGR_ROAM);
    SetSvsCtrlPage(NavSvsCtrlPageType::CTRLPAGE_ROAM);

    auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRoam*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
    if (pPositionMgr)
    {
        pPositionMgr->SetLonLat(lon, lat);
    }

    auto pCtrl = CHmsNavSvsModuleMgr::GetInstance()->GetCurCtrlPage();
    if (pCtrl)
    {
        pCtrl->SetTitle("Roam");
    }
}

void CHmsNavSvsLayer::MoveNear(float dis)
{
    auto pCameraCtrl = CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl();
    if (pCameraCtrl)
    {
        pCameraCtrl->MoveNear(dis);
    }
}

void CHmsNavSvsLayer::MoveFar(float dis)
{
    auto pCameraCtrl = CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl();
    if (pCameraCtrl)
    {
        pCameraCtrl->MoveFar(dis);
    }
}
