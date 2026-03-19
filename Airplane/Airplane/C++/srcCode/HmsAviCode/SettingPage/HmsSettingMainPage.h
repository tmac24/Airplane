#pragma once
#include <HmsAviPf.h>
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiImagePanel.h"
#include "ui/HmsUiVerticalScrollView.h"
#include "ui/HmsUiButtonAbstract.h"
#include "ui/HmsUiImage2StateButton.h"
#include "ui/HmsUiLabelButton.h"
#include "../HmsParallelLayer.h"
#include "base/HmsMap.h"

struct MainPageDeviceStatus
{
	bool	bConnected;
	bool	bCharging;
	int		bBatteryLevel;
};

enum class HMS_DEVICE_STATUS
{
	connecting,
	connected,
	charging,
};

class CHmsSettingMainPage : public HmsAviPf::CHmsNode, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsSettingMainPage();
	~CHmsSettingMainPage();

	static CHmsSettingMainPage * Create(const HmsAviPf::Size & size);

	virtual bool init(const HmsAviPf::Size & size);

	HmsAviPf::CHmsUiVerticalScrollView * GetContentNode(){ return m_nodeContent.get(); }

	void SetParallelLayer(CHmsParallelLayer * layer){ m_layerParallel = layer; }

	void SetCurrentDisplayLanguage(const std::string & strLanguageName);
	void SetCurrentDisplayAirplane(const std::string & strAirplane);

	void SetSvsFirstView();
	void SetSvsThirdView();
	void SetDisplayHudMode();
	void SetDisplayPfdMode();
	void SetRecordDataPower(bool on);

	void SetMenuSyncFuncPfdMode(const std::string & strPfdMode);


	virtual void Update(float delta) override;

protected:
	void InitContent();

	void OnNextPageBtnEvent(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname, const std::string & subItem);
	void OnSliderBtnEvent(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname);
	void OnSliderBtnEventPFD(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname);
	void OnSliderBtnSubEvent(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname, 
		const std::string & subNormal, const std::string & subSelected);
	void OnRadioBtnCheck(int nCheckId, const std::string & strname, const std::string & sub1, const std::string & sub2);

	void OnSliderBtnEventRecordData(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e);
    void OnSliderBtnEventRecordTrackData(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e);
	void OnSliderBtnEventWarningSound(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e);

	void SetDeviceStatus(HMS_DEVICE_STATUS eStatus);

	std::string GetSoftwareVersion();
	std::string GetDeviceVersion();

	void UpdateDeviceVersion();

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>							m_nodeSelected;
	HmsAviPf::RefPtr<HmsAviPf::CHmsUiVerticalScrollView>			m_nodeContent;
	HmsAviPf::RefPtr<HmsAviPf::CHmsStretchImageNode>				m_imageBackground;
	HmsAviPf::RefPtr<CHmsParallelLayer>								m_layerParallel;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>							m_labelLanguageDisplay;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>							m_labelAirplaneDisplay;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>							m_labelDeviceStatusDisplay;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>							m_labelDeviceVersionDisplay;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>							m_labelBtnConnectivity;
	HmsAviPf::RefPtr<HmsAviPf::CHmsUiImage2StateButton>				m_btnStatePfd;
	HmsAviPf::RefPtr<HmsAviPf::CHmsUiImage2StateButton>				m_btnStateHud;
	HmsAviPf::RefPtr<HmsAviPf::CHmsUiImage2StateButton>				m_btnStatePfd2;
	HmsAviPf::RefPtr<HmsAviPf::CHmsUiImage2StateButton>				m_btnStateRecord;

	HmsAviPf::Map<std::string, HmsAviPf::CHmsUiImage2StateButton*>	m_mapBtnPfdGroup;


	float															m_fContentBeginY;
	float															m_fCurDeviceVersion;
	bool															m_bTawsNeedShowWhenShowPFD;
	char															m_cCommunicationType;

	HMS_DEVICE_STATUS												m_eDeviceStatus;
};

