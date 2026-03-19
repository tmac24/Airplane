#pragma once

#include "HmsFrame2DLayerBase.h"
#include "../HmsRouteLineDrawNode.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsFrustum.h"

class CHmsFrame2DAirspaceLayer : public CHmsFrame2DLayerBase
{
public:
	CHmsFrame2DAirspaceLayer();
	~CHmsFrame2DAirspaceLayer();

	static CHmsFrame2DAirspaceLayer* Create(Size layerSize);
	virtual bool Init(Size layerSize);

	virtual void Update(float delta) override;
	virtual void Update2DElements() override;
	virtual void SetEarthLayer(int n) override;

	void ShowAirspaceLayer();
	void HideAirspaceLayer();

private:
	void ProcessInsertNormal(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust);
	void ReDrawLines();
	void CalInsertLonLat(int beginIndex, int endIndex);
	void UpdateAirspaceData(const std::vector<AirspaceEditStu> &vecStu);

private:
	RefPtr<CHmsRouteLineDrawNode> m_pRouteLineDrawNode;
	std::vector<WptNodeStu2D> m_vecWptNode;
	std::vector<HmsAviPf::Vec2> m_vecSyncAirspace;
};



