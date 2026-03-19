#pragma once
#include "HmsResizeableLayer.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiButtonAbstract.h"

class CHmsHSIV2;
class CHmsAirspeedV2;
class CHmsAltmeterV2;
class CHmsVerticalV2;
class CHmsAttitudeV2;
class CHmsLayerPFDV2 :
	public CHmsResizeableLayer,
	public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsLayerPFDV2();
	~CHmsLayerPFDV2();

	CREATE_FUNC_BY_SIZE(CHmsLayerPFDV2);

	virtual bool Init(const HmsAviPf::Size & size);
	virtual void Update(float delta) override;

	virtual void ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos) override;

private:
	CHmsHSIV2								*m_pHSI;
	CHmsAirspeedV2							*m_pAirspeed;
	CHmsAltmeterV2							*m_pAltmeter;
	CHmsVerticalV2							*m_pVertical;
	CHmsAttitudeV2							*m_pAttitude;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>	m_nodeRoot;
};

