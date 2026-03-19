#include "HmsScreenMgr.h"
#include "math/HmsMath.h"
#include "HmsScreenLayer.h"
#include "base/HmsScreen.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsEventListenerKey.h"
#include "base/HmsActionImmediatelyRef.h"
#include "base/HmsActionTimeRef.h"
#include "HmsScreenWelcome.h"

#include "HmsLayerNav.h"
#include <HmsGE/HmsLog.h>

USING_NS_HMS;

CHmsScreenMgr::CHmsScreenMgr()
	: m_pScene(nullptr)
	, m_pSceneWelcome(nullptr)
	, m_pSceneNavInfo(nullptr)
{
	
}


CHmsScreenMgr::~CHmsScreenMgr()
{
	HMS_SAFE_RELEASE(m_pSceneNavInfo);
	HMS_SAFE_RELEASE(m_pSceneWelcome);
	HMS_SAFE_RELEASE(m_pScene);

	HMS_SAFE_RELEASE(m_pScreenAvionics);
}

HmsAviPf::CHmsScreen * CHmsScreenMgr::GetSceneAvionics()
{
	if (nullptr == m_pScene)
	{
		m_pScene = CHmsScreen::Create();
		m_pScene->retain();
        
        // ===== 🔥 在 Scene 上直接画矩形 =====
//               auto draw = CHmsDrawNode::Create();
//               draw->DrawSolidRect(100, 100, 400, 400, Color4F(1, 0, 0, 1));
//               m_pScene->AddChild(draw, 9999);

		auto layer = CHmsScreenAvionics::Create();
		m_pScene->AddChild(layer);

		m_pScreenAvionics = layer;
		m_pScreenAvionics->retain();
	}

	return m_pScene;
}

HmsAviPf::CHmsScreen * CHmsScreenMgr::GetSceneWelcome(const std::function<void()> & callback)
{
	if (nullptr == m_pSceneWelcome)
	{
		m_pSceneWelcome = CHmsScreen::Create();
		m_pSceneWelcome->retain();

		auto layer = CHmsScreenWelcome::Create();

		layer->SetCallbackAfterLoad(callback);
		m_pSceneWelcome->AddChild(layer);
	}
	return m_pSceneWelcome;
}

HmsAviPf::CHmsScreen * CHmsScreenMgr::GetSceneNavInfo()
{
	if (nullptr == m_pSceneNavInfo)
	{
		m_pSceneNavInfo = CHmsScreen::Create();
		m_pSceneNavInfo->retain();

		auto layer = CHmsScreenLayer::Create();
		layer->SetTag(86);
		m_pSceneNavInfo->AddChild(layer);

		auto bg = CHmsImageNode::Create("res/Logo/StartPage/startPageBg.png");
		if (bg)
		{
			layer->AddChild(bg);
		}

		auto logoW = CHmsImageNode::Create("res/Logo/StartPage/startPageLogo.png");
		if (logoW)
		{
			logoW->SetAnchorPoint(Vec2(1.0, 0));
			layer->AddChild(logoW);
		}

	}
	return m_pSceneNavInfo;
}
