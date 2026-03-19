#include "HmsSettingConnectivityPage.h"
#include "ui/HmsUiImageButton.h"
#include "ui/HmsUiRadio.h"
#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiImagePanel.h"
#include "ui/HmsUiLabelButton.h"
#include "Language/HmsLanguageLabel.h"
#include "HmsLog.h"
#include "HmsGlobal.h"
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include "HmsAndroidFunc.h"
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX || HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#else
#include "io.h"
#endif
#include "DataInterface/HmsDataBus.h"


USING_NS_HMS;

CHmsSettingConnectivityPage::CHmsSettingConnectivityPage()
	: m_nActionId(0)
	, m_eConnectivityType(ConnectivityType::NoConnectivity)
{

}

CHmsSettingConnectivityPage::~CHmsSettingConnectivityPage()
{

}

CHmsSettingConnectivityPage * CHmsSettingConnectivityPage::Create(const HmsAviPf::Size & size)
{
	CHmsSettingConnectivityPage *ret = new CHmsSettingConnectivityPage();
	if (ret && ret->init(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

#define XXXINFO(x) (x*CHmsGlobal::GetDesignScale())

bool CHmsSettingConnectivityPage::init(const HmsAviPf::Size & size)
{
	if (!CHmsSettingSubPage::init(size))
	{
		return false;
	}

	float m_fContentBeginY = size.height - 100;
	float fContentBoder = 10;

	do 
	{
		auto labelContent = CHmsLanguageLabel::CreateLabel("Nothing", (38), true)->GetLabel();
		labelContent->SetTextColor(Color4B::BLACK);
		labelContent->SetAnchorPoint(Vec2(0.0, 1.0f));

		float fHeight = m_fContentBeginY;
		float fHeightLow = fHeight - floorf(size.height*0.44);
		
		float fContentWidth = size.width - 2 * fContentBoder;

		labelContent->SetPosition(fContentBoder + (64), fHeight - fContentBoder);
		this->AddChild(labelContent);
		m_labelContent = labelContent;

		m_nodeDraw->DrawSolidRect(fContentBoder, fHeight, fContentBoder + fContentWidth, fHeightLow, Color4F(0xC9 / 255.0f, 0xC9 / 255.0f, 0xC9 / 255.0f, 1.0f));

		m_fContentBeginY = fHeightLow - fContentBoder*2;
	} while (0);

	do 
	{
		auto label = CHmsLanguageLabel::CreateLabel("History Connectivity", 38, true)->GetLabel();
		label->SetAnchorPoint(Vec2(0, 1.0));
		label->SetPosition(fContentBoder + 20, m_fContentBeginY);
		this->AddChild(label);

		m_fContentBeginY -= label->GetContentSize().height + fContentBoder;
	} while (0);

	do 
	{
		auto posPanel = Vec2(0, m_fContentBeginY);

		auto sizeBoder = Size(10, 0);
		auto sizeContentShow = Size(size.width, m_fContentBeginY - 20);
		sizeContentShow = sizeContentShow - (sizeBoder * 2);

		auto window = CHmsUiVerticalScrollView::Create(sizeContentShow);
		window->SetPosition(posPanel + Vec2(sizeBoder.width, -sizeBoder.height));
		window->SetAnchorPoint(Vec2(0, 1.0f));
		window->SetItemVSpace(10);
		this->AddChild(window);

		m_viewWindow = window;
	} while (0);

	InitComfirmDlg();

	RegisterAllNodeChild();
	return true;
}

void CHmsSettingConnectivityPage::InitComfirmDlg()
{
	Size bgSize(GetContentSize().width - 100, 280);

	auto panel = CHmsUiStretchImagePanel::CreateWithImage("res/BasicFrame/Round4SolidWhite.png", bgSize);
	panel->SetColor(Color3B(0xDE, 0xDE, 0xDE));
	panel->SetPosition(GetContentSize() / 2.0f);
	panel->SetLocalZOrder(10);
	this->AddChild(panel);

	Size btnSize(220, 90);

	m_labelConfirmTitle = CHmsGlobal::CreateLanguageLabel("Is nothing!", 40, Color4F::BLACK, true);
	m_labelConfirmTitle->SetAnchorPoint(0.5f, 0.5f);
	m_labelConfirmTitle->SetPosition(bgSize.width / 2.0f, 20 + 90 + (bgSize.height - 20 - 90) * 0.5f);
	panel->AddChild(m_labelConfirmTitle);

	do
	{
		auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
		btn->SetAnchorPoint(Vec2(1.0, 0));
		btn->SetPosition(Vec2(bgSize.width / 2.0 - 10, 20));
		btn->AddText("Cancel", 38, Color4B::WHITE, true);
		btn->SetOnClickedFunc(
			[=](CHmsUiButtonAbstract *btn){
			OnClickConfirmDlgCancel();
		});

		panel->AddChild(btn);
		panel->AddChildInterface(btn);
	} while (0);

	do
	{
		auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
		btn->SetAnchorPoint(Vec2(0, 0));
		btn->SetPosition(Vec2(bgSize.width / 2.0 + 10, 20));
		btn->AddText("Confirm", 38, Color4B::WHITE, true);
		btn->SetOnClickedFunc(
			[=](CHmsUiButtonAbstract *btn){
			OnClickConfirmDlgConfirm();
		});

		panel->AddChild(btn);
		panel->AddChildInterface(btn);
	} while (0);

	this->AddChild(panel);
	this->AddChildInterface(panel);

	
	m_nodeConfirmDlg = panel;
	m_nodeConfirmDlg->SetVisible(false);
}



void CHmsSettingConnectivityPage::SetContentCheckBox(const std::vector<ConnectivityItemInfo> & vItemContent)
{
	float fContentBoder = 10;
	float fPageWidth = GetContentSize().width;
	float fContentWidth = fPageWidth - 2 * fContentBoder;
	Size sizeItem(fContentWidth, 90);

	this->RemoveAllContentNode();

	auto getSelectedBtn = [=](const ConnectivityItemInfo & item)
	{
		auto btn = CHmsUiLabelButton::CreateWithString("Reconnect", (38), true, HMS_CALLBACK_2(CHmsSettingConnectivityPage::OnBtnClick, this, item.strName, item.nParam));
		btn->setTextColor(Color4B::BLACK);
		return btn;
	};

	for (auto c:vItemContent)
	{
		auto panel = CHmsUiStretchImagePanel::CreateWithImage("res/BasicFrame/SolidFrame.png", sizeItem);
		panel->SetColor(Color3B(0xC9, 0xC9, 0xC9));
		panel->SetAnchorPoint(Vec2(0, 1.0));

		std::string strTempName = c.strName;
		auto pos1 = c.strName.find_first_of("\"");
		auto pos2 = c.strName.find_last_of("\"");
		if ((pos2 != pos1) && (pos1 != std::string::npos) && (pos2 != std::string::npos))
		{
			strTempName = c.strName.substr(pos1 + 1, pos2 - pos1 - 1);
		}

		auto labelContent = CHmsLanguageLabel::CreateLabel(strTempName.c_str(), (38), true)->GetLabel();
		labelContent->SetTextColor(Color4B::BLACK);
		labelContent->SetAnchorPoint(Vec2(0, 0.5f));
		labelContent->SetTag(88);
		labelContent->SetPosition(64, sizeItem.height*0.5f);
		panel->AddChild(labelContent);

		auto btn = getSelectedBtn(c);
		btn->SetAnchorPoint(Vec2(1.0f, 0.5f));
		btn->SetPosition(fContentWidth - 32, sizeItem.height*0.5f);
		panel->AddChild(btn);

		panel->RegisterAllNodeChild();
		m_viewWindow->AddWindowItem(panel);
	}
}

void CHmsSettingConnectivityPage::OnEnter()
{
	CHmsSettingSubPage::OnEnter();

	SetScheduleUpdate();
	UpdateItem();
}


void CHmsSettingConnectivityPage::OnExit()
{
	SetUnSchedule();
}

void CHmsSettingConnectivityPage::AddContentNode(HmsAviPf::CHmsNode * node)
{
	if (node)
	{
		m_vContectNode.pushBack(node);
		m_viewWindow->AddWindowItem(node);
	}
}

void CHmsSettingConnectivityPage::RemoveAllContentNode()
{
	m_viewWindow->RemoveAllItem();
	CHmsSettingSubPage::RemoveAllContentNode();
}

void CHmsSettingConnectivityPage::SetContentInfo(const std::string & strContent)
{
	CHmsLanguageLabel::SetText(m_labelContent, strContent.c_str());
}

void CHmsSettingConnectivityPage::Update(float delta)
{
	if (m_timeLastUpdate.GetElapsed() > 1.5)
	{
		SetContentInfo(GetConnectivityInfo());
		m_timeLastUpdate.RecordCurrentTime();
	}	
}

void CHmsSettingConnectivityPage::UpdateItem()
{
	m_vItemInfo.clear();

	SetContentInfo(GetConnectivityInfo());

	GetRecordItemInfo(m_vItemInfo);
	SetContentCheckBox(m_vItemInfo);
	m_timeLastUpdate.RecordCurrentTime();
}

bool CHmsSettingConnectivityPage::GetRecordItemInfo(std::vector<ConnectivityItemInfo> & vItemInfo)
{
	ConnectivityItemInfo info;
	info.nParam = 0;

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID

	if (m_eConnectivityType == ConnectivityType::Bluetooth)
	{

	}
	else if (m_eConnectivityType == ConnectivityType::Wifi)
	{
		std::vector<std::string> vItem;
		GetAndroidWifiHistroyList(vItem);
		for (auto c:vItem)
		{
			info.strName = c;
			info.nParam++;
			vItemInfo.push_back(info);
		}
	}

#else
	info.strName = "Test Wifi1";
	vItemInfo.push_back(info);

	info.strName = "Test Wifi2";
	info.nParam++;
	vItemInfo.push_back(info);
#endif
	
	return true;
}


std::string CHmsSettingConnectivityPage::GetConnectivityInfo()
{
	auto aft = CHmsGlobal::GetInstance()->GetDataBus()->GetData();

	std::string strOut;
#if HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32
	strOut = "This is PC mode!";
#elif HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID
	switch (aft->cCommunicationType)
	{
	case 'B':
		strOut = GetAndroidBluetoothStutus();
		m_eConnectivityType = ConnectivityType::Bluetooth;
		break;
	case 'W':
	default:
		strOut = GetAndroidWifiStutus();
		m_eConnectivityType = ConnectivityType::Wifi;
		break;
	}

#else
	strOut = "undesigned platform";
#endif
	return strOut;
}

void CHmsSettingConnectivityPage::OnBtnClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e, const std::string & strname, int nIndex)
{
	if (e == HMS_UI_BUTTON_STATE::btn_normal)
	{
		if (m_nodeConfirmDlg->IsVisible())
		{
			m_nodeConfirmDlg->SetVisible(false);
		}
		else
		{
			m_nodeConfirmDlg->SetVisible(true);
			CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is reconnect the selected device?");

			m_strActionName = strname;
			m_nActionId = nIndex;
		}
	}
}

void CHmsSettingConnectivityPage::OnClickConfirmDlgCancel()
{
	m_nodeConfirmDlg->SetVisible(false);
}

void CHmsSettingConnectivityPage::OnClickConfirmDlgConfirm()
{
	m_nodeConfirmDlg->SetVisible(false);

	if (m_eConnectivityType == ConnectivityType::Wifi)
	{
		OnActionConnectWifi();
	}
	else if (m_eConnectivityType == ConnectivityType::Bluetooth)
	{
		OnActionConnectBluetooth();
	}
}

void CHmsSettingConnectivityPage::OnActionConnectWifi()
{
#if HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID
	if (SetAndroidConnectTheHistoryWifi(m_strActionName))
	{
		SetContentInfo("Connecting...");
	}
	else
	{
		SetContentInfo("Connect Failed!");
	}
#else

#endif
}

void CHmsSettingConnectivityPage::OnActionConnectBluetooth()
{

}

