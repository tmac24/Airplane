#include "HmsToolBarBottom.h"
#include "../HmsGlobal.h"
#include "../DataInterface/HmsDataBus.h"
#include "HmsToolBarModeBtn.h"
#include "HmsToolBarFuncBtn.h"
#include "HmsAvionicsDisplayMgr.h"

USING_NS_HMS;


CHmsToolBarBottom::CHmsToolBarBottom()
	: HmsUiEventInterface(this)
{
	
}

bool CHmsToolBarBottom::Init()
{
	const float c_fMargin = 8;
	Rect barRect(0, 0, 2560, 90);

	if (!InitWithTexture(nullptr, barRect, false))
	{
		return false;
	}
	this->SetColor(Color3B(0x0E, 0x19, 0x25));

	this->SetAnchorPoint(Vec2(0, 0.0f));

	float fMoveHor = c_fMargin + CHmsToolBarModeBtn::GetDesignBtnSize().width;
	Vec2 pos(c_fMargin + fMoveHor / 2.0f, floorf(barRect.size.height / 2.0f));

	auto createToolbarBtn = [=,&pos](const char * strBtnIcon, const char * strBtnName)
	{
		auto btn = CHmsToolBarModeBtn::CreateWithImage(strBtnIcon, strBtnName);
		if (btn)
		{
			btn->SetToggleBtn(true);
			btn->SetPosition(pos);
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsToolBarBottom::OnToolBarBtnChange, this, strBtnName));
			AddChild(btn);

			pos.x += fMoveHor;
			//mgr->AddBtn(btn);
			//btn->Selected();
			m_mapBtns.insert(strBtnName, btn);
		}
		
		return btn;
	};

	createToolbarBtn("res/ToolBarIcon/aptIcon.png", "Airports");
	createToolbarBtn("res/ToolBarIcon/mapNavIcon.png", "Maps");
	createToolbarBtn("res/ToolBarIcon/processIcon.png", "Charts");
	createToolbarBtn("res/ToolBarIcon/3DViewIcon.png", "3DView");
	createToolbarBtn("res/ToolBarIcon/scratchPadIcon.png", "ScratchPad");

	m_panelDynamicBtns = CHmsUiPanel::Create(Size(CHmsToolBarModeBtn::GetDesignBtnSize().width, CHmsToolBarModeBtn::GetDesignBtnSize().height));
	m_panelDynamicBtns->SetPosition(Vec2(barRect.size.width - c_fMargin, barRect.size.height/2.0f));
	m_panelDynamicBtns->SetAnchorPoint(1.0f, 0.5f);
	this->AddChild(m_panelDynamicBtns);

	InitDynamicBtns();

	this->RegisterAllNodeChild();
	this->RegisterTouchEvent(0);

	return true;
}

void CHmsToolBarBottom::InitDynamicBtns()
{
	auto createDynamicBtn = [=](const char * strBtnIcon, const char * strBtnName, const char * strActionName, const HmsButtonActionCallback & callback)
	{
		auto btn = CHmsToolBarFuncBtn::CreateWithImage(strBtnIcon, strBtnName);
		btn->SetToggleBtn(true);
		btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsToolBarBottom::OnToolBarDynamicBtnChange, this, strActionName));

		ButtonActionInfo * btnInfo = new ButtonActionInfo();
		btnInfo->autorelease();
		btnInfo->node = btn;
		btnInfo->callback = callback;

		m_mapDynamicBtns.insert(strActionName, btnInfo);
		return btnInfo;
	};

	createDynamicBtn("res/ToolBarIcon/settingIcon.png", "Setting", "menubar.setting", HMS_CALLBACK_1(CHmsToolBarBottom::OnBtnSystemSetting, this));
	createDynamicBtn("res/ToolBarIcon/flyplanIcon.png", "FlyPlan", "menubar.flyplan", HMS_CALLBACK_1(CHmsToolBarBottom::OnBtnFlyPlan, this));
	createDynamicBtn("res/ToolBarIcon/dashboardIcon.png", "DashBoard", "menubar.dashboard", HMS_CALLBACK_1(CHmsToolBarBottom::OnBtnDashBoard, this));
//	createDynamicBtn("res/ToolBarIcon/taws2d.png", "2D-TAWS", "menubar.taws2d", HMS_CALLBACK_1(CHmsToolBarBottom::OnBtn2dTaws, this));

}

void CHmsToolBarBottom::OnToolBarBtnChange(HmsAviPf::CHmsUiButtonAbstract*, HmsAviPf::HMS_UI_BUTTON_STATE eState, const char * strBtns)
{
	if (eState == HMS_UI_BUTTON_STATE::btn_selected)
	{
		for (auto c:m_mapBtns)
		{
			if (c.first != strBtns)
			{
				if (c.second->IsSelected())
				{
					c.second->SetState(HMS_UI_BUTTON_STATE::btn_normal);;
				}
			}	
		}

		SetCurrentMode(strBtns);
	}
}

void CHmsToolBarBottom::OnToolBarDynamicBtnChange(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState, const char * strBtns)
{
	bool bSelected = false;
	if (eState == HMS_UI_BUTTON_STATE::btn_selected)
	{
		bSelected = true;
	}
	auto itFind = m_mapDynamicBtns.find(strBtns);
	if (itFind != m_mapDynamicBtns.end())
	{
		auto info = itFind->second;
		if (info && info->callback)
		{
			info->callback(bSelected);
		}
	}
}

void CHmsToolBarBottom::OnBtnSystemSetting(bool bSelected)
{
	CHmsAvionicsDisplayMgr::GetInstance()->ShowSettingPage(bSelected);
}

void CHmsToolBarBottom::OnBtn2dTaws(bool bSelected)
{
    CHmsAvionicsDisplayMgr::GetInstance()->Show2dTawsPage(bSelected);
}

void CHmsToolBarBottom::OnBtnFlyPlan(bool bSelected)
{
    CHmsAvionicsDisplayMgr::GetInstance()->ShowNavFplLayer(bSelected);
}

void CHmsToolBarBottom::OnBtnDashBoard(bool bSelected)
{
	CHmsAvionicsDisplayMgr::GetInstance()->ShowDashBoradBar(bSelected);
}

void CHmsToolBarBottom::SetCurrentMode(const char * strBtns)
{
	if (m_strCurMode == strBtns)
	{
		return;
	}

	m_strCurMode = strBtns;
	auto itFind = m_mapBtns.find(m_strCurMode);
	if (itFind != m_mapBtns.end())
	{
		itFind->second->Selected();
	}

	auto mgr = CHmsAvionicsDisplayMgr::GetInstance();

	if ("Airports" == m_strCurMode)
	{
		mgr->ShowNavType(HmsPageType::PageAirports);
	}
	else if ("Maps" == m_strCurMode)
	{
		mgr->ShowNavType(HmsPageType::PageMaps);
	}
	else if ("Charts" == m_strCurMode)
	{
		mgr->ShowNavType(HmsPageType::PageAviPrograms);
	}
	else if ("ScratchPad" == m_strCurMode)
	{
		mgr->ShowNavType(HmsPageType::PageScratchPad);
	}
	else if ("3DView" == m_strCurMode)
	{
		mgr->ShowNavType(HmsPageType::Page3DView);
	}

	do 
	{
		mgr->SetBarActionDisplaySettingPage(false);
		mgr->SetBarActionDispayFlyPlanBox(false);
		mgr->SetBarActionDisplayDashboradBar(false);
	} while (false);

}

void CHmsToolBarBottom::SetAndShowDynamicBtns(const std::vector<std::string> & vActionNames)
{
	const float c_fMargin = 8;

	m_panelDynamicBtns->RemoveAllChildren();
	m_panelDynamicBtns->RemoveAllChildInterface();

	Size sizePanel = m_panelDynamicBtns->GetContentSize();
	sizePanel.width = c_fMargin;
	Vec2 pos = Vec2(0, 0);

	for (auto c:vActionNames)
	{
		pos.x = sizePanel.width;
		auto itFind = m_mapDynamicBtns.find(c);
		if (itFind != m_mapDynamicBtns.end())
		{
			itFind->second->node->SetPosition(pos);
			m_panelDynamicBtns->AddChild(itFind->second->node);
			
			sizePanel.width += itFind->second->node->GetContentSize().width + c_fMargin;
		}
	}

	sizePanel.width += c_fMargin;
	m_panelDynamicBtns->SetContentSize(sizePanel);
	m_panelDynamicBtns->RegisterAllNodeChild();
}

void CHmsToolBarBottom::SetDynamicBtnState(const std::string & strBtnActionName, bool bOn)
{
	auto itFind = m_mapDynamicBtns.find(strBtnActionName);
	if (itFind != m_mapDynamicBtns.end())
	{
		if (itFind->second)
		{
			CHmsToolBarFuncBtn * btn = dynamic_cast<CHmsToolBarFuncBtn*>(itFind->second->node.get());
			if (btn)
			{
				HMS_UI_BUTTON_STATE eState = (bOn) ? HMS_UI_BUTTON_STATE::btn_selected:HMS_UI_BUTTON_STATE::btn_normal;
				btn->SetState(eState);
			}
		}
	}
}
