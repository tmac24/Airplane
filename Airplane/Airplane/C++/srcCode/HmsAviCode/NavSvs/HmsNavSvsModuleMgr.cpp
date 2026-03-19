#include "HmsNavSvsModuleMgr.h"
#include "CameraCtrl/HmsNavSvsCameraFirst.h"
#include "CameraCtrl/HmsNavSvsCameraThird.h"
#include "CameraCtrl/HmsNavSvsCameraOverlook.h"
#include "CameraCtrl/HmsNavSvsCameraThirdAirport.h"

#include "PositionMgr/HmsNavSvsPositionFixPos.h"
#include "PositionMgr/HmsNavSvsPositionDatabus.h"
#include "PositionMgr/HmsNavSvsPositionRouteView.h"
#include "PositionMgr/HmsNavSvsPositionRoam.h"

#include "SvsCtrlPage/HmsNavSvsCtrlPageBase.h"
#include "SvsCtrlPage/HmsNavSvsCtrlPageRouteView.h"
#include "SvsCtrlPage/HmsNavSvsCtrlPageRoam.h"

CHmsNavSvsModuleMgr* CHmsNavSvsModuleMgr::GetInstance()
{
    static CHmsNavSvsModuleMgr *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsModuleMgr;
    }
    return s_pIns;
}

CHmsNavSvsModuleMgr::CHmsNavSvsModuleMgr()
{
    m_pCurCameraCtrl = nullptr;
    m_pCurPositionMgr = nullptr;
    m_pCurCtrlPage = nullptr;

    m_bNeedInit = true;
}

CHmsNavSvsModuleMgr::~CHmsNavSvsModuleMgr()
{

}


void CHmsNavSvsModuleMgr::InitContent(Size size)
{
    if (!m_bNeedInit)
    {
        return;
    }
    m_bNeedInit = false;

    CHmsNavSvsCameraBase *pCameraCtrl = nullptr;

    pCameraCtrl = new CHmsNavSvsCameraFirst;
    m_mapCameraCtrl[pCameraCtrl->GetCameraType()] = pCameraCtrl;

    pCameraCtrl = new CHmsNavSvsCameraThird;
    m_mapCameraCtrl[pCameraCtrl->GetCameraType()] = pCameraCtrl;

    pCameraCtrl = new CHmsNavSvsCameraOverlook;
    m_mapCameraCtrl[pCameraCtrl->GetCameraType()] = pCameraCtrl;

    pCameraCtrl = new CHmsNavSvsCameraThirdAirport;
    m_mapCameraCtrl[pCameraCtrl->GetCameraType()] = pCameraCtrl;
    //
    CHmsNavSvsPositionBase *pPositionMgr = nullptr;

    pPositionMgr = CHmsNavSvsPositionFixPos::GetInstance();
    m_mapPositionMgr[pPositionMgr->GetPositionMgrType()] = pPositionMgr;

    pPositionMgr = CHmsNavSvsPositionDatabus::GetInstance();
    m_mapPositionMgr[pPositionMgr->GetPositionMgrType()] = pPositionMgr;

    pPositionMgr = CHmsNavSvsPositionRouteView::GetInstance();
    m_mapPositionMgr[pPositionMgr->GetPositionMgrType()] = pPositionMgr;

    pPositionMgr = CHmsNavSvsPositionRoam::GetInstance();
    m_mapPositionMgr[pPositionMgr->GetPositionMgrType()] = pPositionMgr;

    //
    CHmsNavSvsCtrlPageBase *pCtrlPage = nullptr;

    pCtrlPage = CHmsNavSvsCtrlPageBase::Create(size);
    pCtrlPage->retain();
    m_mapCtrlPage[pCtrlPage->GetCtrlPageType()] = pCtrlPage;

    pCtrlPage = CHmsNavSvsCtrlPageRouteView::Create(size);
    pCtrlPage->retain();
    m_mapCtrlPage[pCtrlPage->GetCtrlPageType()] = pCtrlPage;

    pCtrlPage = CHmsNavSvsCtrlPageRoam::Create(size);
    pCtrlPage->retain();
    m_mapCtrlPage[pCtrlPage->GetCtrlPageType()] = pCtrlPage;
}

CHmsNavSvsPositionBase* CHmsNavSvsModuleMgr::GetPositionMgr(NavSvsPositionMgrType type)
{
    auto iter = m_mapPositionMgr.find(type);
    if (iter != m_mapPositionMgr.end())
    {
        return iter->second;
    }
    return nullptr;
}

void CHmsNavSvsModuleMgr::SetCameraCtrl(NavSvsCameraType type)
{
    auto iter = m_mapCameraCtrl.find(type);
    if (iter != m_mapCameraCtrl.end())
    {
        m_pCurCameraCtrl = iter->second;
    }
}

void CHmsNavSvsModuleMgr::SetPositionMgr(NavSvsPositionMgrType type)
{
    auto iter = m_mapPositionMgr.find(type);
    if (iter != m_mapPositionMgr.end())
    {
        m_pCurPositionMgr = iter->second;
    }
}

void CHmsNavSvsModuleMgr::SetCtrlPage(NavSvsCtrlPageType type)
{
    auto iter = m_mapCtrlPage.find(type);
    if (iter != m_mapCtrlPage.end())
    {
        if (m_pCurCtrlPage)
        {
            m_pCurCtrlPage->RemoveFromParent();
            m_pCurCtrlPage->RemoveFromParentInterface();
        }
        m_pCurCtrlPage = iter->second;
    }
}
