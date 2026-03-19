#pragma once
#include "HmsSettingSubPage.h"
#include "HmsSettingTrackDataPage.h"

enum class TrackApplyAction
{
	NoAction,
	ToRoute,
	CleanSelected,
	ToReverseRoute,
};

class CHmsSettingTrackDataApplyPage : public CHmsSettingSubPage
{
public:
	CHmsSettingTrackDataApplyPage();
	~CHmsSettingTrackDataApplyPage();

	static CHmsSettingTrackDataApplyPage * Create(const HmsAviPf::Size & size);

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

    void OnItemSelected(const std::string & strName, int nParam);
    void OnItemUnSelected(const std::string & strName, int nParam);

	void OnClickToRoute();
	void OnClickCleanSelected();
	void OnClickToReverseRoute();
	void OnClickConfirmDlgCancel();
	void OnClickConfirmDlgConfirm();

	void DoToRoute();
	void DoCleanSelected();
	void DoToReverseRoute();

private:
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>					            m_nodeConfirmDlg;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>					            m_labelConfirmTitle;
    HmsAviPf::RefPtr<HmsAviPf::CHmsUiVerticalScrollView>	            m_viewWindow;
    TrackApplyAction										            m_eAction;

    HmsAviPf::Map<std::string, HmsAviPf::CHmsUiImage2StateButton*>		m_mapCheckboxGroup;

	std::vector<TrackItemInfo>								            m_vItemInfo;

    std::string                                                         m_strCurItemName;
};
