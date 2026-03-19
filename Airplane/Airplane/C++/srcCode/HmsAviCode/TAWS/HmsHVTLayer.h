#pragma once
#include "base/HmsNode.h"
#include "HmsAviMacros.h"
#include "render/HmsCustomCommand.h"
#include "display/HmsDrawNode.h"
#include "base/RefPtr.h"
#include "NavSvs/mathd/Vec2d.h"
#include "HmsHVTDrawNode.h"


USING_NS_HMS;

class HmsHVTLayer : public CHmsNode
{
public:
	HmsHVTLayer();
	~HmsHVTLayer();

	virtual bool Init() override;

	void CreateTestLines();

	void CreateTestLines2(const double& pixelsPerLon, const double& pixelsPerLat);
	CREATE_FUNC(HmsHVTLayer);

	virtual void Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags) override;
	void onDraw(const Mat4 &transform, uint32_t flags);

	void UpdateData(const Vec2d& originalLonLat, const double& pixelsPerLon, const double& pixelsPerLat);

	void ConvertLonLat2Pixels(const std::vector<std::vector<Vec2d>>& lines, std::vector<std::vector<Vec2>*>& lines_output,
		const double& pixelsPerLon, const double& pixelsPerLat);
	void UpdateDisplay(const float& fHeading, const Vec2d& planeLonLat, const double& pixelsPerLon, const double& pixelsPerLat);

	void clearLines();
	HmsAviPf::Vec2d LayerOriginalLonLat() const { return m_LayerOriginalLonLat; }
	void setLayerOriginalLonLat(HmsAviPf::Vec2d val) { m_LayerOriginalLonLat = val; }	
	double LayerOriginalPixelsPerLon() const { return m_LayerOriginalPixelsPerLon; }
	void setLayerOriginalPixelsPerLon(double val) { m_LayerOriginalPixelsPerLon = val; }
	double LayerOriginalPixelsPerLat() const { return m_LayerOriginalPixelsPerLat; }
	void setLayerOriginalPixelsPerLat(double val) { m_LayerOriginalPixelsPerLat = val; }
private:
	HmsAviPf::CustomCommand			m_customCommand;
	//RefPtr<CHmsDrawNode>			m_rootdrawnode;
	RefPtr<HmsHVTDrawNode>			m_rootdrawnode;
	std::vector<std::vector<Vec2>*> m_lines;//in pixels
	Vec2d							m_LayerOriginalLonLat;//the Longitude & Latitude of (0,0)pixel 
	double							m_LayerOriginalPixelsPerLon;
	double							m_LayerOriginalPixelsPerLat;
};

