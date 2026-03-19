#pragma once

#include "HmsFrame2DLayerBase.h"
#include "../HmsRouteLineDrawNode.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsFrustum.h"

class CHmsFrame2DAirplaneLayer : public CHmsFrame2DLayerBase
{
public:
	CHmsFrame2DAirplaneLayer();
	~CHmsFrame2DAirplaneLayer();

	static CHmsFrame2DAirplaneLayer* Create(Size layerSize);
	virtual bool Init(Size layerSize);

	virtual void Update(float delta) override;
	virtual void Update2DElements() override;

private:
	RefPtr<CHmsImageNode> m_pImagePlaneTop;
	Vec3 m_posPlaneTop;
};



