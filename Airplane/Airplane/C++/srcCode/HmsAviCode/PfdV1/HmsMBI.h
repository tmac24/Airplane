#pragma once
#include "../HmsGlobal.h"

class CHmsTime;

//PFD Marker Beacon Indication 指点信标
class CHmsMBI : public HmsAviPf::CHmsNode
{
public:
    enum BeaconDisplay{ DISPLAY_NONE = 0, FREEZE_OM = 1, FREEZE_MM = 2, FREEZE_IM = 4, FLASH_MM = 3, FLASH_IM = 6 };

    CHmsMBI();
    ~CHmsMBI();

    CREATE_FUNC(CHmsMBI);

    virtual bool Init() override;

    virtual void Update(float delta) override;

    //设置信标有效性 omValid远指点信标  mmValid中指点信标  imValid近指点信标
    void SetBeaconValid(bool omValid, bool mmValid, bool imValid);

private:
    BeaconDisplay m_beaconDisplay;
    double m_lastTime;
    CHmsTime *m_time;

    HmsAviPf::Color4B m_omColor;//远指点信标颜色
    HmsAviPf::Color4B m_mmColor;//中指点信标颜色
    HmsAviPf::Color4B m_imColor;//近指点信标颜色

    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		m_pNodeOM;//远指点信标图标
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		m_pNodeMM;//中指点信标图标
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		m_pNodeIM;//近指点信标图标

    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		m_pNodeFlash;//闪烁的信标

    void StartFlash(HmsAviPf::CHmsImageNode *flashNode);
    void StopFlash();

    bool m_bUpdateSchedule;
};

