#include "HmsNavInfoWindow.h"
#include "FMS/HmsFMS.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsFrame2DRoot.h"
#include "HmsNavFplLayer.h"
#include "HmsNavComm.h"
#include "NavSvs/HmsNavSvsLayer.h"
#include "ToolBar/HmsAvionicsDisplayMgr.h"

#define TABLE_COLOUM_FIRST			0
#define TABLE_COLOUM_BUTTON			1
#define TABLE_COLOUM_SECOND			2
#define TABLE_COLOUM_CRS			3
#define TABLE_COLOUM_ALTITUDE		4
#define TABLE_COLOUM_TOTAL_NM		5
#define TABLE_COLOUM_TOTAL_TIME		6
#define TABLE_COLOUM_LEG_NM			7
#define TABLE_COLOUM_LEG_TIME		8
#define TABLE_COLOUM_REMAIN_NM	    9
#define TABLE_COLOUM_REMAIN_TIME	10
#define TABLE_COLOUM_ETA			11
#define TABLE_COLOUM_COUNT			12

CHmsNavInfoWindow::CHmsNavInfoWindow()
: HmsUiEventInterface(this)
, m_pCurSelectRowButton(nullptr)
, m_toolButtonSize(Size(200, 95))
, m_toolButtonNormalColor(Color3B(0x0e, 0x19, 0x25))
, m_toolButtonSelectColor(Color3B(0x1e, 0x37, 0x4f))
, m_flySegment(-1)
, m_fGroundSpeed(0)
{
}


CHmsNavInfoWindow::~CHmsNavInfoWindow()
{
}

bool CHmsNavInfoWindow::Init(const HmsAviPf::Size & windowSize)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(windowSize);

    auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!pBgNode)
		return false;
#endif
    pBgNode->SetAnchorPoint(Vec2(0, 0));
    pBgNode->SetPosition(0, 0);
    this->AddChild(pBgNode);
    pBgNode->DrawSolidRect(
        Rect(0, 0, windowSize.width, m_toolButtonSize.height),
        Color4F(m_toolButtonNormalColor));
    pBgNode->DrawSolidRect(
        Rect(0, windowSize.height, windowSize.width, 50),
        Color4F(m_toolButtonNormalColor));
    pBgNode->DrawSolidRect(
        Rect(0, m_toolButtonSize.height, windowSize.width, windowSize.height - m_toolButtonSize.height),
        Color4F(m_toolButtonSelectColor));
    {
        auto pItem = CHmsUiStretchButton::Create("res/airport/bg2.png",
            m_toolButtonSize, m_toolButtonNormalColor, m_toolButtonSelectColor);
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return false;
#endif
        pItem->AddText("Edit", 32);
        pItem->SetAnchorPoint(Vec2(0, 0));
        pItem->SetPosition(Vec2(windowSize.width - m_toolButtonSize.width * 5, 0));
        pItem->SetOnClickedFunc(std::bind(&CHmsNavInfoWindow::ShowPage, this, std::placeholders::_1, NavInfoPage::PAGE_EDIT));
        this->AddChild(pItem);
        m_pButtonEdit = pItem;
    }
    {
        auto pItem = CHmsUiStretchButton::Create("res/airport/bg2.png",
            m_toolButtonSize, m_toolButtonNormalColor, m_toolButtonSelectColor);
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return false;
#endif
        pItem->AddText("NavLog", 32);
        pItem->SetAnchorPoint(Vec2(0, 0));
        pItem->SetPosition(Vec2(windowSize.width - m_toolButtonSize.width * 4, 0));
        pItem->SetOnClickedFunc(std::bind(&CHmsNavInfoWindow::ShowPage, this, std::placeholders::_1, NavInfoPage::PAGE_NAVLOG));
        this->AddChild(pItem);
        m_pButtonNavLog = pItem;
    }
    {
        auto pItem = CHmsUiStretchButton::Create("res/airport/bg2.png",
            m_toolButtonSize, m_toolButtonNormalColor, m_toolButtonSelectColor);
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return false;
#endif
        pItem->AddText("Profile", 32);
        pItem->SetAnchorPoint(Vec2(0, 0));
        pItem->SetPosition(Vec2(windowSize.width - m_toolButtonSize.width * 3, 0));
        pItem->SetOnClickedFunc(std::bind(&CHmsNavInfoWindow::ShowPage, this, std::placeholders::_1, NavInfoPage::PAGE_PROFILE));
        this->AddChild(pItem);
        m_pButtonProfile = pItem;
    }

    InitEdit(windowSize);
    InitNavLog(windowSize);
    InitProfile(windowSize);

    InitBottomToolBtn(windowSize);

    ShowPage(m_pButtonNavLog, NavInfoPage::PAGE_EDIT);

    this->RegisterAllNodeChild();
    this->SetScheduleUpdate();
    return true;
}

void CHmsNavInfoWindow::OnRowButtonClicked(CHmsUiButtonAbstract *pBtn)
{
    auto pUiStretchButton = dynamic_cast<CHmsUiStretchButton*>(pBtn);
    if (!pUiStretchButton)
    {
        return;
    }
    OnSelectCurrentNavLeg(pUiStretchButton);
}

void CHmsNavInfoWindow::OnSelectCurrentNavLeg(CHmsUiStretchButton *pUiStretchButton)
{
    if (!pUiStretchButton)
    {
        return;
    }
    int row, column;
    if (m_pCurSelectRowButton)
    {
        m_pCurSelectRowButton->UnSelected();
    }
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNavLogTable)
		return;
#endif
    m_pNavLogTable->GetCellIndex(pUiStretchButton->GetNode(), row, column);
    m_pNavLogTable->SelectRow(row);
    pUiStretchButton->Selected();

    m_pCurSelectRowButton = pUiStretchButton;

    CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData()->SetSelWptIndex(row + 1);
}

void CHmsNavInfoWindow::InitEdit(const HmsAviPf::Size & windowSize)
{
    m_pRouteElementsEdit = CHmsRouteElementsEdit::Create(windowSize - Size(0, m_toolButtonSize.height));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteElementsEdit)
		return;
#endif
    this->AddChild(m_pRouteElementsEdit);
    m_pRouteElementsEdit->SetAnchorPoint(0, 0);
    m_pRouteElementsEdit->SetPosition(0, m_toolButtonSize.height);
}

void CHmsNavInfoWindow::InitNavLog(const HmsAviPf::Size & windowSize)
{
    m_pNavLogTable = CHmsTableBox::CreateWithImage(NULL, 26);
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNavLogTable)
		return;
#endif
    this->AddChild(m_pNavLogTable);
    m_pNavLogTable->Clear();
    m_pNavLogTable->SetItemHeight(70);
    m_pNavLogTable->SetHeaderHeight(70);
    m_pNavLogTable->SetBoxSize(windowSize - Size(0, m_toolButtonSize.height));
    m_pNavLogTable->SetAnchorPoint(Vec2(0, 0));
    m_pNavLogTable->SetPosition(0, m_toolButtonSize.height);
    m_pNavLogTable->SetColumnCount(TABLE_COLOUM_COUNT);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_FIRST, 300);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_BUTTON, 100);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_SECOND, 300);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_CRS, 160);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_ALTITUDE, 250);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_TOTAL_NM, 150);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_TOTAL_TIME, 150);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_LEG_NM, 150);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_LEG_TIME, 150);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_REMAIN_NM, 150);
    m_pNavLogTable->SetColumnWidth(TABLE_COLOUM_REMAIN_TIME, 150);
    m_pNavLogTable->SetHeaderLabel(TABLE_COLOUM_FIRST, "FROM", false, 26);
    m_pNavLogTable->SetHeaderLabel(TABLE_COLOUM_SECOND, "TO", false, 26);
    m_pNavLogTable->SetHeaderLabel(TABLE_COLOUM_CRS, "CRS", true, 26);
    m_pNavLogTable->SetHeaderLabel(TABLE_COLOUM_ALTITUDE, "ALT", true, 26);
    m_pNavLogTable->SetHeaderLabel(TABLE_COLOUM_TOTAL_NM, "TOTALS", true, 26);
    m_pNavLogTable->SetHeaderLabel(TABLE_COLOUM_LEG_NM, "LEG", true, 26);
    m_pNavLogTable->SetHeaderLabel(TABLE_COLOUM_REMAIN_NM, "REMAINING", true, 26);
    m_pNavLogTable->SetHeaderLabel(TABLE_COLOUM_ETA, "ETA", true, 26);

    m_pNavLogTable->SetClickCellCallback([=](int row, int col){

        if (TABLE_COLOUM_ALTITUDE == col && row != -1)
        {
            if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
            {
                auto index = CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ConvertIntIndexToFPLIndex(row + 1);
                CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowInputAltitude(index);
            }
        }
    });
}

void CHmsNavInfoWindow::InitProfile(const HmsAviPf::Size & windowSize)
{
    Size panelSize = Size(200, windowSize.height);

    m_pNavProfileChart = CHmsNavProfileChart::Create(windowSize - Size(panelSize.width, m_toolButtonSize.height));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNavProfileChart)
		return;
#endif
    this->AddChild(m_pNavProfileChart);
    m_pNavProfileChart->SetAnchorPoint(0, 0);
    m_pNavProfileChart->SetPosition(0, m_toolButtonSize.height);
    m_pNavProfileChart->SetOnRefLinePosChangeCallback(HMS_CALLBACK_3(CHmsNavInfoWindow::OnRefLinePosChange, this));
    m_pNavProfileChart->SetResetCallback([=](){
#if _NAV_SVS_LOGIC_JUDGE
		if (m_pChartConfigWin)
#endif
        m_pChartConfigWin->ResetValue();
    });

    m_pNavProfileRightPanel = CHmsUiPanel::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNavProfileRightPanel)
		return;
#endif
    this->AddChild(m_pNavProfileRightPanel);
    m_pNavProfileRightPanel->SetContentSize(panelSize);
    m_pNavProfileRightPanel->SetPosition(windowSize.width - panelSize.width, 0);
    //
    float leftx = 10;
    float starty = panelSize.height - 90;
    float space = 100;
    float textspace = 50;
    {
        auto tempDraw = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
		if (!tempDraw)
			return;
#endif
        m_pNavProfileRightPanel->AddChild(tempDraw);
        tempDraw->SetContentSize(panelSize);
        tempDraw->SetAnchorPoint(0, 0);
        Rect r1 = Rect(0, m_toolButtonSize.height + 1, panelSize.width, starty - (m_toolButtonSize.height + 1) + 26);
        tempDraw->DrawSolidRect(r1, Color4F(m_toolButtonNormalColor));
        tempDraw->DrawSolidRect(
            Rect(0, r1.origin.y + r1.size.height + 2, panelSize.width, panelSize.height - (r1.origin.y + r1.size.height + 2)),
            Color4F(m_toolButtonNormalColor));
    }
    {
        auto label = CHmsGlobal::CreateLanguageLabel("Route", 26);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return;
#endif
        label->SetAnchorPoint(0.5, 0.5);
        label->SetPosition(panelSize.width / 2, (panelSize.height - starty) * 0.6 + starty);
        m_pNavProfileRightPanel->AddChild(label);
    }
    {
        auto label = CHmsGlobal::CreateLanguageLabel("HIGHEST POINT", 20);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return;
#endif
        label->SetAnchorPoint(0, 0);
        label->SetPosition(leftx, starty);
        m_pNavProfileRightPanel->AddChild(label);
    }
    {
        auto label = CHmsGlobal::CreateLanguageLabel("None", 26);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return;
#endif
        label->SetAnchorPoint(0, 0);
        label->SetPosition(leftx, starty - textspace);
        m_pNavProfileRightPanel->AddChild(label, 1, NAV_PROFILE_LABEL_HIGHEST);
    }
    {
        auto label = CHmsGlobal::CreateLanguageLabel("CLEARANCE", 20, Color4F::RED);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return;
#endif
        label->SetAnchorPoint(0, 0);
        label->SetPosition(leftx, starty - space);
        m_pNavProfileRightPanel->AddChild(label);
    }
    {
        auto label = CHmsGlobal::CreateLanguageLabel("None", 26);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return;
#endif
        label->SetAnchorPoint(0, 0);
        label->SetPosition(leftx, starty - space - textspace);
        m_pNavProfileRightPanel->AddChild(label, 5, NAV_PROFILE_LABEL_CLEARANCE);
    }
    {
        auto label = CHmsGlobal::CreateLanguageLabel("FIRST STRIKE", 20, Color4F::YELLOW);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return;
#endif
        label->SetAnchorPoint(0, 0);
        label->SetPosition(leftx, starty - space * 2);
        m_pNavProfileRightPanel->AddChild(label);
    }
    {
        auto label = CHmsGlobal::CreateLanguageLabel("None", 26);
#if _NAV_SVS_LOGIC_JUDGE
		if (!label)
			return;
#endif
        label->SetAnchorPoint(0, 0);
        label->SetPosition(leftx, starty - space * 2 - textspace);
        m_pNavProfileRightPanel->AddChild(label, 10, NAV_PROFILE_LABEL_STRIKE);
    }
    {
        auto pItem = CHmsUiStretchButton::Create("res/airport/bg2.png",
            Size(panelSize.width, m_toolButtonSize.height), m_toolButtonNormalColor, m_toolButtonSelectColor);
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return;
#endif
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            pItem->AddText("0.5 km\n100m / 1000m", 20);
        }
        else
        {
            pItem->AddText("0.5 nm\n100ft / 1000ft", 20);
        }
        pItem->SetAnchorPoint(Vec2(0, 0));
        pItem->SetPosition(0, 0);
        pItem->AddIcon("res/airport/iconMapSetting_16.png");
        auto pIcon = pItem->GetIconNode();
#if _NAV_SVS_LOGIC_JUDGE
		if (!pIcon)
			return;
#endif
        pIcon->SetAnchorPoint(Vec2(0, 0.5));
        pIcon->SetPosition(Vec2(5, m_toolButtonSize.height / 2));
        auto pText = pItem->GetLabel();
#if _NAV_SVS_LOGIC_JUDGE
		if (!pText)
			return;
#endif
        pText->SetAnchorPoint(Vec2(0, 0.5));
        pText->SetPosition(Vec2(25, m_toolButtonSize.height / 2));
        m_pNavProfileRightPanel->AddChild(pItem);

        pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract *pBtn){
            bool b = m_pChartConfigWin->IsVisible();
            m_pChartConfigWin->SetVisible(!b);
        });

        //
        m_pChartConfigWin = CHmsChartConfigWin::Create(Size(panelSize.width * 2.5, windowSize.height));
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pChartConfigWin)
			return;
#endif
        m_pChartConfigWin->SetAnchorPoint(1, 0);
        m_pChartConfigWin->SetPosition(windowSize.width - panelSize.width, 0);
        m_pChartConfigWin->SetVisible(false);
        this->AddChild(m_pChartConfigWin);

        m_pChartConfigWin->SetCorridorCallback([this, pItem](float width)
        {
            m_pNavProfileChart->SetCorridorWidth(width);
            
            std::string str = pItem->GetText();
            int pos = str.find("\n");
            if (pos != std::string::npos)
            {
                std::string str2 = str.substr(pos + 1);
                std::string text;
                if (width < 1)
                {
                    if (CHmsNavComm::GetInstance()->IsMetric())
                    {
                        text = CHmsGlobal::FormatString("%0.1f km\n%s", width, str2.c_str());
                    }
                    else
                    {
                        text = CHmsGlobal::FormatString("%0.1f nm\n%s", width, str2.c_str());
                    }
                }
                else
                {
                    if (CHmsNavComm::GetInstance()->IsMetric())
                    {
                        text = CHmsGlobal::FormatString("%0.0f km\n%s", width, str2.c_str());
                    }
                    else
                    {
                        text = CHmsGlobal::FormatString("%0.0f nm\n%s", width, str2.c_str());
                    }
                }
                pItem->SetText(text.c_str());
            }

        });
        m_pChartConfigWin->SetHazardCallback([this, pItem](float high, float low)
        {
            if (CHmsNavComm::GetInstance()->IsMetric())
            {
                m_pNavProfileChart->SetHazardAltitudes(MeterToFeet(high), MeterToFeet(low));
            }
            else
            {
                m_pNavProfileChart->SetHazardAltitudes(high, low);
            }
           

            std::string str = pItem->GetText();
            int pos = str.find("\n");
            if (pos != std::string::npos)
            {
                std::string str1 = str.substr(0, pos);
                if (CHmsNavComm::GetInstance()->IsMetric())
                {
                    std::string text = CHmsGlobal::FormatString("%s\n%0.0fm / %0.0fm", str1.c_str(), high, low);
                    pItem->SetText(text.c_str());
                }
                else
                {
                    std::string text = CHmsGlobal::FormatString("%s\n%0.0fft / %0.0fft", str1.c_str(), high, low);
                    pItem->SetText(text.c_str());
                }
            }
        });
    }
    m_pNavProfileRightPanel->RegisterAllNodeChild();
}

void CHmsNavInfoWindow::ShowPage(CHmsUiButtonAbstract *pBtn, NavInfoPage page)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pButtonEdit
		|| !m_pButtonNavLog
		|| !m_pButtonProfile
		|| !m_pRouteElementsEdit
		|| !m_pNavLogTable
		|| !m_pNavProfileChart
		|| !m_pNavProfileRightPanel)
		return;
#endif
    switch (page)
    {
    case NavInfoPage::PAGE_EDIT:
        m_pButtonEdit->Selected();
        m_pButtonNavLog->UnSelected();
        m_pButtonProfile->UnSelected();

        m_pRouteElementsEdit->SetVisible(true);

        m_pNavLogTable->SetVisible(false);

        m_pNavProfileChart->SetVisible(false);
        m_pNavProfileRightPanel->SetVisible(false);
        break;
    case NavInfoPage::PAGE_NAVLOG:
        m_pButtonEdit->UnSelected();
        m_pButtonNavLog->Selected();
        m_pButtonProfile->UnSelected();

        m_pRouteElementsEdit->SetVisible(false);

        m_pNavLogTable->SetVisible(true);
        m_pNavLogTable->ForceUpdateAllChild();
        m_pNavLogTable->ScrollToDefault();

        m_pNavProfileChart->SetVisible(false);
        m_pNavProfileRightPanel->SetVisible(false);
        break;
    case NavInfoPage::PAGE_PROFILE:
        m_pButtonEdit->UnSelected();
        m_pButtonNavLog->UnSelected();
        m_pButtonProfile->Selected();

        m_pRouteElementsEdit->SetVisible(false);

        m_pNavLogTable->SetVisible(false);

        m_pNavProfileChart->SetVisible(true);
        m_pNavProfileRightPanel->SetVisible(true);
        break;
    default:
        break;
    }
}

void CHmsNavInfoWindow::OnRefLinePosChange(float highest, float clearance, float firstStrike)
{
    {
        auto pItem = dynamic_cast<CHmsLabel*>(m_pNavProfileRightPanel->GetChildByTag(NAV_PROFILE_LABEL_HIGHEST));
        if (pItem)
        {
            if (CHmsNavComm::GetInstance()->IsMetric())
            {
                pItem->SetString(CHmsGlobal::FormatString("%0.0fm MSL", FeetToMeter(highest)));
            }
            else
            {
                pItem->SetString(CHmsGlobal::FormatString("%0.0fft MSL", highest));
            }        
        }
    }
    {
        auto pItem = dynamic_cast<CHmsLabel*>(m_pNavProfileRightPanel->GetChildByTag(NAV_PROFILE_LABEL_CLEARANCE));
        if (pItem)
        {
            if (CHmsNavComm::GetInstance()->IsMetric())
            {
                pItem->SetString(CHmsGlobal::FormatString("%0.0fm", FeetToMeter(clearance)));
            }
            else
            {
                pItem->SetString(CHmsGlobal::FormatString("%0.0fft", clearance));
            }  
        }
    }
    {
        auto pItem = dynamic_cast<CHmsLabel*>(m_pNavProfileRightPanel->GetChildByTag(NAV_PROFILE_LABEL_STRIKE));
        if (pItem)
        {
            if (firstStrike < 0)
            {
                pItem->SetString("None");
            }
            else
            {
                //firstStrike 米制时传入是km，英制时传入是nm
                if (CHmsNavComm::GetInstance()->IsMetric())
                {
                    pItem->SetString(CHmsGlobal::FormatString("%0.0f km", firstStrike));
                }
                else
                {
                    pItem->SetString(CHmsGlobal::FormatString("%0.0f nm", firstStrike));
                }  
            }
        }
    }
}

void CHmsNavInfoWindow::SetWptNodeData(const std::vector<FPL2DNodeInfoStu> &vecFPLInfo)
{
    m_vecFPL2DNodeInfoStu = vecFPLInfo;

    std::vector<WptNodeStu2D> vec;
    for (auto &c : vecFPLInfo)
    {
        for (auto &e : c.vWayPoint)
        {
            vec.push_back(e);
        }
    }
    m_vecWptNodeStu2D = vec;
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteElementsEdit || !m_pNavProfileChart)
		return;
#endif
    m_pRouteElementsEdit->SetWptNodeData(vecFPLInfo);
    m_pNavProfileChart->SetWptNodeData(vec);

    //=====
    UpdateTableDisplayData();
    //===
}

bool CHmsNavInfoWindow::OnPressed(const Vec2 & posOrigin)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pGSLineEdit)
		return false;
#endif
    m_pGSLineEdit->Blur();
    auto rect = m_pGSLineEdit->GetRectFromParent();
    if (rect.containsPoint(posOrigin))
    {
        m_pGSLineEdit->Focus();

        if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
        {
            CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(true,
                HMS_CALLBACK_1(CHmsNavInfoWindow::OnKeyboardCallback, this),
                HMS_CALLBACK_0(CHmsNavInfoWindow::LineEditBlur, this));
        }
    }
    else
    {
        if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
        {
            CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false,
                HMS_CALLBACK_1(CHmsNavInfoWindow::OnKeyboardCallback, this),
                HMS_CALLBACK_0(CHmsNavInfoWindow::LineEditBlur, this));
        }

        HmsUiEventInterface::OnPressed(posOrigin);
    }
    return true;
}

void CHmsNavInfoWindow::Update(float delta)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsGlobal::GetInstance()->GetDataBus())
		return;
#endif
    FmsCalData &data = CHmsGlobal::GetInstance()->GetDataBus()->GetFmsData();
    if (m_flySegment != data.flySegment)
    {
        auto button = dynamic_cast<CHmsUiStretchButton*>(m_pNavLogTable->GetCell(data.flySegment - 1, TABLE_COLOUM_BUTTON));
        if (button)
        {
            OnSelectCurrentNavLeg(button);
        }
        else
        {
            m_pNavLogTable->SelectRow(-1);
            if (m_pCurSelectRowButton)
            {
                m_pCurSelectRowButton->UnSelected();
                m_pCurSelectRowButton = nullptr;
            }
        }
        m_flySegment = data.flySegment;
    }

    static float s_elapsedTime = 0;
    s_elapsedTime += delta;
    if (s_elapsedTime > 1)
    {
        s_elapsedTime = 0;

        std::vector<NavLogTableStu> vecNavLogTable;
        CHmsNavComm::GetInstance()->UpdateTableETA(m_fGroundSpeed, vecNavLogTable);

        if ((int)vecNavLogTable.size() == m_pNavLogTable->GetRowCount())
        {
            for (int i = 0; i < (int)vecNavLogTable.size(); ++i)
            {
                auto &stu = vecNavLogTable[i];

                CHmsLabel *pLabel = nullptr;
                pLabel = dynamic_cast<CHmsLabel*>(m_pNavLogTable->GetCell(i, TABLE_COLOUM_REMAIN_NM));
                if (pLabel)
                {
                    pLabel->SetString(stu.remainNMStr);
                }
                pLabel = dynamic_cast<CHmsLabel*>(m_pNavLogTable->GetCell(i, TABLE_COLOUM_REMAIN_TIME));
                if (pLabel)
                {
                    pLabel->SetString(stu.remainTimeStr);
                }
                pLabel = dynamic_cast<CHmsLabel*>(m_pNavLogTable->GetCell(i, TABLE_COLOUM_ETA));
                if (pLabel)
                {
                    pLabel->SetString(stu.etaStr);
                }
            }
        }
    }
}

void CHmsNavInfoWindow::InitBottomToolBtn(const HmsAviPf::Size & windowSize)
{
    auto leftButtonPos = Vec2(10, 10);
    auto leftButtonSize = Size(m_toolButtonSize.width * 1.5, m_toolButtonSize.height - 20);

    auto pWptAddSearchBtn = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", leftButtonSize, m_toolButtonNormalColor, m_toolButtonSelectColor);
    pWptAddSearchBtn->AddText("Add Waypoint", 32);
    pWptAddSearchBtn->SetAnchorPoint(Vec2(0, 0.5));
    pWptAddSearchBtn->SetPosition(Vec2(leftButtonPos.x, leftButtonPos.y + leftButtonSize.height / 2));
    pWptAddSearchBtn->SetOnClickedFunc([=](CHmsUiButtonAbstract*){

        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ToggleWptAddSearch();
    });
    this->AddChild(pWptAddSearchBtn);

    //后面整体向左移动
    leftButtonSize.width -= 100;

    //===========输入空域
    auto CreateButton = [=](const char *imagePath, Vec2 pos)
    {
        Size buttonSize;
        buttonSize.width = 95;
        buttonSize.height = 95;

        auto pItem = CHmsFixSizeUiImageButton::CreateWithImage(imagePath, buttonSize);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->SetAnchorPoint(Vec2(0, 0.5));
			pItem->SetPosition(pos);
			this->AddChild(pItem);
		}
        return pItem;
    };

    CHmsFixSizeUiImageButton *pItem = nullptr;
    pItem = CreateButton("res/NavImage/airspace_show.png", Vec2(leftButtonPos.x + leftButtonSize.width + 100, leftButtonPos.y + leftButtonSize.height / 2));
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetToggleBtn(true);
		pItem->SetCallbackFunc([=](CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
		{
			auto pNode = dynamic_cast<CHmsNode*>(pBtn);
			if (state == HMS_UI_BUTTON_STATE::btn_selected)
			{
				pNode->SetColor(Color3B(0, 71, 157));
			}
			else
			{
				pNode->SetColor(Color3B::WHITE);
			}
			if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
			{
				CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowAirspace(pBtn->IsSelected());
			}
		});
	}
    pItem = CreateButton("res/NavImage/airspace_edit.png", Vec2(leftButtonPos.x + leftButtonSize.width + 200, leftButtonPos.y + leftButtonSize.height / 2));
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetCallbackFunc([=](CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
		{
			auto pNode = dynamic_cast<CHmsNode*>(pBtn);
			if (state == HMS_UI_BUTTON_STATE::btn_selected)
			{
				pNode->SetColor(Color3B(0, 71, 157));
			}
			else
			{
				pNode->SetColor(Color3B::WHITE);

				if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
				{
					CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowEditAirspaceDlg();
				}
			}
		});
	}
    //自定义航路窗口
    pItem = CreateButton("res/NavImage/waypoint_edit.png", Vec2(leftButtonPos.x + leftButtonSize.width + 300, leftButtonPos.y + leftButtonSize.height / 2));
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetCallbackFunc([=](CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
		{
			auto pNode = dynamic_cast<CHmsNode*>(pBtn);
			if (state == HMS_UI_BUTTON_STATE::btn_selected)
			{
				pNode->SetColor(Color3B(0, 71, 157));
			}
			else
			{
				pNode->SetColor(Color3B::WHITE);

				if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
				{
					CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowUserWptEditDlg();
				}
			}
		});
	}
    //航路预览
    pItem = CreateButton("res/AirportInfo/3DViewIcon.png", Vec2(leftButtonPos.x + leftButtonSize.width + 400, leftButtonPos.y + leftButtonSize.height / 2));
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetCallbackFunc([=](CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
		{
			auto pNode = dynamic_cast<CHmsNode*>(pBtn);
			if (state == HMS_UI_BUTTON_STATE::btn_selected)
			{
				pNode->SetColor(Color3B(0, 71, 157));
			}
			else
			{
				pNode->SetColor(Color3B::WHITE);

				if (m_vecFPL2DNodeInfoStu.size() >= 2)
				{
					CHmsNavSvsLayer::GetInstance()->SetRouteViewMode(m_vecFPL2DNodeInfoStu);
					CHmsAvionicsDisplayMgr::GetInstance()->GetToolBarBottom()->SetCurrentMode("3DView");
				}
			}
		});
	}

    //输入地速
    {
        auto pLineEdit = CHmsLineEidt::Create("res/airport/search_bg.png");
#if _NAV_SVS_LOGIC_JUDGE
		if (!pLineEdit)
			return;
#endif
        pLineEdit->SetAnchorPoint(Vec2(0, 0.5));
        pLineEdit->SetPosition(leftButtonPos.x + leftButtonSize.width + 500, leftButtonPos.y + leftButtonSize.height / 2);
        pLineEdit->SetTextColor(Color4B::WHITE);
        pLineEdit->SetSize(Size(130, m_toolButtonSize.height - 20));
        this->AddChild(pLineEdit);

        auto pLabel = CHmsGlobal::CreateUTF8Label("", 32, true);
#if _NAV_SVS_LOGIC_JUDGE
		if (!pLabel)
			return;
#endif
        pLineEdit->SetTextLabel(pLabel);

        m_pGSLineEdit = pLineEdit;

        pLabel = CHmsGlobal::CreateUTF8Label("", 26, true);
#if _NAV_SVS_LOGIC_JUDGE
		if (!pLabel)
			return;
#endif
        pLabel->SetAnchorPoint(Vec2(0, 0.5));
        pLabel->SetPosition(135, pLineEdit->GetContentSize().height * 0.5);
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            pLabel->SetString("km/h");
        }
        else
        {
            pLabel->SetString("knots");
        }
        pLineEdit->AddChild(pLabel);

        auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
            Size(170, m_toolButtonSize.height - 20), m_toolButtonNormalColor, m_toolButtonSelectColor);
#if _NAV_SVS_LOGIC_JUDGE
		if (!pItem)
			return;
#endif
        pItem->AddText("Input GS", 32);
        pItem->SetAnchorPoint(Vec2(0, 0.5));
        pItem->SetPosition(Vec2(leftButtonPos.x + leftButtonSize.width + 730, leftButtonPos.y + leftButtonSize.height / 2));
        pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract*){

            float speed = atof(m_pGSLineEdit->GetText().c_str());

            if (CHmsNavComm::GetInstance()->IsMetric())
            {
                m_fGroundSpeed = KMHToKnot(speed);
            }
            else
            {
                m_fGroundSpeed = speed;
            }

            UpdateTableDisplayData();
        });
        this->AddChild(pItem);
    }
}

void CHmsNavInfoWindow::UpdateAllChildPos(CHmsNode *pNode)
{
    if (pNode)
    {
        pNode->ForceUpdate();

        auto &vec = pNode->GetChildren();
        for (auto &c : vec)
        {
            UpdateAllChildPos(c);
        }
    }
}

void CHmsNavInfoWindow::LineEditBlur()
{
    if (m_pGSLineEdit)
    {
        m_pGSLineEdit->Blur();
    }
}

void CHmsNavInfoWindow::OnKeyboardCallback(char c)
{
    auto str = m_pGSLineEdit->GetText();
    if ('\b' == c)
    {
        if (!str.empty())
        {
            str.pop_back();
        }
    }
    else
    {
        if (c < '0' || c > '9')
        {
            return;
        }
        str.push_back(c);
    }
    m_pGSLineEdit->SetText(str);
}

void CHmsNavInfoWindow::UpdateTableDisplayData()
{
    auto GetFmcAltitudeStr = [](const FmcAltitude &stu)
    {
        std::stringstream ss;

        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            ss << floor(FeetToMeter(stu.nAltitude)) << "m";
        }
        else
        {
            ss << stu.nAltitude << "ft";
        }

        if (stu.uLimit.bitType.bAbove)
        {
            ss << 'A';
        }
        else if (stu.uLimit.bitType.bBelow)
        {
            ss << 'B';
        }
        return ss.str();
    };

    auto GetAltitudeStr = [GetFmcAltitudeStr](const AdditionAttrStu &attr)
    {
        if (attr.altStu[0].nAltitude != -1 && attr.altStu[1].nAltitude != -1)
        {
            return GetFmcAltitudeStr(attr.altStu[1]) + "-" + GetFmcAltitudeStr(attr.altStu[0]);
        }
        else if (attr.altStu[0].nAltitude != -1)
        {
            return GetFmcAltitudeStr(attr.altStu[0]);
        }
        else
        {
            return std::string();
        }
    };

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNavLogTable)
		return;
#endif
    float scrollPosition = m_pNavLogTable->GetScrollPosition();
    m_pNavLogTable->Clear();
    m_flySegment = -1;

    if (m_vecWptNodeStu2D.empty())
    {
        return;
    }

    std::vector<NavLogTableStu> vecNavLogTable;
    CHmsNavComm::GetInstance()->UpdateTableETA(m_fGroundSpeed, vecNavLogTable);

    if ((int)vecNavLogTable.size() == (int)m_vecWptNodeStu2D.size() - 1)
    {
        for (int i = 0; i < (int)m_vecWptNodeStu2D.size() - 1; ++i)
        {
            int row = m_pNavLogTable->AppendRow();
            const WptNodeBrief &curStu = m_vecWptNodeStu2D.at(i).brief;
            const WptNodeBrief &nextStu = m_vecWptNodeStu2D.at(i + 1).brief;
            const NavLogTableStu &stu = vecNavLogTable[i];

            m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_FIRST, curStu.ident.c_str(), TableItemAlign::ALIGN_LEFT, 300, 1);

            auto temp = m_pNavLogTable->AppendButtonItem(row, TABLE_COLOUM_BUTTON);
            temp->SetOnClickedFunc(HMS_CALLBACK_1(CHmsNavInfoWindow::OnRowButtonClicked, this));

            m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_SECOND, nextStu.ident.c_str(), TableItemAlign::ALIGN_LEFT, 300, 1);
            m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_CRS, CHmsGlobal::FormatString("%0.0f\xC2\xB0", stu.angle).c_str());
            m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_ALTITUDE, GetAltitudeStr(nextStu.attr).c_str());

            if (CHmsNavComm::GetInstance()->IsMetric())
            {
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_TOTAL_NM, CHmsGlobal::FormatString("%.0fkm", KnotToKMH(stu.totalNM)).c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_TOTAL_TIME, stu.totalTimeStr.c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_LEG_NM, CHmsGlobal::FormatString("%.0fkm", KnotToKMH(stu.legNM)).c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_LEG_TIME, stu.legTimeStr.c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_REMAIN_NM, stu.remainNMStr.c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_REMAIN_TIME, stu.remainTimeStr.c_str());
            }
            else
            {
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_TOTAL_NM, CHmsGlobal::FormatString("%.0fnm", stu.totalNM).c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_TOTAL_TIME, stu.totalTimeStr.c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_LEG_NM, CHmsGlobal::FormatString("%.0fnm", stu.legNM).c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_LEG_TIME, stu.legTimeStr.c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_REMAIN_NM, stu.remainNMStr.c_str());
                m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_REMAIN_TIME, stu.remainTimeStr.c_str());
            }
            m_pNavLogTable->AppendTextItem(row, TABLE_COLOUM_ETA, stu.etaStr.c_str());
        }
    }
    
    m_pNavLogTable->ScrollTo(scrollPosition);
    m_pNavLogTable->EdgeDetection();
}