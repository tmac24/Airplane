#pragma once

#include "base/HmsAction.h"
#include "math/Vec2.h"
NS_HMS_BEGIN

class CHmsActionMoveTo : public CHmsActionTimeFargment
{
public:
	static CHmsActionMoveTo * Create(float fTimeDuration, const Vec2 & pos);

protected:
	virtual void Update(float fDelta) override;

	virtual void OnActionEnter() override;

	Vec2		m_posBegin;
	Vec2		m_posEnd;
	Vec2		m_posDelta;
};

class CHmsActionMoveToRelative : public CHmsActionTimeFargment
{
public:
	static CHmsActionMoveToRelative * Create(float fTimeDuration, const Vec2 & relativePos);

protected:
	virtual void Update(float fDelta) override;

	virtual void OnActionEnter()  override;

	Vec2		m_posBegin;
	Vec2		m_posDelta;
};

class CHmsActionMoveAsBezier2Relative : public CHmsActionTimeFargment
{
public:
	static CHmsActionMoveAsBezier2Relative * Create(float fTimeDuration, const Vec2 & relativePosCtrl, const Vec2 & relativePosEnd);

protected:
	virtual void Update(float fDelta) override;

	virtual void OnActionEnter()  override;

	Vec2		m_posBegin;
	Vec2		m_posDeltaCtrl;
	Vec2		m_posDeltaEnd;
};

class CHmsActionMoveAsBezier2 : public CHmsActionTimeFargment
{
public:
	static CHmsActionMoveAsBezier2 * Create(float fTimeDuration, const Vec2 & relativePosCtrl, const Vec2 & relativePosEnd);

protected:
	virtual void Update(float fDelta) override;

	virtual void OnActionEnter()  override;

	Vec2		m_posBegin;
	Vec2		m_posCtrl;
	Vec2		m_posEnd;
};

class CHmsActionMoveAsBezier3Relative : public CHmsActionTimeFargment
{
public:
	static CHmsActionMoveAsBezier3Relative * Create(float fTimeDuration, const Vec2 & relativePosCtrl1, const Vec2 & relativePosCtrl2, const Vec2 & relativePosEnd);

protected:
	virtual void Update(float fDelta) override;

	virtual void OnActionEnter()  override;

	Vec2		m_posBegin;
	Vec2		m_posDeltaCtrl1;
	Vec2		m_posDeltaCtrl2;
	Vec2		m_posDeltaEnd;
};

class CHmsActionMoveAsBezier3 : public CHmsActionTimeFargment
{
public:
	static CHmsActionMoveAsBezier3 * Create(float fTimeDuration, const Vec2 & posCtrl1, const Vec2 & posCtrl2, const Vec2 & posEnd);

protected:
	virtual void Update(float fDelta) override;

	virtual void OnActionEnter()  override;

	Vec2		m_posBegin;
	Vec2		m_posCtrl1;
	Vec2		m_posCtrl2;
	Vec2		m_posEnd;
};

class CHmsActionToOpacity : public CHmsActionTimeFargment
{
public:
	static CHmsActionToOpacity * Create(float fTimeDuration, unsigned char opacity);

protected:
	virtual void Update(float fDelta) override;

	virtual void OnActionEnter()  override;

	unsigned char m_bOpacityBegin;
	unsigned char m_bOpacityEnd;
	//char		  m_bOpacityDelta;
	int		  m_bOpacityDelta;
};

class CHmsActionDelayHide : public CHmsActionTimeDelay
{
public:
	static CHmsActionDelayHide * Create(float fTimeDuration);

protected:
	virtual void Update(float fDelta) override;
};

class CHmsActionDelayShow : public CHmsActionTimeDelay
{
public:
	static CHmsActionDelayShow * Create(float fTimeDuration);

protected:
	virtual void Update(float fDelta) override;
};

class CHmsActionDelayRemove : public CHmsActionTimeDelay
{
public:
	static CHmsActionDelayRemove * Create(float fTimeDuration);

protected:
	virtual void Update(float fDelta) override;
};

class CHmsActionDelayRemoveAndCleanUp : public CHmsActionTimeDelay
{
public:
	static CHmsActionDelayRemoveAndCleanUp * Create(float fTimeDuration);

protected:
	virtual void Update(float fDelta) override;
};

class CHmsActionDelayCallback : public CHmsActionTimeDelay
{
public:
	static CHmsActionDelayCallback * Create(float fTimeDuration, const std::function<void()> & callback);

protected:
	virtual void Update(float fDelta) override;

	std::function<void()>		m_func;
};

class CHmsActionDelayCallbackNode : public CHmsActionTimeDelay
{
public:
	static CHmsActionDelayCallbackNode * Create(float fTimeDuration, const std::function<void(CHmsNode*)> & callback);

protected:
	virtual void Update(float fDelta) override;

	std::function<void(CHmsNode*)>		m_func;
};

class CHmsActionToScale : public HmsAviPf::CHmsActionTimeFargment
{
public:
	static CHmsActionToScale * Create(float fTimeDuration, float beginScale, float endScale);

protected:
	virtual void Update(float fDelta) override;

	virtual void OnActionEnter() override;

	float m_fScaleBegin;
	float m_fScaleEnd;
	float m_fScaleDelta;
};

class CHmsActionToScaleY : public HmsAviPf::CHmsActionTimeFargment
{
public:
	static CHmsActionToScaleY * Create(float fTimeDuration, float beginScale, float endScale);

protected:
	virtual void Update(float fDelta) override;

	virtual void OnActionEnter() override;

	float m_fScaleBegin;
	float m_fScaleEnd;
	float m_fScaleDelta;
};

class CHmsActionToFlash : public HmsAviPf::CHmsActionTimeFargment
{
public:
	static CHmsActionToFlash * Create(float fTimeDuration, int flashCount);

protected:
    virtual void Reinit() override;
	virtual void Update(float fDelta) override;
	virtual void OnActionFinished() override;

	float m_fFlashDelta;
	float m_fPeriodTime;
};

NS_HMS_END