#pragma once

#include "base/HmsNode.h"
#include "ui/HmsUiEventInterface.h"
#include "display/HmsLabel.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "display/HmsDrawNode.h"

using namespace HmsAviPf;

class CHmsVSlideBar : public CHmsNode, public HmsUiEventInterface
{
public:
    CHmsVSlideBar();
    virtual ~CHmsVSlideBar();

    CREATE_FUNC_BY_SIZE(CHmsVSlideBar);
    virtual bool Init(const HmsAviPf::Size & size);
    virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;

    void SetValueRange(float min, float max);
    float GetCurValue();
    void SetCurValue(float value);

private:
    void UpdateValueToUI();
    void UpdateValueFromUI();

private:
    RefPtr<CHmsImageNode> m_pImageNode;
	RefPtr<CHmsLabel>	  m_pLabelCurData;
    float m_slideYMin;
    float m_slideYMax;

    float m_minValue;
    float m_maxValue;
    float m_curValue;
};

