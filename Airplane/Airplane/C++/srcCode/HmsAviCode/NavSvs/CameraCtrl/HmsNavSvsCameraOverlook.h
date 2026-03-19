#pragma once

#include "HmsNavSvsCamera.h"

class CHmsNavSvsCameraOverlook : public CHmsNavSvsCameraBase
{
public:
    CHmsNavSvsCameraOverlook();
    ~CHmsNavSvsCameraOverlook();
    
    virtual void Update(float delta, const CalAttitudeStu *pAttitudeStu) override;
    virtual void MoveNear(float dis) override;
    virtual void MoveFar(float dis) override;

protected:
    int m_nLookDownDistance;
};


