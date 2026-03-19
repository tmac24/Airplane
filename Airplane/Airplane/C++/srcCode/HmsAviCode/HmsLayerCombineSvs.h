#pragma once
#include "HmsScreenLayout.h"
#include "base/HmsImage.h"
#include "ui/HmsUiEventInterface.h"
#include "HmsResizeableLayer.h"
#include "Svs/HmsSvs.h"
#include "HmsLayerPfdV1.h"
#include "HmsLayerPfdV2.h"
#include "HmsLayerHud.h"
#include "base/HmsMap.h"

using namespace HmsAviPf;

class CHmsLayerCombineSvs : public CHmsResizeableLayer, public HmsUiEventInterface
{
public:
	CHmsLayerCombineSvs();
	~CHmsLayerCombineSvs();

	virtual bool Init(const HmsAviPf::Size & size);

	CREATE_FUNC_BY_SIZE(CHmsLayerCombineSvs);

	virtual void OnEnter() override;

	virtual void OnExit() override;

	virtual void Update(float delta) override;

	virtual void ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos) override;

	void SetPfdMode(const std::string & strMode);

	CHmsSvs * GetSvsLayer(){ return m_nodeSvs.get(); }

private:
	HmsAviPf::RefPtr<CHmsSvs>							m_nodeSvs;
	HmsAviPf::RefPtr<CHmsLayerPfdV1>					m_nodePfdV1;
	HmsAviPf::RefPtr<CHmsLayerPFDV2>					m_nodePfdV2;
	HmsAviPf::RefPtr<CHmsLayerHud>						m_nodeHud;
	HmsAviPf::Map<std::string, CHmsNode*>				m_mapPfdMode;
};

