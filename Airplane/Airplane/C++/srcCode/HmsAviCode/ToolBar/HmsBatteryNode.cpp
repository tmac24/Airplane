#include "HmsBatteryNode.h"
#include "HmsGlobal.h"
#include "math/HmsMath.h"
USING_NS_HMS;

CHmsBatteryNode::CHmsBatteryNode()
{
	m_nBatteryLevel = 100;
	m_bNotUse = false;
}

CHmsBatteryNode * CHmsBatteryNode::Create(const char * strBatteryBg, const Rect & rectBattery)
{
	CHmsBatteryNode * node = new CHmsBatteryNode();
	if (node && node->Init(strBatteryBg, rectBattery))
	{
		node->autorelease();
		return node;
	}
	HMS_SAFE_DELETE(node);
	return nullptr;
}

bool CHmsBatteryNode::Init(const char * strBatteryBg, const Rect & rectBattery)
{
	if (!CHmsImageNode::InitWithFile(strBatteryBg))
	{
		return false;
	}

	auto size = GetContentSize();
	unsigned char nColorData = 92;
	m_nodePowerProportion = CHmsImageNode::CreateColorfulSolidRect(rectBattery.size, Color3B(nColorData, nColorData, nColorData));
	Vec2 posBattery(rectBattery.origin.x, size.height / 2.0f);
	if (m_nodePowerProportion)
	{
		m_nodePowerProportion->SetAnchorPoint(Vec2(0.0f, 0.5f));
		m_nodePowerProportion->SetPosition(posBattery);
		this->AddChild(m_nodePowerProportion);
	}
	
	
	m_labelPower = CHmsGlobal::CreateLabel("100", 21, Color4F::WHITE, true);
	if (m_labelPower)
	{
		m_labelPower->SetPosition(posBattery + Vec2(rectBattery.size.width*0.5f, 0));
		m_labelPower->SetAnchorPoint(0.5f, 0.5f);
		this->AddChild(m_labelPower);
	}
	SetColor(Color3B::WHITE);

	return true;
}

void CHmsBatteryNode::SetBatteryPowerLevel(int nLevel)
{
	float nTemp = HMS_MIN(HMS_MAX(nLevel, 1), 100);

	if (nTemp == m_nBatteryLevel)
	{
		return;
	}
	m_nBatteryLevel = nTemp;
	if (m_nodePowerProportion)
	{
		m_nodePowerProportion->SetScaleX(m_nBatteryLevel*0.01);
	}
	CHmsGlobal::SetLabelFormat(m_labelPower, "%d", m_nBatteryLevel);
}

void CHmsBatteryNode::SetNotUsed(bool bNotUsed)
{
	if (bNotUsed == m_bNotUse)
	{
		return;
	}
	m_bNotUse = bNotUsed;
	if (bNotUsed)
	{
		m_labelPower->SetVisible(false);
		m_nodePowerProportion->SetVisible(false);
		unsigned char nColorData = 72;
		SetColor(Color3B(nColorData, nColorData, nColorData));
	}
	else
	{
		m_labelPower->SetVisible(true);
		m_nodePowerProportion->SetVisible(true);
		SetColor(Color3B::WHITE);
	}
}
