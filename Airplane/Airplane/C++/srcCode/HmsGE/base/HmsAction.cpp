#include "HmsAction.h"
#include "HmsNode.h"
NS_HMS_BEGIN




CHmsAction::CHmsAction()
: m_pNode(nullptr)
, m_nTag(-1)
, m_bAccomplish(true)
{

}

CHmsAction::~CHmsAction()
{
    if (m_pNode)
    {
        m_pNode->release();
    }
    m_pNode = nullptr;
}

void CHmsAction::SetActionNode(CHmsNode * node)
{
    if (m_pNode)
    {
        m_pNode->release();
    }
    m_pNode = node;
    if (m_pNode)
    {
        m_pNode->retain();
    }
}

void CHmsAction::Update(float fDelta)
{

}

void CHmsAction::RunAction(float fDelta)
{

}

void CHmsAction::Reinit()
{
    m_bAccomplish = false;
}

CHmsActionTimeFargment::CHmsActionTimeFargment()
{
    m_bEntered = false;
    m_bAccomplish = false;
    m_fTimeDuration = 1.0;
    m_fTimeElapsed = 0;
}

void CHmsActionTimeFargment::Reinit()
{
    m_bEntered = false;
    m_fTimeElapsed = 0;
    m_bAccomplish = false;
}

float CHmsActionTimeFargment::GetProgressProportion()
{
    return m_fTimeElapsed / m_fTimeDuration;
}

void CHmsActionTimeFargment::RunAction(float fDelta)
{
    if (!m_bEntered)
    {
        OnActionEnter();
        m_bEntered = true;
    }
    m_fTimeElapsed += fDelta;
    if (m_fTimeElapsed > m_fTimeDuration)
    {
        Update(1.0f);
        m_bAccomplish = true;
        OnActionFinished();
    }
    else
    {
        Update(GetProgressProportion());
    }
}

void CHmsActionTimeFargment::OnActionEnter()
{

}

void CHmsActionTimeFargment::OnActionFinished()
{

}




CHmsActionTimeDelay::CHmsActionTimeDelay()
{
    m_bEntered = false;
    m_bAccomplish = false;
    m_fTimeDuration = 1.0;
    m_fTimeElapsed = 0;
}

void CHmsActionTimeDelay::Reinit()
{
    m_bEntered = false;
    m_fTimeElapsed = 0;
    m_bAccomplish = false;
}

void CHmsActionTimeDelay::RunAction(float fDelta)
{
    if (!m_bEntered)
    {
        OnActionEnter();
        m_bEntered = true;
    }
    m_fTimeElapsed += fDelta;
    if (m_fTimeElapsed > m_fTimeDuration)
    {
        Update(m_fTimeElapsed);
        m_bAccomplish = true;
    }
}

void CHmsActionTimeDelay::OnActionEnter()
{

}

CHmsActionTimeDelay * CHmsActionTimeDelay::Create(float fTimeDuration)
{
    CHmsActionTimeDelay *action = new CHmsActionTimeDelay;
    if (action)
    {
        action->m_fTimeDuration = fTimeDuration;
        action->autorelease();
        return action;
    }
    return nullptr;
}


CHmsActionImmediately::CHmsActionImmediately()
{

}

void CHmsActionImmediately::RunAction(float fDelta)
{
    Update(fDelta);
    m_bAccomplish = true;
}

CHmsActionRepeat * CHmsActionRepeat::Create(int nRepeatTimes, CHmsAction * pAction)
{
    CHmsActionRepeat * ret = new CHmsActionRepeat();
    if (pAction && ret)
    {
        ret->m_nRepeatTimes = nRepeatTimes;
        ret->m_action = pAction;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

CHmsActionRepeat * CHmsActionRepeat::CreateRepeatForever(CHmsAction * pAction)
{
    CHmsActionRepeat * ret = new CHmsActionRepeat();
    if (pAction && ret)
    {
        ret->m_nRepeatTimes = -1;
        ret->m_action = pAction;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

CHmsActionRepeat::CHmsActionRepeat()
{
    m_nRepeatTimes = 1;
    m_bAccomplish = false;
}

void CHmsActionRepeat::RunAction(float fDelta)
{
    if (m_action)
    {
        m_action->RunAction(fDelta);
        if (m_action->IsAccomplish())
        {
            if (m_nRepeatTimes == -1)
            {
                m_action->Reinit();
            }
            else if (m_nRepeatTimes)
            {
                m_action->Reinit();
                m_nRepeatTimes--;
            }
            else
            {
                m_bAccomplish = true;
            }
        }
    }
    else
    {
        m_bAccomplish = true;
    }
}

void CHmsActionRepeat::SetActionNode(CHmsNode * node)
{
    CHmsAction::SetActionNode(node);
    if (m_action)
    {
        m_action->SetActionNode(node);
    }
}


CHmsActionParallel::CHmsActionParallel()
{
    m_bAccomplish = false;
}

CHmsActionParallel * CHmsActionParallel::Create(CHmsAction * action1, ...)
{
    CHmsActionParallel * ret = new CHmsActionParallel();
    if ((nullptr != action1) && ret)
    {
        CHmsAction * tempAction = nullptr;
        va_list params;
        va_start(params, action1);
        while (true)
        {
            tempAction = va_arg(params, CHmsAction*);
            if (tempAction)
            {
                ret->m_actions.pushBack(tempAction);
            }
            else
            {
                break;
            }
        }
        va_end(params);

        ret->autorelease();
        return ret;
    }
    return nullptr;
}

CHmsActionParallel * CHmsActionParallel::Create(const std::vector<CHmsAction*> & vActions)
{
    CHmsActionParallel * ret = new CHmsActionParallel();
    if ((vActions.size() > 0) && ret)
    {
        for (auto c : vActions)
        {
            ret->m_actions.pushBack(c);
        }
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionParallel::RunAction(float fDelta)
{
    bool bTempAccomplish = true;
    for (auto c : m_actions)
    {
        if (c)
        {
            c->RunAction(fDelta);
            if (!c->IsAccomplish())
            {
                bTempAccomplish = false;
            }
        }
    }

    if (bTempAccomplish)
    {
        m_bAccomplish = true;
    }
}

void CHmsActionParallel::Reinit()
{
    for (auto c : m_actions)
    {
        if (c)
        {
            c->Reinit();
        }
    }
}

void CHmsActionParallel::SetActionNode(CHmsNode * node)
{
    CHmsAction::SetActionNode(node);
    for (auto c : m_actions)
    {
        if (c)
        {
            c->SetActionNode(node);
        }
    }
}


CHmsActionQueue::CHmsActionQueue()
{
    m_bAccomplish = false;
    m_nCurIndex = 0;
}

CHmsActionQueue * CHmsActionQueue::Create(CHmsAction * action1, ...)
{
    CHmsActionQueue * ret = new CHmsActionQueue();
    if ((nullptr != action1) && ret)
    {

        ret->m_actions.pushBack(action1);
        CHmsAction * tempAction = nullptr;
        va_list params;
        va_start(params, action1);
        while (true)
        {
            tempAction = va_arg(params, CHmsAction*);
            if (tempAction)
            {
                ret->m_actions.pushBack(tempAction);
            }
            else
            {
                break;
            }
        }
        va_end(params);

        ret->autorelease();
        return ret;
    }
    return nullptr;
}

CHmsActionQueue * CHmsActionQueue::Create(const std::vector<CHmsAction*> & vActions)
{
    CHmsActionQueue * ret = new CHmsActionQueue();
    if ((vActions.size() > 0) && ret)
    {
        for (auto c : vActions)
        {
            ret->m_actions.pushBack(c);
        }
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionQueue::RunAction(float fDelta)
{
    if (m_nCurIndex >= 0 && m_nCurIndex < m_actions.size())
    {
        auto action = m_actions.at(m_nCurIndex);
        if (action)
        {
            action->RunAction(fDelta);
            if (action->IsAccomplish())
            {
                m_nCurIndex++;
            }
        }
        else
        {
            m_nCurIndex++;
        }
    }
    else
    {
        m_bAccomplish = true;
    }
}

void CHmsActionQueue::Reinit()
{
    m_nCurIndex = 0;
    for (auto c : m_actions)
    {
        if (c)
        {
            c->Reinit();
        }
    }
}

void CHmsActionQueue::SetActionNode(CHmsNode * node)
{
    CHmsAction::SetActionNode(node);
    for (auto c : m_actions)
    {
        if (c)
        {
            c->SetActionNode(node);
        }
    }
}

NS_HMS_END