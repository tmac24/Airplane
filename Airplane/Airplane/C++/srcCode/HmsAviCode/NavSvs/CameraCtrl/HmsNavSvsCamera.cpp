#include "HmsNavSvsCamera.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "Calculate/CalculateZs.h"

CHmsNavSvsCameraBase::CHmsNavSvsCameraBase()
{
    m_viewType = NavSvsCameraType::SVSCAMERATYPE_FIRST;
    m_inTouchEventCtrlTime = 0;
    m_bExecuteOnceInUpdate = false;
}

CHmsNavSvsCameraBase::~CHmsNavSvsCameraBase()
{

}

void CHmsNavSvsCameraBase::Update(float delta, const CalAttitudeStu *pAttitudeStu)
{

}

bool CHmsNavSvsCameraBase::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
    return true;
}

void CHmsNavSvsCameraBase::OnMove(const HmsAviPf::Vec2 & posOrigin)
{

}

void CHmsNavSvsCameraBase::OnReleased(const HmsAviPf::Vec2 & posOrigin)
{

}

void CHmsNavSvsCameraBase::CaculatePlaneAttitude(const CalAttitudeStu *pAttitudeStu, Mat4d *pDstMat4d)
{
    Vec3d xaxis;
    Vec3d yaxis;
    Vec3d zaxis;
    Vec3d originPosd;
    CHmsNavSvsMathHelper::CaculateAttitude(pAttitudeStu, pDstMat4d, &xaxis, &yaxis, &zaxis, &originPosd);
    //CHmsMat4dInverse(pDstMat4d);
    m_planeXYZ = originPosd;
    m_planeXaxis = xaxis;
    m_planeYaxis = yaxis;
    m_planeZaxis = zaxis;
}

void CHmsNavSvsCameraBase::CaculateCameraAttitude(const CalAttitudeStu *pAttitudeStu, Mat4d *pDstMat4d)
{
    Vec3d xaxis;
    Vec3d yaxis;
    Vec3d zaxis;
    Vec3d originPosd;
    CHmsNavSvsMathHelper::CaculateAttitude(pAttitudeStu, pDstMat4d, &xaxis, &yaxis, &zaxis, &originPosd);
    pDstMat4d->inverse();
    //
    m_eyePosd = originPosd;
    m_eyeUpd = yaxis;
    m_eyeTargetd = m_eyePosd - zaxis;
}

void CHmsNavSvsCameraBase::SetDistance(float dis)
{

}

void CHmsNavSvsCameraBase::MoveNear(float dis)
{

}

void CHmsNavSvsCameraBase::MoveFar(float dis)
{

}

void CHmsNavSvsCameraBase::SetExecuteOnceInUpdate(bool b)
{
    m_bExecuteOnceInUpdate = b;
}
