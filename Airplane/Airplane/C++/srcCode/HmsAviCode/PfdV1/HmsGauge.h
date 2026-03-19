#pragma once
#include "HmsAviPf.h"
#include "display/HmsDrawNode.h"

class CHmsGauge : public HmsAviPf::CHmsNode
{
public:
    CHmsGauge();
    ~CHmsGauge();
    enum class GaugeMode{ GAUGE_MODE_OK = 0, GAUGE_MODE_ERROR = 1, GAUGE_MODE_NODATA = 2 };

    virtual bool Init() override;

    virtual void SetGaugeMode(GaugeMode flag){};

protected:
    HmsAviPf::Color4F failRectColor;		//故障窗口背景颜色
    HmsAviPf::Color4F failRectBorderColor;	//故障窗口边框颜色
    HmsAviPf::Color4B failCrossLineColor;	//故障窗口叉线条颜色
    GaugeMode m_curGaugeMode;	//当前仪表模式

    void DrawFailWin(HmsAviPf::CHmsDrawNode *failWindNode, float leftTopX, float leftTopY, float rightBottomX, float rightBottomY);
};

