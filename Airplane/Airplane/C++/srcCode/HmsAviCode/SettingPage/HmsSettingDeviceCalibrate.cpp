#include "HmsSettingDeviceCalibrate.h"
#include "ui/HmsUiImageButton.h"
#include "ui/HmsUiRadio.h"
#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiImagePanel.h"
#include "ui/HmsUiLabelButton.h"
#include "ui/HmsUiHotAreaButton.h"
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

CHmsSettingDeviceCalibrate::CHmsSettingDeviceCalibrate()
	: m_nActionId(0)
{

}

CHmsSettingDeviceCalibrate::~CHmsSettingDeviceCalibrate()
{

}

CHmsSettingDeviceCalibrate * CHmsSettingDeviceCalibrate::Create(const HmsAviPf::Size & size)
{
	CHmsSettingDeviceCalibrate *ret = new CHmsSettingDeviceCalibrate();
	if (ret && ret->init(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

#define XXXINFO(x) (x*CHmsGlobal::GetDesignScale())

bool CHmsSettingDeviceCalibrate::init(const HmsAviPf::Size & size)
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
		auto label = CHmsLanguageLabel::CreateLabel("Device Configuration", 38, true)->GetLabel();
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

	InitCalibrateItems();

	InitComfirmDlg();

	RegisterAllNodeChild();
	return true;
}

void CHmsSettingDeviceCalibrate::InitComfirmDlg()
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


void CHmsSettingDeviceCalibrate::InitCalibrateItems()
{
	float fContentBoder = 10;
	float fPageWidth = GetContentSize().width;
	float fContentWidth = fPageWidth - 2 * fContentBoder;
	Size sizeItem(fContentWidth, 90);

	this->RemoveAllContentNode();

	do 
	{
		auto panel = CHmsUiStretchImagePanel::CreateWithImage("res/BasicFrame/SolidFrame.png", sizeItem);
		panel->SetColor(Color3B(0xC9, 0xC9, 0xC9));
		panel->SetAnchorPoint(Vec2(0, 1.0));

		auto labelContent = CHmsLanguageLabel::CreateLabel("Save Magnetic correction", (38), true)->GetLabel();
		labelContent->SetTextColor(Color4B::BLACK);
		labelContent->SetAnchorPoint(Vec2(0, 0.5f));
		labelContent->SetTag(88);
		labelContent->SetPosition(64, sizeItem.height*0.5f);
		panel->AddChild(labelContent);


		auto label = CHmsGlobal::CreateLanguageLabel("Save", 38, true);
		label->SetTextColor(Color4B::BLACK);
		auto btn = CHmsUiHotAreaButton::Create(Size(sizeItem.width - 32 * 2, 90), HMS_CALLBACK_2(CHmsSettingDeviceCalibrate::OnSaveBtnClick, this));
		btn->AddContentAlignRightCenter(label, Vec2(0, 0));
		btn->SetAnchorPoint(Vec2(1.0f, 0.5f));
		btn->SetPosition(fContentWidth - 32, sizeItem.height*0.5f);
		panel->AddChild(btn);

		panel->RegisterAllNodeChild();
		m_viewWindow->AddWindowItem(panel);
	} while (0);

	do
	{
		auto panel = CHmsUiStretchImagePanel::CreateWithImage("res/BasicFrame/SolidFrame.png", sizeItem);
		panel->SetColor(Color3B(0xC9, 0xC9, 0xC9));
		panel->SetAnchorPoint(Vec2(0, 1.0));

		auto labelContent = CHmsLanguageLabel::CreateLabel("Erase Magnetic correction", (38), true)->GetLabel();
		labelContent->SetTextColor(Color4B::BLACK);
		labelContent->SetAnchorPoint(Vec2(0, 0.5f));
		labelContent->SetTag(88);
		labelContent->SetPosition(64, sizeItem.height*0.5f);
		panel->AddChild(labelContent);


		auto label = CHmsGlobal::CreateLanguageLabel("Erase", 38, true);
		label->SetTextColor(Color4B::BLACK);
		auto btn = CHmsUiHotAreaButton::Create(Size(sizeItem.width - 32 * 2, 90), HMS_CALLBACK_2(CHmsSettingDeviceCalibrate::OnEraseBtnClick, this));
		btn->AddContentAlignRightCenter(label, Vec2(0, 0));
		btn->SetAnchorPoint(Vec2(1.0f, 0.5f));
		btn->SetPosition(fContentWidth - 32, sizeItem.height*0.5f);
		panel->AddChild(btn);

		panel->RegisterAllNodeChild();
		m_viewWindow->AddWindowItem(panel);
	} while (0);
}

void CHmsSettingDeviceCalibrate::OnEnter()
{
	CHmsSettingSubPage::OnEnter();

	//SetScheduleUpdate();
	UpdateItem();
}


void CHmsSettingDeviceCalibrate::OnExit()
{
	//SetUnSchedule();
}

void CHmsSettingDeviceCalibrate::AddContentNode(HmsAviPf::CHmsNode * node)
{
	if (node)
	{
		m_vContectNode.pushBack(node);
		m_viewWindow->AddWindowItem(node);
	}
}

void CHmsSettingDeviceCalibrate::RemoveAllContentNode()
{
	m_viewWindow->RemoveAllItem();
	CHmsSettingSubPage::RemoveAllContentNode();
}

void CHmsSettingDeviceCalibrate::SetContentInfo(const std::string & strContent)
{
	CHmsLanguageLabel::SetText(m_labelContent, strContent.c_str());
}

void CHmsSettingDeviceCalibrate::Update(float delta)
{
// 	if (m_timeLastUpdate.GetElapsed() > 1.5)
// 	{
// 		SetContentInfo(GetConnectivityInfo());
// 		m_timeLastUpdate.RecordCurrentTime();
// 	}	
}

void CHmsSettingDeviceCalibrate::UpdateItem()
{
	SetContentInfo(GetConnectivityInfo());

	m_timeLastUpdate.RecordCurrentTime();
}

std::string CHmsSettingDeviceCalibrate::GetConnectivityInfo()
{
	std::string strOut;
	strOut = "This is Device calibrate page!";
	return strOut;
}

void CHmsSettingDeviceCalibrate::OnSaveBtnClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e)
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
			CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is Save the\nMagnetic correction Data?");
			m_nActionId = 1;
		}
	}
}

void CHmsSettingDeviceCalibrate::OnEraseBtnClick(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE e)
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
			CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is Erase the\nMagnetic correction Data?");
			m_nActionId = 2;
		}
	}
}

void CHmsSettingDeviceCalibrate::OnClickConfirmDlgCancel()
{
	m_nodeConfirmDlg->SetVisible(false);
}
#include "DataInterface/network/HmsNetworks.h"
#include "DataInterface/network/GDL90/HmsGDL90Interface.h"
extern HmsNetworks g_hmsNetwork;
#include "base/HmsActionTimeRef.h"
void CHmsSettingDeviceCalibrate::OnClickConfirmDlgConfirm()
{
	m_nodeConfirmDlg->SetVisible(false);
	auto dataInterface = g_hmsNetwork.getInterface();
	auto gdl90Interface = dynamic_cast<CHmsGDL90Interface*>(dataInterface);
	if (gdl90Interface != nullptr)
	{
		if (1 == m_nActionId)
		{
			gdl90Interface->SendSaveMagneticCorrectionMsg();
		}
		else if (2 == m_nActionId)
		{
			gdl90Interface->SendEraseMagneticCorrectionMsg();
		}

		auto callback = CHmsActionDelayCallback::Create(1.0f, [=](){
// 			int nCmd = CDataProc::GetInstance()->GetCmdPeply();
// 			if (nCmd == 0)
// 			{
// 				SetContentInfo("You must connect the device\n the configuration is missing!");
// 			}
// 			else if (nCmd == 0xFF)
// 			{
// 				SetContentInfo("configuration CRC error!");
// 			}
// 			else
			{
				SetContentInfo("You must reset you device\n when configuration is done");
			}
		});
		this->RunAction(callback);
	}

	m_nActionId = 0;
	SetContentInfo("Waiting Reply......");
}

