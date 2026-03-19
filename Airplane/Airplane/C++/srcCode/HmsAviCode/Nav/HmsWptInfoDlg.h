#pragma once

#include "ui/HmsListBox.h"
#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiVerticalScrollView.h"
#include "../Database/Hms424Database.h"
#include "../Calculate/CalculateZs.h"
#include "../Database/HmsHistoryRouteDatabase.h"
#include "HmsNavMathHelper.h"

#include <functional>
#include <map>

#include "HmsNavDataStruct.h"

enum class WptDlgPage
{
	PAGE_ALL,
	PAGE_APT,
	PAGE_NAV,
	PAGE_WPT,

	PAGE_EXIT_CANCEL,
	PAGE_EXIT_DELETE
};

enum class WptDlgMode
{
	WPTDLGMODE_ADD,
	WPTDLGMODE_MOD
};

struct WptNodeStu
{
	WptNodeBrief brief;

	double distance;
	double angle;
	std::string chType;
	float freq;
	std::string name;

	std::string des1;
	std::string des2;
#if _NAV_SVS_LOGIC_JUDGE
	WptNodeStu()
	{
		distance = 0.0;
		angle = 0.0;
		freq = 0.0;
	}
#endif
};

class CHmsWptInfoDlg : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsWptInfoDlg();
	~CHmsWptInfoDlg();

	CREATE_FUNC_BY_SIZE(CHmsWptInfoDlg)

	virtual bool Init(const HmsAviPf::Size & windowSize);

	static const char * GetImagePathByWptType(WptPointType t);

	typedef std::function<void(WptPointType t, float lon, float lat, std::string ident, int params)> t_CallbackFuncAdd;
	typedef std::function<void(int params)> t_CallbackFuncCancel;
	void SetCallbackAdd(t_CallbackFuncAdd func){ m_callbackFuncAdd = func; }
	void SetCallbackCancel(t_CallbackFuncCancel func){ m_callbackFuncCancel = func; }
	void SetCallbackDelete(t_CallbackFuncCancel func){ m_callbackFuncDelete = func; }
	
	void ShowDlg(float lon, float lat, WptDlgMode dlgMode, int params);
    void ShowPage(CHmsUiButtonAbstract *pBtn, WptDlgPage page);

protected:
	virtual bool OnPressed(const Vec2 & posOrigin);

private:
	void InitListBox(const HmsAviPf::Rect & windowRect);

	void GetVorInfo(float lon, float lat, std::vector<WptNodeStu> &vecRet);
	void GetNDBInfo(float lon, float lat, std::vector<WptNodeStu> &vecRet);
	void GetAirport(float lon, float lat, std::vector<WptNodeStu> &vecRet);
	void GetWaypoint(float lon, float lat, std::vector<WptNodeStu> &vecRet);
	void GetUserWptInfo(float lon, float lat, std::vector<WptNodeStu> &vecRet);

	CHmsUiStretchButton * GetOrCreateUiButton(WptPointType t, const char *text1, const char *text2);
	CHmsUiStretchButton * CreateRouteItemButton(WptPointType t, const char *text1, const char *text2);

private:
	RefPtr<CHmsUiVerticalScrollView> m_pList;

	RefPtr<CHmsUiStretchButton> m_pButtonALL;
	RefPtr<CHmsUiStretchButton> m_pButtonAPT;
	RefPtr<CHmsUiStretchButton> m_pButtonNAV;
	RefPtr<CHmsUiStretchButton> m_pButtonWPT;

	RefPtr<CHmsUiStretchButton> m_pButtonCancel;
	RefPtr<CHmsUiStretchButton> m_pButtonDelete;
    RefPtr<CHmsUiStretchButton> m_pButtonRoam;
	RefPtr<CHmsLabel> m_pLabelCancel;
	RefPtr<CHmsLabel> m_pLabelDelete;

    Vec2 m_lonlatClick;

	t_CallbackFuncAdd m_callbackFuncAdd;
	t_CallbackFuncCancel m_callbackFuncCancel;
	t_CallbackFuncCancel m_callbackFuncDelete;

	Size m_listSize;
	float m_lonLatSpace;

	std::map<CHmsUiStretchButton*, WptNodeStu> m_wptNodeMap;

	std::vector<WptNodeStu> m_vecAllNodeInfo;

	int m_callbackParam;

	HmsAviPf::Vector<CHmsUiStretchButton*> m_vecUiButtonPool;
	int m_nCurButtonPoolIndex;
};