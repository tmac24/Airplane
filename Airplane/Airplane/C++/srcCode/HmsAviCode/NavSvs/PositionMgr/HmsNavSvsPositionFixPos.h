#pragma once

#include "HmsNavSvsPosition.h"

class CHmsNavSvsPositionFixPos : public CHmsNavSvsPositionBase
{
public:
    static CHmsNavSvsPositionFixPos* GetInstance();

    void SetLonLatHeight(double lon, double lat, double height);
    void SetYawPitchRoll(double yaw, double pitch, double roll);
    void SetSpeed(double speed);
private:
    CHmsNavSvsPositionFixPos();
    ~CHmsNavSvsPositionFixPos();
};