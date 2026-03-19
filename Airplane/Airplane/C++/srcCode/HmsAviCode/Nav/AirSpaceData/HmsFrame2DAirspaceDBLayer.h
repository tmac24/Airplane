#pragma once

#include "../Frame2DLayer/HmsFrame2DLayerBase.h"
#include "../HmsRouteLineDrawNode.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsFrustum.h"
#include "HmsAirspaceData.h"
#include "HUD/HmsMultLineDrawNode.h"

class CHmsFrame2DAirspaceDBLayer : public CHmsFrame2DLayerBase
{
public:
    CHmsFrame2DAirspaceDBLayer();
    ~CHmsFrame2DAirspaceDBLayer();

    static CHmsFrame2DAirspaceDBLayer* Create(Size layerSize);
    virtual bool Init(Size layerSize);

    virtual void Update(float delta) override;
    virtual void Update2DElements() override;
    virtual void SetEarthLayer(int n) override;

private:
    void ReDrawLines();

private:
    RefPtr<CHmsMultLineDrawNode> m_pRouteLineDrawNode;
    std::vector<AirspaceGroupStu> m_vecAirspaceGroup;
    Vec2 m_lastLonLat;
    float m_updateTime;
};



