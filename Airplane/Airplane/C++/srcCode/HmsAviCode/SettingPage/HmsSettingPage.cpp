#include "HmsSettingPage.h"
#include "../HmsGlobal.h"
#include "HmsSettingGroup.h"
#include "ui/HmsUiImage2StateButton.h"
#include "HmsSettingSubPage.h"
#include "HmsSettingRecordDataPage.h"
#include "HmsSettingTrackDataPage.h"
#include "HmsSettingTrackDataApplyPage.h"
#include "HmsSettingConnectivityPage.h"
#include "HmsSettingDeviceCalibrate.h"
#include "Language/HmsLanguageMananger.h"
#include <stdlib.h>
#include <stdio.h>

USING_NS_HMS;

CHmsSettingPage * CHmsSettingPage::Create(const Size & size)
{
	CHmsSettingPage *ret = new CHmsSettingPage();
	if (ret && ret->init(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsSettingPage::CHmsSettingPage()
	: HmsUiEventInterface(this)
{
	
}


CHmsSettingPage::~CHmsSettingPage()
{
}

bool CHmsSettingPage::init(const Size & size)
{
	if (!CHmsNode::Init())
	{
		return false;
	} 

	SetContentSize(size);

	m_imageBackground = CHmsStretchImageNode::Create("res/BasicFrame/SolidFrame.png");
	if (m_imageBackground)
	{
		m_imageBackground->SetColor(Color3B::BLACK);
		m_imageBackground->SetContentSize(size);
		m_imageBackground->SetAnchorPoint(Vec2(0, 0));
		this->AddChild(m_imageBackground);
	}

	InitContent();
	return true;
}


void CHmsSettingPage::SetParallelLayer(CHmsParallelLayer * layer)
{
	m_layerParallel = layer;
	if (m_pageMian)
	{
		m_pageMian->SetParallelLayer(layer);
	}

	for (auto c:m_mapSubPage)
	{
		c.second->SetParallelLayer(layer);
	}

	m_layerParallel->AddScreenAction(HMS_CALLBACK_1(CHmsSettingPage::ShowSettingSubPage, this), "settingSubPage");
	m_layerParallel->AddScreenAction(HMS_CALLBACK_1(CHmsSettingPage::SetSvsPfdDisplayMode, this), "menuSyncPfdMode");
	//m_layerParallel->AddScreenAction(HMS_CALLBACK_1(CHmsSettingPage::ShowSettingSubPageByConnectivityInfo, this), "settingConnectivity");
	m_layerParallel->AddScreenAction(HMS_CALLBACK_1(CHmsSettingPage::OnActionSetRecordFlyData, this), "recordFlyData");

	//init the page current setting
	OnHelicopterSelected(m_strCurAirplane, 1);
}

void CHmsSettingPage::InitContent()
{
	auto size = GetContentSize();
	
	m_pageMian = CHmsSettingMainPage::Create(size);
	m_pageMian->SetAnchorPoint(Vec2(0, 0));
	m_pageMian->SetPosition(Vec2(0,0));
	m_pageMian->SetLocalZOrder(10);
	
	do 
	{
		auto subPage = CHmsSettingSubPage::Create(size);
		subPage->SetAnchorPoint(Vec2(0, 0));
		subPage->SetPosition(Vec2(0, 0));

		SettingItemContent	itemContent[] = {
			{ "CHINESE", 1, true },
			{ "ENGLISH", 2, false }
		};
		subPage->SetSubPageTitle("Language Setting");
		subPage->SetContent(itemContent, (sizeof(itemContent) / sizeof(itemContent[0])));
		subPage->SetClickCallback(HMS_CALLBACK_2(CHmsSettingPage::OnLanguageSelected, this));
		m_mapSubPage.insert("language", subPage);
	} while (0);

	do
	{
		auto subPage = CHmsSettingSubPage::Create(size);
		subPage->SetAnchorPoint(Vec2(0, 0));
		subPage->SetPosition(Vec2(0, 0));

#if 0
		SettingItemContent	itemContent[] = {
			{ "xiaosongsu1", 0, true },
			{ "xiaosongsu2", 1, false },
			{ "xiaosongsu3", 2, false }
		};
		subPage->SetContent(itemContent, (sizeof(itemContent) / sizeof(itemContent[0])));
#else
		std::vector<SettingItemContent> vItemContent;
		InitAircraftMode(vItemContent);
		subPage->SetContent(vItemContent);
#endif
		subPage->SetSubPageTitle("Helicopter Setting");
		subPage->SetClickCallback(HMS_CALLBACK_2(CHmsSettingPage::OnHelicopterSelected, this));
		m_mapSubPage.insert("helicopter", subPage);
	} while (0);

	do 
	{
		auto subPage = CHmsSettingSubPage::Create(size);
		subPage->SetAnchorPoint(Vec2(0, 0));
		subPage->SetPosition(Vec2(0, 0));

		SettingItemContent	itemContent[] = {
			{ "10m", 10, false },
			{ "20m", 20, false },
			{ "30m", 30, true },
			{ "40m", 40, false },
			{ "50m", 50, false },
			{ "100m", 100, false },
			{ "150m", 150, false },
			{ "200m", 200, false },
			{ "250m", 250, false },
			{ "500m", 500, false },
			{ "1km", 1000, false },
			//{ "2km", 2000, false }
		};
		subPage->SetContent(itemContent, (sizeof(itemContent) / sizeof(itemContent[0])));
		subPage->SetSubPageTitle("Warning Distance Setting");
		subPage->SetClickCallback(HMS_CALLBACK_2(CHmsSettingPage::OnWarningDistanceSelected, this));
		m_mapSubPage.insert("warnDistance", subPage);		
	} while (0);

	do 
	{
		auto subPage = CHmsSettingSubPage::Create(size);
		subPage->SetAnchorPoint(Vec2(0, 0));
		subPage->SetPosition(Vec2(0, 0));

		SettingItemContent	itemContent[] = {
			{ "3 Colors Alarm", 1, true },
			{ "Multiple Colors Alarm", 2, false }
		};
		subPage->SetSubPageTitle("Colors Alarm Mode Setting");
		subPage->SetContent(itemContent, (sizeof(itemContent) / sizeof(itemContent[0])));
		subPage->SetClickCallback(HMS_CALLBACK_2(CHmsSettingPage::OnColorAlarmModeSelected, this));
		m_mapSubPage.insert("warnMode", subPage);
	} while (0);

	do 
	{
		auto subPage = CHmsSettingRecordDataPage::Create(size);
		subPage->SetAnchorPoint(Vec2(0, 0));
		subPage->SetPosition(Vec2(0, 0));
		subPage->SetSubPageTitle("Flight Data Manage");
		m_mapSubPage.insert("flightDataManage", subPage);
	} while (0);

    do
    {
        auto subPage = CHmsSettingTrackDataPage::Create(size);
        subPage->SetAnchorPoint(Vec2(0, 0));
        subPage->SetPosition(Vec2(0, 0));
        subPage->SetSubPageTitle("Track Data Manage");
        m_mapSubPage.insert("trackDataManage", subPage);
    } while (0);

    do
    {
        auto subPage = CHmsSettingTrackDataApplyPage::Create(size);
        subPage->SetAnchorPoint(Vec2(0, 0));
        subPage->SetPosition(Vec2(0, 0));
        subPage->SetSubPageTitle("Track Data Apply");
        m_mapSubPage.insert("trackDataApply", subPage);
    } while (0);


	do 
	{
		auto subPage = CHmsSettingConnectivityPage::Create(size);
		subPage->SetAnchorPoint(Vec2(0, 0));
		subPage->SetPosition(Vec2(0, 0));
		subPage->SetSubPageTitle("Device connectivity");
		//subPage->SetContentByText("192.168.0.1\nWIFI");
		//subPage->SetClickCallback(HMS_CALLBACK_2(CHmsSettingPage::OnColorAlarmModeSelected, this));
		m_mapSubPage.insert("connectivity", subPage);
	} while (0);

	do 
	{
		auto subPage = CHmsSettingDeviceCalibrate::Create(size);
		subPage->SetAnchorPoint(Vec2(0, 0));
		subPage->SetPosition(Vec2(0, 0));
		subPage->SetSubPageTitle("Device calibrate");
		m_mapSubPage.insert("calibrate", subPage);
	} while (0);

	ShowSettingSubPage("main");
	RegisterTouchEvent(-1);
}

void CHmsSettingPage::OnNextPageBtnEvent(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname, const std::string & subItem)
{
	if (m_layerParallel)
	{
		if (e == HMS_UI_BUTTON_STATE::btn_normal)
		{
			m_layerParallel->RunScreenAction(strname, subItem);
		}
	}
}

void CHmsSettingPage::OnSliderBtnEvent(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname)
{
	if (m_layerParallel)
	{
		if (e == HMS_UI_BUTTON_STATE::btn_normal)
		{
			m_layerParallel->ShowSubScreen(strname, false);
		}
		else if (e == HMS_UI_BUTTON_STATE::btn_selected)
		{
			m_layerParallel->ShowSubScreen(strname, true);
		}
	}
}

void CHmsSettingPage::OnSliderBtnSubEvent(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname,
	const std::string & subNormal, const std::string & subSelected)
{
	if (m_layerParallel)
	{
		if (e == HMS_UI_BUTTON_STATE::btn_normal)
		{
			m_layerParallel->RunScreenAction(strname, subNormal);
		}
		else if (e == HMS_UI_BUTTON_STATE::btn_selected)
		{
			m_layerParallel->RunScreenAction(strname, subSelected);
		}
	}
}

bool CHmsSettingPage::ShowSettingSubPage(std::string strSubPage)
{
	RemoveAllChildInterface();
	if (m_curPage)
		m_curPage->RemoveFromParent();

	auto itFind = m_mapSubPage.find(strSubPage);
	if (itFind != m_mapSubPage.end())
	{
		if (itFind->second)
		{
			m_curPage = itFind->second;	
		}
	}
	else
	{
		m_curPage = m_pageMian;
	}
	AddChild(m_curPage);

	RegisterAllNodeChild();
	return true;
}

bool CHmsSettingPage::SetSvsPfdDisplayMode(std::string strPfdMode)
{
	if (m_pageMian)
	{
		m_pageMian->SetMenuSyncFuncPfdMode(strPfdMode);
	}
	return true;
}

// bool CHmsSettingPage::ShowSettingSubPageByConnectivityInfo(std::string strSubPage)
// {
// 	RemoveAllChildInterface();
// 	if (m_curPage)
// 		m_curPage->RemoveFromParent();
// 
// 	auto itFind = m_mapSubPage.find(strSubPage);
// 	if (itFind != m_mapSubPage.end())
// 	{
// 		if (itFind->second)
// 		{
// 			CHmsSettingConnectivityPage * page = dynamic_cast<CHmsSettingConnectivityPage*>(itFind->second);
// 			if (page)
// 			{
// 				page->SetContentInfo(GetConnectivityInfo());
// 			}
// 			m_curPage = itFind->second;
// 		}
// 	}
// 	else
// 	{
// 		m_curPage = m_pageMian;
// 	}
// 	AddChild(m_curPage);
// 
// 	RegisterAllNodeChild();
// 	return true;
// }

void CHmsSettingPage::OnLanguageSelected(const std::string & strName, int nParam)
{
	auto lauguageMananger = CHmsLanguageMananger::GetInstance();
	lauguageMananger->SetLanguageID(nParam, true);
	lauguageMananger->CheckUpdate();

	if (m_pageMian)
	{
		m_pageMian->SetCurrentDisplayLanguage(strName);
	}
}

void CHmsSettingPage::OnWarningDistanceSelected(const std::string & strName, int nParam)
{
	if (m_layerParallel)
	{
		char strName[100] = { 0 };
        sprintf(strName, "%d", nParam);
		std::string strParam = strName;
		m_layerParallel->RunScreenAction("setWarningDistance", strParam);
	}
}

void CHmsSettingPage::OnColorAlarmModeSelected(const std::string & strName, int nParam)
{
	//if (nParam == 1)
	//{
	//	//3 colors
	//}
	//else if (nParam == 2)
	//{
	//	//multiple colors
	//}

    if (m_layerParallel)
    {
        m_layerParallel->RunScreenAction("setTawsColorTheme", std::to_string(nParam));
    }
}


void CHmsSettingPage::OnHelicopterSelected(const std::string & strName, int nParam)
{
	if (m_layerParallel)
	{
		m_layerParallel->RunScreenAction("setPlane", strName);
	}

	if (m_pageMian)
	{
		m_strCurAirplane = strName;
		m_pageMian->SetCurrentDisplayAirplane(strName);
	}
}
#include "Svs/AircraftModels.h"
void CHmsSettingPage::InitAircraftMode(std::vector<SettingItemContent> & vItemContent)
{
	auto model = aft_models::GetSingleModelInfo();
	if (model)
	{
		bool bFirst = true;
		for (auto c:*model)
		{
			SettingItemContent sic;
			sic.bSelected = bFirst;
			sic.nParam = c._idx;
			sic.strName = c._modelName;
			bFirst = false;
			vItemContent.push_back(sic);

			if (sic.bSelected)
			{
				m_strCurAirplane = sic.strName;
			}
		}
	}
}

void CHmsSettingPage::SetSvsFirstView()
{
	if (m_pageMian)
	{
		m_pageMian->SetSvsFirstView();
	}
}

void CHmsSettingPage::SetSvsThirdView()
{
	if (m_pageMian)
	{
		m_pageMian->SetSvsThirdView();
	}
}

void CHmsSettingPage::SetDisplayHudMode()
{
	if (m_pageMian)
	{
		m_pageMian->SetDisplayHudMode();
	}
}

void CHmsSettingPage::SetDisplayPfdMode()
{
	if (m_pageMian)
	{
		m_pageMian->SetDisplayPfdMode();
	}
}

void CHmsSettingPage::SetRecordDataPower(bool on)
{
	if (m_pageMian)
	{
		m_pageMian->SetRecordDataPower(on);
	}
}
#include "ToolBar/HmsAvionicsDisplayMgr.h"
bool CHmsSettingPage::OnPressedOutside(const Vec2 & posOrigin)
{
	if (posOrigin.y > 0 && posOrigin.x < 0)
	{
		CHmsAvionicsDisplayMgr::GetInstance()->SetBarActionDisplaySettingPage(false);
		return false;
	}
	
	return true;
}

void CHmsSettingPage::RegisterTouchEvent(int nEventOrder)
{
	auto listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = [=](HmsAviPf::Touch * touch, HmsAviPf::CHmsEvent * event)
	{
		if (!IsActive())
		{
			return false;
		}

		auto posOrg = Touch2UiPosition(touch);
		// 		auto posAr = m_node->ConvertOriginPointToAR(posOrg);
		auto rect = GetRectByOriginUI();
		if (rect.containsPoint(posOrg))
		{
			if (this->OnPressed(posOrg))
			{
				m_eUiEventType = HMS_UI_EVENT_TYPE::single_ui_event;
				return true;
			}
		}
		else
			OnPressedOutside(posOrg);
		return false;
	};

	listener->onTouchMoved = [=](HmsAviPf::Touch * touch, HmsAviPf::CHmsEvent * event)
	{
		auto pos = Touch2UiPosition(touch);
		if (m_eUiEventType == HMS_UI_EVENT_TYPE::single_ui_event)
		{
			this->OnMove(pos);
		}
	};

	listener->onTouchEnded = [=](HmsAviPf::Touch * touch, HmsAviPf::CHmsEvent * event)
	{
		auto pos = Touch2UiPosition(touch);

		if (m_eUiEventType == HMS_UI_EVENT_TYPE::single_ui_event)
		{
			if (true)
			{
				this->OnReleased(pos);
			}
			else
			{
				this->OnCancel(pos);
			}
			m_eUiEventType = HMS_UI_EVENT_TYPE::none_ui_event;
		}

	};

	auto m_pDU = HmsAviPf::CHmsAviDisplayUnit::GetInstance();
	auto _eventDispatcher = m_pDU->GetEventDispatcher();

	if (nEventOrder < 0)
	{
		listener->setSwallowTouches(true);
	}
	_eventDispatcher->AddEventListener(listener, this->GetNode(), nEventOrder);
}

bool CHmsSettingPage::OnActionSetRecordFlyData(std::string strOn)
{
	bool bRecordOn = false;
	if (strOn == "on")
	{
		bRecordOn = true;
	}
	SetRecordDataPower(bRecordOn);
	return true;
}
