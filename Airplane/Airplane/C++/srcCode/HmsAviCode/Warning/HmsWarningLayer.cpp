#include <HmsGE/base/HmsActionTimeRef.h>
#include "HmsWarningLayer.h"
#include "Audio/HmsAviAudioEngine.h"


CHmsActionRepeat* CreateBlinkAction(float blinkTimePeriod)
{
    auto act1 = CHmsActionToFlash::Create(10.0, 10.0 / blinkTimePeriod);
    auto act2 = CHmsActionRepeat::CreateRepeatForever(act1);
    return act2;
}


CHmsWarningLayer::CHmsWarningLayer()
: HmsUiEventInterface(this)
, m_iYellowWarningThresholdSec(60)
, m_iRedWarningThresholdSec(30)
{

}

CHmsWarningLayer::~CHmsWarningLayer()
{

}

bool CHmsWarningLayer::Init(const HmsAviPf::Size & size)
{
    if (!CHmsResizeableLayer::Init())
    {
        return false;
    }
    SetContentSize(size);

    //注册碰撞信息刷新时的回调
    CollisionManager::getInstance()->registerColliInfoRefreshCallback(
        [this](const CollideInfo& ci){
        m_CurrCollideInfo = ci;
        //auto warningSt = GetCurrAlarmState(m_CurrCollideInfo);
        //WarningStateOnChange(warningSt);//告警声的触发需在主线程
	});

	Vec2 nodeWarningPos = Vec2(size.width* 0.63, size.height - 46 - 30 - 36);

    m_nodeWarningR = CHmsImageNode::Create("res/AftIcon/terrain_r.png");   
	if (m_nodeWarningR)
	{
		m_nodeWarningR->SetAnchorPoint(Vec2(0, 0));
		m_nodeWarningR->SetScale(1.3f * 1.5);
		m_nodeWarningR->SetPosition(nodeWarningPos);
		m_nodeWarningR->SetName("nodeWarningR");
		m_nodeWarningR->SetVisible(false);
		this->AddChild(m_nodeWarningR);
	}

	m_nodeWarningY = CHmsImageNode::Create("res/AftIcon/terrain_y.png");
	if (m_nodeWarningY)
	{
		m_nodeWarningY->SetAnchorPoint(Vec2(0, 0));
		m_nodeWarningY->SetScale(1.3f * 1.5);
		m_nodeWarningY->SetPosition(nodeWarningPos);
		m_nodeWarningY->SetName("nodeWarningY");
		m_nodeWarningY->SetVisible(false);
		this->AddChild(m_nodeWarningY);
	}


	m_nodeWarningbR = CHmsImageNode::Create("res/AftIcon/barrier_r.png");
	if (m_nodeWarningbR)
	{
		m_nodeWarningbR->SetAnchorPoint(Vec2(0, 0));
		m_nodeWarningbR->SetScale(1.3f * 1.5);
		m_nodeWarningbR->SetPosition(nodeWarningPos);
		m_nodeWarningbR->SetName("nodeWarningR");
		m_nodeWarningbR->SetVisible(false);
		this->AddChild(m_nodeWarningbR);
	}


	m_nodeWarningbY = CHmsImageNode::Create("res/AftIcon/barrier_y.png");
	if (m_nodeWarningbY)
	{
		m_nodeWarningbY->SetAnchorPoint(Vec2(0, 0));
		m_nodeWarningbY->SetScale(1.3f * 1.5);
		m_nodeWarningbY->SetPosition(nodeWarningPos);
		m_nodeWarningbY->SetName("nodeWarningY");
		m_nodeWarningbY->SetVisible(false);
		this->AddChild(m_nodeWarningbY);
	}

    m_nodeColliDistLabel = CHmsLabel::createWithTTF("9999999999", "fonts/msyhbd.ttc", 32);
	if (m_nodeColliDistLabel)
	{
		m_nodeColliDistLabel->SetTextColor(Color4B(240, 240, 240, 255));
		m_nodeColliDistLabel->SetAnchorPoint(0, 1);
		m_nodeColliDistLabel->SetPosition(Vec2(size.width* 0.63, size.height - 46 - 30 - 36 - 15));
		this->AddChild(m_nodeColliDistLabel);
		m_nodeColliDistLabel->SetVisible(false);
	}

    return true;
}

void CHmsWarningLayer::WarningStateOnChange(HMS_TAWS_ALARM_STATE currst)
{
    if (m_eAlarmState == currst)
    {
        return;
    }

    m_eAlarmState = currst;

	if (m_nodeWarningR)
	{
		m_nodeWarningR->StopAllActions();
		m_nodeWarningR->SetVisible(false);
	}

	if (m_nodeWarningY)
	{
		m_nodeWarningY->StopAllActions();
		m_nodeWarningY->SetVisible(false);
	}

	if (m_nodeWarningbR)
	{
		m_nodeWarningbR->StopAllActions();
		m_nodeWarningbR->SetVisible(false);
	}

	if (m_nodeWarningbY)
	{
		m_nodeWarningbY->StopAllActions();
		m_nodeWarningbY->SetVisible(false);
	}

    CHmsAviAudioEngine::getInstance()->StopAllSound();

    if (m_eAlarmState == HMS_TAWS_ALARM_STATE::NO_ALARM)
    {
		if (m_nodeColliDistLabel)
		{
			m_nodeColliDistLabel->SetVisible(false);
		}
    }
    else if (m_eAlarmState == HMS_TAWS_ALARM_STATE::YELLOW)
    {
        auto actb = CreateBlinkAction(1);
		if (m_nodeWarningY)
		{
			m_nodeWarningY->RunAction(actb);
		}
		if (m_nodeColliDistLabel)
		{
			m_nodeColliDistLabel->SetVisible(true);
		}

        CHmsAviAudioEngine::getInstance()->PlayEffectByName("terrainTerrain", true);//必须在主线程才能播放声音，否则报错，错误码303
    }
    else if (m_eAlarmState == HMS_TAWS_ALARM_STATE::RED)
    {
        auto actb = CreateBlinkAction(1);
		if (m_nodeWarningR)
		{
			m_nodeWarningR->RunAction(actb);
		}
		if (m_nodeColliDistLabel)
		{
			m_nodeColliDistLabel->SetVisible(true);
		}

        CHmsAviAudioEngine::getInstance()->PlayEffectByName("pullup", true);
    }
    else if (m_eAlarmState == HMS_TAWS_ALARM_STATE::YELLOW_B)
    {
        auto actb = CreateBlinkAction(1);
		if (m_nodeWarningbY)
		{
			m_nodeWarningbY->RunAction(actb);
		}
		if (m_nodeColliDistLabel)
		{
			m_nodeColliDistLabel->SetVisible(true);
		}

        CHmsAviAudioEngine::getInstance()->PlayEffectByName("terrainTerrain", true);//todo: 缺少此项告警声音文件。“障碍物”
    }
    else if (m_eAlarmState == HMS_TAWS_ALARM_STATE::RED_B)
    {
        auto actb = CreateBlinkAction(1);
		if (m_nodeWarningbR)
		{
			m_nodeWarningbR->RunAction(actb);
		}
		if (m_nodeColliDistLabel)
		{
			m_nodeColliDistLabel->SetVisible(true);
		}

        CHmsAviAudioEngine::getInstance()->PlayEffectByName("pullup", true);
    }
}

void CHmsWarningLayer::Update(float delta)
{
    std::string cid = std::to_string((int)HMS_MIN(65535.00, m_CurrCollideInfo.fCollideDistance)) + " M";
	if (m_nodeColliDistLabel)
	{
		m_nodeColliDistLabel->SetString(cid);
	}
    auto warningSt = GetCurrAlarmState(m_CurrCollideInfo);
    WarningStateOnChange(warningSt);
}


void CHmsWarningLayer::OnEnter()
{
    CHmsResizeableLayer::OnEnter();
    SetScheduleUpdate();

    auto warningSt = GetCurrAlarmState(m_CurrCollideInfo);
    WarningStateOnChange(warningSt);
}

void CHmsWarningLayer::OnExit()
{
    CHmsResizeableLayer::OnExit();
    SetUnSchedule();

    auto warningSt = HMS_TAWS_ALARM_STATE::NO_ALARM;
    WarningStateOnChange(warningSt);
}

HMS_TAWS_ALARM_STATE CHmsWarningLayer::GetCurrAlarmState(const CollideInfo& cinfo)
{
    HMS_TAWS_ALARM_STATE ast = HMS_TAWS_ALARM_STATE::INVALID;

    do
    {
        if (!cinfo.bExistCollision)
        {
            ast = HMS_TAWS_ALARM_STATE::NO_ALARM;
            break;
        }

        if (cinfo.bExistCollision
            && cinfo.fCountDownSeconds > m_iRedWarningThresholdSec
            && cinfo.fCountDownSeconds <= m_iYellowWarningThresholdSec)
        {
            if (cinfo.enCollisionTargetType == HMS_COLLISION_TARGET_TYPE::TERRAIN)
            {
                ast = HMS_TAWS_ALARM_STATE::YELLOW;
            }
            else if (cinfo.enCollisionTargetType == HMS_COLLISION_TARGET_TYPE::BARRIER)
            {
                ast = HMS_TAWS_ALARM_STATE::YELLOW_B;
            }
        }
        else if (cinfo.bExistCollision
            && cinfo.fCountDownSeconds <= m_iRedWarningThresholdSec)
        {
            if (cinfo.enCollisionTargetType == HMS_COLLISION_TARGET_TYPE::TERRAIN)
            {
                ast = HMS_TAWS_ALARM_STATE::RED;
            }
            else if (cinfo.enCollisionTargetType == HMS_COLLISION_TARGET_TYPE::BARRIER)
            {
                ast = HMS_TAWS_ALARM_STATE::RED_B;
            }
        }
        else
        {//计算结果为有碰撞，但碰撞时间不在告警阈值范围内。设置为no_alarm
            ast = HMS_TAWS_ALARM_STATE::NO_ALARM;
        }
    } while (0);

    return ast;
}
