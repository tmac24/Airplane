#pragma once

#include "../HmsGlobal.h"
#include "base/HmsNode.h"
#include "base/HmsClippingNode.h"
#include "base/HmsImage.h"
#include "display/HmsStretchImageNode.h"
#include "Warning/HmsWarningColorStyle.h"

#define MAX_WARNING_THRESHOLD_NUMBER 8

class CHmsChartAxis;
class CHmsProfileGraph;
class CHmsProfileChart : public HmsAviPf::CHmsNode
{
public:
    CHmsProfileChart();

    static CHmsProfileChart* Create(float width, float height);

    virtual bool Init(float width, float height);

    virtual void Update(float delta) override;

    void Resize(float width, float height);

    void SetRange(float fRangeShow);

    void SetBorderColor(const HmsAviPf::Color4B &color);

    void SetBorder(bool left, bool bottom, bool right, bool top);

    void SetData(std::vector<HmsAviPf::Vec2> & data, float fHightestTerrain);
    void SetDataClear();

    void SetAftAltMeter(float fAltMeter);

    void SetSubdivisionCnt(int nSubdivisionCnt){ m_nSubdivisionCnt = nSubdivisionCnt; }

    void UpdateBorder();

    float LogicYToWorldPixel(float logicY);
private:
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>			m_pGraphBorder;
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>			m_pGraphAftLevel;
    HmsAviPf::Color4F m_borderColor;
    HmsAviPf::Color4F m_bgColor;
    bool m_bLeftBorder;
    bool m_bRightBorder;
    bool m_bTopBorder;
    bool m_bBottomBorder;

    HmsAviPf::Size									m_sizeContent;
    HmsAviPf::RefPtr<CHmsChartAxis>					xAxis;
    HmsAviPf::RefPtr<CHmsChartAxis>					yAxis;
    HmsAviPf::RefPtr<CHmsProfileGraph>				graph;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>			m_nodeAxisXUnit;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>			m_nodeAxisYUnit;
    HmsAviPf::RefPtr<HmsAviPf::CHmsClippingNode>	m_pNodeClipping;
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>		m_pNodeStencil;
    int												m_nSubdivisionCnt;
    float											m_fMaxHeight;
    float											m_fCurHeight;

    std::vector<warningColorStyle*>                 m_warningColorStylePtrs;
    std::vector<float>                              m_altThresholdLevelsPixel;
    std::vector<HmsAviPf::Vec4>                     m_warningColors;
};
