#pragma once

#include "../HmsGlobal.h"
#include "HmsAviPf.h"
#include "display/HmsDrawNode.h"
#include <list>
#include "HUD/HmsMultLineDrawNode.h"

class CHmsHudAltMeter : public HmsAviPf::CHmsNode
{
public:
	CHmsHudAltMeter();
	~CHmsHudAltMeter();

	CREATE_FUNC(CHmsHudAltMeter)

	static CHmsHudAltMeter* Create(HmsAviPf::Size size);
	bool InitWithSize(HmsAviPf::Size size);
	void SetUnValid(int unValid);
	void SetSelAltmeter(float fSelAltVlaue);
	void SetAltMeter(float fAltMeterVlaue);
	void MetricSwitch(bool bOn);

private:
	void GenHudAltmeter();
	void GenDightPlace(
		HmsAviPf::CHmsImageNode **pPalce, 
		float fScale, 
		float x, 
		float y, 
		const std::string &szRes,
		bool bDisplay);
	void CreateDight(bool bPositive, bool bDisplay);
	void UpdateAltBar(float fAltVlaue);
	HmsAviPf::CHmsLabel* GetValue(bool bFront, float *pOut);
	void SetValue(HmsAviPf::CHmsLabel *pLabel, bool bFront, float fVlaue);

protected:
	/*气压、高度表和选择高度背景.*/
	HmsAviPf::RefPtr<CHmsMultLineDrawNode>  m_pUnValid;//无效节点绘制.

	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>      m_pSelAltmeter;//选择高度. 
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>       m_pAltBar;//气压刻度条.
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>       m_pLabelNode;//刻度文本节点集.
	std::list<HmsAviPf::CHmsLabel*>            m_pAltLabel;//刻度值.
	HmsAviPf::CHmsImageNode*  m_pAltSm;//高位占位符.
	HmsAviPf::CHmsImageNode*  m_pAltTP;//高度千位值(正数).
	HmsAviPf::CHmsImageNode*  m_pAltTN;//高度千位值(负数).
	HmsAviPf::CHmsImageNode*  m_pAltHP;//高度百位值(正数).
	HmsAviPf::CHmsImageNode*  m_pAltHN;//高度百位值(负数).
	HmsAviPf::CHmsImageNode*  m_pAltDP;//高度十位值(正数).
	HmsAviPf::CHmsImageNode*  m_pAltDN;//高度十位值(负数).
	HmsAviPf::CHmsImageNode*  m_pAltUP;//高度个位值(正数).
	HmsAviPf::CHmsImageNode*  m_pAltUN;//高度个位值(负数).
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>  m_pSelAltmeterMark;//选择高度游标.
	//HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>   m_pAltLines;//刻度线.
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			  m_pAltLines;

	/*高度指针和背景色.*/
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pPointer;//高度指针.
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pBoxMetricCurAlt;		//米制当前高度框
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pBoxMetricCurAltMask;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pLableMetricCurAlt;	//米制当前高度标签

	float     m_PressurePixels;//多少像素表示1米.
	float     m_tickSpan;//高度刻度间隔值.
	float     m_DightHeight;//数值高度.
	float     m_SingleUseHeight;//个位数字使用高度.
	float     m_AltValue;//当前高度值.
	float     m_SelAltValue;//当前选择高度值.
	float     m_PressureVal;//气压值.
	HmsAviPf::Size     m_HighBySize;//百位和十位宽度和高度.
	HmsAviPf::Size     m_SingleSize;//个位宽度和高度.
	unsigned int       m_tickNumber;//刻度数量.
	unsigned int       m_CurUnValid;//当前有效值.
	bool               m_bInit;//是否为初始化.
};