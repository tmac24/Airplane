#pragma once

#include "HmsNavSvsCamera.h"

class CHmsNavSvsCameraFirst : public CHmsNavSvsCameraBase
{
public:
    CHmsNavSvsCameraFirst();
    ~CHmsNavSvsCameraFirst();
   
    virtual void Update(float delta, const CalAttitudeStu *pAttitudeStu) override;
};

