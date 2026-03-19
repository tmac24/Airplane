#include "HmsTerrainMenuBar.h"
#include "../HmsGlobal.h"
#include "ui/HmsUiButton.h"
#include "../HmsScreenMgr.h"
#include "../Nav/HmsNavMenuButton.h"
#include "../HmsNavMgr.h"
#include "../HmsLayerTaws.h"
USING_NS_HMS;

CHmsTerrainMenuBar * CHmsTerrainMenuBar::Create(CHmsScreenLayout *parent, const HmsAviPf::Size & size, float fBorder)
{
	CHmsTerrainMenuBar *ret = new CHmsTerrainMenuBar();
	Rect rect(0, 0, size.width-fBorder*2, size.height-fBorder*2);
	if (ret && ret->Init(parent, rect))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsTerrainMenuBar::CHmsTerrainMenuBar()
	: HmsUiEventInterface(this)
	, m_pLayerTaws(nullptr)
{

}

bool CHmsTerrainMenuBar::Init(CHmsScreenLayout *parent, const HmsAviPf::Rect & rect)
{
	if (!InitWithTexture(nullptr, rect, false))
	{
		return false;
	}
	SetColor(Color3B(14, 25, 37));
	this->SetAnchorPoint(Vec2(0, 0));

	auto mgr = &m_btnMgr;
	mgr->CreateOrSelectGroup(parent);

	float fMoveHor = floor(rect.size.width / 4.0f);
	Vec2 pos(fMoveHor / 2.0f, rect.size.height / 2.0f);

	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/Terrain/IconFan.png", "Arc");
		if (btn)
		{
			btn->SetPosition(pos); pos.x += fMoveHor;
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsTerrainMenuBar::OnBarStateChange, this, TerrainMenuBarBtn::CompassArc));
			btn->SetToggleBtn(true);
			//btn->SetSelected(true);
			AddChild(btn);

			mgr->AddBtn(btn);
		}
	};

	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/Terrain/IconCompassHide.png", "HideCompass");
		if (btn)
		{
			btn->SetPosition(pos); pos.x += fMoveHor;
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsTerrainMenuBar::OnBarStateChange, this, TerrainMenuBarBtn::CompassHide));
			btn->SetToggleBtn(true);
			AddChild(btn);

			mgr->AddBtn(btn);
		}
	};

	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMapNear.png", "Near");
		if (btn)
		{
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsTerrainMenuBar::OnBarStateChange, this, TerrainMenuBarBtn::MapNear));
			btn->SetPosition(pos);
			pos.x += fMoveHor;
			AddChild(btn);

			mgr->AddBtn(btn);
		}
	};
	{
		auto btn = CHmsNavMenuButton::CreateWithImage("res/NavMap/iconMapFar.png", "Far");
		if (btn)
		{
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsTerrainMenuBar::OnBarStateChange, this, TerrainMenuBarBtn::MapFar));
			btn->SetPosition(pos);
			pos.x += fMoveHor;
			AddChild(btn);

			mgr->AddBtn(btn);
		}
	};

	this->RegisterAllNodeChild();
	

	return true;
}

void CHmsTerrainMenuBar::SetBarSize(const Size & size)
{
	SetTextureRect(Rect(Vec2(), size), false, size);
	CHmsImageNode::SetContentSize(size);
}

void CHmsTerrainMenuBar::OnBarStateChange(HmsAviPf::CHmsUiButtonAbstract*, HmsAviPf::HMS_UI_BUTTON_STATE eState, TerrainMenuBarBtn eBtn)
{
	if (eState == HMS_UI_BUTTON_STATE::btn_normal)
	{
		switch (eBtn)
		{
		case TerrainMenuBarBtn::MapFar:
			if (m_pLayerTaws)
			{
				m_pLayerTaws->ZoomOut();
			}
			break;
		case TerrainMenuBarBtn::MapNear:
			if (m_pLayerTaws)
			{
				m_pLayerTaws->ZoomIn();
			}
			break;
		case TerrainMenuBarBtn::CompassHide:
			if (m_pLayerTaws)
			{
				m_pLayerTaws->SetShowCompass(true);
			}
			break;
		case TerrainMenuBarBtn::CompassArc:
			if (m_pLayerTaws)
			{
				m_pLayerTaws->SetMode(HMS_TAWS_MODE::Center);
			}
			break;
		}
	}
	else if (eState == HMS_UI_BUTTON_STATE::btn_selected)
	{
		switch (eBtn)
		{
		case TerrainMenuBarBtn::CompassHide:
			if (m_pLayerTaws)
			{
				m_pLayerTaws->SetShowCompass(false);
			}
			break;
		case TerrainMenuBarBtn::CompassArc:
			if (m_pLayerTaws)
			{
				m_pLayerTaws->SetMode(HMS_TAWS_MODE::Arc);
			}
			break;
		}
	}
}

void CHmsTerrainMenuBar::SwitchMapScr(const std::string & strScr)
{
// 	CHmsMapTileSql &mapTileSql = CHmsMapTileSql::GetInstance();
// 	if (mapTileSql.ChangeMapTypeName(strScr))
// 	{
// 		if (m_navEarthFrame)
// 			m_navEarthFrame->ReloadEarthTile();
// 	}
}




