#pragma once

#include "NavSvs/HmsNavSvsCommDef.h"
#include "math/HmsMath.h"

using namespace HmsAviPf;

class CHmsNavSvsCameraBase
{
public:
    CHmsNavSvsCameraBase();
    virtual ~CHmsNavSvsCameraBase();

    virtual void Update(float delta, const CalAttitudeStu *pAttitudeStu);
    virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin);
    virtual void OnMove(const HmsAviPf::Vec2 & posOrigin);
    virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin);

    virtual void CaculatePlaneAttitude(const CalAttitudeStu *pAttitudeStu, Mat4d *pDstMat4d);
    virtual void CaculateCameraAttitude(const CalAttitudeStu *pAttitudeStu, Mat4d *pDstMat4d);
    virtual void SetDistance(float dis);
    virtual void MoveNear(float dis);
    virtual void MoveFar(float dis);
    virtual void SetExecuteOnceInUpdate(bool b);
    virtual NavSvsCameraType GetCameraType(){ return m_viewType; }
    virtual float GetInTouchEventCtrlTime(){ return m_inTouchEventCtrlTime; }

protected:
    NavSvsCameraType m_viewType;
    Mat4d m_mat4dV;

    Mat4d m_mat4dPlaneModel;
    Vec3d m_planeXYZ;
    Vec3d m_planeXaxis;
    Vec3d m_planeYaxis;
    Vec3d m_planeZaxis;

    Vec3d m_eyePosd;
    Vec3d m_eyeTargetd;
    Vec3d m_eyeUpd;

    Vec3d m_eyePosdTouchCtrl;
    Vec3d m_eyeTargetdTouchCtrl;
    Vec3d m_eyeUpdTouchCtrl;

    Vec2 m_onPressPos;
    Vec2 m_onMovePos;
    double m_inTouchEventCtrlTime;
    bool m_bExecuteOnceInUpdate;

    friend class CHmsNavSvsMgr;
};