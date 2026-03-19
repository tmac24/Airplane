#include "CameraIndicator.h"
#include "display/HmsDrawNode.h"
#include "display/HmsImageNode.h"
#include "../Calculate/CalculateZs.h"

USING_NS_HMS;

CHmsCameraIndicator::CHmsCameraIndicator()
: _yaw(0.0)
, _pitch(0.0)
, _tmRemaining(0.0)
, _tmTotal(0.0)
{

}


CHmsCameraIndicator::~CHmsCameraIndicator()
{
}

bool CHmsCameraIndicator::Init()
{
	m_pCamInd = CHmsDrawNode::Create();

	this->AddChild(m_pCamInd);

	this->SetScheduleUpdate();
	return true;
}
#define IND_RADIUS 70
void CHmsCameraIndicator::Update(float delta)
{
	m_pCamInd->clear();
	if (_tmRemaining <= 0)
	{
		//SetVisible(false);
	}

	m_pCamInd->DrawFan(0, 360, IND_RADIUS, 50, Color4F(1, 1, 1, 0.2F));

	m_pCamInd->DrawFan(
		ToAngle(_yaw) - 35,
		ToAngle(_yaw) + 35,
		IND_RADIUS + _pitch * 180 * (270.0F / 360) / SIM_Pi,//coefficient convert angle to "distance"(Radius)
		20,
		Color4F(1, 1, 1, 0.35F));

    if (_tmRemaining>=0)
    {
        m_pCamInd->DrawFanBorder(
            (_tmTotal - _tmRemaining) * 360 / 5,
            360,
            IND_RADIUS + 4,
            4,
            50,
            Color4F(1, 1, 1, 0.6F)
            );
    }	
}

void CHmsCameraIndicator::SetYaw(float yaw)//Radian
{
	_yaw = yaw;
}

void CHmsCameraIndicator::SetPitch(float pitch)
{
	_pitch = pitch;
}

void CHmsCameraIndicator::SetTimeRemaining(float tmr)
{
	_tmRemaining = tmr;
}

void CHmsCameraIndicator::SetTimeTotal(float tmt)
{
	_tmTotal = tmt;
}
