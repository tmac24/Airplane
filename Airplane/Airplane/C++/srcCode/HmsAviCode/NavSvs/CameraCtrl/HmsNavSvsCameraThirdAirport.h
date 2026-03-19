#pragma once

#include "HmsNavSvsCamera.h"

class CHmsNavSvsCameraThirdAirport : public CHmsNavSvsCameraBase
{
public:
    CHmsNavSvsCameraThirdAirport();
    ~CHmsNavSvsCameraThirdAirport();
    
    virtual void Update(float delta, const CalAttitudeStu *pAttitudeStu) override;
    virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void SetDistance(float dis) override;
    virtual void MoveNear(float dis) override;
    virtual void MoveFar(float dis) override;
    void SetEnableVAngleLimit(bool b);

protected:
    void RotateHorizontalAngle(float angle);
    void RotateVerticalAngle(float angle);

protected:
    Vec3d m_planeToCamera;
    Vec3d m_axisX;
    Vec3d m_axisZ;

    float m_cameraDis;
    CalAttitudeStu m_attitudeStu;

    bool m_bEnableVAngleLimit;
};

