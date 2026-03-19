#pragma once
#include "HmsSettingSubPage.h"

struct ConnectivityItemInfo
{
	std::string												strName;
	int														nParam;
};

enum class ConnectivityType
{
	NoConnectivity,
	Wifi,
	Bluetooth,
};

class CHmsSettingConnectivityPage : public CHmsSettingSubPage
{
public:
	CHmsSettingConnectivityPage();
	~CHmsSettingConnectivityPage();

	static CHmsSettingConnectivityPage * Create(const HmsAviPf::Size & size);

	virtual bool init(const HmsAviPf::Size & size);

	void SetContentCheckBox(const std::vector<ConnectivityItemInfo> & vItemContent);


	virtual void OnEnter() override;
	virtual void OnExit() override;

	virtual void AddContentNode(HmsAviPf::CHmsNode * node) override;
	virtual void RemoveAllContentNode() override;

	void SetContentInfo(const std::string & strContent);


	virtual void Update(float delta) override;

protected:
	void InitComfirmDlg();

	void UpdateItem();
	bool GetRecordItemInfo(std::vector<ConnectivityItemInfo> & vItemInfo);

	std::string GetConnectivityInfo();

	void OnBtnClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname, int nIndex);

	void OnClickConfirmDlgCancel();
	void OnClickConfirmDlgConfirm();

	void OnActionConnectWifi();
	void OnActionConnectBluetooth();

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>					m_nodeConfirmDlg;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>					m_labelConfirmTitle;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>					m_labelContent;
	HmsAviPf::RefPtr<HmsAviPf::CHmsUiVerticalScrollView>	m_viewWindow;

	std::vector<ConnectivityItemInfo>						m_vItemInfo;

	ConnectivityType										m_eConnectivityType;
	std::string												m_strActionName;
	int														m_nActionId;
	CHmsTime												m_timeLastUpdate;
};
