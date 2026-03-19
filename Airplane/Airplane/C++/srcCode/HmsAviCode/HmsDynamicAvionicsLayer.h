#pragma once
#include <HmsAviPf.h>
#include <ui/HmsUiEventInterface.h>
#include "HmsParallelLayer.h"
#include "HmsLayerCombineSvs.h"
#include "HmsLayerTerrain.h"
#include "Nav/HmsNavEarthLayer.h"
#include "SettingPage/HmsSettingPage.h"
#include "HmsResizeableLayer.h"
#include "ToolBar/HmsToolBarFlowBtn.h"
#include "ToolBar/HmsToolBarDataViewer.h"
#include "Warning/HmsWarningLayer.h"

enum class DynamicViewMode
{
	NoneMode = 0,
	EarthNavMode,
	EarthNavAndSvsMode,
	SvsMode,
	SvsAndTawsMode,
	EarthNavAndTawsMode,
	MaxMode,
};

class CHmsDynamicAvionicsLayer : public CHmsResizeableLayer, public HmsAviPf::HmsUiEventInterface
{
public:
	CREATE_FUNC_BY_SIZE(CHmsDynamicAvionicsLayer);

	CHmsDynamicAvionicsLayer();

	virtual bool Init(const HmsAviPf::Size & size);

	bool ShowSettingPage(std::string strOn);
	bool ShowSettingSubPage(std::string strSubPage);
	bool SetWarningDistance(std::string strDistance);
    bool SetTawsColorMode(std::string strTawsColorMode);
    bool SetMetricDisplay(std::string strOn);
	bool SetPfdTrueTrackDisplay(std::string strOn);
	bool SetAttitudeZeroDisplay(std::string strOn);
	bool SetDeviceDataDisplay(std::string strOn);
	bool ChangeCurrentPlane(std::string strPlaneName);
	bool SetSvsPfdMode(std::string strMode);

	void SetParallelLayer(CHmsParallelLayer * pLayer);

	void SetSysViewIndicate(HmsAviPf::CHmsNode * pNode, const HmsAviPf::Vec2 & posNormal, const HmsAviPf::Vec2 & posSettingShow);


	virtual void SetVisible(bool visible) override;

	void SetViewMode(DynamicViewMode eMode);
	void SetViewModeAndSetBtnState(DynamicViewMode eMode);
	void SetPfdMode(const std::string & strMode);

    CHmsNavEarthLayer * GetNavEarthLayer(){ return m_nodeNavEarth.get(); }

protected:
	void InitMenuButtons();

	void OnModeBtnsChange(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState, DynamicViewMode eMode);
	

private:
	HmsAviPf::RefPtr<CHmsLayerCombineSvs>				m_nodeSvs;
	HmsAviPf::RefPtr<CHmsNavEarthLayer>					m_nodeNavEarth;
	HmsAviPf::RefPtr<CHmsLayerTerrain>					m_nodeTerrain;
	HmsAviPf::RefPtr<CHmsToolBarDataViewer>				m_nodeDataViewer;
    HmsAviPf::RefPtr<CHmsWarningLayer>                  m_nodeWarning;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>				m_nodeSettingBtn;

	HmsAviPf::RefPtr<CHmsParallelLayer>					m_parallelLayer;

	HmsAviPf::Map<int, CHmsToolBarFlowBtn*>				m_mapModeFlowBtn;

	float												m_fSettingPageWidth;

	DynamicViewMode										m_eCurViewMode;
};

