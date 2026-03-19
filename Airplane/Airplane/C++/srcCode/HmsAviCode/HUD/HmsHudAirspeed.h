#pragma once
#include "../HmsGlobal.h"
#include "HmsAviPf.h"
#include "display/HmsDrawNode.h"
#include "HUD/HmsMultLineDrawNode.h"
#include "HmsHudSpeedBar.h"

class CHmsHudAirspeed : public HmsAviPf::CHmsNode
{
public:
	CHmsHudAirspeed();
	~CHmsHudAirspeed();

	CREATE_FUNC(CHmsHudAirspeed)

	static CHmsHudAirspeed* Create(HmsAviPf::Size size);
	bool InitWithSize(HmsAviPf::Size size);

	void SetAirSpeed(float fAirSpeedVlaue);
	void SetSelSpeed(float fSelSpeedVlaue);
	void SetUnValid(int unValid);

	void MetricSwitch(bool bOn);

protected:
	void GenAirspeed();

protected:
	/*马赫数、刻度和选择空速背景.*/
	CHmsMultLineDrawNode  *m_pUnValid;//无效节点绘制.

	/*选择空速.*/
	HmsAviPf::CHmsLabel     *m_pSelSpeed;//选择空速.  

	/*空速条.*/
	CHmsHudSpeedBar         *m_pSpeedBar;

	/*空速指示值.*/
	//HmsAviPf::CHmsImageNode *m_pSpeedT;//空速千位值.
	HmsAviPf::CHmsImageNode *m_pSpeedH;//空速百位值.
	HmsAviPf::CHmsImageNode *m_pSpeedD;//空速十位值.
	HmsAviPf::CHmsImageNode *m_pSpeedU;//空速个位值.

	/*空速指针背景.*/
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pSpeedPointer;//空速指针.
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pBoxMetricCurSpeed;		//米制当前速度框
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pBoxMetricCurSpeedMask;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pLableMetricCurSpeed;		//米制当前速度标签

	/*计算相关量.*/
	HmsAviPf::Size m_DightSize;//数值图大小.
	float     m_DightHeight;//数值高度.
	float     m_AirSpeedValue;//当前空速值.
	float     m_SelSpeedValue;//选择空速值.
	float     m_MachValue;//马赫数值.
	int       m_CurUnValid;//当前无效数值.
};

