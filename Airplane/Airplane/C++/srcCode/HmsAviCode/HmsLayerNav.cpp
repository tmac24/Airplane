#include "HmsGlobal.h"
#include "HmsLayerNav.h"
#include "Nav/HmsNavMenuBar.h"
#include "Nav/HmsNavEarthFrame.h"
#include "HmsNavMgr.h"
USING_NS_HMS;

CHmsLayerNav::CHmsLayerNav()
	: HmsUiEventInterface(this)
{
}


CHmsLayerNav::~CHmsLayerNav()
{
}

bool CHmsLayerNav::Init()
{
	InitLayoutType(Hms_Screen_Layout_Type::Layout_Full);

	if (!CHmsScreenLayout::Init())
	{
		return false;
	}
	
	CreateOrSetPageLabel("NAV");

	auto navMgr = CHmsNavMgr::GetInstance();
	navMgr->SetNavLayer(this);
	navMgr->Init(GetSize());
	navMgr->ShowNavType(HmsNavType::NavMap);

	//HmsUiEventInterface::RegisterTouchEvent();

	HmsUiEventInterface::RegisterMutiTouchEvent(-1);
	//Test();
	//return true;

	return true;
}

void CHmsLayerNav::ResetViewer()
{
	auto eType = GetLayoutType();
	switch (eType)
	{
	case Hms_Screen_Layout_Type::Layout_Normal:
		break;
	case Hms_Screen_Layout_Type::Layout_Middle:
		break;
	case Hms_Screen_Layout_Type::Layout_Large:
		break;
	}
	CHmsScreenLayout::ResetViewer();
}

void CHmsLayerNav::OnScreenLayoutChanged(Hms_Screen_Layout_Type eOld, Hms_Screen_Layout_Type eNew)
{
	if (eNew != eOld)
	{
		auto size = GetSize();
		auto navMgr = CHmsNavMgr::GetInstance();
		navMgr->Resize(size);
		navMgr->ShowNavType(HmsNavType::NavMap);
	}
}

void CHmsLayerNav::Update(float delta)
{

}

void CHmsLayerNav::OnEnter()
{
	CHmsScreenLayout::OnEnter();

	SetScheduleUpdate();
}

void CHmsLayerNav::OnExit()
{
	CHmsScreenLayout::OnExit();

	this->SetUnSchedule();
}

void CHmsLayerNav::OnLayoutBtnChange(int nLayoutBtnIndex, int nBtnState)
{
	auto mgr = CHmsNavMgr::GetInstance();
	mgr->GetBtnMgr()->OnBtnChange(nLayoutBtnIndex, nBtnState);
}
