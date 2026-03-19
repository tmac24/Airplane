#pragma once
#include "HmsAviPf.h"
#include "HmsBatteryNode.h"

class CHmsToolBarTop : public HmsAviPf::CHmsImageNode
{
public:
	CHmsToolBarTop();

	CREATE_FUNC(CHmsToolBarTop);

	virtual bool Init() override;

	virtual void Update(float delta) override;

protected:
	int GetPadBatteryLevel();
	HmsAviPf::Color3B GetInvalidColor();
	char * GetTimeString();

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_labelDate;
	HmsAviPf::RefPtr<CHmsBatteryNode>			m_nodeBatteryPad;
	HmsAviPf::RefPtr<CHmsBatteryNode>			m_nodeBatteryDevice;

	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_nodeWifi;
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_nodeBluetooth;
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_nodeLocation;

	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_nodeCurrentCommunication;

	bool										m_bNeedInitProtocalType;

	bool										m_bLocalDeviceConnected;
	bool										m_bLocalLocationValid;			//whether used location is valid,such as gps
};


