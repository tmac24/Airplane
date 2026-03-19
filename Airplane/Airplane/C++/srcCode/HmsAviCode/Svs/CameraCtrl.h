#pragma once
#include <iostream>
#include <string>
#include "math/Vec2.h"
#include "math/Vec3.h"
#include "base/HmsTimer.h"
#include "base/HmsNode.h"
#include "base/HmsActionImmediatelyRef.h"
#include "base/HmsActionTimeRef.h"
#include "CameraIndicator.h"


typedef enum{
    CM_MODE_1ST,
    CM_MODE_3RD,
    CM_MODE_NA,
} EN_CM_MODE;

//Pixel to Rad
#define CAM_ROTE_TOUCH_SENSITIVITY ((3.14159/(768.0/2.0))*0.15)

class CameraMode;

class CameraController : public HmsAviPf::CHmsNode
{
public:
    CameraController(CHmsCameraIndicator* camInd);

    EN_CM_MODE GetCurrentMode(void) const;

    void Set1stPM(void);
    void Set3rdPM(void);

    virtual void Update(float delta) override;
    void onPress(const HmsAviPf::Vec2& absPos);
    void onRelease(const HmsAviPf::Vec2& absPos);
    void onMove(const HmsAviPf::Vec2& relPos);

    HmsAviPf::Vec2 AbsAngleOffset() const;
    double TimeTotal() const;
    double TimeRemaining() const;
private:
    void SetCurrentMode(EN_CM_MODE cmm);
    void ChangeMode(CameraMode* pcm);
    void RotateWithTouchMovement(const HmsAviPf::Vec2& tchmov);
    void Reset();
    CameraMode* _pCameraMode;
    EN_CM_MODE _enMode;

    HmsAviPf::Vec2 _touchSensitivity;
    HmsAviPf::Vec2 _absAngleOffset;//offset angle of (yaw,pitch) ref to (0.0, 0.0) radian
    double _timeTotal;
    double _timeRemaining;//when time out, reset the camera
    CHmsCameraIndicator* _pCameraIndicator;
    //if friend set to CameraMode, the children class is still not friend with this class. so...
    friend class CameraMode1stPM;
    friend class CameraMode3rdPM;
};
