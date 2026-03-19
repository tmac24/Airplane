#pragma once
#include "../HmsGlobal.h"
#include "display/HmsDrawNode.h"
#include "base/HmsTimer.h"

class CHmsUnitBox : public HmsAviPf::CHmsNode
{
public:
	enum Units{ inHg, Metric };

	CHmsUnitBox();
	~CHmsUnitBox();

	CREATE_FUNC(CHmsUnitBox);

	virtual bool Init() override;

	void ChangeUnits(Units u);

private:
	float m_unitBoxWidth;
	float m_unitBoxHeight;
	HmsAviPf::Color4B m_defBorderColor;//默认边框颜色
	HmsAviPf::Color4B m_selBorderColor;//选中时的边框颜色

	Units m_curUnits;//当前使用单位

	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>		m_pNodeInHg;//英制切换按钮
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>		m_pNodeMetric;//米制切换按钮
};

