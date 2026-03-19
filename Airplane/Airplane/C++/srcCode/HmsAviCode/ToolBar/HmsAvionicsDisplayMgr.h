#pragma once
#include "base/HmsMap.h"
#include "base/HmsNode.h"
#include <ui/HmsUiButtonsManger.h>

#include "HmsAvionicsDisplayLayer.h"
#include "HmsToolBarTop.h"
#include "HmsToolBarBottom.h"
#include "HmsToolBarDisplayFlyData.h"
#include "HmsToolBarDataViewer.h"
#include "SettingPage/HmsSettingPage.h"
#include "HmsLayerTerrain.h"
#include "HmsParallelLayer.h"

enum class HmsPageType
{
	PageNone,
	PageAirports,					//the airports page
	PageMaps,						//the map page
	PageAviPrograms,				//the avionics program display page
	PageScratchPad,					//the scratch pad display page
	Page3DView,						//the 3D View display page
};

struct AvionicsDisplayInfo : public HmsAviPf::Ref
{
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>	node;
	std::vector<std::string>				vDynamicBtns;
};

class CHmsNavFplLayer;

class CHmsAvionicsDisplayMgr
{
public:
	CHmsAvionicsDisplayMgr();
	~CHmsAvionicsDisplayMgr();

	static CHmsAvionicsDisplayMgr * GetInstance();

	void Init(const HmsAviPf::Size & size);

	//nLevel 1=1/4Screen  2=1/2Screen   3=3/4Screen 
	void Resize(const HmsAviPf::Size & size, int nLevel = 2);

	void SetToolBarTop(CHmsToolBarTop * bar);
	void SetDisplayLayer(CHmsAvionicsDisplayLayer * nav);
	void SetToolBarBottom(CHmsToolBarBottom * bar);
    CHmsToolBarBottom* GetToolBarBottom();

	void SetBarActionDisplaySettingPage(bool bShow);
	void SetBarActionDispayFlyPlanBox(bool bShow);
	void SetBarActionDisplayDashboradBar(bool bShow);
	void RunActions(const std::string & strGroup, const std::string & strAction);
	void SetRecordDataPower(bool bPowerOn);

public:
	/* get or set functions begin */
	HmsAviPf::CHmsNode * ShowNavType(HmsPageType eType);

	HmsAviPf::CHmsNode * GetNavType(HmsPageType eType);

	HmsAviPf::CHmsUiButtonsManger * GetBtnMgr(){ return &m_btnMgr; }

	void ShowSettingPage(bool bShow);
    void Show2dTawsPage(bool bShow);
	void ShowDashBoradBar(bool bShow);
    void ShowNavFplLayer(bool bShow);
	void SetPfdMode(const std::string & strMode);
	void SetAviMode(int nModeIndex);

    HmsAviPf::CHmsNode * GetTrackLayer();

protected:
	void AddAvionicsDisplayInfo(HmsPageType eType, HmsAviPf::CHmsNode * node);
	void AddAvionicsDisplayInfo(HmsPageType eType, HmsAviPf::CHmsNode * node, const std::vector<std::string> & vActionName);

private:
	HmsPageType											m_eType;
	HmsAviPf::Map<int, AvionicsDisplayInfo*>			m_mapAviDisplayInfo;
	HmsAviPf::RefPtr<CHmsAvionicsDisplayLayer>			m_layerDisplay;
	HmsAviPf::RefPtr<CHmsToolBarTop>					m_toolbarTop;
	HmsAviPf::RefPtr<CHmsToolBarBottom>					m_toolbarBottom;
	HmsAviPf::RefPtr<CHmsToolBarDisplayFlyData>			m_toolbarDashBoard;
    HmsAviPf::RefPtr<CHmsNavFplLayer>			        m_navFplLayer;

	HmsAviPf::RefPtr<CHmsSettingPage>					m_settingPage;
    HmsAviPf::RefPtr<CHmsLayerTerrain>                  m_2dTawsPage;
	HmsAviPf::RefPtr<CHmsParallelLayer>					m_parallelLayer;

	HmsAviPf::CHmsUiButtonsManger						m_btnMgr;
};