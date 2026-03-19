#pragma once

#include "../HmsGlobal.h"
#include "HmsAviPf.h"
#include "display/HmsDrawNode.h"
#include "base/HmsClippingNode.h"
#include "HmsHudLine.h"


class CHmsHudAttitudeFullDraw : public HmsAviPf::CHmsNode
{
public:
	CHmsHudAttitudeFullDraw();
	~CHmsHudAttitudeFullDraw();

	CREATE_FUNC(CHmsHudAttitudeFullDraw)

	static CHmsHudAttitudeFullDraw* Create(HmsAviPf::Size size);

	bool InitWithSize(HmsAviPf::Size size);
	void SetHudPitch(float fpitch);
	void SetRoll(float fRoll);

private:
	void GenHudAttitude();
	HmsAviPf::CHmsNode * CreateAircraftSymbal(float fLineWidth);

protected:
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeRoot;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodePdf;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeLadderPitch;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeLadderRoll;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeBankPointer;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeBankScale;


	HmsAviPf::RefPtr<HmsAviPf::CHmsClippingNode>	m_pPitch;	//俯仰刻线.
	
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		m_CmdLoad;		//指令杆.
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_RollTick;		//滚转刻度.
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_pRoll;		//滚转角指针(三角形).

	float    m_fPitchValue;//俯仰角度值.
	float    m_fRollValue; //滚转角度值.
	float    m_CmmdValue;//
	float    m_FivePixels;//5度间隔像素.
	float    m_TextOffset;
	float	 m_fHeightPerPitch;
};