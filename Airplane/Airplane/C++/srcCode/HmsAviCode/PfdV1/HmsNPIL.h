#pragma once
#include "../HmsGlobal.h"
#include "display/HmsDrawNode.h"
#include "base/HmsTimer.h"

//PFD Navigation Performance Indications Level水平的
class CHmsNPIL : public HmsAviPf::CHmsNode
{
public:
    CHmsNPIL();
    ~CHmsNPIL();

    CREATE_FUNC(CHmsNPIL);

    virtual bool Init() override;
    //设置指示范围
    void SetRange(float fMin, float fMax);

    //设置偏差
    void SetDeviation(float fDev);

private:
    float m_boxWidth;
    float m_boxHeight;
    float m_scaleGap_px;
    HmsAviPf::Color4B m_boxColor;//背景
    HmsAviPf::Color4B m_scaleColor;//刻度颜色
    HmsAviPf::Color4B m_pointerColor;//指针颜色

    float m_minRange;
    float m_maxRange;
    float m_unitDeviationPx;//所在范围内单位偏差
    float m_curDeviation;//当前偏差

    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>		m_pNodeBox;//背景
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>		m_pNodeScale;//刻度
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>		m_pNodePointer;//指针

    void UpdatePointer();
};

