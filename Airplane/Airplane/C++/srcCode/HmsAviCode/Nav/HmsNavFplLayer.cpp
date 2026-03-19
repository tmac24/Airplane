#include "HmsNavFplLayer.h"
#include "HmsNavInfoWindow.h"
#include "HmsFrame2DRoot.h"

CHmsNavFplLayer::CHmsNavFplLayer()
: HmsUiEventInterface(this)
{

}

CHmsNavFplLayer::~CHmsNavFplLayer()
{

}

bool CHmsNavFplLayer::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    CHmsNavComm::GetInstance()->SetCHmsNavFplLayer(this);

    SetContentSize(size);

    auto layerSize = size;
    float navInfoSubWidth = 120;
    //该类原来是EarthLayer的子节点，现在放到frame2D下, frame2D比layer的高少66，所以加上了66*0.3
    m_pNavInfoWin = CHmsNavInfoWindow::Create(Size(layerSize.width - navInfoSubWidth, layerSize.height * 0.382 - 20));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pNavInfoWin)
		return false;
#endif
    m_pNavInfoWin->SetAnchorPoint(0.5, 0);
    m_pNavInfoWin->SetPosition(Vec2(layerSize.width / 2, layerSize.height - m_pNavInfoWin->GetContentSize().height - 50));
    m_pNavInfoWin->SetVisible(true);
    this->AddChild(m_pNavInfoWin);

    m_pRouteNodeWin = CHmsRouteNodeWin::Create(Size(640, 800));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteNodeWin)
		return false;
#endif
    m_pRouteNodeWin->SetAnchorPoint(0.5, 1);
    m_pRouteNodeWin->SetVisible(false);
    this->AddChild(m_pRouteNodeWin);

    m_pProcedure = CHmsProcedureDlg::Create(layerSize - Size(60, 60));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pProcedure)
		return false;
#endif
    m_pProcedure->SetAnchorPoint(Vec2(0, 0));
    m_pProcedure->SetPosition(30, 30);
    m_pProcedure->SetVisible(false);
    this->AddChild(m_pProcedure);

    m_pHistoryRouteDlg = CHmsHistoryRouteDlg::Create(Size(layerSize.width - navInfoSubWidth, m_pNavInfoWin->GetPoistion().y));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pHistoryRouteDlg)
		return false;
#endif
    m_pHistoryRouteDlg->SetAnchorPoint(Vec2(0, 1));
    m_pHistoryRouteDlg->SetPosition(navInfoSubWidth / 2.0 + 1, m_pNavInfoWin->GetPoistion().y);
    m_pHistoryRouteDlg->SetVisible(false);
    this->AddChild(m_pHistoryRouteDlg);

    m_pHistoryRouteInputName = CHmsHistoryRouteInputName::Create(Size(layerSize.width - navInfoSubWidth, 250));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pHistoryRouteInputName)
		return false;
#endif
    m_pHistoryRouteInputName->SetAnchorPoint(Vec2(0, 0));
    m_pHistoryRouteInputName->SetPosition(Vec2(navInfoSubWidth / 2.0 + 1, m_pNavInfoWin->GetPoistion().y));
    m_pHistoryRouteInputName->SetVisible(false);
    this->AddChild(m_pHistoryRouteInputName);

    m_pInputAltitude = CHmsInputAltitude::Create(Size(layerSize.width - navInfoSubWidth, 250));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pInputAltitude)
		return false;
#endif
    m_pInputAltitude->SetAnchorPoint(Vec2(0, 0));
    m_pInputAltitude->SetPosition(Vec2(navInfoSubWidth / 2.0 + 1, m_pNavInfoWin->GetPoistion().y));
    m_pInputAltitude->SetVisible(false);
    this->AddChild(m_pInputAltitude);

    m_pSearchShape = CHmsSearchShape::Create(Size(640, 800));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pSearchShape)
		return false;
#endif
    m_pSearchShape->SetAnchorPoint(Vec2(0.5, 1));
    m_pSearchShape->SetPosition(Vec2(layerSize.width * 0.5, layerSize.height));
    m_pSearchShape->SetVisible(false);
    this->AddChild(m_pSearchShape);

    m_pEditAirspace = CHmsEditAirspace::Create(Size(810, 920));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pEditAirspace)
		return false;
#endif
    m_pEditAirspace->SetAnchorPoint(Vec2(0.5, 1));
    m_pEditAirspace->SetPosition(Vec2(layerSize.width * 0.5, layerSize.height));
    m_pEditAirspace->SetVisible(false);
    this->AddChild(m_pEditAirspace);

    m_pUserWptEditDlg = CHmsUserWptEditDlg::Create(Size(910, 920));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pUserWptEditDlg)
		return false;
#endif
    m_pUserWptEditDlg->SetAnchorPoint(Vec2(0.5, 1));
    m_pUserWptEditDlg->SetPosition(Vec2(layerSize.width * 0.5, layerSize.height));
    m_pUserWptEditDlg->SetVisible(false);
    this->AddChild(m_pUserWptEditDlg);

    m_pWptAddSearch = CHmsWptAddSearch::Create(Size(2000, layerSize.height - 20));
    m_pWptAddSearch->SetAnchorPoint(Vec2(0.5, 0));
    m_pWptAddSearch->SetPosition(Vec2(layerSize.width * 0.5, 0));
    m_pWptAddSearch->SetVisible(false);
    this->AddChild(m_pWptAddSearch);

    m_pKeyBoard = CHmsKeyboard::Create(Size(layerSize.width, layerSize.height * 0.4));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pKeyBoard)
		return false;
#endif
    m_pKeyBoard->SetAnchorPoint(Vec2(0.5, 0));
    m_pKeyBoard->SetPosition(layerSize.width * 0.5, 0);
    m_pKeyBoard->SetVisible(false);
    this->AddChild(m_pKeyBoard);

    this->RegisterAllNodeChild();

    SetScheduleUpdate();

    return true;
}

void CHmsNavFplLayer::Update(float delta)
{
    auto pFrame2DRoot = CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot();
    if (pFrame2DRoot)
    {
        if (!pFrame2DRoot->IsUpdateAble())
        {
            pFrame2DRoot->Update(delta);
        }
    }
}

void CHmsNavFplLayer::ShowOrHideKeyboard(bool bShow, CHmsKeyboard::HmsOnKeyPress func, CHmsKeyboard::HmsOnKbdHide funcWhenHide)
{
    if (m_pKeyBoard)
    {
        m_pKeyBoard->SetKeyPressCallback(func);
        m_pKeyBoard->SetKeybdHideCallback(funcWhenHide);
        m_pKeyBoard->SetVisible(bShow);
    }
}

void CHmsNavFplLayer::ShowRouteNodeWin(WptNodeBrief stu, int stuIndex, Vec2 worldPos)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot() 
		|| !CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->m_pRouteLayer
		|| !m_pRouteNodeWin)
		return;
#endif
    const auto &vecWptNode = CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->m_pRouteLayer->GetWptNodes();

    auto SetRunwayFilter = [=](WinPageShow page, std::string &airport)
    {
        m_pRouteNodeWin->SetSTARRunwayFilter("");
        m_pRouteNodeWin->SetAPPRunwayFilter("");

        if (page == WinPageShow::PAGE_ACTION_STARAPP || page == WinPageShow::PAGE_ACTION_CHGSTAR)
        {
            auto vecTmp = CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->FindSameProgram(airport, FPL_TYPE_PROGRAM_APP);
            if (vecTmp.size() > 0)
            {
               m_pRouteNodeWin->SetAPPRunwayFilter(vecWptNode[vecTmp[0].primaryIndex].rnwName);
            }
        }
        if (page == WinPageShow::PAGE_ACTION_STARAPP || page == WinPageShow::PAGE_ACTION_CHGAPP)
        {
            auto vecTmp = CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->FindSameProgram(airport, FPL_TYPE_PROGRAM_STAR);
            if (vecTmp.size() > 0)
            {
                m_pRouteNodeWin->SetSTARRunwayFilter(vecWptNode[vecTmp[0].primaryIndex].rnwName);
            }
        }
    };

    auto pos = this->ConvertToNodeSpace(worldPos);
    auto winSize = m_pRouteNodeWin->GetContentSize();
    if (pos.x - winSize.width * 0.5 < 0)
    {
        pos.x = winSize.width * 0.5;
    }
    if (pos.x + winSize.width * 0.5 > this->GetContentSize().width)
    {
        pos.x = this->GetContentSize().width - winSize.width * 0.5;
    }
    m_pRouteNodeWin->SetPosition(pos);

    if (stuIndex >= 0 && stuIndex < (int)vecWptNode.size())
    {
        auto info = vecWptNode[stuIndex];
        if (info.ePriType == FPL_TYPE_POINT)
        {
            if (info.vWayPoint.size() > 0)
            {
                if (info.vWayPoint[0].brief.wType == WptPointType::WPT_AIRPORT)
                {
                    if (stuIndex == 0)
                    {
                        m_pRouteNodeWin->ShowPage(WinPageShow::PAGE_ACTION_SID,
                            ShowPageWptNode(stu.lon, stu.lat, stu.ident, stu.ident, true), stuIndex);
                    }
                    else if (stuIndex == (int)vecWptNode.size() - 1)
                    {
                        SetRunwayFilter(WinPageShow::PAGE_ACTION_STARAPP, stu.ident);
                        m_pRouteNodeWin->ShowPage(WinPageShow::PAGE_ACTION_STARAPP,
                            ShowPageWptNode(stu.lon, stu.lat, stu.ident, stu.ident, true), stuIndex);
                    }
                    else
                    {
                        m_pRouteNodeWin->ShowPage(WinPageShow::PAGE_ACTION_NORMAL,
                            ShowPageWptNode(stu.lon, stu.lat, stu.ident, stu.ident, true), stuIndex);
                    }
                }
                else
                {
                    m_pRouteNodeWin->ShowPage(WinPageShow::PAGE_ACTION_NORMAL,
                        ShowPageWptNode(stu.lon, stu.lat, stu.ident, stu.ident, true), stuIndex);
                }
            }
        }
        else if (info.ePriType == FPL_TYPE_PROGRAM)
        {
            if (info.eSubType == FPL_TYPE_PROGRAM_SID)
            {
                m_pRouteNodeWin->ShowPage(WinPageShow::PAGE_ACTION_CHGSID,
                    ShowPageWptNode(stu.lon, stu.lat, stu.ident, info.belongAirport, false), stuIndex);
            }
            else if (info.eSubType == FPL_TYPE_PROGRAM_STAR)
            {
                SetRunwayFilter(WinPageShow::PAGE_ACTION_CHGSTAR, info.belongAirport);
                m_pRouteNodeWin->ShowPage(WinPageShow::PAGE_ACTION_CHGSTAR,
                    ShowPageWptNode(stu.lon, stu.lat, stu.ident, info.belongAirport, false), stuIndex);
            }
            else if (info.eSubType == FPL_TYPE_PROGRAM_APP)
            {
                SetRunwayFilter(WinPageShow::PAGE_ACTION_CHGAPP, info.belongAirport);
                m_pRouteNodeWin->ShowPage(WinPageShow::PAGE_ACTION_CHGAPP,
                    ShowPageWptNode(stu.lon, stu.lat, stu.ident, info.belongAirport, false), stuIndex);
            }
        }
    }
}

void CHmsNavFplLayer::HideRouteNodeWin()
{
    if (m_pRouteNodeWin)
    {
        m_pRouteNodeWin->SetVisible(false);
    }
}

void CHmsNavFplLayer::ShowWptAddSearch(bool bShow)
{
    if (m_pWptAddSearch)
    {
        m_pWptAddSearch->ShowDlg();
    }
}

void CHmsNavFplLayer::ToggleWptAddSearch()
{
    if (m_pWptAddSearch)
    {
        if (m_pWptAddSearch->IsVisible())
        {
            m_pWptAddSearch->CloseDlg();
        }
        else
        {
            m_pWptAddSearch->ShowDlg();
        }
    }
}
