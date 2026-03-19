#pragma once
#include "../HmsGlobal.h"
#include "display/HmsDrawNode.h"

class CHmsGlideslope : public HmsAviPf::CHmsDrawNode
{
public:
	CHmsGlideslope();
	~CHmsGlideslope();

	CREATE_FUNC(CHmsGlideslope);

	virtual bool Init() override;

	virtual void Update(float delta) override;

	//设置指示范围
	void SetRange(float fMin, float fMax);

	//设置偏差
	void SetDeviation(float fDev);

	//
	void SetNoData();
private:
	float m_boxWidth;
	float m_boxHeight;
	float m_scaleGap_px;
	HmsAviPf::Color4B m_boxColor;//背景
	HmsAviPf::Color4B m_scaleColor;//刻度颜色
	HmsAviPf::Color4B m_pointerColor;//指针颜色

	float m_minRange;
	float m_maxRange;
	float m_unitDeviationPx;//所在范围内单位偏差
	float m_curDeviation;//当前偏差

	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>		m_pNodeBox;//背景
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		m_pNodePointer;//指针

	void MovePointer();
};

