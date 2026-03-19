#pragma once
#include "HmsSettingSubPage.h"


class CHmsSettingDeviceCalibrate : public CHmsSettingSubPage
{
public:
	CHmsSettingDeviceCalibrate();
	~CHmsSettingDeviceCalibrate();

	static CHmsSettingDeviceCalibrate * Create(const HmsAviPf::Size & size);

	virtual bool init(const HmsAviPf::Size & size);

	virtual void OnEnter() override;
	virtual void OnExit() override;

	virtual void AddContentNode(HmsAviPf::CHmsNode * node) override;
	virtual void RemoveAllContentNode() override;

	void SetContentInfo(const std::string & strContent);


	virtual void Update(float delta) override;

protected:
	void InitCalibrateItems();
	void InitComfirmDlg();

	void UpdateItem();

	std::string GetConnectivityInfo();

	void OnSaveBtnClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e);
	void OnEraseBtnClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e);

	void OnClickConfirmDlgCancel();
	void OnClickConfirmDlgConfirm();

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>					m_nodeConfirmDlg;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>					m_labelConfirmTitle;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>					m_labelContent;
	HmsAviPf::RefPtr<HmsAviPf::CHmsUiVerticalScrollView>	m_viewWindow;

	std::string												m_strActionName;
	int														m_nActionId;
	CHmsTime												m_timeLastUpdate;
};
