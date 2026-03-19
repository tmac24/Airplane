#include "HmsHistoryRouteDlg.h"
#include "HmsFrame2DRoot.h"
#include "Language/HmsLanguageMananger.h"

CHmsHistoryRouteDlg::CHmsHistoryRouteDlg()
:HmsUiEventInterface(this)
{
}

CHmsHistoryRouteDlg::~CHmsHistoryRouteDlg()
{
}

bool CHmsHistoryRouteDlg::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	float topHeight = 115;

	Vec2 useArea_pos = Vec2(10, 10);
	Size useArea_size = windowSize - Size(20, 10 + topHeight);

	auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (pBgNode)
#endif
	{
		pBgNode->SetAnchorPoint(Vec2(0, 0));
		pBgNode->SetPosition(0, 0);
		this->AddChild(pBgNode);
		pBgNode->DrawSolidRect(Rect(Vec2(0, 0), windowSize), Color4F(Color3B(0x0e, 0x19, 0x25)));
		pBgNode->DrawSolidRect(Rect(useArea_pos.x, useArea_pos.y, useArea_size.width, useArea_size.height),
			Color4F(Color3B::GRAY));
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

	auto CreateButton = [=](Size s, Vec2 pos, const char * text)
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(text, 32, Color4B::WHITE, true);
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetPosition(pos);
			this->AddChild(pItem);
		}
		return pItem;
	};

	Size btnSize = Size(260, 95);
	CHmsUiStretchButton *pItem = nullptr;
	pItem = CreateButton(btnSize, Vec2(useArea_pos.x, useArea_pos.y + useArea_size.height + 10), "Delete");
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetTextColor(Color4B::RED);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			this->DeleteRoute();
		});
	}

	pItem = CreateButton(Size(520, 95), 
		Vec2(useArea_pos.x +useArea_size.width / 2 - 260, useArea_pos.y + useArea_size.height + 10), "Use Route");
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetTextColor(Color4B::GREEN);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			this->UseRoute();
		});
	}

	pItem = CreateButton(btnSize, 
		Vec2(useArea_pos.x + useArea_size.width - btnSize.width, useArea_pos.y + useArea_size.height + 10), "Close");
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetTextColor(Color4B::RED);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			this->CloseDialog();
		});
	}

	InitListBox(Rect(useArea_pos, Size(useArea_size.width * 0.382, useArea_size.height)));

	m_pRouteList = CHmsRouteElementsList::Create(Size(useArea_size.width * 0.618, useArea_size.height), 0);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pRouteList)
#endif
	{
		m_pRouteList->SetAnchorPoint(Vec2(0, 1));
		m_pRouteList->SetPosition(Vec2(useArea_pos.x + useArea_size.width * 0.382, useArea_pos.y + useArea_size.height));
		m_pRouteList->SetEnableEvent(false);
		m_pRouteList->SetToolBtnVisible(false);
		this->AddChild(m_pRouteList);
	}

	this->RegisterAllNodeChild();

	return true;
}

void CHmsHistoryRouteDlg::InitListBox(const HmsAviPf::Rect & windowRect)
{
	auto boxSize = windowRect.size;

	auto pListBox = CHmsListBox::CreateWithImage(
		NULL,
		NULL,
		"res/airport/blue_selbox.png",
		"res/airport/star_normal.png",
		NULL);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pListBox)
		return;
#endif
	pListBox->SetBoxSize(boxSize);
	pListBox->SetScrollbarWidth(4);
	pListBox->SetBacktopButtonVisible(false);
	pListBox->SetSelectWithCheck(true);
	pListBox->SetItemHeight(95);
	pListBox->SetAnchorPoint(Vec2(0, 0));
	pListBox->SetPosition(windowRect.origin);
	pListBox->SetItemNormalTextColor(Color4B::BLACK);
	pListBox->SetItemSelectChangeCallBack(HMS_CALLBACK_3(CHmsHistoryRouteDlg::OnListBoxItemClick, this));
	this->AddChild(pListBox);

	m_pListBox = pListBox;
}

void CHmsHistoryRouteDlg::OnListBoxItemClick(CHmsListBoxItem *pItem, int index, bool selected)
{
	if (selected)
	{
		if (index >= 0 && index < (int)m_vecArray.size())
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (!m_pRouteList)
				return;
#endif
			m_pRouteList->SetWptNodeData(m_vecArray[index]);
		}
	}
}

void CHmsHistoryRouteDlg::SaveRoute(const std::vector<FPL2DNodeInfoStu> &vecFPL, std::string &name)
{
	//3秒内只能保存一次
	if (m_time.GetElapsed() < 3)
	{
		return;
	}
	m_time.RecordCurrentTime();

	HistoryRouteStu stu;
	stu.name = name;
	stu.vecPoints.resize(vecFPL.size());
    for (int i = 0; i < (int)vecFPL.size(); ++i)
	{
		ConvertFPLNodeToHistory(vecFPL[i], stu.vecPoints[i]);
	}

	if (stu.name.size() > 0 && stu.vecPoints.size() > 0)
	{
		CHmsHistoryRouteDatabase::GetInstance()->SaveHistoryRoute(stu);
	}
}

void CHmsHistoryRouteDlg::OpenDialog()
{
	m_vecArray.clear();

	CHmsHistoryRouteDatabase::GetInstance()->GetHistoryRoute(m_vecArray);
	UpdateListBox();

	this->SetVisible(true);
}

void CHmsHistoryRouteDlg::ConvertFPLNodeToHistory(const FPL2DNodeInfoStu &node, HistoryRouteOnePointStu &hisStu)
{
	if (node.ePriType == FPL_TYPE_POINT)
	{
		const auto &brief = node.vWayPoint[0].brief;
		hisStu.ePriType = FPL_TYPE_POINT;
		hisStu.eSubType = FPL_TYPE_POINT;
		hisStu.pointAttr.type = brief.wType;
		hisStu.pointAttr.lon = brief.lon;
		hisStu.pointAttr.lat = brief.lat;
		//strcpy(hisStu.pointAttr.ident, brief.ident.c_str());
		hisStu.pointAttr.SetIdent(brief.ident);
		hisStu.pointAttr.altStu[0] = brief.attr.altStu[0];
		hisStu.pointAttr.altStu[1] = brief.attr.altStu[1];
		hisStu.pointAttr.searchShape = brief.attr.searchShape;
	}
	else if (node.ePriType == FPL_TYPE_PROGRAM)
	{
		hisStu.ePriType = node.ePriType;
		hisStu.eSubType = node.eSubType;
		//strcpy(hisStu.programAttr.belongAirport, node.belongAirport.c_str());
		//strcpy(hisStu.programAttr.programName, node.programName.c_str());
		//strcpy(hisStu.programAttr.transName, node.transName.c_str());
		//strcpy(hisStu.programAttr.rnwName, node.rnwName.c_str());
		hisStu.programAttr.SetBelongAirport(node.belongAirport);
		hisStu.programAttr.SetProgramName(node.programName);
		hisStu.programAttr.SetTransName(node.transName);
		hisStu.programAttr.SetRnwName(node.rnwName);
	}
}

void CHmsHistoryRouteDlg::UpdateListBox()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pListBox)
		return;
#endif
	m_pListBox->Clear();
	m_pRouteList->SetWptNodeData(HistoryRouteStu());

	for (auto &c : m_vecArray)
	{
		m_pListBox->AppendItem(c.name.c_str(), false, false, 32, true);
	}
}

void CHmsHistoryRouteDlg::DeleteRoute()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pListBox)
		return;
#endif
	auto index = m_pListBox->GetCurSelectIndex();
	if (index < 0 || index >= (int)m_vecArray.size())
	{
		return;
	}
	CHmsHistoryRouteDatabase::GetInstance()->DeleteHistoryRoute(m_vecArray[index].id);

	m_vecArray.erase(m_vecArray.begin() + index);
	UpdateListBox();
}

void CHmsHistoryRouteDlg::UseRoute()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pListBox)
		return;
#endif
	auto index = m_pListBox->GetCurSelectIndex();
	if (index < 0 || index >= (int)m_vecArray.size())
	{
		return;
	}
    if (!CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
	{
		return;
	}

    CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ClearAll();

	auto &stu = m_vecArray[index];
	for (auto &c : stu.vecPoints)
	{
		if (c.ePriType == FPL_TYPE_POINT)
		{
			WptNodeBrief brief;
			brief.wType = c.pointAttr.type;
			brief.lon = c.pointAttr.lon;
			brief.lat = c.pointAttr.lat;
			brief.ident = c.pointAttr.ident;
			brief.attr.altStu[0] = c.pointAttr.altStu[0];
			brief.attr.altStu[1] = c.pointAttr.altStu[1];
			brief.attr.searchShape = c.pointAttr.searchShape;

            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertWptNode(brief, FPLNodeIndex(), false);
		}
	}
	for (auto &c : stu.vecPoints)
	{
		if (c.ePriType == FPL_TYPE_PROGRAM)
		{
            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->InsertProgram(
				c.programAttr.belongAirport, 
				c.programAttr.programName, 
				c.programAttr.transName, 
				c.programAttr.rnwName,
				c.eSubType);
		}
	}

	CloseDialog();
}

void CHmsHistoryRouteDlg::CloseDialog()
{
	this->SetVisible(false);
}

std::string CHmsHistoryRouteDlg::GetRouteContentStr(const HistoryRouteStu &stu)
{
	std::string str;
	for (const auto &e : stu.vecPoints)
	{
		str += e.GetIdent() + "      ";
	}
	return str;
}

bool CHmsHistoryRouteDlg::OnPressed(const Vec2 & posOrigin)
{
	HmsUiEventInterface::OnPressed(posOrigin);
	return true;
}
