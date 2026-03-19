#pragma once
#include "ui/HmsUiEventInterface.h"
#include "base/HmsNode.h"
#include "HmsResizeableLayer.h"
#include "HmsLayerTaws.h"

enum class HMS_TAWS_ALARM_STATE
{
    NO_ALARM = 0x00,		//0
    YELLOW = 0x01,			//1
    RED = 0x02,				//2
    YELLOW_B = 0x03,		//0x3
    RED_B = 0x04,			//0x4
    ALARM_DISABLED = 0x05,	//0x5
    INVALID					//0x6
};

class CHmsWarningLayer : public CHmsResizeableLayer, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsWarningLayer();
	~CHmsWarningLayer();

	CREATE_FUNC_BY_SIZE(CHmsWarningLayer);

	virtual bool Init(const HmsAviPf::Size & size);
    void WarningStateOnChange(HMS_TAWS_ALARM_STATE currst);
    virtual void Update(float delta) override;
    typedef std::function<void(HMS_TAWS_ALARM_STATE)> CallbackAlarmOnChange;

    virtual void OnEnter() override;
    virtual void OnExit() override;

private:
    HMS_TAWS_ALARM_STATE GetCurrAlarmState(const CollideInfo& cinfo);
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeWarningR;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeWarningY;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeWarningbR;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_nodeWarningbY;
    HMS_TAWS_ALARM_STATE							m_eAlarmState;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>			m_nodeColliDistLabel;//碰撞距离显示label
    CollideInfo                                     m_CurrCollideInfo;
    std::vector<CallbackAlarmOnChange>              m_AlarmOnChangeCallBacks;
    int												m_iYellowWarningThresholdSec;//预警时间阈值（黄色告警）
    int												m_iRedWarningThresholdSec;//告警时间阈值（红色告警）
};
