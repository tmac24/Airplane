#pragma once

#include "CameraCtrl/HmsNavSvsCamera.h"
#include "PositionMgr/HmsNavSvsPosition.h"
#include "SvsCtrlPage/HmsNavSvsCtrlPageBase.h"
#include <map>
#include "math/HmsMath.h"

class CHmsNavSvsModuleMgr
{
public:
    static CHmsNavSvsModuleMgr* GetInstance();
    void InitContent(Size size);

    inline CHmsNavSvsCameraBase* GetCurCameraCtrl(){ return m_pCurCameraCtrl; }
    inline CHmsNavSvsPositionBase* GetCurPositionMgr(){ return m_pCurPositionMgr; }
    inline CHmsNavSvsCtrlPageBase* GetCurCtrlPage(){ return m_pCurCtrlPage; }

    CHmsNavSvsPositionBase* GetPositionMgr(NavSvsPositionMgrType type);

    void SetCameraCtrl(NavSvsCameraType type);
    void SetPositionMgr(NavSvsPositionMgrType type);
    void SetCtrlPage(NavSvsCtrlPageType type);

private:
    CHmsNavSvsModuleMgr();
    ~CHmsNavSvsModuleMgr();

private:
    bool m_bNeedInit;

    std::map<NavSvsCameraType, CHmsNavSvsCameraBase*> m_mapCameraCtrl;
    CHmsNavSvsCameraBase *m_pCurCameraCtrl;

    std::map<NavSvsPositionMgrType, CHmsNavSvsPositionBase*> m_mapPositionMgr;
    CHmsNavSvsPositionBase *m_pCurPositionMgr;

    std::map<NavSvsCtrlPageType, CHmsNavSvsCtrlPageBase*> m_mapCtrlPage;
    CHmsNavSvsCtrlPageBase *m_pCurCtrlPage;
};

