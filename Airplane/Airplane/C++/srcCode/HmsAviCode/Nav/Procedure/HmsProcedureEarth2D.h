#pragma once

#include "../HmsFrame2DInterface.h"

#include "../HmsWptInfoDlg.h"
#include "../HmsRouteLineDrawNode.h"
#include "display/HmsStretchImageNode.h"
#include "HmsStandardDef.h"

struct PreviewRouteStu
{
	std::string programName;
	ROUTE_TYPE proType;
	//key是trans的名称
	std::map<std::string, std::vector<WptNodeStu2D> > mapRoute;
};

class CHmsProcedureEarth2D : public CHmsFrame2DInterface
{
public:
	CHmsProcedureEarth2D();
	~CHmsProcedureEarth2D();

	virtual void Update(float delta) override;
	virtual void OnClickOnEarth(const HmsAviPf::Vec2 & pos) override;
	virtual void Update2DElements() override;
	virtual void SetEarthLayer(int n) override;
	virtual CHmsMapMeasure* GetMapMeasure() override;

	static CHmsProcedureEarth2D* Create(HmsAviPf::Camera *pCamera, Size layerSize);
	virtual bool Init(HmsAviPf::Camera *pCamera, Size layerSize) override;

	void SetRouteData(std::vector<PreviewRouteStu> &vec);

private:

	void CalInsertLonLat();
	void CalInsertData2DPos();

	void ClearAllRouteNode();
	void ReDrawLines();

	Color4B GetColor(bool bAutoInc);

private:
	std::vector<PreviewRouteStu> m_vecArray;
	int m_earthLayer;
	RefPtr<CHmsRouteLineDrawNode> m_pRouteLineDrawNode;

	std::string m_cNoTransition = "No Transition";

	std::map<std::string, Color4B> m_mapColor;
	int m_colorIndex;
};

