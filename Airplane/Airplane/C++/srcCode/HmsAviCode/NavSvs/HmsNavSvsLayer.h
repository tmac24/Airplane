#pragma once

#include "../HmsGlobal.h"
#include "math/HmsGeometry.h"
#include "base/HmsNode.h"
#include "display/HmsImageNode.h"
#include "display/HmsLabel.h"
#include "display/HmsDrawNode.h"
#include "HmsNavSvsMgr.h"
#include "ui/HmsUiEventInterface.h"

class CHmsNavSvsLayer : public CHmsNode, public HmsUiEventInterface
{
public:
    static CHmsNavSvsLayer* GetInstance();

    virtual void Update(float delta) override;
    virtual void Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags) override;
    virtual void OnEnter() override;
    virtual void OnExit() override;

    void ChangeSize(Size size);

    void SetSvsView(NavSvsCameraType view);
    void SetSvsCtrlPage(NavSvsCtrlPageType type);

    void SetAirportViewMode(double lon, double lat, double height);
    void SetRouteViewMode(const std::vector<FPL2DNodeInfoStu> &vec);
    void SetRoamMode(double lon, double lat);

    void MoveNear(float dis);
    void MoveFar(float dis);

private:
    CHmsNavSvsLayer();
    ~CHmsNavSvsLayer();
    CREATE_FUNC_BY_SIZE(CHmsNavSvsLayer);
    virtual bool Init(const HmsAviPf::Size & size);

private:
    float m_fCameraNear;
    float m_fCameraFar;
    float m_fFieldOfView;
    float m_fFieldOfViewRadian;
    float m_fAspectRatio;
    Camera *m_pCamera;
    CHmsNavSvsMgr *m_pSvsMgr;

    CHmsImageNode *m_pSvsImageNode;
    CHmsDrawNode *m_pBgDrawNode;
    CHmsLabel *m_pLoadLabel;
};

