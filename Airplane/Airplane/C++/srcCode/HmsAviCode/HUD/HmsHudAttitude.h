#pragma once

#include "../HmsGlobal.h"
#include "HmsAviPf.h"
#include "display/HmsDrawNode.h"
#include "base/HmsClippingNode.h"
#include "HmsHudLine.h"



class CHmsHudAttitude : public HmsAviPf::CHmsNode
{
public:
	CHmsHudAttitude();
	~CHmsHudAttitude();

	CREATE_FUNC(CHmsHudAttitude)

	static CHmsHudAttitude* Create(HmsAviPf::Size size);
	bool InitWithSize(HmsAviPf::Size size);
	void SetHudPitch(float fpitch);
	void SetRoll(float fRoll);

private:
	HmsAviPf::CHmsNode* GenZeroLine(float vOffset);
	HmsAviPf::CHmsNode* GenSolidLine(float vOffset);
	HmsAviPf::CHmsNode* GenDotteLine(float vOffset);
	void GenHudAttitude();
	void HudDrawLine(float fOffset, float fValue, unsigned int uIndex);
	void HudDrawText(float fOffset, float fValue, unsigned int nIndex);

protected:
	HmsAviPf::RefPtr<HmsAviPf::CHmsClippingNode>	m_pPitch; //俯仰刻线.
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_pZeroLine;//0刻线.
	std::vector<HmsAviPf::CHmsNode *>				m_pPostiveLine;//正数刻线.
	std::vector<HmsAviPf::CHmsNode *>				m_pNgativeLine;//负数刻线.
	std::vector<HmsAviPf::CHmsLabel *>				m_pLeftLabel;//文本集(左侧).
	std::vector<HmsAviPf::CHmsLabel *>				m_pRigtLabel;//文本集(右侧).
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		m_CmdLoad;//指令杆.
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_RollTick;//滚转刻度.
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_pRoll; //滚转角指针(三角形).

	float											m_PitchValue;//俯仰角度值.
	float											m_RollValue; //滚转角度值.
	float											m_CmmdValue;//
	float											m_FivePixels;//5度间隔像素.
	float											m_TextOffset;
};