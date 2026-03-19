#pragma once
#include "base/RefPtr.h"
#include "base/HmsMap.h"
#include "display/HmsStretchImageNode.h"
#include "display/HmsImageNode.h"
#include "display/HmsDrawNode.h"
#include "display/HmsLabel.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiButtonAbstract.h"
#include "ui/HmsUiImagePanel.h"
#include "HmsToolBarModeBtn.h"

typedef std::function<void(bool)> HmsButtonActionCallback;

struct ButtonActionInfo : public HmsAviPf::Ref
{
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>	node;
	HmsButtonActionCallback					callback;
};

class CHmsToolBarBottom : public HmsAviPf::CHmsImageNode, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsToolBarBottom();

	CREATE_FUNC(CHmsToolBarBottom);

	virtual bool Init() override;

	void SetCurrentMode(const char * strBtns);

	void SetAndShowDynamicBtns(const std::vector<std::string> & vActionNames);

	void SetDynamicBtnState(const std::string & strBtnActionName, bool bOn);

protected:
	void InitDynamicBtns();

	void OnToolBarBtnChange(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState, const char * strBtns);
	void OnToolBarDynamicBtnChange(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState, const char * strBtns);

	void OnBtnSystemSetting(bool bSelected);
	void OnBtn2dTaws(bool bSelected);
	void OnBtnFlyPlan(bool bSelected);
	void OnBtnDashBoard(bool bSelected);
	
private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsUiPanel>				m_panelDynamicBtns;
	HmsAviPf::Map<std::string, CHmsToolBarModeBtn*>		m_mapBtns;
	HmsAviPf::Map<std::string, ButtonActionInfo*>		m_mapDynamicBtns;
	std::string											m_strCurMode;
};


