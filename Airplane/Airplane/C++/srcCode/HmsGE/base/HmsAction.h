#pragma once

#include "base/Ref.h"
#include "base/RefPtr.h"
#include "base/HmsVector.h"
NS_HMS_BEGIN

class CHmsNode;


/**
*the base of the action that action use
*param
*returns
*by [12/14/2017 song.zhang]
*/

class CHmsAction : public Ref
{
public:
    CHmsAction();
    virtual ~CHmsAction();

    CHmsNode * GetActionNode(){ return m_pNode; }
    virtual void SetActionNode(CHmsNode * node);

    void SetTag(int nTag){ m_nTag = nTag; }
    int GetTag(){ return m_nTag; }

    virtual void RunAction(float fDelta);
    virtual void Update(float fDelta);
    virtual void Reinit();

    //whether the action is accomplish
    bool IsAccomplish(){ return m_bAccomplish; }

protected:
    CHmsNode		*	m_pNode;			//the node of the action
    int					m_nTag;				//the tag id of the 
    bool				m_bAccomplish;
};

class CHmsActionTimeFargment : public CHmsAction
{
public:
    CHmsActionTimeFargment();

    void Reinit() override;

    float GetTimeElapsed(){ return m_fTimeElapsed; }

    /**
    *Get the proportion of the progress which is [0,1]
    *And the proportion as the input of the Update
    *param
    *returns
    *by [12/15/2017 song.zhang]
    */
    float GetProgressProportion();

    virtual void RunAction(float fDelta) override;
    virtual void OnActionEnter();
    virtual void OnActionFinished();

protected:
    float				m_fTimeDuration;
    float				m_fTimeElapsed;
    bool				m_bEntered;
};

class CHmsActionTimeDelay : public CHmsAction
{
public:
    CHmsActionTimeDelay();

    static CHmsActionTimeDelay * Create(float fTimeDuration);

    void Reinit() override;

protected:
    virtual void RunAction(float fDelta) override;
    virtual void OnActionEnter();

protected:
    float				m_fTimeDuration;
    float				m_fTimeElapsed;
    bool				m_bEntered;
};

class CHmsActionImmediately : public CHmsAction
{
public:
    CHmsActionImmediately();

protected:
    virtual void RunAction(float fDelta) override;
};

class CHmsActionRepeat : public CHmsAction
{
public:
    CHmsActionRepeat();

    static CHmsActionRepeat * Create(int nRepeatTimes, CHmsAction * pAction);
    static CHmsActionRepeat * CreateRepeatForever(CHmsAction * pAction);

protected:
    virtual void RunAction(float fDelta) override;
    virtual void SetActionNode(CHmsNode * node) override;

protected:
    int							m_nRepeatTimes;			//the times of the repeat
    RefPtr<CHmsAction>			m_action;
};

//����
class CHmsActionParallel : public CHmsAction
{
public:
    CHmsActionParallel();

    static CHmsActionParallel * Create(CHmsAction * action1, ...);
    static CHmsActionParallel * Create(const std::vector<CHmsAction*> & vActions);

protected:
    virtual void RunAction(float fDelta) override;
    virtual void SetActionNode(CHmsNode * node) override;
    virtual void Reinit() override;

protected:
    Vector<CHmsAction*>			m_actions;
};

//����
class CHmsActionQueue : public CHmsAction
{
public:
    CHmsActionQueue();

    static CHmsActionQueue * Create(CHmsAction * action1, ...);
    static CHmsActionQueue * Create(const std::vector<CHmsAction*> & vActions);

protected:
    virtual void RunAction(float fDelta) override;
    virtual void SetActionNode(CHmsNode * node) override;
    virtual void Reinit() override;

protected:
    Vector<CHmsAction*>			m_actions;
    int							m_nCurIndex;
};

NS_HMS_END