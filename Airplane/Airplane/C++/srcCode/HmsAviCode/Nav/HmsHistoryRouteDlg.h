#pragma once

#include "ui/HmsListBox.h"
#include "Database/HmsHistoryRouteDatabase.h"
#include "ui/HmsUiStretchButton.h"
#include "HmsNavDataStruct.h"
#include "HmsRouteElementsList.h"
class CHmsFrame2DRoot;

class CHmsHistoryRouteDlg : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsHistoryRouteDlg();
	~CHmsHistoryRouteDlg();

	CREATE_FUNC_BY_SIZE(CHmsHistoryRouteDlg)

	virtual bool Init(const HmsAviPf::Size & windowSize);

	void SaveRoute(const std::vector<FPL2DNodeInfoStu> &vecFPL, std::string &name);
	void OpenDialog();

protected:
	virtual bool OnPressed(const Vec2 & posOrigin);

private:
	void InitListBox(const HmsAviPf::Rect & windowRect);
	void OnListBoxItemClick(CHmsListBoxItem *pItem, int index, bool selected);

	void ConvertFPLNodeToHistory(const FPL2DNodeInfoStu &node, HistoryRouteOnePointStu &hisStu);
	void UpdateListBox();

	void DeleteRoute();
	void UseRoute();

	void CloseDialog();

	std::string GetRouteContentStr(const HistoryRouteStu &stu);

private:
	RefPtr<CHmsListBox> m_pListBox;
	std::vector<HistoryRouteStu> m_vecArray;
	CHmsTime m_time;
	RefPtr<CHmsRouteElementsList> m_pRouteList;
};



