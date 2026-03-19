#pragma once

#include "base/HmsAction.h"
#include "math/Vec2.h"
NS_HMS_BEGIN

class CHmsActionHide : public CHmsActionImmediately
{
public:
	static CHmsActionHide * Create();

protected:
	virtual void Update(float fDelta) override;
};

class CHmsActionShow : public CHmsActionImmediately
{
public:
	static CHmsActionShow * Create();

protected:
	virtual void Update(float fDelta) override;
};

class CHmsActionRemove : public CHmsActionImmediately
{
public:
	static CHmsActionRemove * Create();

protected:
	virtual void Update(float fDelta) override;
};

class CHmsActionRemoveAndCleanUp : public CHmsActionImmediately
{
public:
	static CHmsActionRemoveAndCleanUp * Create();

protected:
	virtual void Update(float fDelta) override;
};

class CHmsActionCallback : public CHmsActionImmediately
{
public:
	static CHmsActionCallback * Create( const std::function<void()> & callback);

protected:
	virtual void Update(float fDelta) override;

	std::function<void()>		m_func;
};

class CHmsActionCallbackNode : public CHmsActionImmediately
{
public:
	static CHmsActionCallbackNode * Create( const std::function<void(CHmsNode*)> & callback);

protected:
	virtual void Update(float fDelta) override;

	std::function<void(CHmsNode*)>		m_func;
};

NS_HMS_END