#include "HmsToolBarTop.h"
#include "../HmsGlobal.h"
#include "../DataInterface/HmsDataBus.h"
#if HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID
//#include "HmsAndroidFunc.h"
#endif
#include "DataInterface/HmsDataBus.h"

USING_NS_HMS;


CHmsToolBarTop::CHmsToolBarTop()
{
	m_bNeedInitProtocalType = true;
	m_bLocalDeviceConnected = false;
	m_bLocalLocationValid = false;
}

bool CHmsToolBarTop::Init()
{
	const float c_fMargin = 8;
	Rect barRect(0, 0, 2560, 46);

	if (!InitWithTexture(nullptr, barRect, false))
	{
		return false;
	}
	//this->SetColor(Color3B(0x0E, 0x19, 0x25));
	this->SetColor(Color3B::BLACK);
	this->SetAnchorPoint(Vec2(0, 1.0f));

	m_labelDate = CHmsGlobal::CreateLabel("15:50 AUG 20", 24, Color4F::WHITE, true);
	if (m_labelDate)
	{
		m_labelDate->SetPosition(Vec2(c_fMargin, barRect.size.height / 2.0f));
		m_labelDate->SetAnchorPoint(0.0f, 0.5f);
		this->AddChild(m_labelDate);
	}

	auto leftBar = CHmsNode::Create();
	leftBar->SetPosition(Vec2(barRect.size.width - c_fMargin, barRect.size.height / 2.0f));
	{
		auto pos = Vec2(0, 0);

		auto CreateBatteryIcon = [=, &pos](const char * strBatteryBg)
		{
			auto icon = CHmsBatteryNode::Create(strBatteryBg, Rect(Vec2(4, 0), Size(47, 23)));
			icon->SetPosition(pos);
			icon->SetAnchorPoint(Vec2(1.0f, 0.5f));
			leftBar->AddChild(icon);
			pos.x -= icon->GetContentSize().width + c_fMargin;
			return icon;
		};

		auto CreateAndAddIcon = [=, &pos](const char * strIconPath)
		{
			auto icon = CHmsImageNode::Create(strIconPath);
			icon->SetPosition(pos);
			icon->SetAnchorPoint(Vec2(1.0f, 0.5f));
			//icon->SetColor(Color3B::GREEN);
			leftBar->AddChild(icon);
			pos.x -= icon->GetContentSize().width + c_fMargin;
			return icon;
		};

		m_nodeBatteryPad = CreateBatteryIcon("res/ToolBarIcon/batteryIcon.png");
		m_nodeBatteryDevice = CreateBatteryIcon("res/ToolBarIcon/batteryDeviceIcon.png");

		m_nodeWifi = CreateAndAddIcon("res/ToolBarIcon/wifiIcon.png");
		m_nodeLocation = CreateAndAddIcon("res/ToolBarIcon/localPosIcon.png");
		m_nodeBluetooth = CreateAndAddIcon("res/ToolBarIcon/bluetoothIcon.png");

		m_nodeWifi->SetColor(GetInvalidColor());
		m_nodeLocation->SetColor(GetInvalidColor());
		m_nodeBluetooth->SetColor(GetInvalidColor());

	};
	this->AddChild(leftBar);

	this->SetScheduleUpdate();
	return true;
}

void CHmsToolBarTop::Update(float delta)
{
	static float interval = 0.0;
	interval += delta;
	if (interval < 2.0f)
	{
		return;
	}
	interval = 0.0;

	if (m_nodeBatteryPad)
	{
		m_nodeBatteryPad->SetBatteryPowerLevel(GetPadBatteryLevel());
	}

	auto aftData = CHmsGlobal::GetInstance()->GetDataBus()->GetData();
	if (aftData->bDeviceConnected)
	{
		m_nodeBatteryDevice->SetNotUsed(false);
		m_nodeBatteryDevice->SetBatteryPowerLevel(aftData->batteryLevel);
	}
	else
	{
		m_nodeBatteryDevice->SetNotUsed(true);
	}

	if (m_bNeedInitProtocalType)
	{
		if (aftData->cCommunicationType != 0)
		{
			if (aftData->cCommunicationType == 'B')
			{
				m_nodeWifi->SetColor(GetInvalidColor());
				m_nodeBluetooth->SetColor(Color3B::WHITE);
				m_nodeCurrentCommunication = m_nodeBluetooth;
			}
			else if (aftData->cCommunicationType == 'W')
			{
				m_nodeWifi->SetColor(Color3B::WHITE);
				m_nodeBluetooth->SetColor(GetInvalidColor());
				m_nodeCurrentCommunication = m_nodeWifi;
			}
			m_bNeedInitProtocalType = false;
		}

		if (aftData->cProtocalType != 0)
		{
			m_bNeedInitProtocalType = false;
		}
	}

	if (m_bLocalDeviceConnected != aftData->bDeviceConnected)
	{
		if (m_nodeCurrentCommunication)
		{
			m_bLocalDeviceConnected = aftData->bDeviceConnected;
			if (m_bLocalDeviceConnected)
			{
				m_nodeCurrentCommunication->SetColor(Color3B::GREEN);
			}
			else
			{
				m_nodeCurrentCommunication->SetColor(Color3B::WHITE);
			}
		}
	}

	if (m_bLocalLocationValid != aftData->bLatLonValid)
	{
		m_bLocalLocationValid = aftData->bLatLonValid;
		if (m_bLocalLocationValid)
		{
			m_nodeLocation->SetColor(Color3B::WHITE);
		}
		else
		{
			m_nodeLocation->SetColor(GetInvalidColor());
		}
	}

	if (m_labelDate)
	{
		CHmsGlobal::SetLabelText(m_labelDate, GetTimeString());
	}
}

int CHmsToolBarTop::GetPadBatteryLevel()
{
#if HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID
//	return GetAndroidBatteryCapacity();
#endif
	return 99;
}

Color3B CHmsToolBarTop::GetInvalidColor()
{
	//return Color3B::GRAY;
	unsigned char nColorData = 72;
	return Color3B(nColorData, nColorData, nColorData);
}
#include "time.h"
char * CHmsToolBarTop::GetTimeString()
{
	time_t nSeconds = 0;
	time(&nSeconds);
	tm * pTM = localtime(&nSeconds);

	static char strTime[40] = { 0 };
	sprintf(strTime, "%02d-%02d %02d:%02d",
		 pTM->tm_mon + 1, pTM->tm_mday,
		pTM->tm_hour, pTM->tm_min);
	return strTime;
}
