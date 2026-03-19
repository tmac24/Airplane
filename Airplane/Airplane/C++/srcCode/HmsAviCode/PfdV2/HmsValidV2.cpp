#include "HmsValidV2.h"
#include "display/HmsDrawNode.h"
#include "display/HmsLightDrawNode.h"
#include "HmsPfdHeader.h"

USING_NS_HMS;

CHmsValidV2::CHmsValidV2()
{
	brushColor  = Color4F(Color4B(0x8C, 0x24, 0x13, 0x96));
	borderColor = Color4B(0xC3, 0xC1, 0xC1, 0xFF);
	crossColor  = Color4B(0xC8, 0x24, 0x08, 0xFF);
	m_bUnValid = true;
}

CHmsValidV2::~CHmsValidV2()
{
}

bool CHmsValidV2::Init()
{
	if (!CHmsNode::Init()){
		return false;
	}

	return true;
}

HmsAviPf::CHmsNode* CHmsValidV2::GenValid(HmsAviPf::Size size)
{
	m_pUnValid = CHmsNode::Create();
	m_pUnValid->SetContentSize(size);

	Vec2 UnValid[4];
	UnValid[0] = Vec2(0.0, 0.0);
	UnValid[1] = Vec2(size.width, size.height);
	CHmsLightDrawNode *pUnValidLine = CHmsLightDrawNode::Create();
	pUnValidLine->DrawSolidRect(0.0, 0.0, size.width, size.height, brushColor, false);
	pUnValidLine->DrawLineStrip(UnValid, 2, 2.0, crossColor);
	UnValid[0] = Vec2(size.width, 0.0);
	UnValid[1] = Vec2(0.0, size.height);
	pUnValidLine->DrawLineStrip(UnValid, 2, 2.0, crossColor);
	UnValid[0] = Vec2(0.0, 0.0);
	UnValid[1] = Vec2(size.width, 0.0);
	UnValid[2] = Vec2(size.width, size.height);
	UnValid[3] = Vec2(0.0, size.height);
	pUnValidLine->DrawLineLoop(UnValid, 4, 2.0, borderColor);

	m_pUnValid->AddChild(pUnValidLine);

	return m_pUnValid;
}

bool CHmsValidV2::ValidSwitch(bool bValid)
{
	PFD_RETURN_VAL(0 == m_pUnValid || m_bUnValid == bValid, false);

	m_bUnValid = bValid;
	if (!bValid)
	{
		this->RemoveChild(m_pUnValid);
		this->SetScheduleUpdate();
	}
	else
	{
		this->AddChild(m_pUnValid);
		this->SetUnSchedule();
	}

	return true;
}
