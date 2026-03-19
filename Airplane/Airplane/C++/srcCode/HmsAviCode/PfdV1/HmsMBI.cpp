#include "HmsMBI.h"
#include "base/HmsTimer.h"

USING_NS_HMS;

CHmsMBI::CHmsMBI()
: m_beaconDisplay(DISPLAY_NONE)
, m_lastTime(0.0)
{
    m_time = new CHmsTime;
    m_bUpdateSchedule = false;
}


CHmsMBI::~CHmsMBI()
{
    if (m_time)
    {
        delete m_time;
        m_time = nullptr;
    }
}

bool CHmsMBI::Init()
{
    if (!CHmsNode::Init())
    {
        return false;
    }

    m_pNodeOM = CHmsImageNode::Create("res/Pfd/MBI/beacon_om.png");
    m_pNodeMM = CHmsImageNode::Create("res/Pfd/MBI/beacon_mm.png");
    m_pNodeIM = CHmsImageNode::Create("res/Pfd/MBI/beacon_im.png");

    m_beaconDisplay = DISPLAY_NONE;
    if (m_pNodeOM)
    {
        m_pNodeOM->SetVisible(false);
        this->AddChild(m_pNodeOM);
    }

    if (m_pNodeMM)
    {
        m_pNodeMM->SetVisible(false);
        this->AddChild(m_pNodeMM);
    }
    if (m_pNodeIM)
    {
        m_pNodeIM->SetVisible(false);
        this->AddChild(m_pNodeIM);
    }

    return true;
}

void CHmsMBI::Update(float delta)
{
    if (m_pNodeFlash == nullptr)
    {
        return;
    }

    auto temp = m_time->GetTime();
    static int flashFlag = 0;
    if (temp - m_lastTime > 0.5)
    {
        if (flashFlag % 2 == 0)
        {
            m_pNodeFlash->SetColor(Color3B(0x51, 0x51, 0x51));
            //m_pNodeFlash->SetVisible(false);
        }
        else
        {
            m_pNodeFlash->SetColor(Color3B::WHITE);
            //m_pNodeFlash->SetVisible(true);
        }
        flashFlag++;
        m_lastTime = temp;
    }
}

void CHmsMBI::SetBeaconValid(bool omValid, bool mmValid, bool imValid)
{
    //将远中近信标转为111二进制排列的数字，判断与当前显示状态是否一致，如果一致，则直接返回
    int display = ((int)imValid) << 2 | ((int)mmValid) << 1 | ((int)omValid);
    display = display == 7 ? 6 : display;
    if (display == m_beaconDisplay)
    {
        return;
    }

    m_beaconDisplay = (BeaconDisplay)display;

    if (display == DISPLAY_NONE || display == FREEZE_OM || display == FREEZE_MM || display == FREEZE_IM)
    {
        StopFlash();
        if (m_pNodeOM)
        {
            m_pNodeOM->SetVisible(m_beaconDisplay == FREEZE_OM);
        }
        if (m_pNodeMM)
        {
            m_pNodeMM->SetVisible(m_beaconDisplay == FREEZE_MM);
        }
        if (m_pNodeIM)
        {
            m_pNodeIM->SetVisible(m_beaconDisplay == FREEZE_IM);
        }
    }
    else if (display == FLASH_MM || display == FLASH_IM)
    {
        if (m_pNodeOM)
        {
            m_pNodeOM->SetVisible(false);
        }
        if (m_pNodeMM)
        {
            m_pNodeMM->SetVisible(false);
        }
        if (m_pNodeIM)
        {
            m_pNodeIM->SetVisible(false);
        }
        if (display == FLASH_MM)
        {
            StartFlash(m_pNodeMM);
        }

        if (display == FLASH_IM)
        {
            StartFlash(m_pNodeIM);
        }
    }

}

void CHmsMBI::StartFlash(CHmsImageNode *flashNode)
{
    if (!flashNode)
    {
        return;
    }
    //如果原来的节点不为空，则将原来节点设置不可见
    if (m_pNodeFlash)
    {
        m_pNodeFlash->SetVisible(false);
    }

    m_pNodeFlash = flashNode;
    m_pNodeFlash->SetVisible(true);

    if (!m_bUpdateSchedule)
    {
        this->SetScheduleUpdate();
        m_bUpdateSchedule = true;
    }
}

void CHmsMBI::StopFlash()
{
    if (m_bUpdateSchedule)
    {
        this->SetUnSchedule();
        m_bUpdateSchedule = false;
    }

    if (m_pNodeFlash)
    {
        m_pNodeFlash->SetColor(Color3B::WHITE);
        m_pNodeFlash->SetVisible(false);
        m_pNodeFlash = nullptr;
    }
}
