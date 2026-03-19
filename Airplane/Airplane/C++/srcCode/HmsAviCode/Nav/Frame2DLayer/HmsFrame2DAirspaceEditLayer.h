#pragma once

#include "HmsFrame2DLayerBase.h"
#include "../HmsRouteLineDrawNode.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsFrustum.h"

class CHmsFrame2DAirspaceEditLayer : public CHmsFrame2DLayerBase
{
public:
	CHmsFrame2DAirspaceEditLayer();
	~CHmsFrame2DAirspaceEditLayer();

	static CHmsFrame2DAirspaceEditLayer* Create(Size layerSize);
	virtual bool Init(Size layerSize);

	virtual void Update(float delta) override;
	virtual void Update2DElements() override;
	virtual void SetEarthLayer(int n) override;

	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
	virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;
	virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;

	void ShowEditLayer(const std::vector<AirspaceEditStu> &vecStu);
	void HideEditLayer();
	
private:
	bool TestPressOnOldNode(const HmsAviPf::Vec2 & posOrigin);
	void ProcessInsertNormal(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust);
	void ReDrawLines();
	CHmsNode * CreateRouteNode(std::string &strText, float textSize);
	void UpdateWptNode();
	void CalInsertLonLat(int beginIndex, int endIndex);

private:
	RefPtr<CHmsRouteLineDrawNode> m_pRouteLineDrawNode;
	std::vector<WptNodeStu2D> m_vecWptNode;

	std::vector<AirspaceEditStu> m_vecStu;
	WptNodeStu2D *m_pCurDragNodeStu2D;
	Vec2 m_posPress;
	Vec2 m_posImageWhenPress;
	int m_curPressIndex;
};



