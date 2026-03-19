#include "HmsSettingMainPage.h"
#include "HmsAviPf.h"
#include "../HmsGlobal.h"
#include "HmsSettingGroup.h"
#include "ui/HmsUiImage2StateButton.h"
#include "ui/HmsUiLabelButton.h"
#include "ui/HmsUiHotAreaButton.h"
#include "HmsSettingSubPage.h"
#include "DataInterface/HmsDataBus.h"
#include "HmsAviVersion.h"

USING_NS_HMS;

CHmsSettingMainPage * CHmsSettingMainPage::Create(const Size & size)
{
	CHmsSettingMainPage *ret = new CHmsSettingMainPage();
	if (ret && ret->init(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsSettingMainPage::CHmsSettingMainPage()
	: HmsUiEventInterface(this)
	, m_eDeviceStatus(HMS_DEVICE_STATUS::connecting)
	, m_bTawsNeedShowWhenShowPFD(false)
	, m_fCurDeviceVersion(0)
{
	m_fContentBeginY = 100;
	m_cCommunicationType = 'B';
}


CHmsSettingMainPage::~CHmsSettingMainPage()
{
}

#define XXXINFO(x) (x*CHmsGlobal::GetDesignScale())

bool CHmsSettingMainPage::init(const Size & size)
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

	do
	{
		auto posTopLeft = GetUiTopLeft();

		Vec2 pos((40), (-64));
		Size sizeBtn(64, 64);
		m_fContentBeginY = posTopLeft.y + pos.y * 2 + 10;
		pos += posTopLeft;

		auto backBtn = CHmsFixSizeUiImageButton::CreateWithImage("res/Btns/move2Left.png", sizeBtn, [=](CHmsUiButtonAbstract * btn, HMS_UI_BUTTON_STATE e){
			if (e == HMS_UI_BUTTON_STATE::btn_normal)
			{
				if (m_layerParallel)
					m_layerParallel->RunScreenAction("showSetting", "off");
			}
		});
		if (backBtn)
		{
			auto label = CHmsLanguageLabel::CreateLabel("System Setting", 48, true)->GetLabel();
			label->SetAnchorPoint(Vec2(0, 0.5));
			label->SetPosition(sizeBtn.width + 20, sizeBtn.height / 2);
			backBtn->SetAnchorPoint(Vec2(0, 0.5));
			backBtn->AddLabel(label);
			backBtn->SetPosition(pos);
			this->AddChild(backBtn);

			// 		sizeBtn.width += label->GetContentSize().width + 20;
			// 		backBtn->SetContentSize(sizeBtn);
			backBtn->SetScale((1));
		}
		
	} while (0);

	InitContent();

	RegisterAllNodeChild();
	//RegisterTouchEvent(-1);

	SetScheduleUpdate();
	return true;
}


void CHmsSettingMainPage::SetCurrentDisplayLanguage(const std::string & strLanguageName)
{
	if (m_labelLanguageDisplay)
	{
		CHmsLanguageLabel::SetText(m_labelLanguageDisplay, strLanguageName.c_str());
	}
}

void CHmsSettingMainPage::SetCurrentDisplayAirplane(const std::string & strAirplane)
{
	if (m_labelAirplaneDisplay)
	{
		CHmsLanguageLabel::SetText(m_labelAirplaneDisplay, strAirplane.c_str());
	}
}

void CHmsSettingMainPage::SetSvsFirstView()
{
	if (m_btnStatePfd)
	{
		m_btnStatePfd->SetSelected(true);
	}
}

void CHmsSettingMainPage::SetSvsThirdView()
{
	if (m_btnStatePfd)
	{
		m_btnStatePfd->SetSelected(false);
	}
}

void CHmsSettingMainPage::SetDisplayHudMode()
{
	if (m_btnStateHud)
	{
		m_btnStateHud->SetSelected(true);
	}
}

void CHmsSettingMainPage::SetDisplayPfdMode()
{
	if (m_btnStatePfd)
	{
		m_btnStatePfd->SetSelected(true);
	}
}

void CHmsSettingMainPage::SetRecordDataPower(bool bOn)
{
	if (m_btnStateRecord)
	{
		m_btnStateRecord->SetSelected(bOn);
	}
}

void CHmsSettingMainPage::SetMenuSyncFuncPfdMode(const std::string & strPfdMode)
{
	auto itFind =  m_mapBtnPfdGroup.find(strPfdMode);
	if (itFind != m_mapBtnPfdGroup.end())
	{
		itFind->second->SetSelected(true);
	}
}

void CHmsSettingMainPage::Update(float delta)
{
	CHmsNode::Update(delta);

	auto aftData = CHmsGlobal::GetInstance()->GetDataBus()->GetData();

	HMS_DEVICE_STATUS eStatus;
	if (aftData->bDeviceConnected)
	{
		if (aftData->bDeviceIsCharging)
			eStatus = HMS_DEVICE_STATUS::charging;
		else
			eStatus = HMS_DEVICE_STATUS::connected;
	}
	else
	{
		eStatus = HMS_DEVICE_STATUS::connecting;
	}

	SetDeviceStatus(eStatus);

	if (aftData->cCommunicationType != m_cCommunicationType)
	{
		m_cCommunicationType = aftData->cCommunicationType;
		switch (m_cCommunicationType)
		{
		case 'B':
			CHmsLanguageLabel::SetText(m_labelBtnConnectivity, "Bluetooth");
			break;
		case 'W':
		default:
			CHmsLanguageLabel::SetText(m_labelBtnConnectivity, "WIFI");
			break;
		}
	}

	if (aftData->version != m_fCurDeviceVersion)
	{
		m_fCurDeviceVersion = aftData->version;
		UpdateDeviceVersion();
	}
}

#include "HmsGE/HmsConfigXMLReader.h"
#include "Audio/HmsAviAudioEngine.h"

void CHmsSettingMainPage::InitContent()
{
	auto size = GetContentSize();
	auto posPanel = Vec2(0, m_fContentBeginY);

	auto sizeBoder = Size(10, 0);
	auto sizeContentShow = Size(size.width, m_fContentBeginY - sizeBoder.height);
	sizeContentShow = sizeContentShow - (sizeBoder * 2);
	sizeContentShow.height -= 10;

	auto window = CHmsUiVerticalScrollView::Create(sizeContentShow);
	window->SetPosition(posPanel + Vec2(sizeBoder.width, -sizeBoder.height));
	window->SetAnchorPoint(Vec2(0, 1.0f));
	window->SetItemVSpace(10);
	this->AddChild(window);

	// 7 is the highest mode
	int UserMode = CHmsConfigXMLReader::GetSingleConfig()->GetConfigInfo("UserMode", 0);


	auto getSubPageBtn = [=](const CHmsUiImageButton::HmsUiBtnCallback & callback){
		//return CHmsUiImageButton::CreateWithImage("res/Btns/subPage.png", callback);
		auto btn = CHmsUiHotAreaButton::Create(Size( size.width - 32 * 2 , 90), callback);
		btn->AddContentAlignRightCenter(CHmsImageNode::Create("res/Btns/subPage.png"), Vec2(0, 0));
		return btn;
	};

	auto getSliderBtn = [](const CHmsUiImage2StateButton::HmsUiBtnCallback & callback, bool bSelected)
	{
		auto btn =  CHmsUiImage2StateButton::CreateWithImage("res/Btns/unselect.png", "res/Btns/select.png", callback);
		if (btn)
		{
			btn->SetToggleBtn(true);
			btn->SetSelected(bSelected);
		}
		return btn;
	};

	auto CreateGroupTextItem = [=](CHmsSettingGroup * group, const char * strTitle,  const char * strName, const CHmsUiImageButton::HmsUiBtnCallback & callback){
		auto label = CHmsGlobal::CreateLanguageLabel(strName, 38, true);
		auto btn = CHmsUiHotAreaButton::Create(Size(size.width - 32 * 2, 90), callback);
		if (label)
		{
			btn->AddContentAlignRightCenter(label, Vec2(0, 0));
		}
		group->AddItem(strTitle, btn);
		return label;
	};

	do 
	{
		auto group = CHmsSettingGroup::Create("Device Status", sizeContentShow.width);
		m_labelDeviceStatusDisplay = group->AddItem("Connecting...", nullptr);

		m_labelBtnConnectivity = CreateGroupTextItem(group, "Device connectivity", "Bluetooth", HMS_CALLBACK_2(CHmsSettingMainPage::OnNextPageBtnEvent, this, "settingSubPage", "connectivity"));
		m_labelBtnConnectivity->SetTextColor(Color4B::BLACK);

		group->AddItem("Device calibrate", getSubPageBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnNextPageBtnEvent, this, "settingSubPage", "calibrate")));

		group->RegisterAllNodeChild();
		window->AddWindowItem(group);
	
	} while (0);

    do
    {
        auto group = CHmsSettingGroup::Create("Track Manage", sizeContentShow.width);
        
        auto label = CreateGroupTextItem(group, "Track Data Apply", "TrackUse", HMS_CALLBACK_2(CHmsSettingMainPage::OnNextPageBtnEvent, this, "settingSubPage", "trackDataApply"));
        label->SetTextColor(Color4B::BLACK);
        group->RegisterAllNodeChild();
        window->AddWindowItem(group);
    } while (0);

	do
	{
		auto group = CHmsSettingGroup::Create("Display Setting", sizeContentShow.width);
		//group->AddRadioItem("First Person", "Third Person", true, HMS_CALLBACK_1(CHmsSettingMainPage::OnRadioBtnCheck, this, "viewmode", "first", "third"));

		m_btnStatePfd = getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnEventPFD, this, "pfdv1"), false);
		m_mapBtnPfdGroup.insert("pfdv1", m_btnStatePfd);
		group->AddItem("PFD", m_btnStatePfd);

		m_btnStatePfd2 = getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnEventPFD, this, "pfdv2"), false);
		m_mapBtnPfdGroup.insert("pfdv2", m_btnStatePfd2);
		group->AddItem("PFD SIMPLE", m_btnStatePfd2);
		
		m_btnStateHud = getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnEventPFD, this, "hud"), false);
		m_mapBtnPfdGroup.insert("hud", m_btnStateHud);
		group->AddItem("HUD", m_btnStateHud);
		//group->AddItem("Metric Display", getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnSubEvent, this, "metric", "off", "on"), false));
		group->AddItem("PFD compass true track", getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnSubEvent, this, "trueTrack", "off", "on"), false));
		group->AddItem("English Units Display", getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnSubEvent, this, "metric", "off", "on"), false));
		group->AddItem("Attitude ZERO", getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnSubEvent, this, "attitudeZero", "off", "on"), false));
		group->RegisterAllNodeChild();
		window->AddWindowItem(group);
	} while (0);

	do
	{
		auto group = CHmsSettingGroup::Create("TAWS Setting", sizeContentShow.width);
		//group->AddItem("arc compass", getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnSubEvent, this, "setmode", "center", "arc"), false));
		//group->AddRadioItem("CenterMode", "ArcMode", true, HMS_CALLBACK_1(CHmsSettingMainPage::OnRadioBtnCheck, this, "setmode", "center", "arc"));
		group->AddItem("Warning Distance Setting", getSubPageBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnNextPageBtnEvent, this, "settingSubPage", "warnDistance")));
		group->AddItem("Colors Alarm Mode", getSubPageBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnNextPageBtnEvent, this, "settingSubPage", "warnMode")));
		group->AddItem("Warning Sound", getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnEventWarningSound, this), true));
		group->RegisterAllNodeChild();
		window->AddWindowItem(group);
	} while (0);

    do
    {
        auto group = CHmsSettingGroup::Create("Language Display", sizeContentShow.width);
        m_labelLanguageDisplay = group->AddItem("CHINESE", getSubPageBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnNextPageBtnEvent, this, "settingSubPage", "language")));
        group->RegisterAllNodeChild();
        window->AddWindowItem(group);
    } while (0);

	do
	{
		auto group = CHmsSettingGroup::Create("Helicopter Setting", sizeContentShow.width);
		m_labelAirplaneDisplay = group->AddItem("AS350", getSubPageBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnNextPageBtnEvent, this, "settingSubPage", "helicopter")));
		group->RegisterAllNodeChild();
		window->AddWindowItem(group);
	} while (0);

	do
	{
		auto group = CHmsSettingGroup::Create("Data Records", sizeContentShow.width);
		m_btnStateRecord = getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnEventRecordData, this), false);
		group->AddItem("Flight Data", m_btnStateRecord);

		if (UserMode & 4)
		{
			auto label = CreateGroupTextItem(group, "Flight Data Manage", "Manage", HMS_CALLBACK_2(CHmsSettingMainPage::OnNextPageBtnEvent, this, "settingSubPage", "flightDataManage"));
			if (label)
			{
				label->SetTextColor(Color4B::BLACK);
			}
		}

        auto m_btnStateRecordTrack = getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnEventRecordTrackData, this), true);
        group->AddItem("Track Data", m_btnStateRecordTrack);

        auto label = CreateGroupTextItem(group, "Track Data Manage", "Manage", HMS_CALLBACK_2(CHmsSettingMainPage::OnNextPageBtnEvent, this, "settingSubPage", "trackDataManage"));
        label->SetTextColor(Color4B::BLACK);

		group->RegisterAllNodeChild();
		window->AddWindowItem(group);
	} while (0);

	
	if (UserMode == 7)
	{
		auto group = CHmsSettingGroup::Create("Data Debug", sizeContentShow.width);
		group->AddItem("Device Data Display", getSliderBtn(HMS_CALLBACK_2(CHmsSettingMainPage::OnSliderBtnSubEvent, this, "DeviceData", "off", "on"), false));

// 		auto label = CreateGroupTextItem(group, "Test Audio Effect", "Play", [=](CHmsUiButtonAbstract * btn, HMS_UI_BUTTON_STATE e){
// 			if (e == HMS_UI_BUTTON_STATE::btn_selected)
// 			{
// 				CHmsAviAudioEngine::getInstance()->PlayTerrainTerrain();
// 			}
// 		});
// 		label->SetTextColor(Color4B::RED);

		group->RegisterAllNodeChild();
		window->AddWindowItem(group);
	}

	do
	{
		auto group = CHmsSettingGroup::Create("Versions", sizeContentShow.width);
		
		auto btnSoftWare = CHmsUiLabelButton::CreateWithString(GetSoftwareVersion().c_str(), (38), true, nullptr);
		btnSoftWare->setTextColor(Color4B::BLACK);
		group->AddItem("Software Version", btnSoftWare);

		if (UserMode & 1)
		{
			m_labelDeviceVersionDisplay = CreateGroupTextItem(group, "Device Version", GetDeviceVersion().c_str(), nullptr);
			if (m_labelDeviceVersionDisplay)
			{
				m_labelDeviceVersionDisplay->SetTextColor(Color4B::BLACK);;
			}
		}

		group->RegisterAllNodeChild();
		window->AddWindowItem(group);
	} while (0);

	m_nodeContent = window;
}

void CHmsSettingMainPage::OnNextPageBtnEvent(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname, const std::string & subItem)
{
	if (m_layerParallel)
	{
		if (e == HMS_UI_BUTTON_STATE::btn_normal)
		{
			m_layerParallel->RunScreenAction(strname, subItem);
		}
	}
}

void CHmsSettingMainPage::OnSliderBtnEvent(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname)
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

void CHmsSettingMainPage::OnSliderBtnEventPFD(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname)
{
	static std::string strMode = "";

	if (m_layerParallel)
	{
		if (e == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (strMode == strname)
			{
				m_layerParallel->RunScreenAction("svsDisplayMode", "off");
			}
		}
		else if (e == HMS_UI_BUTTON_STATE::btn_selected)
		{
			for (auto c : m_mapBtnPfdGroup)
			{
				auto btnTemp = c.second;
				if (btnTemp)
				{
					if (c.first != strname)
					{
						btnTemp->SetSelected(false);
					}
				}
			}

			strMode = strname;
			m_layerParallel->RunScreenAction("svsDisplayMode", strname);
		}
	}
}

void CHmsSettingMainPage::OnSliderBtnSubEvent(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname,
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

void CHmsSettingMainPage::OnRadioBtnCheck(int nCheckId, const std::string & strname, const std::string & sub1, const std::string & sub2)
{
	if (m_layerParallel)
	{
		if (1 == nCheckId)
		{
			m_layerParallel->RunScreenAction(strname, sub1);
		}
		else
		{
			m_layerParallel->RunScreenAction(strname, sub2);
		}
	}
}
#include "DataInterface/network/GDL90/DataProc.h"
void CHmsSettingMainPage::OnSliderBtnEventRecordData(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e)
{
	bool bUseRecode;
	if (e == HMS_UI_BUTTON_STATE::btn_normal)
	{
		bUseRecode = false;
	}
	else if (e == HMS_UI_BUTTON_STATE::btn_selected)
	{
		bUseRecode = true;
	}
	CDataProc::GetInstance()->GetDataRecord()->SetRecordCmd(bUseRecode);
}
#include "DataInterface/HmsTrackDataServer.h"
void CHmsSettingMainPage::OnSliderBtnEventRecordTrackData(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e)
{
    bool bUseRecode;
    if (e == HMS_UI_BUTTON_STATE::btn_normal)
    {
        bUseRecode = false;
    }
    else if (e == HMS_UI_BUTTON_STATE::btn_selected)
    {
        bUseRecode = true;
    }
    CHmsGlobal::GetInstance()->GetTrackDataServer()->SetRecordCmd(bUseRecode);
}

void CHmsSettingMainPage::OnSliderBtnEventWarningSound(HmsAviPf::CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE e)
{
	bool bWarningSound = false;
	if (e == HMS_UI_BUTTON_STATE::btn_selected)
	{
		bWarningSound = true;
	}

	CHmsAviAudioEngine::getInstance()->SetActive(bWarningSound);
}

void CHmsSettingMainPage::SetDeviceStatus(HMS_DEVICE_STATUS eStatus)
{
	if (eStatus == m_eDeviceStatus)
		return;

	m_eDeviceStatus = eStatus;

	switch (m_eDeviceStatus)
	{
	case HMS_DEVICE_STATUS::connecting:
		CHmsLanguageLabel::SetText(m_labelDeviceStatusDisplay, "Connecting...");
		break;
	case HMS_DEVICE_STATUS::connected:
		CHmsLanguageLabel::SetText(m_labelDeviceStatusDisplay, "Connected");
		break;
	case HMS_DEVICE_STATUS::charging:
		CHmsLanguageLabel::SetText(m_labelDeviceStatusDisplay, "Charging...");
		break;
	default:
		break;
	}
}

std::string CHmsSettingMainPage::GetSoftwareVersion()
{
	std::string strVersion = HMS_FLARM_VERSION;
	return strVersion;
}

std::string CHmsSettingMainPage::GetDeviceVersion()
{
	return "------";
}

void CHmsSettingMainPage::UpdateDeviceVersion()
{
	auto aftData = CHmsGlobal::GetInstance()->GetDataBus()->GetData();
	float fMain = floorf(aftData->version);
	float fSubMain = (aftData->version - fMain) * 10;
	char strTextShow[64] = { 0 };
	sprintf(strTextShow, "%.0f.%.0f(%c%c)", fMain, fSubMain, aftData->cOrgCommunicationType, aftData->cProtocalType);
	if (m_labelDeviceVersionDisplay)
	{
		CHmsLanguageLabel::SetText(m_labelDeviceVersionDisplay, strTextShow);
	}
}

