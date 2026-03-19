#pragma once
#include "../HmsGlobal.h"
#include "base/HmsNode.h"

class CHmsCameraIndicator : public HmsAviPf::CHmsNode
{
public:
	CHmsCameraIndicator();
	~CHmsCameraIndicator();

	CREATE_FUNC(CHmsCameraIndicator)

	virtual bool Init() override;
	virtual void Update(float delta) override;
	void SetYaw(float yaw);
	void SetPitch(float pitch);
	void SetTimeRemaining(float tmr);
	void SetTimeTotal(float tmt);
private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>  m_pCamInd;	//摄像机朝向指示
	float _yaw;
	float _pitch;
	float _tmRemaining;
	float _tmTotal;
};

