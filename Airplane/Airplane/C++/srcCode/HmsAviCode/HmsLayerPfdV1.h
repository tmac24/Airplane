#pragma once
#include "HmsResizeableLayer.h"
#include "display/HmsDrawNode.h"
//#include "HmsDataBus.h"
#include "PfdV1/HmsAirspeed.h"
#include "PfdV1/HmsAltimeter.h"
#include "PfdV1/HmsAttitude.h"
#include "PfdV1/HmsVSpeed.h"
#include "PfdV1/HmsNavInfoLeft.h"
#include "PfdV1/HmsNavInfoRight.h"
#include "PfdV1/HmsWind.h"
#include "PfdV1/HmsUnitBox.h"
#include "PfdV1/HmsRadioAlt.h"
#include "PfdV1/HmsNPIL.h"
#include "PfdV1/HmsNPIV.h"
#include "PfdV1/HmsGlideslope.h"
#include "PfdV1/HmsMBI.h"
#include "PfdV1/HmsHSI.h"
#include "PfdV1/HmsHSIBox.h"
#include "PfdV1/HmsSampleBox.h"
#include "PfdV1/HmsGaugeOAT.h"
#include "PfdV1/HmsGaugeAOA.h"
#include "ui/HmsUiEventInterface.h"

// class CHmsAirspeed;
// class CHmsAttitude;
// class CHmsVSpeed;
// class CHmsHSI;
// class CHmsWind;
// class CHmsUnitBox;
// class CHmsRadioAlt;
// class CHmsGlideslope;
// class CHmsMBI;
// class CHmsNavInfoLeft;
// class CHmsNavInfoRight;
// class CHmsHSIBox;
// class CHmsSampleBox;
// class CHmsGaugeOAT;
// class CHmsGaugeAOA;

class CHmsLayerPfdV1 : public CHmsResizeableLayer, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsLayerPfdV1();
	~CHmsLayerPfdV1();

	CREATE_FUNC_BY_SIZE(CHmsLayerPfdV1)

	virtual bool Init(const HmsAviPf::Size & size);

	virtual void Update(float delta) override;

	virtual void SetVisible(bool visible) override;

	virtual void ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos) override;

protected:
	void InitBackground();//背景

private:
	float m_fSkyInitHeight;//初始化天空高度

	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>				m_nodePfdRoot;

	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>				m_pNodeSkyGroundRotation;
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>			m_pNodeSkyGround;


	
	HmsAviPf::RefPtr<CHmsAirspeed>					m_pNodeAirspeed;
	HmsAviPf::RefPtr<CHmsAltimeter>					m_pNodeAltimeter;
	HmsAviPf::RefPtr<CHmsAttitude>					m_pNodeAttitude;
	HmsAviPf::RefPtr<CHmsVSpeed>					m_pNodeVSpeed;
	HmsAviPf::RefPtr<CHmsHSI>						m_pNodeHSI;
	HmsAviPf::RefPtr<CHmsHSIBox>					m_pHsiBoxHdg;
	HmsAviPf::RefPtr<CHmsHSIBox>					m_pHsiBoxCrs;
	HmsAviPf::RefPtr<CHmsNavInfoLeft>				m_pNavInfoLeft;
	HmsAviPf::RefPtr<CHmsNavInfoRight>				m_pNavInfoRight;
	HmsAviPf::RefPtr<CHmsRadioAlt>					m_pNodeRadioAlt;//无线电高度
	HmsAviPf::RefPtr<CHmsGlideslope>				m_pNodeGlideslope;//下滑道指示器
	HmsAviPf::RefPtr<CHmsMBI>						m_pNodeMBI;//信标指示器

#if 0
	void OnPitchChanged(float pitch_px);
	void OnRollChanged(float roll_angle);
	void OnCourseChanged(int course);
	void OnSelHeadingChanged(int heading);
	void OnNavSrcChanged(CHmsHSI::NAV_SRC navSrc);
	void OnMetricSelectedChanged(bool bSelected);
	void OnHPASelectedChanged(bool bSelected);
	void OnStdClicked(bool bSelected);

	void OnKnobSelHeadingInnerChanged(float fValue);
	void OnKnobSelHeadingOuterChanged(float fValue);
	void OnKnobSelCourseInnerChanged(float fValue);
	void OnKnobSelCourseOuterChanged(float fValue);
	void OnKnobSelAltitudeInnerChanged(float fValue);
	void OnKnobSelAltitudeOuterChanged(float fValue);
	void OnKnobSetBaroInnerChanged(float fValue);
	void OnKnobSetBaroOuterChanged(float fValue);

	void SendBaroSettingData(BaroSettingData*);
	void SendSettingData(SettingData*);
#endif
};

