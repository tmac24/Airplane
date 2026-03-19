#pragma once
#include "HmsScreenLayout.h"
#include "HmsMapNode.h"
#include "Terrain/HmsTerrainMenuBar.h"
#include "HmsLayerTaws.h"
#include "HmsResizeableLayer.h"


class CHmsProfileView;
class CHmsLayerTerrain : public CHmsResizeableLayer, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsLayerTerrain();
	~CHmsLayerTerrain();

	CREATE_FUNC_BY_SIZE(CHmsLayerTerrain);

	virtual bool Init(const HmsAviPf::Size & size);

	virtual void Update(float delta) override;
    CHmsLayerTaws* GetTawsLayer(){ return m_pTawsView.get(); }
protected:
	void InitFlowBtns();

private:
	HmsAviPf::RefPtr<CHmsLayerTaws>						m_pTawsView;
	HmsAviPf::RefPtr<CHmsProfileView>					m_pProfileView;
};

