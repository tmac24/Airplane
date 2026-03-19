#pragma once
#include "base/HmsNode.h"
#include "HmsGlobal.h"

class CHmsScreenWelcome : public HmsAviPf::CHmsNode
{
public:

	CREATE_FUNC(CHmsScreenWelcome);

	virtual bool Init() override;

	void OnLoad(float delta);

	void SetCallbackAfterLoad(const std::function<void()> & callback)
	{
		m_callbackAfterLoad = callback;
	}

	virtual void OnEnter() override;


	virtual void OnExit() override;


	virtual void Update(float delta) override;

private:
	std::function<void()>	m_callbackAfterLoad;
	int						m_nUpdateTimes;
};

