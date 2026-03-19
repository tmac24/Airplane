#include "HmsNavSvsCameraThirdAirport.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "Calculate/CalculateZs.h"

CHmsNavSvsCameraThirdAirport::CHmsNavSvsCameraThirdAirport()
{
    m_viewType = NavSvsCameraType::SVSCAMERATYPE_THIRD_AIRPORT;
    m_cameraDis = 100;
    m_bEnableVAngleLimit = true;
}

CHmsNavSvsCameraThirdAirport::~CHmsNavSvsCameraThirdAirport()
{

}

void CHmsNavSvsCameraThirdAirport::Update(float delta, const CalAttitudeStu *pAttitudeStu)
{
    CaculatePlaneAttitude(pAttitudeStu, &m_mat4dPlaneModel);
    Vec3d eyePos, eyeUp, eyeTarget;
    eyePos = m_planeZaxis;
    eyePos = eyePos * CHmsNavSvsMathHelper::MeterToWorldUnit(m_cameraDis);
    eyePos = m_planeXYZ + eyePos;

    eyeUp = m_planeYaxis;
    eyeTarget = m_planeXYZ;

    m_eyePosd = eyePos;
    m_eyeUpd = eyeUp;
    m_eyeTargetd = eyeTarget;

    m_eyePosdTouchCtrl = m_eyePosd;
    m_eyeTargetdTouchCtrl = m_eyeTargetd;
    m_eyeUpdTouchCtrl = m_eyeUpd;

    if (m_bExecuteOnceInUpdate)
    {
        m_bExecuteOnceInUpdate = false;

        m_planeToCamera = m_eyePosdTouchCtrl - m_planeXYZ;
        m_axisX = m_planeXaxis;
        m_axisZ = m_planeZaxis;

        RotateVerticalAngle(-15);
    }

    //touch
    RotateHorizontalAngle((m_onMovePos.x - m_onPressPos.x) / 2560.0 * 360);
    RotateVerticalAngle((m_onMovePos.y - m_onPressPos.y) / 1440.0 * 180);
    Mat4d::createLookAt(m_eyePosdTouchCtrl, m_eyeTargetdTouchCtrl, m_eyeUpdTouchCtrl, &m_mat4dV);

    m_onPressPos = m_onMovePos;
}

bool CHmsNavSvsCameraThirdAirport::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
    m_onPressPos = posOrigin;
    m_onMovePos = posOrigin;

    return true;
}

void CHmsNavSvsCameraThirdAirport::OnMove(const HmsAviPf::Vec2 & posOrigin)
{
    auto tmp = posOrigin - m_onPressPos;
    if (tmp.length() > 5)
    {
        m_onMovePos = posOrigin;
    }
}

void CHmsNavSvsCameraThirdAirport::OnReleased(const HmsAviPf::Vec2 & posOrigin)
{
    m_onPressPos = m_onMovePos;
}

void CHmsNavSvsCameraThirdAirport::SetDistance(float dis)
{
    m_cameraDis = dis;
}

void CHmsNavSvsCameraThirdAirport::MoveNear(float dis)
{
    auto len = m_cameraDis - dis;
    len = SVS_BETWEEN_VALUE(len, 100, 5000);

    m_planeToCamera *= (len / m_cameraDis);

    m_cameraDis = len;
}

void CHmsNavSvsCameraThirdAirport::MoveFar(float dis)
{
    auto len = m_cameraDis + dis;
    len = SVS_BETWEEN_VALUE(len, 100, 5000);

    m_planeToCamera *= (len / m_cameraDis);

    m_cameraDis = len;
}

void CHmsNavSvsCameraThirdAirport::SetEnableVAngleLimit(bool b)
{
    m_bEnableVAngleLimit = b;
}

void CHmsNavSvsCameraThirdAirport::RotateHorizontalAngle(float angle)
{
    if (angle != 0)
    {
        auto phi = angle / 180.0 * SIM_Pi;
        Quaterniond q(m_planeYaxis, phi);

        m_axisX = q * m_axisX;
        m_axisZ = q * m_axisZ;

        m_planeToCamera = q * m_planeToCamera;
    }
    m_eyePosdTouchCtrl = m_planeXYZ + m_planeToCamera;
}

void CHmsNavSvsCameraThirdAirport::RotateVerticalAngle(float angle)
{
    if (angle != 0)
    {
        auto zAngle = MATH_RAD_TO_DEG(Vec3d::angle(m_planeToCamera, m_axisZ));
        if (fabs(zAngle) > 90)
        {
            angle = -angle;
        }
        if (m_bEnableVAngleLimit)
        {
            if (zAngle - angle < 3)
            {
                angle = zAngle - 3;
            }
            if (zAngle - angle > 177)
            {
                angle = zAngle - 177;
            }
        }
        
        auto phi = angle / 180.0 * SIM_Pi;
        Quaterniond q(m_axisX, phi);

        m_planeToCamera = q * m_planeToCamera;
    }
    m_eyePosdTouchCtrl = m_planeXYZ + m_planeToCamera;
}
