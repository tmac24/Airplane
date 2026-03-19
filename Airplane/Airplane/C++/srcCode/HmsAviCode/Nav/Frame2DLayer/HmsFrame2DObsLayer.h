#pragma once

#include "HmsFrame2DLayerBase.h"
#include "../HmsRouteLineDrawNode.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsFrustum.h"

class CHmsFrame2DObsLayer : public CHmsFrame2DLayerBase
{
public:
	CHmsFrame2DObsLayer();
	~CHmsFrame2DObsLayer();

	static CHmsFrame2DObsLayer* Create(Size layerSize);
	virtual bool Init(Size layerSize);

	virtual void Update(float delta) override;
	virtual void Update2DElements() override;

private:
	CHmsNode * CreateObsScaleNode(std::string &strText, float textSize);
	void InsertDot(Vec2 lonLat1, Vec2 lonLat2, NavLineAttrStu &lineAttr);
	void ReInsertObsLine3Dpos(float angle);
	void ReDrawObsLine();
	void UpdateObsLine2DPos();
	void ReInsertPlaneLine3Dpos();
	void ReDrawPlaneLine();
	void UpdatePlaneLine2DPos();
	void ClearObsData();
	void ClearAllLine();

private:
	RefPtr<CHmsRouteLineDrawNode> m_pAircraftWptLineNode;
	RefPtr<CHmsRouteLineDrawNode> m_pObsFullLineNode;
	RefPtr<CHmsRouteLineDrawNode> m_pObsDashLineNode;
	PlaneSegStu m_planeSegStu;
};



