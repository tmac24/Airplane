#include "HmsNavMenuBar.h"
#include "../HmsGlobal.h"
#include "ui/HmsUiButton.h"
#include "../HmsScreenMgr.h"
#include "HmsNavMenuButton.h"
#include "../HmsNavMgr.h"
USING_NS_HMS;

CHmsNavMenuBar * CHmsNavMenuBar::Create(CHmsNavEarthLayer *parent, const HmsAviPf::Size & size, float fBorder)
{
	CHmsNavMenuBar *ret = new CHmsNavMenuBar();
	Rect rect(0, 0, size.width-fBorder*2, size.height-fBorder*2);
	if (ret && ret->Init(parent, rect))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsNavMenuBar::CHmsNavMenuBar()
	: HmsUiEventInterface(this)
	, m_navEarthFrame(nullptr)
	, m_navInfoWindow(nullptr)
{

}

#include "HmsMercatorEarthTile.h"

bool CHmsNavMenuBar::Init(CHmsNavEarthLayer *parent, const HmsAviPf::Rect & rect)
{
	if (!InitWithTexture(nullptr, rect, false))
	{
		return false;
	}
	SetColor(Color3B(14, 25, 37));
	this->SetAnchorPoint(Vec2(0, 0));

	auto mgr = CHmsNavMgr::GetInstance()->GetBtnMgr();
#if _NAV_SVS_LOGIC_JUDGE
	if (!mgr)
		return false;
#endif
	mgr->CreateOrSelectGroup(parent);

	float fMoveHor = floor(rect.size.width / 8.0f);
	Vec2 pos(fMoveHor / 2.0f, rect.size.height / 2.0f);

	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMapCenter.png", "Center");
#if _NAV_SVS_LOGIC_JUDGE
		if (!btn)
			return false;
#endif
		btn->SetToggleBtn(true);
		btn->SetPosition(pos); 
		pos.x += fMoveHor;
		btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::AftCenter));
		AddChild(btn);
		btn->Selected();

		m_pCenterBtn = btn;

		mgr->AddBtn(btn);
	};
	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconFlyPlan.png", "FPL");
#if _NAV_SVS_LOGIC_JUDGE
		if (!btn)
			return false;
#endif
		btn->SetPosition(pos); 
		pos.x += fMoveHor;
		btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::FPL));
		AddChild(btn);

		mgr->AddBtn(btn);
	};
	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMapDirection.png", "Standard");
#if _NAV_SVS_LOGIC_JUDGE
		if (!btn)
			return false;
#endif
		btn->SetPosition(pos); 
		pos.x += fMoveHor;
		btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::North));
		AddChild(btn);

		mgr->AddBtn(btn);
	};
	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMaps.png", "Maps");
#if _NAV_SVS_LOGIC_JUDGE
		if (!btn)
			return false;
#endif
		btn->SetPosition(pos); 
		pos.x += fMoveHor;
		btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::MapsBar));
		AddChild(btn);

		mgr->AddBtn(btn);
	};
	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconAirport.png", "Airports");
#if _NAV_SVS_LOGIC_JUDGE
		if (!btn)
			return false;
#endif
		btn->SetPosition(pos); 
		pos.x += fMoveHor;
		btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::Airport));
		AddChild(btn);

		mgr->AddBtn(btn);
	};
	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMapNear.png", "Near");
#if _NAV_SVS_LOGIC_JUDGE
		if (!btn)
			return false;
#endif
		btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::Near));
		btn->SetPosition(pos); 
		pos.x += fMoveHor;
		AddChild(btn);

		mgr->AddBtn(btn);
	};
	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMapFar.png", "Far");
#if _NAV_SVS_LOGIC_JUDGE
		if (!btn)
			return false;
#endif
		btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::Far));
		btn->SetPosition(pos); 
		pos.x += fMoveHor;
		AddChild(btn);

		mgr->AddBtn(btn);
	};
	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconScratchPad.png", "ScratchPad");
#if _NAV_SVS_LOGIC_JUDGE
		if (!btn)
			return false;
#endif
		btn->SetToggleBtn(true);
		btn->SetCallbackFunc([=](HmsAviPf::CHmsUiButtonAbstract*, HmsAviPf::HMS_UI_BUTTON_STATE eState)
		{
			if (eState == HMS_UI_BUTTON_STATE::btn_normal)
			{
				//CHmsMercatorEarthTile::SetDrawMode(true);
				if (m_scratchPad)
				{
					m_scratchPad->SetVisible(false);
				}	
			}	
			else
			{
				//CHmsMercatorEarthTile::SetDrawMode(false);
				if (m_scratchPad)
				{
					m_scratchPad->SetVisible(true);
					m_scratchPad->ForceUpdateAllChild();
				}	
			}		
		});
		btn->SetPosition(pos); pos.x += fMoveHor;
		AddChild(btn);

		mgr->AddBtn(btn);
	};

	{
		m_nodeMapsBar = CHmsNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_nodeMapsBar)
			return false;
#endif
		mgr->CreateOrSelectGroup(m_nodeMapsBar);
		Vec2 pos(fMoveHor / 2.0f, rect.size.height / 2.0f);

		{
			auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMaps.png", "Satellite");
#if _NAV_SVS_LOGIC_JUDGE
			if (!btn)
				return false;
#endif
			btn->SetPosition(pos); pos.x += fMoveHor;
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::MapSatellite));
			AddChild(btn);

			mgr->AddBtn(btn);
		};

		{
			auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMaps.png", "VFR");
#if _NAV_SVS_LOGIC_JUDGE
			if (!btn)
				return false;
#endif
			btn->SetPosition(pos); pos.x += fMoveHor;
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::MapVFR));
			AddChild(btn);

			mgr->AddBtn(btn);
		};

		{
			auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMaps.png", "IFR-Low");
#if _NAV_SVS_LOGIC_JUDGE
			if (!btn)
				return false;
#endif
			btn->SetPosition(pos); pos.x += fMoveHor;
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::MapIFRLow));
			AddChild(btn);

			mgr->AddBtn(btn);
		};

		{
			auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMaps.png", "IFR-High");
#if _NAV_SVS_LOGIC_JUDGE
			if (!btn)
				return false;
#endif
			btn->SetPosition(pos); pos.x += fMoveHor;
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::MapIFRHigh));
			AddChild(btn);

			mgr->AddBtn(btn);
		};

		pos.x += fMoveHor;
		pos.x += fMoveHor;
		pos.x += fMoveHor;

		{
			auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconBack.png", "BACK");
#if _NAV_SVS_LOGIC_JUDGE
			if (!btn)
				return false;
#endif
			btn->SetPosition(pos); pos.x += fMoveHor;
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsNavMenuBar::OnBarStateChange, this, NavMenuBarBtn::MapBack));
			AddChild(btn);

			mgr->AddBtn(7, btn);
		};

		mgr->SetShow(false);
		mgr->CreateOrSelectGroup(parent);
	}

	this->RegisterAllNodeChild();
	

	return true;
}

void CHmsNavMenuBar::SetBarSize(const Size & size)
{
	SetTextureRect(Rect(Vec2(), size), false, size);
	CHmsImageNode::SetContentSize(size);
}

void CHmsNavMenuBar::OnBarStateChange(HmsAviPf::CHmsUiButtonAbstract*, HmsAviPf::HMS_UI_BUTTON_STATE eState, NavMenuBarBtn eBtn)
{
	if (eState == HMS_UI_BUTTON_STATE::btn_normal)
	{
		switch (eBtn)
		{
		case NavMenuBarBtn::FPL:
			if (m_navInfoWindow)
			{
				//m_navInfoWindow->ForceUpdate();
				m_navInfoWindow->SetVisible(!m_navInfoWindow->IsVisible());
			}
			break;
		case NavMenuBarBtn::Far:
			if (m_navEarthFrame)
				m_navEarthFrame->MoveFar();
			break;
		case NavMenuBarBtn::Near:
			if (m_navEarthFrame)
				m_navEarthFrame->MoveNear();
			break;
		case NavMenuBarBtn::Airport:
			CHmsNavMgr::GetInstance()->ShowNavType(HmsNavType::NavAirport);
			break;
		case NavMenuBarBtn::AftCenter:
			if (m_navEarthFrame)
			{
				m_navEarthFrame->SetPlaneAlwaysCenter(false);
			}
			break;
		case NavMenuBarBtn::North:
			if (m_navEarthFrame)
			{
				m_navEarthFrame->SetCurLayerToTen();
			}
			break;
		case NavMenuBarBtn::MapsBar:
			{
				auto mgr = CHmsNavMgr::GetInstance()->GetBtnMgr();
#if _NAV_SVS_LOGIC_JUDGE
				if (!mgr)
					return;
#endif
				mgr->SetShow(false);
				mgr->CreateOrSelectGroup(m_nodeMapsBar, true);
			}
			break;
		case NavMenuBarBtn::MapSatellite:
			SwitchMapScr("satellite");
			break;
		case NavMenuBarBtn::MapVFR:
			SwitchMapScr("vfr");
			break;
		case NavMenuBarBtn::MapIFRLow:
			SwitchMapScr("ifr_Low");
			break;
		case NavMenuBarBtn::MapIFRHigh:
			SwitchMapScr("ifr_High");
			break;
		case NavMenuBarBtn::MapBack:
			{
				auto mgr = CHmsNavMgr::GetInstance()->GetBtnMgr();
#if _NAV_SVS_LOGIC_JUDGE
				if (!mgr)
					return;
#endif
				mgr->SetShow(false);
				mgr->CreateOrSelectGroup(this->GetParent(), true);
			}
			break;
		}
	}
	else if (eState == HMS_UI_BUTTON_STATE::btn_selected)
	{
		switch (eBtn)
		{
		case NavMenuBarBtn::AftCenter:
			if (m_navEarthFrame)
			{
				m_navEarthFrame->SetPlaneAlwaysCenter(true);
			}
			break;
		}
	}
}

void CHmsNavMenuBar::SwitchMapScr(const std::string & strScr)
{
	CHmsMapTileSql &mapTileSql = CHmsMapTileSql::GetInstance();
	if (mapTileSql.ChangeMapTypeName(strScr))
	{
		if (m_navEarthFrame)
			m_navEarthFrame->ReloadEarthTile();
	}
}

void CHmsNavMenuBar::Resize(const HmsAviPf::Size & size, int nLevel, float fBorder)
{
	Rect rect(0, 0, size.width - fBorder * 2, size.height - fBorder * 2);
	SetTextureRect(rect);
}

void CHmsNavMenuBar::SetCenterBtnSelect(bool bSelect)
{
	if (m_pCenterBtn)
	{
		if (bSelect && !m_pCenterBtn->IsSelected())
		{
			m_pCenterBtn->Selected();
		}
		if (!bSelect && m_pCenterBtn->IsSelected())
		{
			m_pCenterBtn->Selected();
		}
	}
}




