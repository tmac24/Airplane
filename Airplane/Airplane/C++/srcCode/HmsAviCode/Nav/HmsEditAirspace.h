#pragma once

#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiButton.h"
#include "display/HmsLineEdit.h"
#include "ui/HmsKeyboard.h"
#include "RoutePlanData/mcduDef.h"
#include "HmsNavDataStruct.h"
#include "ui/HmsTableBox.h"
#include <sstream>
#include "HmsLonLatEditWidget.h"
#if 0
#include "DataInterface/SyncModule/chmsuisyncdatamgr.h"
#endif

using namespace HmsAviPf;

class CHmsEditAirspace : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsEditAirspace();
	~CHmsEditAirspace();

	CREATE_FUNC_BY_SIZE(CHmsEditAirspace)
	virtual bool Init(const HmsAviPf::Size & windowSize);

	void LineEditBlur();
	void ShowDlg();
	void CloseDlg();
	void OnDragButtonCallback(float lon, float lat);
	void UpdateAirspaceEditData(const std::vector<AirspaceEditStu> &vecStu);

	virtual void Update(float delta) override;

#if 0
	void SendSyncData();
	void OnSyncDataUpdate(ESyncDataID dataID, char *data, int length);
	void UseSyncData();
#endif

protected:
	virtual bool OnPressed(const Vec2 & posOrigin) override;

private:
	void AddAirspacePoint();
	void DeleteAirspacePoint();
	void MovePointUp();
	void MovePointDown();
	void ReCreateName();
	void UpdateDataFromDb();
	void UpdateList(bool bCall2D);
	void ShowOrHideDragButton(bool bShow);
	void SaveAirspaceToDb();

private:
	RefPtr<CHmsTableBox> m_pTableBox;
    RefPtr<CHmsLonLatEditWidget> m_pLonLatEditWidget;
	std::vector<AirspaceEditStu> m_vecStu;

	bool m_bNeedSync;
	std::mutex m_syncMutex;
	bool m_bNeedUseSyncData;
	std::vector<Vec2> m_vecSyncAirspace;
};

