#include "HmsActionTimeRef.h"
#include "HmsNode.h"
NS_HMS_BEGIN

CHmsActionMoveTo * CHmsActionMoveTo::Create(float fTimeDuration, const Vec2 & pos)
{
    CHmsActionMoveTo * ret = new CHmsActionMoveTo();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_posEnd = pos;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionMoveTo::Update(float fDelta)
{
    auto pos = m_posBegin + m_posDelta * fDelta;
    if (m_pNode)
    {
        m_pNode->SetPosition(pos);
    }
}

void CHmsActionMoveTo::OnActionEnter()
{
    if (m_pNode)
    {
        m_posBegin = m_pNode->GetPoistion();
        m_posDelta = m_posEnd - m_posBegin;
    }
}




CHmsActionMoveToRelative * CHmsActionMoveToRelative::Create(float fTimeDuration, const Vec2 & relativePos)
{
    CHmsActionMoveToRelative * ret = new CHmsActionMoveToRelative();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_posDelta = relativePos;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionMoveToRelative::Update(float fDelta)
{
    auto pos = m_posBegin + m_posDelta * fDelta;
    if (m_pNode)
    {
        m_pNode->SetPosition(pos);
    }
}

void CHmsActionMoveToRelative::OnActionEnter()
{
    if (m_pNode)
    {
        m_posBegin = m_pNode->GetPoistion();
    }
}


CHmsActionMoveAsBezier2Relative * CHmsActionMoveAsBezier2Relative::Create(float fTimeDuration, const Vec2 & relativePosCtrl, const Vec2 & relativePosEnd)
{
    CHmsActionMoveAsBezier2Relative * ret = new CHmsActionMoveAsBezier2Relative();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_posDeltaCtrl = relativePosCtrl;
        ret->m_posDeltaEnd = relativePosEnd;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionMoveAsBezier2Relative::Update(float fDelta)
{
    double t = fDelta;
    //double exp1 = (1 - t)*(1 - t);
    double exp2 = 2 * t*(1 - t);
    double exp3 = t*t;

    Vec2 posDelta;
    posDelta.x = m_posDeltaCtrl.x*exp2 + m_posDeltaEnd.x*exp3;
    posDelta.y = m_posDeltaCtrl.y*exp2 + m_posDeltaEnd.y*exp3;

    if (m_pNode)
    {
        m_pNode->SetPosition(m_posBegin + posDelta);
    }
}

void CHmsActionMoveAsBezier2Relative::OnActionEnter()
{
    if (m_pNode)
    {
        m_posBegin = m_pNode->GetPoistion();
    }
}


CHmsActionMoveAsBezier2 * CHmsActionMoveAsBezier2::Create(float fTimeDuration, const Vec2 & posCtrl, const Vec2 & posEnd)
{
    CHmsActionMoveAsBezier2 * ret = new CHmsActionMoveAsBezier2();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_posCtrl = posCtrl;
        ret->m_posEnd = posEnd;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionMoveAsBezier2::Update(float fDelta)
{
    double t = fDelta;
    double exp1 = (1 - t)*(1 - t);
    double exp2 = 2 * t*(1 - t);
    double exp3 = t*t;

    Vec2 pos;
    pos.x = m_posBegin.x*exp1 + m_posCtrl.x*exp2 + m_posEnd.x*exp3;
    pos.y = m_posBegin.y*exp1 + m_posCtrl.y*exp2 + m_posEnd.y*exp3;

    if (m_pNode)
    {
        m_pNode->SetPosition(pos);
    }
}

void CHmsActionMoveAsBezier2::OnActionEnter()
{
    if (m_pNode)
    {
        m_posBegin = m_pNode->GetPoistion();
    }
}


CHmsActionMoveAsBezier3Relative * CHmsActionMoveAsBezier3Relative::Create(float fTimeDuration, const Vec2 & relativePosCtrl1, const Vec2 & relativePosCtrl2, const Vec2 & relativePosEnd)
{
    CHmsActionMoveAsBezier3Relative * ret = new CHmsActionMoveAsBezier3Relative();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_posDeltaCtrl1 = relativePosCtrl1;
        ret->m_posDeltaCtrl2 = relativePosCtrl2;
        ret->m_posDeltaEnd = relativePosEnd;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionMoveAsBezier3Relative::Update(float fDelta)
{
    double t = fDelta;
    double tMinus = 1 - t;
    double tMinus2 = tMinus*tMinus;
    double t2 = t*t;
    //double exp1 = tMinus2*tMinus;
    double exp2 = 3 * tMinus2*t;
    double exp3 = 3 * tMinus*t2;
    double exp4 = t2*t;

    Vec2 pos;
    pos.x = m_posDeltaCtrl1.x*exp2 + m_posDeltaCtrl2.x*exp3 + m_posDeltaEnd.x*exp4;
    pos.y = m_posDeltaCtrl1.y*exp2 + m_posDeltaCtrl2.y*exp3 + m_posDeltaEnd.y*exp4;

    if (m_pNode)
    {
        m_pNode->SetPosition(pos + m_posBegin);
    }
}

void CHmsActionMoveAsBezier3Relative::OnActionEnter()
{
    if (m_pNode)
    {
        m_posBegin = m_pNode->GetPoistion();
    }
}


CHmsActionMoveAsBezier3 * CHmsActionMoveAsBezier3::Create(float fTimeDuration, const Vec2 & posCtrl1, const Vec2 & posCtrl2, const Vec2 & posEnd)
{
    CHmsActionMoveAsBezier3 * ret = new CHmsActionMoveAsBezier3();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_posCtrl1 = posCtrl1;
        ret->m_posCtrl2 = posCtrl2;
        ret->m_posEnd = posEnd;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionMoveAsBezier3::Update(float fDelta)
{
    double t = fDelta;
    double tMinus = 1 - t;
    double tMinus2 = tMinus*tMinus;
    double t2 = t*t;
    double exp1 = tMinus2*tMinus;
    double exp2 = 3 * tMinus2*t;
    double exp3 = 3 * tMinus*t2;
    double exp4 = t2*t;

    Vec2 pos;
    pos.x = m_posBegin.x*exp1 + m_posCtrl1.x*exp2 + m_posCtrl2.x*exp3 + m_posEnd.x*exp4;
    pos.y = m_posBegin.y*exp1 + m_posCtrl1.y*exp2 + m_posCtrl2.y*exp3 + m_posEnd.y*exp4;

    if (m_pNode)
    {
        m_pNode->SetPosition(pos);
    }
}

void CHmsActionMoveAsBezier3::OnActionEnter()
{
    if (m_pNode)
    {
        m_posBegin = m_pNode->GetPoistion();
    }
}

CHmsActionToOpacity * CHmsActionToOpacity::Create(float fTimeDuration, unsigned char opacity)
{
    CHmsActionToOpacity * ret = new CHmsActionToOpacity();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_bOpacityEnd = opacity;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionToOpacity::Update(float fDelta)
{
    unsigned char bTempOpacity = (unsigned char)(m_bOpacityBegin + (char)(m_bOpacityDelta * fDelta));
    if (m_pNode)
    {
        m_pNode->SetOpacity(bTempOpacity);
    }
}

void CHmsActionToOpacity::OnActionEnter()
{
    if (m_pNode)
    {
        m_bOpacityBegin = m_pNode->GetOpacity();
        m_bOpacityDelta = m_bOpacityEnd - m_bOpacityBegin;
    }
}


CHmsActionDelayHide * CHmsActionDelayHide::Create(float fTimeDuration)
{
    CHmsActionDelayHide * ret = new CHmsActionDelayHide();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionDelayHide::Update(float fDelta)
{
    if (m_pNode)
    {
        m_pNode->SetVisible(false);
    }
}


CHmsActionDelayShow * CHmsActionDelayShow::Create(float fTimeDuration)
{
    CHmsActionDelayShow * ret = new CHmsActionDelayShow();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionDelayShow::Update(float fDelta)
{
    if (m_pNode)
    {
        m_pNode->SetVisible(true);
    }
}


CHmsActionDelayRemove * CHmsActionDelayRemove::Create(float fTimeDuration)
{
    CHmsActionDelayRemove * ret = new CHmsActionDelayRemove();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionDelayRemove::Update(float fDelta)
{
    if (m_pNode)
    {
        m_pNode->RemoveFromParent();
    }
}


CHmsActionDelayRemoveAndCleanUp * CHmsActionDelayRemoveAndCleanUp::Create(float fTimeDuration)
{
    CHmsActionDelayRemoveAndCleanUp * ret = new CHmsActionDelayRemoveAndCleanUp();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionDelayRemoveAndCleanUp::Update(float fDelta)
{
    if (m_pNode)
    {
        m_pNode->RemoveFromParentAndCleanup(true);
    }
}


CHmsActionDelayCallback * CHmsActionDelayCallback::Create(float fTimeDuration, const std::function<void()> & callback)
{
    CHmsActionDelayCallback * ret = new CHmsActionDelayCallback();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_func = callback;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionDelayCallback::Update(float fDelta)
{
    if (m_func)
    {
        m_func();
    }
}


CHmsActionDelayCallbackNode * CHmsActionDelayCallbackNode::Create(float fTimeDuration, const std::function<void(CHmsNode*)> & callback)
{
    CHmsActionDelayCallbackNode * ret = new CHmsActionDelayCallbackNode();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_func = callback;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionDelayCallbackNode::Update(float fDelta)
{
    if (m_func)
    {
        m_func(m_pNode);
    }
}


CHmsActionToScale * CHmsActionToScale::Create(float fTimeDuration, float beginScale, float endScale)
{
    CHmsActionToScale * ret = new CHmsActionToScale();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_fScaleBegin = beginScale;
        ret->m_fScaleEnd = endScale;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionToScale::Update(float fDelta)
{
    float bTempOpacity = m_fScaleBegin + m_fScaleDelta * fDelta;
    if (m_pNode)
    {
        m_pNode->SetScale(bTempOpacity);
    }
}

void CHmsActionToScale::OnActionEnter()
{
    if (m_pNode)
    {
        m_fScaleDelta = m_fScaleEnd - m_fScaleBegin;
        m_pNode->SetScale(m_fScaleBegin);
    }
}

CHmsActionToScaleY * CHmsActionToScaleY::Create(float fTimeDuration, float beginScale, float endScale)
{
    CHmsActionToScaleY * ret = new CHmsActionToScaleY();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_fScaleBegin = beginScale;
        ret->m_fScaleEnd = endScale;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionToScaleY::Update(float fDelta)
{
    float bTempOpacity = m_fScaleBegin + m_fScaleDelta * fDelta;
    if (m_pNode)
    {
        m_pNode->SetScaleY(bTempOpacity);
    }
}

void CHmsActionToScaleY::OnActionEnter()
{
    if (m_pNode)
    {
        m_fScaleDelta = m_fScaleEnd - m_fScaleBegin;
        m_pNode->SetScaleY(m_fScaleBegin);
    }
}

CHmsActionToFlash * CHmsActionToFlash::Create(float fTimeDuration, int flashCount)
{
    CHmsActionToFlash * ret = new CHmsActionToFlash();
    if (ret)
    {
        ret->m_fTimeDuration = fTimeDuration;
        ret->m_fFlashDelta = fTimeDuration / (flashCount * 2.0f);
        ret->m_fPeriodTime = 0.0;
        ret->autorelease();
        return ret;
    }
    return nullptr;
}

void CHmsActionToFlash::Reinit()
{
    CHmsActionTimeFargment::Reinit();
    m_fPeriodTime = 0.0;
}

void CHmsActionToFlash::Update(float fDelta)
{
    float es = fDelta * m_fTimeDuration - m_fPeriodTime;
    if (m_pNode && (es - m_fFlashDelta) > 0.0001)
    {
        m_pNode->SetVisible(!m_pNode->IsVisible());
        m_fPeriodTime += m_fFlashDelta;
    }
}

void CHmsActionToFlash::OnActionFinished()
{
    if (m_pNode)
    {
        m_pNode->SetVisible(true);
    }
}

NS_HMS_END
