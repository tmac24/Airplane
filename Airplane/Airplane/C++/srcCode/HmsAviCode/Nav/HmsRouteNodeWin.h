#pragma once

#include "ui/HmsListBox.h"
#include "ui/HmsUiStretchButton.h"
#include "HmsNavDataStruct.h"

#include "../Database/HmsSSADatabase.h"
#include "../Database/Hms424Database.h"

class CHmsFrame2DRoot;

enum class WinPageShow
{
	PAGE_ACTION_NORMAL = 0,
	PAGE_ACTION_SID,
	PAGE_ACTION_STARAPP,
	PAGE_ACTION_CHGSID,
	PAGE_ACTION_CHGSTAR,
	PAGE_ACTION_CHGAPP,
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
struct ShowPageWptNode
{
	float lon;
	float lat;
	std::string ident;//program时传入的是机场
	std::string belongAirport;
	bool bEnableShowOnMap;

	ShowPageWptNode()
	{
		lon = 0;
		lat = 0;
		bEnableShowOnMap = false;
	}
	ShowPageWptNode(float lon, float lat, std::string ident, std::string belongAirport, bool bEnableShowOnMap)
	{
		this->lon = lon;
		this->lat = lat;
		this->ident = ident;
		this->belongAirport = belongAirport;
		this->bEnableShowOnMap = bEnableShowOnMap;
	}
};

class CHmsRouteNodeWin : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsRouteNodeWin();
	~CHmsRouteNodeWin();

	CREATE_FUNC_BY_SIZE(CHmsRouteNodeWin)

	virtual bool Init(const HmsAviPf::Size & windowSize);

	void ShowPage(WinPageShow page, ShowPageWptNode node, int index);

	void SetSTARRunwayFilter(std::string str);
	void SetAPPRunwayFilter(std::string str);

	void UseProgram(std::string airport, std::string program, std::string trans, std::string runway, FPL2DNodeType type, int index);

	static void ProcessSpecialPoint(std::vector<WptNodeStu2D> &vecWptNode);

private:
	void InitListBox(const HmsAviPf::Rect & windowRect);
	void InitTriangleNode(Size tSize, Size outSize);

	void OnListBoxItemClick(CHmsListBoxItem *pItem, int index, bool selected);

	void SelectSIDSTARFinished();

private:
	RefPtr<CHmsListBox> m_pListBox;
	RefPtr<CHmsDrawNode> m_pTriangleNode;

	ShowPageWptNode m_showPageNode;
	std::string m_strSTARRunwayFilter;
	std::string m_strAPPRunwayFilter;
	int m_curNodeIndex;
	WinPageShow m_curPage;

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


	//const std::string m_cInsertBefore = "Insert Before";
	//const std::string m_cInsertAfter = "Insert After";
	
	const std::string m_cSelectArrival = "Select Arrival";
	const std::string m_cChangeArrival = "Change Arrival";
	const std::string m_cSelectDeparture = "Select Departure";
	const std::string m_cChangeDeparture = "Change Departure";
	const std::string m_cSelectApproach = "Select Approach";
	const std::string m_cChangeApproach = "Change Approach";
	const std::string m_cLinkString = ".........";
};