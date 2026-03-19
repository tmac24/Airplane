#pragma once

#include "base/HmsClippingNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiImagePanel.h"
#include "ui/HmsUiStretchButton.h"
#include "base/HmsTimer.h"
#include "../HmsGlobal.h"
#include "DataInterface/TerrainServer/HmsTerrainServer.h"

#include "HmsDrawNodeChart.h"
#include "HmsNavDataStruct.h"


#define REFLINE_TEXT_TAG  0xFA

NS_HMS_BEGIN

class CHmsNavProfileChart : public CHmsClippingNode, public HmsUiEventInterface
{
public:
	CHmsNavProfileChart();
	~CHmsNavProfileChart();

	static CHmsNavProfileChart* Create(const Size & sizeShow);

	void SetWptNodeData(const std::vector<WptNodeStu2D> &vec);

	void SetRefLineAltitude(float altitude);

	typedef std::function<void(float highest, float clearance, float firstStrike)> HmsOnRefLinePosChangeCallback;

	void SetOnRefLinePosChangeCallback(HmsOnRefLinePosChangeCallback func){ m_onRefLineChangeCallback = func; }

	void ZoomIn(float step = 0.2);
	void ZoomOut(float step = 0.2);
	void ZoomReset();

	void SetCorridorWidth(float width);
	void SetHazardAltitudes(float high, float low);

	typedef std::function<void()> t_resetCorridorAndHazardFunc;
	void SetResetCallback(t_resetCorridorAndHazardFunc func){ m_resetCallback = func; }

	virtual void Update(float delta) override;

protected:
	virtual bool Init(const Size & sizeShow);

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

	void EdgeDetection();

private:
	inline float LogicYToWorldPixel(float logicY);
	inline float LogicYToRelativePixel(float logicY);
	inline float RelativePixelToLogicY(float pixel);
	void InitChartLayer(const Size & sizeShow);
	void InitRefLine(const Size & sizeShow);
	void RefLineBtnPosChanged();

	void FindFirstStrikeIndex(float refLinePos);

	void SetXYLogicRange(Vec2 x, Vec2 y);
	void UpdateChart();

	void ZoomByScale(float scale);

	void DrawScaleplate();

	void SetAltitudeData(std::vector<AltitudeDataStu> vecAltData, std::vector<NavNodeStu> vecNavNodeName);
	void SetLowHighColorAltitude(float lowAlt, float highAlt);

	void UpdateRefLineMoveStep(Vec2 cursorPos);

private:
	RefPtr<CHmsDrawNode> m_nodeMask;					//the mask of the node

	RefPtr<CHmsNode>			m_pChartContainer;
	RefPtr<CHmsDrawNode>		m_pNodeChartBg;
	RefPtr<CHmsDrawNodeChart>	m_pNodeChart;
	RefPtr<CHmsDrawNode>		m_pNodeChartScaleplate;
	RefPtr<CHmsDrawNode>		m_pNodeChartRefLine;
	
	CHmsTime m_time;
	Vec2 m_posPressStart;
	Vec2 m_posPanelPressStart;
	Vec2 m_posRefLineBtnPressStart;

	std::vector<AltitudeDataStu> m_vecAltDataSource;
	std::vector<AltitudeDataStu> m_vecAltData;
	std::vector<NavNodeStu> m_vecNavNodeNameSource;
	std::vector<NavNodeStu> m_vecNavNodeName;
	Vec2 m_xLogicRange;
	Vec2 m_yLogicRange;
	float m_maxYAltitude;

	float m_yRatio;//一个像素表示多少高度

	const float c_chartBottomLabelHeight = 50;
	const float c_chartTopSpaceHeight = 50;

	const float c_refLineBtnWidth = 160;
	const float c_refLineBtnHeight = 70;
	const float c_refLineBtnLeftSpace = 30;
	bool m_bIsMoveRefLineBtn;

	float m_lowColorDisToRefLine;
	float m_highColorDisToRefLine;
	float m_xStepLength;//nm
	int m_firstStrikeIndex;

	HmsOnRefLinePosChangeCallback m_onRefLineChangeCallback;

    struct NavNodeLabelStu
    {
        Vec2 pos;
        CHmsLabel *pLabel;

        NavNodeLabelStu()
        {
            pLabel = nullptr;
        }
        NavNodeLabelStu(Vec2 pos, CHmsLabel *pLabel)
        {
            this->pos = pos;
            this->pLabel = pLabel;
        }
    };
    std::vector<NavNodeLabelStu> m_vecNavNodeLabel;

    struct NavNodeVLinesStu
    {
        Vec2 pos;
        CHmsDrawNode *pVLineDrawNode;

        NavNodeVLinesStu()
        {
            pVLineDrawNode = nullptr;
        }
        NavNodeVLinesStu(Vec2 pos, CHmsDrawNode *pNode)
        {
            this->pos = pos;
            this->pVLineDrawNode = pNode;
        }
    };
    std::vector<NavNodeVLinesStu> m_vecNavNodeVLines;

	float m_fixWindowX;

	t_resetCorridorAndHazardFunc m_resetCallback;//每次改变数据源，都重置CorridorAndHazard

	bool m_bNeedUpdateChart;
	bool m_bNeedGetAltDataByAsyn;

	float m_fRefLineChangePeriod;
	int m_nRefLineChangeStep;
	CHmsTime m_refLineTime;
	Vec2 m_cursorLastMovePos;

	CHmsTime m_rangeYChangeTime;
	bool m_bDetectRangeY;
};

NS_HMS_END
