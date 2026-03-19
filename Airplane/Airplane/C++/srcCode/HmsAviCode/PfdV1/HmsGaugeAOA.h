#pragma once
#include "../HmsGlobal.h"
#include "HmsGauge.h"

class CHmsGaugeAOA : public CHmsGauge
{
public:
	CHmsGaugeAOA();
	~CHmsGaugeAOA();

	CREATE_FUNC(CHmsGaugeAOA);

	virtual bool Init() override;

	void SetValue(const float &fValue);//设置攻角值，单位°

	virtual void SetGaugeMode(GaugeMode flag) override;

	virtual void Update(float delta) override;
private:
	float m_boxWidth;
	float m_boxHeight;
	HmsAviPf::Color4B m_boxColor;
	HmsAviPf::Color4B m_defaultColor;//默认边框颜色
	HmsAviPf::Color4B m_slectedColor;//选中时的边框颜色

	float m_aoaValue;

	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pBoxNode;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pValue;
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pFailWin;
};

