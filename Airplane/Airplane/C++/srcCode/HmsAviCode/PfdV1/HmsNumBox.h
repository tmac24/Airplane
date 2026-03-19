#pragma once
#include "../HmsGlobal.h"
#include "base/HmsTimer.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"

class CHmsNumBox : public HmsAviPf::CHmsNode, public HmsAviPf::HmsUiEventInterface
{
public:
    typedef std::function<void(int)> OnValueChangedCallback;
    CHmsNumBox();
    ~CHmsNumBox();

    static CHmsNumBox* Create(float width, float height, float minValue = 0.0f, float maxValue = 500.0f, float valueStep = 10.0f);

    virtual bool init(float width, float height, float minValue, float maxValue, float valueStep);

    virtual void Update(float delta) override;

    virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;

    void SetValueFormat(const std::string format);

    OnValueChangedCallback OnValueChanged;
private:
    float m_boxWidth;
    float m_boxHeight;
    HmsAviPf::Color4B m_boxColor;
    float m_scaleLineWidth;
    float m_scaleGap_px;	//刻度像素间隔
    float m_scaleValueStep;	//刻度值间隔

    float m_curValue;
    float m_scrollIncreament;//滚动增量
    float m_minValue;
    float m_maxValue;
    std::string m_valueFormat;

    std::map<int, HmsAviPf::CHmsLabel*>		m_labelMap;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pCurValueLabel;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pScaleBar;//刻度尺
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pScaleLines;//刻度尺
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pScrollBoxNode;

    CHmsMicroSecondTime					m_timeLast;
    bool								m_isPressed;

    std::string							m_strTitle;

    void printLog(const char * strFormat, ...);

    void ScrollScaleBar(const float &fValue);
    void RefreshScaleLinePos(bool downToUp);
    void RefreshLabelPos(bool downToUp, const float &fCurSpeed);
    float ValueToPosY(const float &fSpeed);
};

