#pragma once

#include "HmsAviMacros.h"
#include "base/HmsNode.h"
#include "ui/HmsUiEventInterface.h"
#include "CameraIndicator.h"
#include "CameraCtrl.h"
#include "../HmsResizeableLayer.h"

using namespace HmsAviPf;
class HmsSVSFrame;

class CHmsSvs : public CHmsResizeableLayer, public HmsUiEventInterface
{
public:
    CHmsSvs();
    ~CHmsSvs();

    CREATE_FUNC_BY_SIZE(CHmsSvs);

    virtual void OnEnter() override;
    virtual void OnExit() override;
    virtual void Update(float delta) override ;

    virtual void ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos) override;

    void Set1stPM(void);
    void Set3rdPM(void);
    void SetAircraftModel(const std::string &name);
    void SetWarnningDistance(float fDistanceMeter);
	void SetFlyPathDisplay(bool bShow);

protected:
    virtual bool Init(const HmsAviPf::Size & size);
    virtual bool OnPressed(const Vec2 & posOrigin) override;
    virtual void OnReleased(const Vec2 & posOrigin) override;
    virtual void OnMove(const Vec2 & posOrigin) override;

    void InitFlowBtns();

private:
    HmsAviPf::Vec2                          m_posLast;
    bool									m_bChildTouch;

    HmsAviPf::Size                          m_sizeLayout;
    CameraController*                       m_pCameraControler;
    HmsSVSFrame *							m_pSvsFrame;
    HmsAviPf::RefPtr<CHmsCameraIndicator>   m_nodeCameraInd;
};
