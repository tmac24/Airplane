#pragma once
#include "../HmsGlobal.h"
#include "HmsGauge.h"

class CHmsAttitude : public CHmsGauge
{
public:
    typedef std::function<void(float)> OnPitchChangedCallback;//俯仰变化回调函数 参数为俯仰角转化为像素
    typedef std::function<void(float)> OnRollChangedCallback;

    CHmsAttitude();
    ~CHmsAttitude();

    CREATE_FUNC(CHmsAttitude)

        virtual bool Init() override;
    virtual void Update(float delta) override;

    void UpdatePitch(const float &fPitch);//设置俯仰角度
    void UpdateRoll(const float &fRoll);//设置滚转角
    void UpdateSkid(const float &fSkid);//设置侧滑

    void SetGaugeMode(GaugeMode flag) override;

    OnPitchChangedCallback OnPitchChanged;
    OnRollChangedCallback OnRollChanged;
private:
    CHmsDataBus *m_dataBus;

    float m_picthScaleWidth;// = 124.0f;
    float m_maxPitch;// = 90.0f;
    float m_pitchGap;// = 2.5f;
    float m_pitchGap_px;// = 12.0f;
    float m_skidBoxWidth;
    float m_maxSkid;// = 15.0f

    float m_curPitch;
    float m_curRoll;
    float m_curSkid;

    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pRollScaleDial;//滚转刻度盘
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pPitchScale;//俯仰刻度尺
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>	 m_pPicthDial;//俯仰表盘
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode> m_pSkidIndicator;//侧滑指示器
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode> m_pRollPointer;//横滚指针
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode> m_pFailWin;

    void RotateNodeOnCenter(HmsAviPf::CHmsNode *target, const float &angle, const HmsAviPf::Vec2 &center, const float &radius);
};

