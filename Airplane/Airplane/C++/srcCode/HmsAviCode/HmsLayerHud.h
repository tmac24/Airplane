#pragma once
#include "HmsResizeableLayer.h"
#include "display/HmsDrawNode.h"
#include <ui/HmsUiEventInterface.h>
#include <ui/HmsUiButtonAbstract.h>
#include "HUD/HmsHudAttitudeFullDraw.h"
#include "HUD/HmsHudAltmeter.h"
#include "HUD/HmsHudAirspeed.h"
#include "HUD/HmsHudHeading.h"
#include "HUD/HmsMultLineDrawNode.h"

class CHmsLayerHud : public CHmsResizeableLayer, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsLayerHud();
	~CHmsLayerHud();

	CREATE_FUNC_BY_SIZE(CHmsLayerHud);

	virtual bool Init(const HmsAviPf::Size & size);
	virtual void Update(float delta) override;

	virtual void ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos) override;
	
protected:
	HmsAviPf::CHmsLabel *  GenOverload();
	void InitInvalidDraw(const HmsAviPf::Size & size);

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>	m_rootContent;
	CHmsHudAttitudeFullDraw *				m_pHudAttitude;
	CHmsHudAltMeter *						m_pHudAltmeter;
	CHmsHudAirspeed *						m_pHudAirspeed;
	CHmsHudHeading *						m_pHudHeading;
	HmsAviPf::CHmsLabel *					m_pOverload;
	HmsAviPf::CHmsMultLineDrawNode *		m_drawInvalid;
};

