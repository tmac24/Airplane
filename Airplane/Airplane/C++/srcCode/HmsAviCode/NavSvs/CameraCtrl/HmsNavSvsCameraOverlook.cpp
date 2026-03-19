#include "HmsNavSvsCameraOverlook.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "Calculate/CalculateZs.h"

CHmsNavSvsCameraOverlook::CHmsNavSvsCameraOverlook()
{
    m_viewType = NavSvsCameraType::SVSCAMERATYPE_OVERLOOK;
    m_nLookDownDistance = 2000;
}

CHmsNavSvsCameraOverlook::~CHmsNavSvsCameraOverlook()
{

}

void CHmsNavSvsCameraOverlook::Update(float delta, const CalAttitudeStu *pAttitudeStu)
{
    CaculatePlaneAttitude(pAttitudeStu, &m_mat4dPlaneModel);

    CalAttitudeStu stu = *pAttitudeStu;
    stu.Height = stu.Height + m_nLookDownDistance;
    stu.pitch = -89;
    stu.roll = 0;
    CaculateCameraAttitude(&stu, &m_mat4dV);
}

void CHmsNavSvsCameraOverlook::MoveNear(float dis)
{
    m_nLookDownDistance -= dis;
    m_nLookDownDistance = SVS_BETWEEN_VALUE(m_nLookDownDistance, 500, 5000);
}

void CHmsNavSvsCameraOverlook::MoveFar(float dis)
{
    m_nLookDownDistance += dis;
    m_nLookDownDistance = SVS_BETWEEN_VALUE(m_nLookDownDistance, 500, 5000);
}
