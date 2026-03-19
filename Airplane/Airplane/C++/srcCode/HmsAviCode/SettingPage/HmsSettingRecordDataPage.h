#pragma once
#include "HmsSettingSubPage.h"

struct RecordItemInfo
{
	std::string												strName;
	int														nParam;
	int														nSize;
	bool													bSelected;
};

enum class RecordFileAction
{
	NoAction,
	CopyAll,
    CopySelected,
	CleanSelected,
	CleanAll,
};

class CHmsSettingRecordDataPage : public CHmsSettingSubPage
{
public:
	CHmsSettingRecordDataPage();
	~CHmsSettingRecordDataPage();

	static CHmsSettingRecordDataPage * Create(const HmsAviPf::Size & size);

	virtual bool init(const HmsAviPf::Size & size);

	void SetContentCheckBox(const std::vector<RecordItemInfo> & vItemContent);


	virtual void OnEnter() override;

	virtual void AddContentNode(HmsAviPf::CHmsNode * node) override;
	virtual void RemoveAllContentNode() override;

protected:
	void InitComfirmDlg();

	void UpdateItem();
	bool GetRecordItemInfo(std::vector<RecordItemInfo> & vItemInfo);

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
	RecordFileAction										m_eAction;

	std::vector<RecordItemInfo>								            m_vItemInfo;
    HmsAviPf::Map<std::string, HmsAviPf::CHmsUiImage2StateButton *>     m_mapCheckBoxGroup;
};
