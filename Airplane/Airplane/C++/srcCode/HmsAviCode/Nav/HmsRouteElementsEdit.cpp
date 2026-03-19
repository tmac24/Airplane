#include "HmsRouteElementsEdit.h"
#include "HmsFrame2DRoot.h"
#include "Language/HmsLanguageMananger.h"

CHmsRouteElementsEdit::CHmsRouteElementsEdit()
: HmsUiEventInterface(this)
{

}

CHmsRouteElementsEdit::~CHmsRouteElementsEdit()
{

}

bool CHmsRouteElementsEdit::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	m_pRouteList = CHmsRouteElementsList::Create(windowSize - Size(215,10), 115);
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteList)
		return false;
#endif
	this->AddChild(m_pRouteList);
	m_pRouteList->SetAnchorPoint(Vec2(0, 0));
	m_pRouteList->SetPosition(Vec2(5, 5));


	auto CreateButton = [=](const char *text, int index)
	{
		Size buttonSize;
		buttonSize.width = 200;
		buttonSize.height = (windowSize.height - 10 - 10) / 3;

		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", 
			buttonSize, Color3B(0x0e, 0x19, 0x25), Color3B::GRAY);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(text, 30, Color4B::WHITE);
			pItem->SetAnchorPoint(Vec2(0, 1));
			pItem->SetPosition(Vec2(windowSize.width - 205, windowSize.height - 5 - (buttonSize.height + 5) * index));
			this->AddChild(pItem);
		}
		return pItem;
	};

	CHmsUiStretchButton *pItem = nullptr;
	pItem = CreateButton("Procedures", 0);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
        if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
		{
            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->ShowProcedureDlg();
		}
	});

	pItem = CreateButton("Save Route", 1);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
        if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
		{
            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->SaveRoute();
		}
	});

	pItem = CreateButton("History Route", 2);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
        if (CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot())
		{
            CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->OpenHistoryRouteDlg();
		}
	});

	this->RegisterAllNodeChild();

	return true;
}

void CHmsRouteElementsEdit::SetWptNodeData(const std::vector<FPL2DNodeInfoStu> &vec)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteList)
		return;
#endif
	m_pRouteList->SetWptNodeData(vec);
}
