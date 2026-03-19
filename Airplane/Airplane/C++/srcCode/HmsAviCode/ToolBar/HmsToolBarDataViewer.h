#pragma once

#include "HmsAviPf.h"
#include "display/HmsStretchImageNode.h"

class CHmsToolBarDataViewer : public HmsAviPf::CHmsNode
{
public:

	static CHmsToolBarDataViewer * create();

	CHmsToolBarDataViewer();

	virtual bool Init() override;

	virtual void Update(float delta) override;

	std::string GetLonLatString(double dLon, double dLat);
	std::string GetLonString(double dLon);
	std::string GetLatString(double dLat);

	std::string GetDeviceData();
	std::string GetCorrectedData();

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>				m_labelDeviceTitle;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>				m_labelDeviceData;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>				m_labelCorrectTitle;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>				m_labelCorrectData;

	HmsAviPf::RefPtr<HmsAviPf::CHmsStretchImageNode>	m_bg;
	float												m_fMargin;
};
