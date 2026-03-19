#pragma once
#include "HmsMapLayerInterface.h"
class CHmsAftObjectLayer : public CHmsMapLayerInterface
{
public:
	CHmsAftObjectLayer();
	~CHmsAftObjectLayer();

	CREATE_FUNC(CHmsAftObjectLayer);

	virtual bool Init() override;
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float delta) override;

	virtual void UpdateLayer();

	void SetCurAftCenter(bool bCenter);
	void SetNorthMode(bool bNorth);

	void SetDistanceCicleNode(HmsAviPf::CHmsNode * pNode);

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		m_aftCurInRoot;
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		m_aftCurInMoveable;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeDistanceCicle;
	bool									m_bCurAftCenter;
	bool									m_bNorthMode;
};

