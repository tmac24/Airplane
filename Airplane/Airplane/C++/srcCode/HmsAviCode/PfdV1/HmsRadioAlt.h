#pragma once
#include "../HmsGlobal.h"
#include "HmsGauge.h"

class CHmsRadioAlt : public CHmsGauge
{
public:
	CHmsRadioAlt();
	~CHmsRadioAlt();

	CREATE_FUNC(CHmsRadioAlt);

	virtual bool Init() override;

	void SetValue(const int &iValue);

	virtual void SetGaugeMode(GaugeMode flag) override;

	virtual void Update(float delta) override;
private:
	float m_boxWidth;
	float m_boxHeight;
	HmsAviPf::Color4B m_boxColor;
	HmsAviPf::Color4B m_defaultColor;//默认边框颜色
	HmsAviPf::Color4B m_slectedColor;//选中时的边框颜色

	int m_radioAlt;

	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pBoxNode;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pValue;
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pFailWin;
};

