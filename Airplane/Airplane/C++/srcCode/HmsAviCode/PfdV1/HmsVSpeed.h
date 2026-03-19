#pragma once
#include "../HmsGlobal.h"
#include "display/HmsDrawNode.h"
#include "HmsGauge.h"

class CHmsVSpeed : public CHmsGauge
{
public:
    CHmsVSpeed();
    ~CHmsVSpeed();

    CREATE_FUNC(CHmsVSpeed)

        virtual bool Init() override;
    virtual void Update(float delta) override;

    void SetGaugeMode(GaugeMode flag) override;

private:
    float m_boxWidth;
    float m_boxHeight;
    HmsAviPf::Color4B m_boxColor;
    float m_hollowWidth;//凹陷处宽度
    HmsAviPf::Vec2 m_pointerCenterPos;
    float m_pointerStartX;
    float m_pointerEndX;

    float m_gapPx0_1;// = 48.0f;
    float m_gapPx1_2;// = 32.0f;
    float m_gapPx2_6;// = 24.0f;

    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode> m_pVSpeedPointer;//垂直速度指针
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	 m_pVSpeedLabel;	//垂直速度显示标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode> m_pFailWin;	   //故障窗体

    void refreshPointer(const float &fVSpeed);//根据垂直速度刷新指针
    void UpdateVSpeed(const float &fVSpeed);//单位英尺/分钟
};

