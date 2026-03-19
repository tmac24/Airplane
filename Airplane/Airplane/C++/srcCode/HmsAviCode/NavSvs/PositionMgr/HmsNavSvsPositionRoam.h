#pragma once

#include "HmsNavSvsPosition.h"

class CHmsNavSvsPositionRoam : public CHmsNavSvsPositionBase
{
public:
    static CHmsNavSvsPositionRoam* GetInstance();
    virtual void Update(float delta) override;

    void SetLonLat(double lon, double lat);
    void SetHeight(double height);
    void SetSpeed(double speed);

    void SetPitchSpeed(float v);
    void SetRollSpeed(float v);
    void SetRightSpeed(float v);

    void SetPitch(float v);
    void SetYaw(float v);

    void Play();
    void Pause();

private:
    CHmsNavSvsPositionRoam();
    ~CHmsNavSvsPositionRoam();

    float m_pitchSpeed;
    float m_rollSpeed;
    float m_rightSpeed;
    bool m_bRun;
};