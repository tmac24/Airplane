#include "CameraCtrl.h"
#include "../Calculate/CalculateZs.h"
#include "math/HmsMath.h"
#include <cmath>
#include "FPPManipulator.h"	



USING_NS_HMS;

class CameraMode
{
public:
	virtual ~CameraMode(){};
    virtual void RotateWithTouch(CameraController* pcam, const Vec2& deltaTouchMovement) = 0;
    virtual void ResetRotation(CameraController* pcam) = 0;

    virtual void Update(CameraController* pcam, float delta) = 0;
    virtual void onPress(CameraController* pcam, const Vec2& absPos) = 0;
    virtual void onRelease(CameraController* pcam, const Vec2& absPos) = 0;
    virtual void onMove(CameraController* pcam, const Vec2& relPos) = 0;
};

//First Person Mode
class CameraMode1stPM : public CameraMode
{
public:
    CameraMode1stPM(CHmsCameraIndicator* camind);

    virtual void ResetRotation(CameraController* pcam) override;
    virtual void RotateWithTouch(CameraController* pcam, const Vec2& touchMovement) override;

    void RotateWithYawPitch(CameraController* pcam, const double& yawDelta, const double& pitchDelta);
    virtual void onPress(CameraController* pcam, const Vec2& absPos) override;
    virtual void onRelease(CameraController* pcam, const Vec2& absPos) override;
    virtual void onMove(CameraController* pcam, const Vec2& relPos) override;

    virtual void Update(CameraController* pcam, float delta) override;

private:
    CHmsMicroSecondTime _tm;//for reseting the camera to default pos & att
    bool _bOnPressed;
    double _starttm;
    CHmsCameraIndicator* _pCamInd;
};

//Third Person Mode
class CameraMode3rdPM :public CameraMode
{
public:
    CameraMode3rdPM();

    virtual void ResetRotation(CameraController* pcam) override;
    virtual void RotateWithTouch(CameraController* pcam, const Vec2& deltaTouchMovement) override;

    virtual void onPress(CameraController* pcam, const Vec2& absPos) override;
    virtual void onRelease(CameraController* pcam, const Vec2& absPos) override;
    virtual void onMove(CameraController* pcam, const Vec2& relPos) override;

    virtual void Update(CameraController* pcam, float delta) override;

private:
};

CameraMode1stPM::CameraMode1stPM(CHmsCameraIndicator* camind)
: _pCamInd(camind)
{
    g_camMode = 0;
    g_camyaw = 0;
    g_campitch = 0;
    _bOnPressed = false;
	_starttm = 0.0;
}

void CameraMode1stPM::ResetRotation(CameraController* pcam)
{
	if (0 == pcam){ return; }
    int frameCnt = 30;//fram count of animation.
    double duration = 0.5; //animation duration time (second)
    double frameTime = duration / frameCnt;

    if (pcam->_absAngleOffset.x> 0)
    {
        pcam->_absAngleOffset.x = std::fmod(pcam->_absAngleOffset.x + SIM_Pi, SIM_Pi_x2) - SIM_Pi;
    }
    else
    {
        pcam->_absAngleOffset.x = std::fmod(pcam->_absAngleOffset.x - SIM_Pi, SIM_Pi_x2) + SIM_Pi;
    }

    Vec2 returnStroke = pcam->_absAngleOffset;

    Vec2 a = 2 * returnStroke / (duration * duration);//acceleraton

    std::vector<CHmsAction*> rotateToCenters;

    for (int i = frameCnt - 1; i >= 0; --i)
    {
        auto onePosMark = 0.5 * a * (frameTime * i) * (frameTime * i);
        auto oneStepCallback = HmsAviPf::CHmsActionDelayCallback::Create(frameTime, [=](){
            pcam->_absAngleOffset = onePosMark;
            g_camyaw = onePosMark.x;
            g_campitch = onePosMark.y;
        });
        rotateToCenters.push_back(oneStepCallback);
    }

    auto done = HmsAviPf::CHmsActionCallback::Create([=](){
        std::cout << pcam->_absAngleOffset << std::endl;
        auto dlyhd = HmsAviPf::CHmsActionDelayHide::Create(0.5);
        _pCamInd->RunAction(dlyhd);
    });
    auto rotateQueue = HmsAviPf::CHmsActionQueue::Create(rotateToCenters);

    auto resetAnimation = HmsAviPf::CHmsActionQueue::Create(rotateQueue, done, nullptr);

    pcam->StopAllActions();
    pcam->RunAction(resetAnimation);
}

void CameraMode1stPM::RotateWithTouch(CameraController* pcam, const Vec2& deltaTouchMovement)
{
	if (pcam && _pCamInd)
	{
		pcam->StopAllActions();
		_pCamInd->StopAllActions();
		RotateWithYawPitch(pcam,
			deltaTouchMovement.x*pcam->_touchSensitivity.x,
			deltaTouchMovement.y*pcam->_touchSensitivity.y);
	}
}

void CameraMode1stPM::RotateWithYawPitch(CameraController* pcam, const double& yawDelta, const double& pitchDelta)
{
	if (pcam)
	{
		Vec2 deltaTM = Vec2(yawDelta, pitchDelta);

    pcam->_absAngleOffset += deltaTM;

    //y 60 range limit
    pcam->_absAngleOffset.y = HMS_MAX(-SIM_Pi / 3, pcam->_absAngleOffset.y);
    pcam->_absAngleOffset.y = HMS_MIN(+SIM_Pi / 3, pcam->_absAngleOffset.y);

    //x mod
    pcam->_absAngleOffset.x = std::fmod(pcam->_absAngleOffset.x, SIM_Pi_x2);

		g_camyaw = pcam->_absAngleOffset.x;
		g_campitch = pcam->_absAngleOffset.y;
	}
}

void CameraMode1stPM::onPress(CameraController* pcam, const Vec2& absPos)
{
    _starttm = _tm.GetTimeCurrent();//update the start time
    _bOnPressed = true;
	if (_pCamInd)
    {
		_pCamInd->SetVisible(true);
    }
}

void CameraMode1stPM::onRelease(CameraController* pcam, const Vec2& absPos)
{
    _bOnPressed = false;
}

void CameraMode1stPM::onMove(CameraController* pcam, const Vec2& relPos)
{
	if (pcam)
	{
		pcam->RotateWithTouchMovement(relPos);
	}  
}

void CameraMode1stPM::Update(CameraController* pcam, float delta)
{
	if (pcam && _pCamInd)
	{
		static auto s_lasttime = pcam->_timeRemaining;
		pcam->_timeRemaining = pcam->_timeTotal - (_tm.GetTimeCurrent() - _starttm);

    if (pcam->_timeRemaining <= 0 && s_lasttime > 0)
    {
        pcam->Reset();
    }

    s_lasttime = pcam->_timeRemaining;

    if (_bOnPressed)
    {
        _starttm = _tm.GetTimeCurrent();//update the start time
    }

		_pCamInd->SetYaw(pcam->_absAngleOffset.x);
		_pCamInd->SetPitch(pcam->_absAngleOffset.y);
		_pCamInd->SetTimeTotal(pcam->_timeTotal);
		_pCamInd->SetTimeRemaining(pcam->_timeRemaining);
	} 
}

CameraMode3rdPM::CameraMode3rdPM()
{
    g_camMode = 1;
}

void CameraMode3rdPM::ResetRotation(CameraController* pcam)
{
}

void CameraMode3rdPM::RotateWithTouch(CameraController* pcam, const Vec2& deltaTouchMovement)
{
	if (pcam)
	{
		Vec2 deltaTM = Vec2(
			deltaTouchMovement.x*pcam->_touchSensitivity.x,
			deltaTouchMovement.y*pcam->_touchSensitivity.y);

    pcam->_absAngleOffset += deltaTM;

    pcam->_absAngleOffset.y = HMS_MAX(-SIM_Pi / 3, pcam->_absAngleOffset.y);
    pcam->_absAngleOffset.y = HMS_MIN(+SIM_Pi / 3, pcam->_absAngleOffset.y);

    //x mod
    pcam->_absAngleOffset.x = std::fmod(pcam->_absAngleOffset.x, SIM_Pi_x2);

		g_camyaw = pcam->_absAngleOffset.x;
		g_campitch = pcam->_absAngleOffset.y;
	} 
}

void CameraMode3rdPM::onPress(CameraController* pcam, const Vec2& absPos)
{
}

void CameraMode3rdPM::onRelease(CameraController* pcam, const Vec2& absPos)
{
}

void CameraMode3rdPM::onMove(CameraController* pcam, const Vec2& relPos)
{
	if (pcam)
	{
		pcam->RotateWithTouchMovement(relPos);
	}
}

void CameraMode3rdPM::Update(CameraController* pcam, float delta)
{
	if (pcam)
	{
		pcam->_timeRemaining = pcam->_timeTotal;
	}
}

void CameraController::ChangeMode(CameraMode* pcm)
{
    if (_pCameraMode)
    {
        delete _pCameraMode;
    }
    _pCameraMode = pcm;
}

CameraController::CameraController(CHmsCameraIndicator* camInd)
: _pCameraMode(nullptr)
, _enMode(EN_CM_MODE::CM_MODE_NA)
, _touchSensitivity((float)CAM_ROTE_TOUCH_SENSITIVITY, (float)CAM_ROTE_TOUCH_SENSITIVITY)
, _absAngleOffset(Vec2(0, 0))
, _timeTotal(5.0f)
, _timeRemaining(0.0f)
, _pCameraIndicator(camInd)
{
    //SetCurrentMode(EN_CM_MODE::CM_MODE_1ST);
    SetCurrentMode(EN_CM_MODE::CM_MODE_3RD);
}

void CameraController::SetCurrentMode(EN_CM_MODE enCm)
{
    if (_enMode != enCm)
    {
        _enMode = enCm;
        StopAllActions();

        if (_enMode == CM_MODE_1ST)
        {
            _absAngleOffset = Vec2(0.0F, 0.0F);
            ChangeMode(new CameraMode1stPM(_pCameraIndicator));
        }
        else if (_enMode == CM_MODE_3RD)
        {
            ChangeMode(new CameraMode3rdPM);
            _pCameraIndicator->SetVisible(false);
        }
        else
        {
            std::cout << "camera ctrl errror." << std::endl;
        }
    }
}

EN_CM_MODE CameraController::GetCurrentMode(void) const
{
    return _enMode;
}

void CameraController::RotateWithTouchMovement(const Vec2& dtTchMov)
{
	if (_pCameraMode)
	{
		_pCameraMode->RotateWithTouch(this, dtTchMov);
	}
}

void CameraController::Reset()
{
	if (_pCameraMode)
	{
		_pCameraMode->ResetRotation(this);
	}
}

void CameraController::Set1stPM(void)
{
    SetCurrentMode(EN_CM_MODE::CM_MODE_1ST);
}

void CameraController::Set3rdPM(void)
{
    SetCurrentMode(EN_CM_MODE::CM_MODE_3RD);    
}

void CameraController::Update(float delta)
{
	if (_pCameraMode)
	{
		_pCameraMode->Update(this, delta);
	}
}

void CameraController::onPress(const Vec2& absPos)
{
	if (_pCameraMode)
	{
		_pCameraMode->onPress(this, absPos);
	}
}

void CameraController::onRelease(const Vec2& absPos)
{
	if (_pCameraMode)
	{
		_pCameraMode->onRelease(this, absPos);
	}
}

void CameraController::onMove(const Vec2& relPos)
{
	if (_pCameraMode)
	{
		_pCameraMode->onMove(this, relPos);
	}
}

Vec2 CameraController::AbsAngleOffset() const
{
    return _absAngleOffset;
}

double CameraController::TimeTotal() const
{
    return _timeTotal;
}

double CameraController::TimeRemaining() const
{
    return _timeRemaining;
}

