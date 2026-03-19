#include "HmsEditAirspace.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsFrame2DRoot.h"
#include "HmsNavFplLayer.h"

CHmsEditAirspace::CHmsEditAirspace()
: HmsUiEventInterface(this)
, m_bNeedSync(false)
, m_bNeedUseSyncData(false)
{

}

CHmsEditAirspace::~CHmsEditAirspace()
{

}

bool CHmsEditAirspace::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

    float toolButtonHeight = 70;
    Vec2 tablePos = Vec2(10, (toolButtonHeight + 10) * 2 + 10);
    Size tableSize = Size(windowSize.width - 20, windowSize.height - tablePos.y - (toolButtonHeight + 30 + 200));

	auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (pBgNode)
#endif
	{
		pBgNode->SetAnchorPoint(Vec2(0, 0));
		pBgNode->SetPosition(0, 0);
		this->AddChild(pBgNode);
		pBgNode->DrawSolidRect(Rect(Vec2(0, 0), windowSize), Color4F(Color3B(0x0e, 0x19, 0x25)));
		pBgNode->DrawSolidRect(Rect(tablePos.x, tablePos.y, tableSize.width, tableSize.height), Color4F(Color3B(0x1e, 0x37, 0x4f)));
		//画边框
		{
			std::vector<Vec2> vec;
			vec.push_back(Vec2(0, 0));
			vec.push_back(Vec2(windowSize.width, 0));
			vec.push_back(Vec2(windowSize.width, windowSize.height));
			vec.push_back(Vec2(0, windowSize.height));
			vec.push_back(Vec2(0, 0));
			pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
		}
	}
    m_pLonLatEditWidget = CHmsLonLatEditWidget::Create();
    m_pLonLatEditWidget->SetAnchorPoint(Vec2(0, 0));
    m_pLonLatEditWidget->SetPosition(Vec2(10, windowSize.height - 210));
    this->AddChild(m_pLonLatEditWidget);

	auto CreateStretchButton = [=](Size s, Vec2 pos, const char * text)
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(text, 26, Color4B::WHITE, true);
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetPosition(pos);
			this->AddChild(pItem);
		}
		return pItem;
	};

	auto btnSize = Size((windowSize.width - 10) / 3 - 10, toolButtonHeight);
	CHmsUiStretchButton *pItem = nullptr;
    pItem = CreateStretchButton(Size(260, toolButtonHeight), 
        Vec2(windowSize.width*0.5 - 130, windowSize.height - (toolButtonHeight + 10 + 10 + 200)), "Add");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::GREEN);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		AddAirspacePoint();
		ShowOrHideDragButton(false);
	});
    pItem = CreateStretchButton(btnSize, Vec2(10 + (btnSize.width + 10) * 0, (toolButtonHeight + 10) * 0 + 10), "Delete");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::RED);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		DeleteAirspacePoint();
	});
    pItem = CreateStretchButton(btnSize, Vec2(10 + (btnSize.width + 10) * 1, (toolButtonHeight + 10) * 0 + 10), "Up");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::GREEN);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		MovePointUp();
	});
    pItem = CreateStretchButton(btnSize, Vec2(10 + (btnSize.width + 10) * 2, (toolButtonHeight + 10) * 0 + 10), "Down");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::GREEN);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		MovePointDown();
	});

    btnSize = Size((windowSize.width - 10) / 2 - 10, toolButtonHeight);
    pItem = CreateStretchButton(btnSize, Vec2(10 + (btnSize.width + 10) * 0, (toolButtonHeight + 10) * 1 + 10), "Save");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::RED);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		SaveAirspaceToDb();
		CloseDlg();
        if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
		{
			//发生更改后，如果空域可见，更新画面
            if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->AirspaceIsVisiable())
			{
                CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowAirspace(true);
			}
		}

		m_bNeedSync = true;

	});
    pItem = CreateStretchButton(btnSize, Vec2(10 + (btnSize.width + 10) * 1, (toolButtonHeight + 10) * 1 + 10), "Cancel");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::GRAY);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		CloseDlg();
	});

	auto pTable = CHmsTableBox::CreateWithImage(NULL);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pTable)
		return false;
#endif
    pTable->Clear();
    pTable->SetHeaderHeight(70);
    pTable->SetItemHeight(70);
	pTable->SetBoxSize(tableSize);
	pTable->SetAnchorPoint(Vec2(0, 0));
	pTable->SetPosition(tablePos);
	pTable->SetColumnCount(2);
	pTable->SetColumnWidth(0, 200);
	pTable->SetHeaderLabel(0, "Name", true);
	pTable->SetHeaderLabel(1, "Position", true);
	pTable->SetClickCellCallback([=](int row, int col){

		m_pTableBox->SelectRow(row);
	});
	this->AddChild(pTable);
	m_pTableBox = pTable;

	//CHmsUISyncDataMgr::GetInstance()->RegisterUiCallback(ESyncDataID::DATA_ID_AIRSPACE, HMS_CALLBACK_3(CHmsEditAirspace::OnSyncDataUpdate, this));

	this->RegisterAllNodeChild();

	return true;
}

void CHmsEditAirspace::Update(float delta)
{
	if (m_bNeedUseSyncData)
	{
		m_bNeedUseSyncData = false;
		//UseSyncData();
	}
	if (m_bNeedSync)
	{
		m_bNeedSync = false;
		//SendSyncData();
	}
}
#if 0
void CHmsEditAirspace::SendSyncData()
{
	std::vector<Vec2> vecData;
	for (auto &c : m_vecStu)
	{
		vecData.push_back(Vec2(c.lon, c.lat));
	}
	int len1 = vecData.size() * sizeof(Vec2);
	int lenAll = len1 + sizeof(int) * 1;
	char *pSyncData = new char[lenAll]();
	if (pSyncData)
	{
		char *pTmp = pSyncData;

		memcpy(pTmp, &len1, sizeof(int));
		pTmp += sizeof(int);
		memcpy(pTmp, vecData.data(), len1);
		pTmp += len1;

		if (lenAll < 65535)
		{
			CHmsUISyncDataMgr::GetInstance()->SendSyncDataToMS(ESyncDataID::DATA_ID_AIRSPACE, pSyncData, lenAll);
		}

		delete[] pSyncData;
	}
}
void CHmsEditAirspace::OnSyncDataUpdate(ESyncDataID dataID, char *data, int length)
{
	char *pTmp = data;

	int len1 = 0;
	memcpy(&len1, pTmp, sizeof(int));
	pTmp += sizeof(int);
	std::vector<Vec2> vecAirspace;
	if (len1 > 0)
	{
		vecAirspace.resize(len1 / sizeof(Vec2));
		memcpy(vecAirspace.data(), pTmp, len1);
		pTmp += len1;
	}

	//===========使用同步数据
	m_syncMutex.lock();
	m_vecSyncAirspace = vecAirspace;
	m_bNeedUseSyncData = true;
	m_syncMutex.unlock();
}
void CHmsEditAirspace::UseSyncData()
{
	std::lock_guard<std::mutex> locker(m_syncMutex);

	m_vecStu.clear();
	for (auto &c : m_vecSyncAirspace)
	{
		AirspaceEditStu stu;
		stu.lon = c.x;
		stu.lat = c.y;

		m_vecStu.push_back(stu);
	}
	ReCreateName();
	CHmsHistoryRouteDatabase::GetInstance()->SaveAirspace(m_vecStu);
	UpdateList(this->IsVisible());

	if (m_pFrame2DRoot)
	{
		//发生更改后，如果空域可见，更新画面
		if (m_pFrame2DRoot->AirspaceIsVisiable())
		{
			m_pFrame2DRoot->ShowAirspace(true);
		}
	}
}
#endif

void CHmsEditAirspace::LineEditBlur()
{
#if _NAV_SVS_LOGIC_JUDGE
    if (!m_pLonLatEditWidget)
		return;
#endif
    m_pLonLatEditWidget->Blur();
}

void CHmsEditAirspace::ShowDlg()
{
	UpdateDataFromDb();
	UpdateList(true);

    m_pLonLatEditWidget->ClearText();

	this->SetVisible(true);
}

void CHmsEditAirspace::CloseDlg()
{
	LineEditBlur();
	this->SetVisible(false);
    if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
	{
		ShowOrHideDragButton(false);

        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false,
            [=](char c){


        }, HMS_CALLBACK_0(CHmsEditAirspace::LineEditBlur, this));

        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->HideAirspaceEditLayer();
	}
}

bool CHmsEditAirspace::OnPressed(const Vec2 & posOrigin)
{
    HmsUiEventInterface::OnPressed(posOrigin);

	return true;
}

void CHmsEditAirspace::OnDragButtonCallback(float lon, float lat)
{
	
}

void CHmsEditAirspace::AddAirspacePoint()
{
	AirspaceEditStu stu;

    if (m_pLonLatEditWidget->IsRightResult())
    {
        if (CHmsNavMathHelper::StrLonLatToFloat(stu.lon, stu.lat, m_pLonLatEditWidget->GetFormatString()))
        {
            m_pLonLatEditWidget->ClearText();

            m_vecStu.push_back(stu);
            ReCreateName();
            UpdateList(true);
        }
    }
}

void CHmsEditAirspace::DeleteAirspacePoint()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pTableBox)
		return;
#endif
	auto index = m_pTableBox->GetCurSelectRowIndex();
	if (index < 0 || index >= (int)m_vecStu.size())
	{
		return;
	}
	m_vecStu.erase(m_vecStu.begin() + index);
	ReCreateName();
	UpdateList(true);
}

void CHmsEditAirspace::MovePointUp()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pTableBox)
		return;
#endif
	auto index = m_pTableBox->GetCurSelectRowIndex();
	if (index <= 0 || index >= (int)m_vecStu.size())
	{
		return;
	}
	int last = index - 1;

	auto tmp = m_vecStu[index];
	m_vecStu[index].lon = m_vecStu[last].lon;
	m_vecStu[index].lat = m_vecStu[last].lat;

	m_vecStu[last].lon = tmp.lon;
	m_vecStu[last].lat = tmp.lat;

	UpdateList(true);
	m_pTableBox->SelectRow(last);
}

void CHmsEditAirspace::MovePointDown()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pTableBox)
		return;
#endif
	auto index = m_pTableBox->GetCurSelectRowIndex();
	if (index < 0 || index >= (int)m_vecStu.size() - 1)
	{
		return;
	}
	int last = index + 1;

	auto tmp = m_vecStu[index];
	m_vecStu[index].lon = m_vecStu[last].lon;
	m_vecStu[index].lat = m_vecStu[last].lat;

	m_vecStu[last].lon = tmp.lon;
	m_vecStu[last].lat = tmp.lat;

	UpdateList(true);
	m_pTableBox->SelectRow(last);
}

void CHmsEditAirspace::ReCreateName()
{
	for (int i = 0; i < (int)m_vecStu.size(); ++i)
	{
		std::stringstream ss;
		ss << "Position" << i + 1;
		m_vecStu[i].name = ss.str();
	}
}

void CHmsEditAirspace::UpdateDataFromDb()
{
	m_vecStu.clear();
	CHmsHistoryRouteDatabase::GetInstance()->GetAirspace(m_vecStu);
}

void CHmsEditAirspace::UpdateList(bool bCall2D)
{
	//m_vecStu发生改变都会调用UpdateList

    if (bCall2D && CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
	{
        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowAirspaceEditLayer(m_vecStu);
	}
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pTableBox)
		return;
#endif
	m_pTableBox->Clear();
	for (auto &c : m_vecStu)
	{
		int row = m_pTableBox->AppendRow();

		m_pTableBox->AppendTextItem(row, 0, c.name.c_str(), TableItemAlign::ALIGN_LEFT);
		m_pTableBox->AppendTextItem(row, 1, CHmsNavMathHelper::LonLatToString(c.lon, c.lat).c_str(), TableItemAlign::ALIGN_LEFT);
	}
}

void CHmsEditAirspace::ShowOrHideDragButton(bool bShow)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
		return;
#endif
	if (bShow)
	{
        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowOrHideDragButton(bShow, 
            HMS_CALLBACK_2(CHmsEditAirspace::OnDragButtonCallback, this));
	}
	else
	{
        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowOrHideDragButton(bShow, nullptr);
	}
}

void CHmsEditAirspace::SaveAirspaceToDb()
{
	CHmsHistoryRouteDatabase::GetInstance()->SaveAirspace(m_vecStu);
}

void CHmsEditAirspace::UpdateAirspaceEditData(const std::vector<AirspaceEditStu> &vecStu)
{
	m_vecStu = vecStu;
	UpdateList(false);
}




