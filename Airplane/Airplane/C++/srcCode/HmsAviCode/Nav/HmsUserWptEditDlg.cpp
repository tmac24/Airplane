#include "HmsUserWptEditDlg.h"

#include "Language/HmsLanguageMananger.h"
#include "HmsFrame2DRoot.h"
#include "HmsNavFplLayer.h"

CHmsUserWptEditDlg::CHmsUserWptEditDlg()
: HmsUiEventInterface(this)
{

}

CHmsUserWptEditDlg::~CHmsUserWptEditDlg()
{

}

bool CHmsUserWptEditDlg::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

    float toolButtonHeight = 70;
    Vec2 tablePos = Vec2(10, (toolButtonHeight + 10) * 1 + 10);
    Size tableSize = Size(windowSize.width - 20, windowSize.height - tablePos.y - (toolButtonHeight * 2 + 200 + 40));

	auto pBgNode = CHmsDrawNode::Create();
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

	auto CreateSysEdit = [=](Size s, Vec2 pos, const char * labelText)
	{
        auto pItem = CHmsUiSystemEditBox::Create("", 32, s, "res/airport/search_bg.png");
		pItem->SetAnchorPoint(0, 0);
		pItem->SetPosition(pos);
		pItem->SetTextColor(Color4B::WHITE);
		this->AddChild(pItem);

        auto pLabel = CHmsGlobal::CreateLanguageLabel(labelText, 26);
		pLabel->SetAnchorPoint(0, 0.5);
		pLabel->SetPosition(Vec2(-130, s.height * 0.5));
		pLabel->SetTextColor(Color4B::WHITE);
		pItem->AddChild(pLabel);

		return pItem;
	};

    auto editSize = Size(windowSize.width - 160, toolButtonHeight);
    float leftSpace = 150;
    m_pLineEidtName = CreateSysEdit(editSize, Vec2(leftSpace, windowSize.height - (toolButtonHeight + 10)), "Name");

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID
    m_pLineEidtName->SetMaxCharCount(15);
#endif

    m_pLonLatEditWidget = CHmsLonLatEditWidget::Create();
    m_pLonLatEditWidget->SetAnchorPoint(Vec2(0, 0));
    m_pLonLatEditWidget->SetPosition(Vec2(10, windowSize.height - toolButtonHeight - 20 - 200));
    this->AddChild(m_pLonLatEditWidget);
	
	auto CreateStretchButton = [=](Size s, Vec2 pos, const char * text)
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
		pItem->AddText(text, 26, Color4B::WHITE, true);
		pItem->SetAnchorPoint(Vec2(0, 0));
		pItem->SetPosition(pos);
		this->AddChild(pItem);
		return pItem;
	};

    auto btnSize = Size(260, toolButtonHeight);
	CHmsUiStretchButton *pItem = nullptr;
    pItem = CreateStretchButton(btnSize, Vec2(windowSize.width*0.5 - 130, windowSize.height - (toolButtonHeight + 10) * 2 - 210), "Add");
	pItem->SetTextColor(Color4B::GREEN);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		AddUserWpt();
		ShowOrHideDragButton(false);
	});
    pItem = CreateStretchButton(btnSize, Vec2(windowSize.width / 2 - btnSize.width - 10, 10), "Delete");
	pItem->SetTextColor(Color4B::RED);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		DeleteUserWpt();
	});
	pItem = CreateStretchButton(btnSize, Vec2(windowSize.width / 2 + 10, 10), "Close");
	pItem->SetTextColor(Color4B::GRAY);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		CloseDlg();
	});

	auto pTable = CHmsTableBox::CreateWithImage(NULL);
    pTable->Clear();
    pTable->SetHeaderHeight(70);
    pTable->SetItemHeight(70);
	pTable->SetBoxSize(tableSize);
	pTable->SetAnchorPoint(Vec2(0, 0));
	pTable->SetPosition(tablePos);
	pTable->SetColumnCount(2);
	pTable->SetColumnWidth(0, 500);
	pTable->SetHeaderLabel(0, "Name", true);
	pTable->SetHeaderLabel(1, "Position", true);
	pTable->SetClickCellCallback([=](int row, int col){

		m_pTableBox->SelectRow(row);
	});
	this->AddChild(pTable);
	m_pTableBox = pTable;


	this->RegisterAllNodeChild();

	return true;
}

void CHmsUserWptEditDlg::LineEditBlur()
{
	if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
	{
		ShowOrHideDragButton(false);

		CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false, nullptr, nullptr);
	}
    m_pLineEidtName->CloseLastEditBoxKeyboard();
    m_pLonLatEditWidget->Blur();
}

void CHmsUserWptEditDlg::ShowDlg()
{
	UpdateDataFromDb();
	UpdateList();

	ClearLineEdit();

	this->SetVisible(true);
}

void CHmsUserWptEditDlg::CloseDlg()
{
	LineEditBlur();
	this->SetVisible(false);
    if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
	{
		ShowOrHideDragButton(false);

        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false,
            [=](char c){


        }, HMS_CALLBACK_0(CHmsUserWptEditDlg::LineEditBlur, this));
	}
}

bool CHmsUserWptEditDlg::OnPressed(const Vec2 & posOrigin)
{
	LineEditBlur();

    HmsUiEventInterface::OnPressed(posOrigin);

	return true;
}

void CHmsUserWptEditDlg::AddUserWpt()
{
	UserWptStu stu;
	stu.name = m_pLineEidtName->GetText();

    if (stu.name.size() > 0 && m_pLonLatEditWidget->IsRightResult())
    {
        if (CHmsNavMathHelper::StrLonLatToFloat(stu.lon, stu.lat, m_pLonLatEditWidget->GetFormatString()))
        {
            ClearLineEdit();

            CHmsHistoryRouteDatabase::GetInstance()->SaveUserWpt(stu);

            UpdateDataFromDb();
            UpdateList();
        }
    }
}

void CHmsUserWptEditDlg::DeleteUserWpt()
{
	auto index = m_pTableBox->GetCurSelectRowIndex();
    if (index < 0 || index >= (int)m_vecUserWpt.size())
	{
		return;
	}
	CHmsHistoryRouteDatabase::GetInstance()->DeleteUserWpt(m_vecUserWpt[index].idInDb);

	UpdateDataFromDb();
	UpdateList();
}

void CHmsUserWptEditDlg::UpdateDataFromDb()
{
	m_vecUserWpt.clear();
	CHmsHistoryRouteDatabase::GetInstance()->GetUserWpt(m_vecUserWpt);
}

void CHmsUserWptEditDlg::UpdateList()
{
	m_pTableBox->Clear();
	for (auto &c : m_vecUserWpt)
	{
		int row = m_pTableBox->AppendRow();

		m_pTableBox->AppendTextItem(row, 0, c.name.c_str(), TableItemAlign::ALIGN_LEFT);
		m_pTableBox->AppendTextItem(row, 1, CHmsNavMathHelper::LonLatToString(c.lon, c.lat).c_str(), TableItemAlign::ALIGN_LEFT);
	}
}

void CHmsUserWptEditDlg::OnDragButtonCallback(float lon, float lat)
{
	
}

void CHmsUserWptEditDlg::ShowOrHideDragButton(bool bShow)
{
	if (bShow)
	{
        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowOrHideDragButton(bShow, 
            HMS_CALLBACK_2(CHmsUserWptEditDlg::OnDragButtonCallback, this));
	}
	else
	{
        CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowOrHideDragButton(bShow, nullptr);
	}
}

void CHmsUserWptEditDlg::ClearLineEdit()
{
	m_pLineEidtName->SetText("");
	m_pLonLatEditWidget->ClearText();
}




