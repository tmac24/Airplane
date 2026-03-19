#pragma once
#include "../HmsGlobal.h"
#include "math/HmsMath.h"
#include "math/HmsGeometry.h"
#include "base/HmsNode.h"
#include "base/HmsClippingNode.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "display/HmsLabel.h"
#include "display/HmsDrawNode.h"
#include "HmsVerticalV2.h"

class CHmsAttitudeV2 : public CHmsValidV2
{
	typedef std::vector<HmsAviPf::CHmsNode*> TextTicks;
public:
	CHmsAttitudeV2();
	~CHmsAttitudeV2();

	CREATE_FUNC(CHmsAttitudeV2)

	static CHmsAttitudeV2* Create(HmsAviPf::Size size);

	bool InitWithSize(HmsAviPf::Size size);

	void SetPitch(float fPitch);
	void SetRoll(float fRoll);

protected:
	HmsAviPf::CHmsNode* GenCircleTick(float fRadius);
	HmsAviPf::CHmsNode* GenRollPointer(float fRadius);
	HmsAviPf::CHmsNode* GenPlaneMarker(HmsAviPf::Size size);
	HmsAviPf::CHmsNode* GenPitchPointer(float fRadius);

	HmsAviPf::CHmsNode* GenTickText(float fLength, float fLineHigh,float txtLineSpan);
	void ShowTick(float fOffset, float fValue, unsigned int uIndex);
private:
	float			m_CurPitch;
	float           m_CurRoll;
	float			m_FiveSpan;
	unsigned int	m_nShowNumber;

	HmsAviPf::RefPtr<HmsAviPf::CHmsNode> m_RollPointer;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode> m_PitchPointer;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode> m_pZeroTick;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode> m_pUnValid;

	TextTicks m_LongsTick;
	TextTicks m_ShortTick;
};

