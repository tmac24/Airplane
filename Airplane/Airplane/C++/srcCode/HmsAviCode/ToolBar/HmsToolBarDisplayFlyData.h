#pragma once

#include "HmsAviPf.h"


class CHmsToolBarDisplayFlyData : public HmsAviPf::CHmsImageNode
{
public:

	static CHmsToolBarDisplayFlyData * create(const HmsAviPf::Size & size);

	CHmsToolBarDisplayFlyData();

	virtual bool Init(const HmsAviPf::Size & size);

	HmsAviPf::CHmsLabel * CreateDisplayLabel(const char * strTitle, const char * strContent, HmsAviPf::Vec2 & pos, bool bMutiLine = false);

	virtual void Update(float delta) override;

	std::string GetLonLatString(double dLon, double dLat);

protected:
	float GetItemWidth();

private:

	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelDistanceToNext;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelEteNext;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelEteDest;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelGroundSpeed;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelGpsAltitude;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelMagneticHeading;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelTrack;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelAccuracy;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelVerticalSpeed;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelCurrentCoord;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	m_labelClearance;

	float									m_fMargin;
};
