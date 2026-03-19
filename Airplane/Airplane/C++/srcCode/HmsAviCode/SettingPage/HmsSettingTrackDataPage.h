#pragma once
#include "HmsSettingSubPage.h"

struct TrackItemInfo
{
	std::string												strName;
	int														nParam;
	int														nSize;
	bool													bSelected;
};

enum class TrackFileAction
{
	NoAction,
	CopyAll,
    CopySelected,
	CleanSelected,
	CleanAll,
};

class CHmsSettingTrackDataPage : public CHmsSettingSubPage
{
public:
	CHmsSettingTrackDataPage();
	~CHmsSettingTrackDataPage();

	static CHmsSettingTrackDataPage * Create(const HmsAviPf::Size & size);

	virtual bool init(const HmsAviPf::Size & size);

	void SetContentCheckBox(const std::vector<TrackItemInfo> & vItemContent);


	virtual void OnEnter() override;

	virtual void AddContentNode(HmsAviPf::CHmsNode * node) override;
	virtual void RemoveAllContentNode() override;

protected:
	void InitComfirmDlg();

	void UpdateItem();
	bool GetRecordItemInfo(std::vector<TrackItemInfo> & vItemInfo);

	void OnCheckBoxClick(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE state, const std::string & strName, int nParam);

	void OnClickCopyAll();
    void OnClickCopySelected();
	void OnClickCleanSelected();
	void OnClickCleanAll();
    void OnClickSelectAll();
	void OnClickConfirmDlgCancel();
	void OnClickConfirmDlgConfirm();

	void DoCopyAll();
    void DoCopySelected();
	void DoCleanSelected();
	void DoCleanAll();

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>					m_nodeConfirmDlg;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>					m_labelConfirmTitle;
	HmsAviPf::RefPtr<HmsAviPf::CHmsUiVerticalScrollView>	m_viewWindow;
	TrackFileAction										    m_eAction;

	std::vector<TrackItemInfo>								            m_vItemInfo;
    HmsAviPf::Map<std::string, HmsAviPf::CHmsUiImage2StateButton *>     m_mapCheckBoxGroup;

};
