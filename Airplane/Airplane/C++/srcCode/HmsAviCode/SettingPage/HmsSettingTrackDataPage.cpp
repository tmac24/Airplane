#include "HmsSettingTrackDataPage.h"
#include "ui/HmsUiImageButton.h"
#include "ui/HmsUiRadio.h"
#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiImagePanel.h"
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

extern std::string g_stringRecordTrackPath;

USING_NS_HMS;

CHmsSettingTrackDataPage::CHmsSettingTrackDataPage()
	: m_eAction(TrackFileAction::NoAction)
{

}

CHmsSettingTrackDataPage::~CHmsSettingTrackDataPage()
{

}

CHmsSettingTrackDataPage * CHmsSettingTrackDataPage::Create(const HmsAviPf::Size & size)
{
	CHmsSettingTrackDataPage *ret = new CHmsSettingTrackDataPage();
	if (ret && ret->init(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

#define XXXINFO(x) (x*CHmsGlobal::GetDesignScale())

bool CHmsSettingTrackDataPage::init(const HmsAviPf::Size & size)
{
	if (!CHmsSettingSubPage::init(size))
	{
		return false;
	}
	float m_fContentBeginY = size.height - 100;
	auto posPanel = Vec2(0, m_fContentBeginY);

	auto sizeBoder = Size(10, 0);
	auto sizeContentShow = Size(size.width, m_fContentBeginY - 20*2 - 90);
	sizeContentShow = sizeContentShow - (sizeBoder * 2);

	auto window = CHmsUiVerticalScrollView::Create(sizeContentShow);
	window->SetPosition(posPanel + Vec2(sizeBoder.width, -sizeBoder.height));
	window->SetAnchorPoint(Vec2(0, 1.0f));
	window->SetItemVSpace(10);
	this->AddChild(window);

	m_viewWindow = window;

	Size btnSize(250, 90);
	float fFontSize = 34;

	do 
	{
		auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
		btn->SetAnchorPoint(Vec2(1.0, 0));
		btn->SetPosition(Vec2((size.width - btnSize.width) * 0.5 - 10, 20));
#if 0
		btn->AddText("Copy All", fFontSize, Color4B::WHITE, true);
		btn->SetOnClickedFunc(
			[=](CHmsUiButtonAbstract *btn){
			OnClickCopyAll();
		});
#else
        btn->AddText("Copy Selected", fFontSize, Color4B::WHITE, true);
        btn->SetOnClickedFunc(
            [=](CHmsUiButtonAbstract *btn){
            OnClickCopySelected();
        });
#endif

		this->AddChild(btn);
		this->AddChildInterface(btn);
	} while (0);

	do
	{
		auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
		btn->SetAnchorPoint(Vec2(0.5f, 0.0f));
		btn->SetPosition(Vec2(size.width / 2.0, 20));
		btn->AddText("Clean Selected", fFontSize, Color4B::WHITE, true);
		btn->SetOnClickedFunc(
			[=](CHmsUiButtonAbstract *btn){
			OnClickCleanSelected();
		});

		this->AddChild(btn);
		this->AddChildInterface(btn);
	} while (0);

	do
	{
		auto btn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", btnSize, Color3B(0x36, 0x36, 0x36), Color3B(0, 71, 157), Color3B::GRAY, nullptr);
		btn->SetAnchorPoint(Vec2(0, 0));
		btn->SetPosition(Vec2((size.width + btnSize.width) * 0.5 + 10, 20));
#if 0
		btn->AddText("Clean All", fFontSize, Color4B::WHITE, true);
		btn->SetOnClickedFunc(
			[=](CHmsUiButtonAbstract *btn){
			OnClickCleanAll();
		});
#else
        btn->AddText("Select All", fFontSize, Color4B::WHITE, true);
        btn->SetOnClickedFunc(
            [=](CHmsUiButtonAbstract *btn){
            OnClickSelectAll();
        });
#endif
		this->AddChild(btn);
		this->AddChildInterface(btn);
	} while (0);


	InitComfirmDlg();

	RegisterAllNodeChild();
	return true;
}

void CHmsSettingTrackDataPage::InitComfirmDlg()
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



void CHmsSettingTrackDataPage::SetContentCheckBox(const std::vector<TrackItemInfo> & vItemContent)
{
	float fContentBoder = 10;
	float fPageWidth = GetContentSize().width;
	float fContentWidth = fPageWidth - 2 * fContentBoder;
	Size sizeItem(fContentWidth, 90);

	m_nodeDraw->clear();
    m_mapCheckBoxGroup.clear();
    m_mapRadioGroup.clear();
	this->RemoveAllContentNode();

	auto getSelectedBtn = [=](const TrackItemInfo & item)
	{
		auto btn = CHmsUiImage2StateButton::CreateWithImage("res/Btns/checkRightRU.png", "res/Btns/checkRightRS.png");
		btn->SetToggleBtn(true);
		btn->SetSelected(item.bSelected);
        m_mapCheckBoxGroup.insert(item.strName, btn);
		btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsSettingTrackDataPage::OnCheckBoxClick, this, item.strName, item.nParam));
		return btn;
	};

	auto getRadioBtn = [=](const TrackItemInfo & item)
	{
		auto radio = CHmsUiRadio::CreateWithImage("res/Btns/checkRightCU.png", "res/Btns/checkRightCS.png");
		radio->SetSelected(item.bSelected);
		m_mapRadioGroup.insert(item.strName, radio);
		radio->SetCallbackFunc(HMS_CALLBACK_2(CHmsSettingTrackDataPage::OnRadioClick, this, item.strName, item.nParam));
		return radio;
	};

	for (auto c:vItemContent)
	{
		auto panel = CHmsUiStretchImagePanel::CreateWithImage("res/BasicFrame/SolidFrame.png", sizeItem);
		panel->SetColor(Color3B(0xC9, 0xC9, 0xC9));
		panel->SetAnchorPoint(Vec2(0, 1.0));

		auto labelContent = CHmsLanguageLabel::CreateLabel(c.strName.c_str(), (38), true)->GetLabel();
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

void CHmsSettingTrackDataPage::OnEnter()
{
	CHmsSettingSubPage::OnEnter();

	UpdateItem();
}


void CHmsSettingTrackDataPage::AddContentNode(HmsAviPf::CHmsNode * node)
{
	if (node)
	{
		m_vContectNode.pushBack(node);
		m_viewWindow->AddWindowItem(node);
	}
}

void CHmsSettingTrackDataPage::RemoveAllContentNode()
{
	m_viewWindow->RemoveAllItem();
	CHmsSettingSubPage::RemoveAllContentNode();
}

void CHmsSettingTrackDataPage::UpdateItem()
{
	m_vItemInfo.clear();
	GetRecordItemInfo(m_vItemInfo);
	SetContentCheckBox(m_vItemInfo);
}

bool CHmsSettingTrackDataPage::GetRecordItemInfo(std::vector<TrackItemInfo> & vItemInfo)
{
	TrackItemInfo info;
	info.nParam = 0;
	info.nSize = 0;
	info.bSelected = false;

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
    DIR *dir = opendir(g_stringRecordTrackPath.c_str());
	if (nullptr == dir)
	{
		HMSLOG("Can not open dir. Check path or permission!");
	}

	struct dirent *file;
	// read all the files in dir
	while ((file = readdir(dir)) != NULL)
	{
		if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
		{
			HMSLOG("ignore . and ..");
			continue;
		}
		if (file->d_type == DT_DIR) {
			//string filePath = g_stringRecordPath + file->d_name;
			HMSLOG("ignore Folder");
			continue;
		}
		else {
			if (nullptr != strstr(file->d_name, ".trk"));
			{
				struct stat statbuff;	
				info.strName = file->d_name;
                std::string filePath = g_stringRecordTrackPath + file->d_name;
				if(stat(filePath.c_str(), &statbuff) < 0){
					info.nSize = 0;
				}
				else
				{
					info.nSize = statbuff.st_size;
				}
				vItemInfo.push_back(info);
				info.nParam++;
			}
		}
	}
	closedir(dir);
#else
    std::string strPath = g_stringRecordTrackPath + "*.trk";

	intptr_t handle;
	_finddata_t findData;
	handle = _findfirst(strPath.c_str(), &findData);    // find the fist file

	if (handle == -1)
	{
		HMSLOG("Failed to find first file!\n");
		return false;
	}

	do
	{
		if (findData.attrib & _A_SUBDIR
			&& strcmp(findData.name, ".") == 0
			&& strcmp(findData.name, "..") == 0)
		{
			HMSLOG("%s\t<dir>\n", findData.name);
		}
		else
		{
			info.strName = findData.name;
			info.nSize = findData.size;
			vItemInfo.push_back(info);
			info.nParam++;

			HMSLOG("%s size=%d\n", findData.name, findData.size);
		}
	} while (_findnext(handle, &findData) == 0);    // find the next file

	HMSLOG("Done!\n");
	_findclose(handle); //close the find handle
#endif

	return true;
}

void CHmsSettingTrackDataPage::OnCheckBoxClick(CHmsUiImage2StateButton * btn, HmsAviPf::HMS_UI_BUTTON_STATE state, const std::string & strName, int nParam)
{
	for (auto & c: m_vItemInfo)
	{
		if (c.strName == strName)
		{
            if (state == HMS_UI_BUTTON_STATE::btn_selected)
            {
                c.bSelected = true;
            }
            else
            {
                c.bSelected = false;
            }
		}
	}
}

void CHmsSettingTrackDataPage::OnClickCopyAll()
{
	if (m_nodeConfirmDlg->IsVisible())
	{
		m_nodeConfirmDlg->SetVisible(false);
	}
	else
	{
		m_nodeConfirmDlg->SetVisible(true);
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
		CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is copy all the files to dest folder!");
		m_eAction = TrackFileAction::CopyAll;
#else
		CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Not support the Action!");
		m_eAction = TrackFileAction::NoAction;
#endif
	}
}

void CHmsSettingTrackDataPage::OnClickCopySelected()
{
    if (m_nodeConfirmDlg->IsVisible())
    {
        m_nodeConfirmDlg->SetVisible(false);
    }
    else
    {
        m_nodeConfirmDlg->SetVisible(true);
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
        if (m_labelConfirmTitle)
        {
            CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is copy the selected files to dest folder!");
        }
        m_eAction = TrackFileAction::CopySelected;
#else
        if (m_labelConfirmTitle)
        {
            CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Not support the Action!");
        }
        m_eAction = TrackFileAction::NoAction;
#endif
    }
}

void CHmsSettingTrackDataPage::OnClickCleanSelected()
{
	if (m_nodeConfirmDlg->IsVisible())
	{
		m_nodeConfirmDlg->SetVisible(false);
	}
	else
	{
		m_nodeConfirmDlg->SetVisible(true);
		CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is Clean the selected files!");
		m_eAction = TrackFileAction::CleanSelected;
	}
}

void CHmsSettingTrackDataPage::OnClickCleanAll()
{
	if (m_nodeConfirmDlg->IsVisible())
	{
		m_nodeConfirmDlg->SetVisible(false);
	}
	else
	{
		m_nodeConfirmDlg->SetVisible(true);
		CHmsLanguageLabel::SetText(m_labelConfirmTitle, "Is Clean all the files!");
		m_eAction = TrackFileAction::CleanAll;
	}
}

void CHmsSettingTrackDataPage::OnClickSelectAll()
{
    for (auto c : m_mapCheckBoxGroup)
    {
        c.second->SetSelected(true);
    }
}

void CHmsSettingTrackDataPage::OnClickConfirmDlgCancel()
{
	m_nodeConfirmDlg->SetVisible(false);
	m_eAction = TrackFileAction::NoAction;
}

void CHmsSettingTrackDataPage::OnClickConfirmDlgConfirm()
{
	m_nodeConfirmDlg->SetVisible(false);
	switch (m_eAction)
	{
	case TrackFileAction::NoAction:
		break;
	case TrackFileAction::CopyAll:
		DoCopyAll();
		break;
    case TrackFileAction::CopySelected:
        DoCopySelected();
        break;
	case TrackFileAction::CleanSelected:
		DoCleanSelected();
		break;
	case TrackFileAction::CleanAll:
		DoCleanAll();
		break;
	default:
		break;
	}
	m_eAction = TrackFileAction::NoAction;
}

void CHmsSettingTrackDataPage::DoCopyAll()
{
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
    CallAndroidMoveFileToSelectDocTree(g_stringRecordTrackPath, ".trk");
#else

#endif
}

void CHmsSettingTrackDataPage::DoCopySelected()
{
    std::string strNames;

    for (auto & c : m_vItemInfo)
    {
        if (c.bSelected)
        {
            //std::string filePath = g_stringRecordPath + c.strName;
            if (strNames.size() > 0)
            {
                strNames += "@";
            }
            strNames += c.strName;
        }
    }

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
    CallAndroidMoveMutiFilesToSelectDocTree(g_stringRecordTrackPath, strNames, ".trk");
#else

#endif
}

void CHmsSettingTrackDataPage::DoCleanSelected()
{
	int nRemove = 0;
	int nRemoveError = 0;

	for (auto & c : m_vItemInfo)
	{
		if (c.bSelected)
		{
            std::string filePath = g_stringRecordTrackPath + c.strName;
			if (0 == remove(filePath.c_str()))
			{
				HMSLOG("delete file %s success!", filePath.c_str());
				nRemove++;
			}
			else
			{
				HMSLOG("delete file %s Error!", filePath.c_str());
				nRemoveError++;
			}
		}
	}

	UpdateItem();
}

void CHmsSettingTrackDataPage::DoCleanAll()
{
	int nRemove = 0;
	int nRemoveError = 0;

	for (auto & c : m_vItemInfo)
	{
		if (true)
		{
            std::string filePath = g_stringRecordTrackPath + c.strName;
			if (0 == remove(filePath.c_str()))
			{
				HMSLOG("delete file %s success!", filePath.c_str());
				nRemove++;
			}
			else
			{
				HMSLOG("delete file %s Error!", filePath.c_str());
				nRemoveError++;
			}
		}
	}

	HMSLOG("Remove *.rda %d success And %d error!", nRemove, nRemoveError);

	UpdateItem();
}
