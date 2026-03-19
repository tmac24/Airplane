#pragma once
#include <HmsAviPf.h>
#include "HmsSettingMainPage.h"
#include "HmsSettingSubPage.h"
#include "../HmsParallelLayer.h"

class CHmsSettingPage : public HmsAviPf::CHmsNode, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsSettingPage();
	~CHmsSettingPage();

	static CHmsSettingPage * Create(const HmsAviPf::Size & size);

	virtual bool init(const HmsAviPf::Size & size);

	void SetParallelLayer(CHmsParallelLayer * layer);

	void InitAircraftMode(std::vector<SettingItemContent> & vItemContent);

	void SetSvsFirstView();

	void SetSvsThirdView();

	void SetDisplayHudMode();

	void SetDisplayPfdMode();

	void SetRecordDataPower(bool on);


	bool OnPressedOutside(const Vec2 & posOrigin);

	virtual void RegisterTouchEvent(int nEventOrder) override;

public:
	bool OnActionSetRecordFlyData(std::string strOn);

protected:
	void InitContent();

	void OnNextPageBtnEvent(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname, const std::string & subItem);
	void OnSliderBtnEvent(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname);
	void OnSliderBtnSubEvent(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname, 
		const std::string & subNormal, const std::string & subSelected);

	bool ShowSettingSubPage(std::string strSubPage);
	bool SetSvsPfdDisplayMode(std::string strPfdMode);
	//bool ShowSettingSubPageByConnectivityInfo(std::string strSubPage);
	//bool ShowSettingSubPageFlightDataManage(std::string strSubPage);

	void OnLanguageSelected(const std::string & strName, int nParam);
	void OnHelicopterSelected(const std::string & strName, int nParam);
	void OnWarningDistanceSelected(const std::string & strName, int nParam);
	void OnColorAlarmModeSelected(const std::string & strName, int nParam);

	std::string GetConnectivityInfo();

private:
	HmsAviPf::RefPtr<CHmsSettingMainPage>					m_pageMian;
	HmsAviPf::Map<std::string, CHmsSettingSubPage*>			m_mapSubPage;
	HmsAviPf::RefPtr<HmsAviPf::CHmsStretchImageNode>		m_imageBackground;
	HmsAviPf::RefPtr<CHmsParallelLayer>						m_layerParallel;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>					m_curPage;
	std::string												m_strCurAirplane;
};

