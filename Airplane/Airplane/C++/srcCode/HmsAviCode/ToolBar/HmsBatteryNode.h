#pragma once
#include "HmsAviPf.h"

class CHmsBatteryNode : public HmsAviPf::CHmsImageNode
{
public:
	CHmsBatteryNode();

	static CHmsBatteryNode * Create(const char * strBatteryBg, const HmsAviPf::Rect & rectBattery);

	virtual bool Init(const char * strBatteryBg, const HmsAviPf::Rect & rectBattery);

	//fProportion in [0, 100]
	void SetBatteryPowerLevel(int nLevel);
	void SetNotUsed(bool bNotUsed);

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_nodePowerProportion;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_labelPower;

	int											m_nBatteryLevel;
	bool										m_bNotUse;
};

