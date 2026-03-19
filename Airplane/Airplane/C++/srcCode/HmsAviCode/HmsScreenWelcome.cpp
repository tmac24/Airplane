#include "HmsScreenWelcome.h"
#include "HmsGlobal.h"

USING_NS_HMS;

bool CHmsScreenWelcome::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	auto height = CHmsGlobal::GetInstance()->GetDesignHeight();
	auto width = CHmsGlobal::GetInstance()->GetDesignWidth();

#if 0
	auto logoW = CHmsImageNode::Create("res/Logo/hermes_logo.png");
	logoW->SetPosition(width / 2.0f, height / 2.0f);
	logoW->SetScale(2.0f);
#else
	auto logoW = CHmsImageNode::Create("res/Logo/hermes_logo_new.png");
	if (logoW)
	{
		logoW->SetPosition(width / 2.0f, height / 2.0f);
	}
#endif
	if (logoW)
	{
		this->AddChild(logoW);
	}
	m_nUpdateTimes = 0;

	return true;
}

void CHmsScreenWelcome::OnLoad(float delta)
{
	if (m_callbackAfterLoad)
	{
		m_callbackAfterLoad();
	}
}

void CHmsScreenWelcome::OnEnter()
{
	CHmsNode::OnEnter();
	this->SetScheduleUpdate();
}

void CHmsScreenWelcome::OnExit()
{
	CHmsNode::OnExit();
	this->SetUnSchedule();
}

void CHmsScreenWelcome::Update(float delta)
{
	m_nUpdateTimes++;

	if (m_nUpdateTimes == 3)
	{
		this->SetScheduleOnce(HMS_CALLBACK_1(CHmsScreenWelcome::OnLoad, this));
	}
}
