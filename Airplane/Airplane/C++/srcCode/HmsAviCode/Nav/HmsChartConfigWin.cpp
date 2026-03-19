#include "HmsChartConfigWin.h"
#include "HmsNavComm.h"

CHmsChartConfigWin::CHmsChartConfigWin()
: HmsUiEventInterface(this)
, m_corridorCallback(nullptr)
, m_hazardCallback(nullptr)
{

}

CHmsChartConfigWin::~CHmsChartConfigWin()
{

}

bool CHmsChartConfigWin::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	float triangleHeight = 20;

	auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (pBgNode)
#endif
	{
		pBgNode->SetAnchorPoint(Vec2(0, 0));
		pBgNode->SetPosition(0, 0);
		this->AddChild(pBgNode);
		pBgNode->DrawSolidRect(
			Rect(0, 0, windowSize.width - triangleHeight, windowSize.height),
			Color4F(Color3B(0x0e, 0x19, 0x25)));
		{
			std::vector<Vec2> vec;
			vec.push_back(Vec2(windowSize.width - triangleHeight, 10));
			vec.push_back(Vec2(windowSize.width, 10 + triangleHeight * 0.8));
			vec.push_back(Vec2(windowSize.width - triangleHeight, 10 + triangleHeight * 0.8 * 2));

			pBgNode->DrawTriangles(vec, Color4F(Color3B(0x0e, 0x19, 0x25)));
		}
		{
			std::vector<Vec2> vec;
			vec.push_back(Vec2(0, 0));
			vec.push_back(Vec2(windowSize.width - triangleHeight, 0));
			vec.push_back(Vec2(windowSize.width - triangleHeight, 10));
			vec.push_back(Vec2(windowSize.width, 10 + triangleHeight * 0.8));
			vec.push_back(Vec2(windowSize.width - triangleHeight, 10 + triangleHeight * 0.8 * 2));
			vec.push_back(Vec2(windowSize.width - triangleHeight, windowSize.height));
			vec.push_back(Vec2(0, windowSize.height));
			pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
		}
	}

    auto toolButtonSize = Size(180, 70);
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
            toolButtonSize, Color3B(0x1e, 0x37, 0x4f), Color3B::WHITE);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText("Corridor Width", 20);
			pItem->SetAnchorPoint(Vec2(1, 0));
			pItem->SetPosition(Vec2(windowSize.width / 2, 10));
			pItem->SetOnClickedFunc(std::bind(&CHmsChartConfigWin::ShowPage, this, std::placeholders::_1, ChartCfgPage::PAGE_CORRIDOR));
			this->AddChild(pItem);
			m_pButtonCorridor = pItem;
		}
	}
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
            toolButtonSize, Color3B(0x1e, 0x37, 0x4f), Color3B::WHITE);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText("Hazard Altitudes", 20);
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetPosition(Vec2(windowSize.width / 2, 10));
			pItem->SetOnClickedFunc(std::bind(&CHmsChartConfigWin::ShowPage, this, std::placeholders::_1, ChartCfgPage::PAGE_ALTITUDE));
			this->AddChild(pItem);
			m_pButtonAltitude = pItem;
		}
	}

    InitListBox(Rect(0, toolButtonSize.height + 20, windowSize.width - triangleHeight, windowSize.height - (toolButtonSize.height + 20)));

#if _NAV_SVS_LOGIC_JUDGE
	if (m_pButtonCorridor)
#endif
	ShowPage(m_pButtonCorridor, ChartCfgPage::PAGE_CORRIDOR);

	this->RegisterAllNodeChild();

	return true;
}

void CHmsChartConfigWin::InitListBox(const HmsAviPf::Rect & winsowRect)
{
	auto boxSize = winsowRect.size;
    float fontSize = 26;
	{
		auto pListBox = CHmsListBox::CreateWithImage(
			NULL,
			NULL,
			"res/airport/blue_selbox.png",
			"res/airport/star_normal.png",
			"res/airport/star_checked.png");
#if _NAV_SVS_LOGIC_JUDGE
		if (pListBox)
#endif
		{
			this->AddChild(pListBox);
			m_pListCorridor = pListBox;
			pListBox->SetBoxSize(boxSize);
			pListBox->SetScrollbarWidth(4);
			pListBox->SetBacktopButtonVisible(false);
			pListBox->SetSelectWithCheck(true);
			pListBox->SetItemHeight(70);
			pListBox->SetAnchorPoint(Vec2(0, 0));
			pListBox->SetPosition(winsowRect.origin);
			pListBox->SetItemSelectChangeCallBack([=](CHmsListBoxItem *pItem, int index, bool selected){

				if (m_corridorCallback && selected && index >= 0)
				{
					float arr[] = { 0.5, 1, 2, 4, 6, 8, 20 };
					m_corridorCallback(arr[index]);
				}
			});
			if (CHmsNavComm::GetInstance()->IsMetric())
			{
				pListBox->AppendItem("1/2 km", true, true, fontSize);
				pListBox->AppendItem("1 km", true, true, fontSize);
				pListBox->AppendItem("2 km", true, true, fontSize);
				pListBox->AppendItem("4 km", true, true, fontSize);
				pListBox->AppendItem("6 km", true, true, fontSize);
				pListBox->AppendItem("8 km", true, true, fontSize);
				pListBox->AppendItem("20 km", true, true, fontSize);
			}
			else
			{
				pListBox->AppendItem("1/2 nm", true, true, fontSize);
				pListBox->AppendItem("1 nm", true, true, fontSize);
				pListBox->AppendItem("2 nm", true, true, fontSize);
				pListBox->AppendItem("4 nm", true, true, fontSize);
				pListBox->AppendItem("6 nm", true, true, fontSize);
				pListBox->AppendItem("8 nm", true, true, fontSize);
				pListBox->AppendItem("20 nm", true, true, fontSize);
			}

			pListBox->SelectItem(0);
		}
	}
	{
		auto pListBox = CHmsListBox::CreateWithImage(
			NULL,
			NULL,
			"res/airport/blue_selbox.png",
			"res/airport/star_normal.png",
			"res/airport/star_checked.png");
#if _NAV_SVS_LOGIC_JUDGE
		if (pListBox)
#endif
		{
			this->AddChild(pListBox);
			m_pListAltitude = pListBox;
			pListBox->SetBoxSize(boxSize);
			pListBox->SetScrollbarWidth(4);
			pListBox->SetBacktopButtonVisible(false);
			pListBox->SetSelectWithCheck(true);
			pListBox->SetItemHeight(70);
			pListBox->SetAnchorPoint(Vec2(0, 0));
			pListBox->SetPosition(winsowRect.origin);
#if 0
			pListBox->SetItemSelectChangeCallBack([=](CHmsListBoxItem *pItem, int index, bool selected){

				if (m_hazardCallback && selected && index >= 0)
				{
					float arr[] = {100,1000, 50,300, 25,200, 25,100};
					m_hazardCallback(arr[index * 2], arr[index * 2 + 1]);
				}
			});
			pListBox->AppendItem("Normal: 100' / 1000'", true, true, fontSize);
			pListBox->AppendItem("Heli-Normal: 50' / 300'", true, true, fontSize);
			pListBox->AppendItem("Heli-Medium: 25' / 200'", true, true, fontSize);
			pListBox->AppendItem("Heli-Low: 25' / 100'", true, true, fontSize);
#else
			pListBox->SetItemSelectChangeCallBack([=](CHmsListBoxItem *pItem, int index, bool selected){

				if (m_hazardCallback && selected && index >= 0)
				{
					float arr[] = { 100, 1000, 50, 300, 25, 100 };
					m_hazardCallback(arr[index * 2], arr[index * 2 + 1]);
				}
			});
			if (CHmsNavComm::GetInstance()->IsMetric())
			{
				pListBox->AppendItem("100m / 1000m", true, true, fontSize);
				pListBox->AppendItem("50m / 300m", true, true, fontSize);
				pListBox->AppendItem("25m / 100m", true, true, fontSize);
			}
			else
			{
				pListBox->AppendItem("100ft / 1000ft", true, true, fontSize);
				pListBox->AppendItem("50ft / 300ft", true, true, fontSize);
				pListBox->AppendItem("25ft / 100ft", true, true, fontSize);
			}

#endif

			pListBox->SelectItem(0);
		}
	}
}

void CHmsChartConfigWin::ShowPage(CHmsUiButtonAbstract *pBtn, ChartCfgPage page)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pListCorridor || !m_pListAltitude || !m_pButtonCorridor || !m_pButtonAltitude)
		return;
#endif
	switch (page)
	{
	case ChartCfgPage::PAGE_CORRIDOR:
		m_pListCorridor->SetVisible(true);
		m_pButtonCorridor->GetLabel()->SetTextColor(Color4B::BLACK);
		m_pButtonCorridor->Selected();

		m_pListAltitude->SetVisible(false);
		m_pButtonAltitude->GetLabel()->SetTextColor(Color4B::WHITE);
		m_pButtonAltitude->UnSelected();
		break;
	case ChartCfgPage::PAGE_ALTITUDE:
		m_pListCorridor->SetVisible(false);
		m_pButtonCorridor->GetLabel()->SetTextColor(Color4B::WHITE);
		m_pButtonCorridor->UnSelected();

		m_pListAltitude->SetVisible(true);
		m_pButtonAltitude->GetLabel()->SetTextColor(Color4B::BLACK);
		m_pButtonAltitude->Selected();
		break;
	default:
		break;
	}
}

void CHmsChartConfigWin::ResetValue()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pListCorridor || !m_pListAltitude)
		return;
#endif
	m_pListCorridor->SelectItem(0);
	m_pListAltitude->SelectItem(0);
}

bool CHmsChartConfigWin::OnPressed(const Vec2 & posOrigin)
{
	HmsUiEventInterface::OnPressed(posOrigin);
	return true;
}
