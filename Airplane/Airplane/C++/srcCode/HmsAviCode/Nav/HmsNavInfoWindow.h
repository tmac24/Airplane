#pragma once

#include "ui/HmsTableBox.h"
#include "HmsNavProfileChart.h"
#include "HmsChartConfigWin.h"
#include "HmsRouteElementsEdit.h"
#include "Calculate/CalculateZs.h"
#include "DataInterface/TerrainServer/HmsTerrainServer.h"

#include "HmsNavDataStruct.h"
#include "display/HmsLineEdit.h"
#include "ui/HmsKeyboard.h"
#include "HmsNavMenuButton.h"

#define  NAV_PROFILE_LABEL_HIGHEST		723
#define  NAV_PROFILE_LABEL_CLEARANCE	724
#define  NAV_PROFILE_LABEL_STRIKE		725

class CHmsFrame2DRoot;

class CHmsNavInfoWindow : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	enum class NavInfoPage
	{
		PAGE_EDIT,
		PAGE_NAVLOG,
		PAGE_PROFILE
	};
public:
	CHmsNavInfoWindow();
	~CHmsNavInfoWindow();

	CREATE_FUNC_BY_SIZE(CHmsNavInfoWindow)

	virtual bool Init(const HmsAviPf::Size & windowSize);
	virtual void Update(float delta) override;
	void SetWptNodeData(const std::vector<FPL2DNodeInfoStu> &vecFPLInfo);
	
	void UpdateAllChildPos(CHmsNode *pNode);
	void LineEditBlur();
	void OnKeyboardCallback(char c);

    void UpdateTableDisplayData();

protected:
	virtual bool OnPressed(const Vec2 & posOrigin);

private:
	void OnRowButtonClicked(CHmsUiButtonAbstract *pBtn);
    void OnSelectCurrentNavLeg(CHmsUiStretchButton *pUiStretchButton);
	void InitEdit(const HmsAviPf::Size & windowSize);
	void InitNavLog(const HmsAviPf::Size & windowSize);
	void InitProfile(const HmsAviPf::Size & windowSize);
	void InitBottomToolBtn(const HmsAviPf::Size & windowSize);
	void ShowPage(CHmsUiButtonAbstract *pBtn, NavInfoPage page);
	void OnRefLinePosChange(float highest, float clearance, float firstStrike);

private:
    RefPtr<CHmsUiStretchButton> m_pCurSelectRowButton;
	Size m_toolButtonSize;
	Color3B m_toolButtonNormalColor;
	Color3B m_toolButtonSelectColor;

	RefPtr<CHmsUiStretchButton> m_pButtonEdit;
	RefPtr<CHmsUiStretchButton> m_pButtonNavLog;
	RefPtr<CHmsUiStretchButton> m_pButtonProfile;

	RefPtr<CHmsRouteElementsEdit> m_pRouteElementsEdit;

	RefPtr<CHmsTableBox> m_pNavLogTable;

	RefPtr<CHmsNavProfileChart> m_pNavProfileChart;
	RefPtr<CHmsUiPanel> m_pNavProfileRightPanel;

	RefPtr<CHmsChartConfigWin> m_pChartConfigWin;

	int m_flySegment;

    RefPtr<CHmsLineEidt> m_pGSLineEdit;

    std::vector<FPL2DNodeInfoStu> m_vecFPL2DNodeInfoStu; //用于传递到航路预览
    std::vector<WptNodeStu2D> m_vecWptNodeStu2D;//用于表格显示
    float m_fGroundSpeed;//nm/h
};
