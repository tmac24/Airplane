#include "HmsNavSvsCameraFirst.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "Calculate/CalculateZs.h"

CHmsNavSvsCameraFirst::CHmsNavSvsCameraFirst()
{
    m_viewType = NavSvsCameraType::SVSCAMERATYPE_FIRST;
}

CHmsNavSvsCameraFirst::~CHmsNavSvsCameraFirst()
{

}

void CHmsNavSvsCameraFirst::Update(float delta, const CalAttitudeStu *pAttitudeStu)
{
    CaculateCameraAttitude(pAttitudeStu, &m_mat4dV);
}