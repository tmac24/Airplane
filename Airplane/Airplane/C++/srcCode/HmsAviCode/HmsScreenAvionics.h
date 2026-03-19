#pragma once
#include "base/HmsNode.h"
#include "HmsGlobal.h"
#include "ToolBar/HmsAvionicsDisplayLayer.h"
#include "ToolBar/HmsAvionicsDisplayMgr.h"

class CHmsToolBarTop;
class CHmsToolBarBottom;

class CHmsScreenAvionics : public HmsAviPf::CHmsNode
{
public:
	CHmsScreenAvionics();
	~CHmsScreenAvionics();

	CREATE_FUNC(CHmsScreenAvionics);

	virtual bool Init() override;

	virtual void OnEnter() override;


	virtual void OnExit() override;


	virtual void Update(float delta) override;

private:

	CHmsAvionicsDisplayMgr *					m_pAviDisplayMgr;
	HmsAviPf::RefPtr<CHmsAvionicsDisplayLayer>	m_aviDisplayLayer;

	CHmsToolBarTop								* m_pToolbarTop;
	CHmsToolBarBottom							* m_pToolbarBottom;
};

