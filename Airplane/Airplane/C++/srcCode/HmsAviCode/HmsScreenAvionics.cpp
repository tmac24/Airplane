#include "HmsScreenAvionics.h"
#include "HmsGlobal.h"
#include "ToolBar/HmsToolBarTop.h"
#include "ToolBar/HmsToolBarBottom.h"
#include "HmsLog.h"
#include "DataInterface/HmsDataBus.h"
#include "DataInterface/HmsTrackDataServer.h"
USING_NS_HMS;

CHmsScreenAvionics::CHmsScreenAvionics()
	: m_pAviDisplayMgr(nullptr)
	, m_pToolbarTop(nullptr)
	, m_pToolbarBottom(nullptr)
{

}

CHmsScreenAvionics::~CHmsScreenAvionics()
{
	HMS_SAFE_RELEASE(m_pToolbarTop);
	HMS_SAFE_RELEASE(m_pToolbarBottom);
}

bool CHmsScreenAvionics::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	auto height = CHmsGlobal::GetInstance()->GetDesignHeight();
	auto width = CHmsGlobal::GetInstance()->GetDesignWidth();

	HMSLOG("Bar Top And Bottom Create Begin");

	auto topBar = CHmsToolBarTop::Create();
	topBar->SetPosition(0, height);
	this->AddChild(topBar);

	auto bottomBar = CHmsToolBarBottom::Create();
	this->AddChild(bottomBar);

	HMSLOG("Bar Top And Bottom Create Ended!");

	Size conntentSize(width, height - bottomBar->GetContentSize().height - topBar->GetContentSize().height);
	m_aviDisplayLayer = CHmsAvionicsDisplayLayer::Create();
	m_aviDisplayLayer->SetPosition(0, bottomBar->GetContentSize().height);
	m_aviDisplayLayer->SetContentSize(conntentSize);
	m_aviDisplayLayer->EnableUi(true);
	this->AddChild(m_aviDisplayLayer);

	HMSLOG("CHmsAvionicsDisplayLayer Create Ended!");

 	m_pAviDisplayMgr = CHmsAvionicsDisplayMgr::GetInstance();
	m_pAviDisplayMgr->SetDisplayLayer(m_aviDisplayLayer);
	m_pAviDisplayMgr->SetToolBarBottom(bottomBar);
 	m_pAviDisplayMgr->Init(conntentSize);
 	//m_pAviDisplayMgr->ShowNavType(HmsPageType::PageMaps);

	HMSLOG("CHmsAvionicsDisplayMgr Create Ended!");

	bottomBar->SetCurrentMode("Maps");
	m_pAviDisplayMgr->RunActions("menuSyncPfdMode", "pfdv2");

	m_pToolbarTop = topBar;
	m_pToolbarTop->retain();
	m_pToolbarBottom = bottomBar;
	m_pToolbarBottom->retain();

	return true;
}

void CHmsScreenAvionics::OnEnter()
{
	CHmsNode::OnEnter();
	this->SetScheduleUpdate();
}

void CHmsScreenAvionics::OnExit()
{
	CHmsNode::OnExit();
	this->SetUnSchedule();
}
#include "DataInterface/network/HmsNetworks.h"
#include "DataInterface/network/GDL90/HmsGDL90Interface.h"
extern HmsNetworks g_hmsNetwork;
void CHmsScreenAvionics::Update(float delta)
{
    static float s_fTimeRecord = 0;
	CHmsGlobal::GetInstance()->GetDataBus()->UpdateDataBus(delta);
    CHmsGlobal::GetInstance()->GetTrackDataServer()->Update();
    s_fTimeRecord += delta;
    if (s_fTimeRecord > 0.050f)
    {
        auto gdl90Interface = dynamic_cast<CHmsGDL90Interface*>(g_hmsNetwork.getInterface());
        if (gdl90Interface != nullptr)
        {
            gdl90Interface->SendKeepLinkStatusMsg();
        }
        s_fTimeRecord = 0.0f;
    }
}
