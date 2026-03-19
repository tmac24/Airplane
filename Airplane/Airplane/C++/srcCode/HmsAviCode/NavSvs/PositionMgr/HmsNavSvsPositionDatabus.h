#pragma once

#include "HmsNavSvsPosition.h"

class CHmsNavSvsPositionDatabus : public CHmsNavSvsPositionBase
{
public:
    static CHmsNavSvsPositionDatabus* GetInstance();
    virtual void Update(float delta) override;
private:
    CHmsNavSvsPositionDatabus();
    ~CHmsNavSvsPositionDatabus();
};