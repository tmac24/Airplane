#pragma once

#include "ui/HmsUiEventInterface.h"
#include "base/HmsNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsListBox.h"
#include "HmsAviMacros.h"
#include "ui/HmsUiStretchButton.h"
#include "../HmsNavEarthFrame.h"

#include "../HmsNavDataStruct.h"
#include "../../Database/HmsSSADatabase.h"

#include "HmsProcedureEarth2D.h"

#include <sstream>

using namespace HmsAviPf;

class CHmsFrame2DRoot;


enum class ProcedurePageShow
{
	PAGE_LIST_BRIEF = 0,
	PAGE_LIST_SID,
	PAGE_LIST_SID_TRANS,
	PAGE_LIST_SID_RNW,
	PAGE_LIST_STAR,
	PAGE_LIST_STAR_TRANS,
	PAGE_LIST_STAR_RNW,
	PAGE_LIST_APP,
	PAGE_LIST_APP_TRANS,
	PAGE_LIST_APP_RNW
};

struct ProcedureParamStu
{
	std::string airportIdent;
	float lon;
	float lat;
	int index;

	ProcedureParamStu()
	{
		lon = 0;
		lat = 0;
		index = -1;
	}
};

class CHmsProcedureDlg : public CHmsNode, public HmsUiEventInterface
{
public:
	CHmsProcedureDlg();
	~CHmsProcedureDlg();

	CREATE_FUNC_BY_SIZE(CHmsProcedureDlg);

	virtual bool Init(const HmsAviPf::Size & size);

	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;

	void SetSIDSTARAirport(ProcedureParamStu sid, ProcedureParamStu star);

	void ShowPage(ProcedurePageShow page);

private:
	void InitModules(const HmsAviPf::Rect & windowRect);
	void InitLeftList(const HmsAviPf::Rect & windowRect);
	void InitRightEarth(const HmsAviPf::Rect & windowRect);
	void OnListBoxItemClick(CHmsListBoxItem *pItem, int index, bool selected);

	void ChangeBtnColor(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);

	void BackTo();
	void ShowAddToRouteButton();
	void SelectSIDSTARFinished();

	//多个程序
	void SetRouteDataToEarth2D(std::map<std::string, McduProgramInfo> &mapProgram);
	//一个程序多个trans
	void SetRouteDataToEarth2D(McduProgramInfo &program, std::string transName, std::string runwayName);

	void TransMcduToWpt2DNode(PreviewRouteStu &stu, McduProgramInfo program, std::string transName, std::string runwayName);

	void ConvertMCDUToWPT2D(std::vector<WptNodeStu2D> &dst, std::vector<McduWayCtrlPoint> &src, const std::string &airport, ROUTE_TYPE type);

private:
	RefPtr<CHmsListBox> m_pListBox;
	RefPtr<CHmsUiStretchButton> m_pBtnAddToRoute;
	RefPtr<CHmsNavEarthFrame> m_pEarthFrame;
	RefPtr<CHmsProcedureEarth2D> m_pProcedureEarth2D;

	std::map<std::string, McduProgramInfo> m_mapProgramInfo;

	struct SelectInfoStu
	{
		McduProgramInfo program;
		McduTRANSITIONInfo programTrans;
		McduRunwayInfo programRunway;

		void Clear()
		{
			memset(program.name, 0, sizeof(program.name));
			program.vWayPoint.clear();
			program.mapTrans.clear();
			program.mapRunway.clear();

			memset(programTrans.transName, 0, sizeof(programTrans.transName));
			programTrans.vWayPoint.clear();

			memset(programRunway.runwayName, 0, sizeof(programRunway.runwayName));
			programRunway.vWayPoint.clear();
		}
	};
	SelectInfoStu m_selectInfo;

	ProcedureParamStu m_airportSID;
	ProcedureParamStu m_airportSTAR;

	ProcedurePageShow m_curPage;

	std::string m_lastMapTypeName;

	const std::string m_cLinkString = ".........";
	const std::string m_cDepartureStr = "Departure";
	const std::string m_cArrivalStr = "Arrival";
	const std::string m_cApproachStr = "Approach";
	const std::string m_cAll = "ALL";
};