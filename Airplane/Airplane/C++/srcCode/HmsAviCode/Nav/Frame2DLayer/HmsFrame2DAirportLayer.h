#pragma once

#include "HmsFrame2DLayerBase.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsFrustum.h"

class CHmsFrame2DAirportLayer : public CHmsFrame2DLayerBase
{
public:
    CHmsFrame2DAirportLayer();
    ~CHmsFrame2DAirportLayer();

    static CHmsFrame2DAirportLayer* Create(Size layerSize);
    virtual bool Init(Size layerSize);

    virtual void Update(float delta) override;
    virtual void Update2DElements() override;
    virtual void SetEarthLayer(int n) override;

private:
    std::vector<WptNodeStu2D> m_vecWptNode;
    Vec2 m_lastLonLat;
    float m_updateTime;
};



