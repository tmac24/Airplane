#pragma once
#include "../HmsGlobal.h"
#include "../DataInterface/SmartAviDataDef.h"
#include "ui/HmsUiEventInterface.h"
#include "HmsGauge.h"
#include "HmsNumBox.h"
#include "../PfdV2/HmsRollLable.h"

class CHmsAirspeed : public CHmsGauge, public HmsUiEventInterface
{
public:
    struct AirspeedMarking
    {
        AirspeedMarking(float _vMin, float _vMax, HmsAviPf::Color4B _color)
        {
            vMin = _vMin;
            vMax = _vMax;
            color = _color;
        }
        float vMin;
        float vMax;
        HmsAviPf::Color4B color;
    };

    CHmsAirspeed();
    ~CHmsAirspeed();

    CREATE_FUNC(CHmsAirspeed)

        virtual bool Init() override;

    virtual void Update(float delta) override;
    //设置空速表标识 即空速表上红绿色的带
    void InitAirspeedMarking(std::vector<AirspeedMarking> list);

    void SetSelectedSpeed(float fSpd);//设置选择空速

    void SetGaugeMode(GaugeMode flag) override;

    void OnSelectedSpeedChanged(int speed);

    void MetricSwitch(bool bOn);
private:
    float m_scaleBoxHeight;
    float m_scaleBoxWidth;
    HmsAviPf::Color4B m_boxColor;
    HmsAviPf::Color4B m_vectorColor;
    float m_scaleLineWidth;
    float m_maxSpeed;
    float m_scaleGap_px;	//刻度像素间隔
    float m_speedGap;		//速度间隔
    float m_markingWidth;	//空速表标识宽度

    float m_selSpeed;//选择速度
    float m_curSpeed;//当前速度

    std::map<int, HmsAviPf::CHmsLabel*> m_labelMap;

    //{{added by wzh 20190710
    //增加米制显示
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pBoxMetricSelSpeed;		//米制选择速度框
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pBoxMetricCurSpeed;		//米制当前速度框
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pLableMetricSelSpeed;		//米制选择速度标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pLableMetricCurSpeed;		//米制当前速度标签
    //}}

    HmsAviPf::RefPtr<CHmsNumBox>			m_pSelAirspeedBox;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pScaleBar;//刻度尺
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pScaleLines;//刻度尺
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pAirspeedMarking;//空速表标识
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pSelSpeedBug;//选择空速游标
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pNumberLap;//数字带，滚动显示个位数值
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pNumber2Label;//十位百位数值
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pSpeedVector;//速度趋势线
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pFailWin;//故障窗体

    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_rootNode;
    CHmsRollLabel							   *m_pRollLabel; /*滚动文本对象.*/

    void UpdateIAS(float fSpdIas);//设置指示空速
    void UpdateSpeedTrend(const float &fSpeedTrend);//设置速度趋势 6秒后能够到达的速度
    void ScrollScaleBar(const float &fSpd);//根据给定速度滚动刻度尺
    void MoveSelectedSpeedBug();//移动选定速度游标的位置

    void RefreshScaleLinePos(bool downToUp);
    //修改刻度标签的位置。从上往下滚动时：底部的标签依次移动到最顶端；从下往上滚动时：顶部标签依次移动到最底部
    //upToDown：是否从下往上滚动，false为从上往下,fCurSpeed：当前速度
    void RefreshLabelPos(bool downToUp, const float &fCurSpeed);

    //速度转Y位置坐标
    float SpeedToPosY(const float &fSpeed);
};

