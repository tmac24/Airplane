#pragma once
#include "../HmsGlobal.h"
#include "HmsGauge.h"
#include "HmsNumBox.h"
#include "ui/HmsUiEventInterface.h"
#include "../PfdV2/HmsRollLable.h"

class CHmsAltimeter : public CHmsGauge, public HmsUiEventInterface
{
public:
    enum BARO_UNIT{ UNIT_INHG = 0, UNIT_HPA = 1 };		//INHG英寸汞柱 HPA百帕
    enum BARO_REF{ REF_STD = 0, REF_QFE = 1, REF_QNH = 2 };	//STD标准大气压 QFE场压 QNH修正海平面气压

    CHmsAltimeter();
    ~CHmsAltimeter();

    CREATE_FUNC(CHmsAltimeter)

        virtual bool Init() override;
    virtual void Update(float delta) override;

    void MetricSwitch(bool bOn);
    void SetSelectedAlt(const float &fAlt);//设置选定高度值，单位英尺
    float GetSelectedAlt();
    float GetBarometric();

    void SetGaugeMode(GaugeMode flag) override;

    void OnSelectedAltitudeChanged(int altitude);
private:
    CHmsDataBus *m_dataBus;

    float scaleBoxHeight_px;		//高度表高度
    float scaleBoxWidth_px;			//高度表宽度
    HmsAviPf::Color4B m_boxColor;	//高度表背景颜色
    HmsAviPf::Color4B m_vectorColor;//速度趋势线颜色
    float m_scaleWidth_px;			//刻度线宽度
    float m_scaleGap_px;			//刻度像素间隔
    float m_minAlt;					//高度表最小显示高度
    float m_maxAlt;					//高度表最大显示高度
    float m_altGap;					//高度间隔
    std::string BARO_UNIT_STR[2];
    std::string BARO_REF_STR[3];
    std::map<int, HmsAviPf::CHmsLabel*> m_labelMap;

    float m_minBaro;		//最小气压 单位inHg
    float m_maxBaro;		//最大气压 单位inHg
    float m_selAlt;			//选定高度
    float m_curAlt;			//当前高度
    float m_curBarometric;	//当前气压 单位英寸汞柱
    BARO_UNIT m_curBaroUnit;	//当前使用单位
    BARO_REF m_curBaroRef;	//当前气压参考面

    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pBoxMetricSelAlt;		//米制选择高度框
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pBoxMetricCurAlt;		//米制当前高度框
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pLableMetricSelAlt;	//米制选择高度标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pLableMetricCurAlt;	//米制当前高度标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pScaleBar;			//刻度尺
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pScaleLines;			//刻度尺上的刻度线
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pSelAlt3Num;			//选择高度个位、十位和百位数值标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pSelAlt2Num;			//选择高度千位和万位数值标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pSelAltBug;			//选择高度游标
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pNumberLap;			//数字条
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pPlaceholder;			//万位占位符，高度小于10000时显示
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pPointerHundred;		//指针中百位数值标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pPointerThousand;		//指针中千位和万位数值标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pAltVector;			//高度趋势矢量线
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pFailWin;				//故障窗体

    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_nodeRoot;
    CHmsRollLabel							   *m_pRollLabel; /*滚动文本对象.*/

    void UpdateAltTrend(const float &fAltTrend);//设置高度趋势，6秒后所能达到的高度，单位英尺/分钟
    void UpdateAltitude(const float &fMetricAlt);//设置当前高度，单位英尺
    void ScrollScaleBar(const float &fFeetAlt);//根据新高度滚动刻度尺，单位英尺
    void MoveSelectedAltBug();//移动选定高度游标
    float InHgToHpa(const float &finHg);//英寸汞柱转百帕
    void RefreshBaroDisplay();//刷新气压显示方式

    void RefreshScaleLinePos(bool downToUp);
    //修改刻度标签的位置。从上往下滚动时：底部的标签依次移动到最顶端；从下往上滚动时：顶部标签依次移动到最底部
    //downToUp：是否从下往上滚动，false为从上往下,fCurAlt：当前高度
    void RefreshLabelPos(bool downToUp, const float &fCurAlt);
};

