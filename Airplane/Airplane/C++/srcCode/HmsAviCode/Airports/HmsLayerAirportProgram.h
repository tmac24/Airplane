#pragma once

#include "../HmsGlobal.h"
#include "../HmsScreenLayout.h"
#include "display/HmsDrawNode.h"
#include "display/HmsLabel.h"
#include "display/HmsLineEdit.h"
#include "base/HmsTypes.h"
#include "ui/HmsUiImageButton.h"
#include "ui/HmsUiEventInterface.h"

#include "HmsNavImageSql.h"
#include "HmsImageScrollView.h"
#include "DataInterface/HmsDataBus.h"
#include "../Calculate/CalculateZs.h"
#include "../Calculate/PositionZs.h"
#include "../Nav/HmsNavMenuButton.h"

#include "../HmsNavMgr.h"

#include "../Nav/HmsMercatorEarthTileMgr.h"
#include "../Nav/ScratchPad/HmsScratchPadColorDlg.h"
#include "../Nav/ScratchPad/HmsScratchPadClipping.h"
#include "../Nav/ScratchPad/HmsScratchPadDraw.h"

enum APToolBtnType
{
	AP_Back,
	AP_Map,
	AP_Move,
	AP_TurnLeft,
	AP_TurnRight,
	AP_FullScreen,
	AP_ZoomIn,
	AP_ZoomOut,
	AP_Left,
	AP_Right,
	AP_Up,
	AP_Down,

	AP_Draw,
	AP_PenColor,
	AP_Clear,
	AP_Undo,
	AP_Redo,
    AP_Favorite,
    AP_Close
};

using namespace HmsAviPf;

class CHmsLayerAirports;

struct XYLineStu
{
	double pixelValue;
	double logicValue;
	double ratio;

	XYLineStu() :pixelValue(0), logicValue(0), ratio(0)
	{}
};

class CHmsLayerAirportProgram : public CHmsNode, public HmsUiEventInterface
{
public:
	CHmsLayerAirportProgram();
	~CHmsLayerAirportProgram();

    static CHmsLayerAirportProgram* GetInstance();

	//CREATE_FUNC_BY_SIZE(CHmsLayerAirportProgram)

	virtual bool Init(const HmsAviPf::Size & size);

	virtual void Update(float delta) override;

	void SetStarSidStu(StarSidStu stu);

    void SetOnCloseCallback(std::function<void()> func);

private:
	void InitEvent();

	void InitBottom();
	void InitRight();

	void BottomToolBtnCallback(CHmsUiButtonAbstract *btn, HMS_UI_BUTTON_STATE eState, APToolBtnType btnType);
	void RightToolBtnCallback(CHmsUiButtonAbstract *btn, HMS_UI_BUTTON_STATE eState, APToolBtnType btnType);

	void MapImageToEarth();
	void UnMapImageToEarth();

	void ChangeBtnColor(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);
	void ReSetImageScale();
	void ZoomImage(float scale);

	void CalXYTable();
	//X向右增加
	void FindIndexIncToRight(int &xIndex, int &yIndex, double logicX, double logicY, 
		const std::vector<XYLineStu> &vecX, const std::vector<XYLineStu> &vecY);
	
	static void AreaInfoToVector(std::string &infoStr, std::vector<XYLineStu> &vecX, std::vector<XYLineStu> &vecY);
	static std::vector<std::string> StrSplit(std::string& s, const std::string& delim);

    void CloseThis();

private:
	Size m_toolBottom;
	Size m_toolRight;
	float m_margin;

	StarSidStu m_starSidStu;

	std::vector<XYLineStu> m_vecFirstAreaXLines;
	std::vector<XYLineStu> m_vecFirstAreaYLines;
	std::vector<XYLineStu> m_vecSecondAreaXLines;
	std::vector<XYLineStu> m_vecSecondAreaYLines;

	//RefPtr<CHmsImageNode> m_pImageNode;
	RefPtr<CHmsImageNode> m_pImagePlaneTop;
	RefPtr<CHmsImageNode> m_pImagePlaneSide;

	RefPtr<CHmsNavMenuButton> m_pToolBtnTurnLeft;
	RefPtr<CHmsNavMenuButton> m_pToolBtnTurnRight;
	RefPtr<CHmsNavMenuButton> m_pToolBtnZoomIn;
	RefPtr<CHmsNavMenuButton> m_pToolBtnZoomOut;

	RefPtr<CHmsNavMenuButton> m_pToolBtnLeft;
	RefPtr<CHmsNavMenuButton> m_pToolBtnRight;
	RefPtr<CHmsNavMenuButton> m_pToolBtnUp;
	RefPtr<CHmsNavMenuButton> m_pToolBtnDown;

	//RefPtr<CHmsImageScrollView> m_pImageScrollView;

	RefPtr<CHmsNode>			m_nodePageMove;
	RefPtr<CHmsNode>			m_nodeCurBtnGroup;
	bool m_bIsFullScreen;

	RefPtr<CHmsScratchPadColorDlg> m_colorDlg;
	RefPtr<CHmsScratchPadClipping> m_scratchPadClipping;
	RefPtr<CHmsScratchPadDraw> m_scratchPadDraw;

    CHmsFixSizeUiImageButton *m_pRightFavoriteBtn;
    bool m_bEnableModifyFavorite;

    std::function<void()> m_onCloseCallback;
};

