#pragma once
#include "../HmsGlobal.h"
#include "HmsAviPf.h"
#include "display/HmsDrawNode.h"
#include <list>
#include "DataInterface/HmsDataBus.h"

class CHmsHudHeading : public HmsAviPf::CHmsNode
{
public:
	CHmsHudHeading();
	~CHmsHudHeading();

	CREATE_FUNC(CHmsHudHeading)

	static CHmsHudHeading* Create(HmsAviPf::Size size);
	bool InitWithSize(HmsAviPf::Size size);
	void SetHudHeading(float fHeading);
	void SetTrueOrMagneticType(bool bTrue);
	void SetTrackOrHeadingType(bool bTrack);
	void SetPfdCompassDataMode(PFD_COMPASS_DATA_MODE eMode);
	void CheckPfdCompassMode(PFD_COMPASS_DATA_MODE eMode);

private:
	HmsAviPf::CHmsImageNode * GenTick(bool bLong);
	HmsAviPf::CHmsLabel* GenText();
	void GenHudHeading();

protected:
	HmsAviPf::CHmsNode*              m_pTickText;
	HmsAviPf::CHmsLabel*             m_pHeadingText;
	HmsAviPf::CHmsLabel*             m_pSignText;
	HmsAviPf::CHmsLabel*             m_pTrackText;
	std::list<HmsAviPf::CHmsLabel *> m_Texts;//文本集.

	PFD_COMPASS_DATA_MODE			 m_ePfdCompassDataMode;

	int   m_TickNumber;/*刻度线数.*/
	float m_CurHeading;/*当前航向值.*/
	float m_TickSpan;  /*刻线间隔距离.*/
	bool  m_bTrueHead; /*真航向使用标识.*/
	bool  m_bTrack;    /*航迹使用标记.*/
};