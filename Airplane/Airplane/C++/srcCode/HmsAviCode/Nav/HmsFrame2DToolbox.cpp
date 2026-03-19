#include "HmsFrame2DToolbox.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsFrame2DRoot.h"

CHmsFrame2DToolbox::CHmsFrame2DToolbox()
: HmsUiEventInterface(this)
, m_pFrame2DRoot(nullptr)
{

}

CHmsFrame2DToolbox::~CHmsFrame2DToolbox()
{

}

bool CHmsFrame2DToolbox::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	auto CreateButton = [=](Vec2 pos, const char * text, const char *imagePath, const char *imageSelect)
	{
		auto pItem = CHmsUiButton::CreateWithImage(imagePath, imageSelect);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(text, 28, Color4B::WHITE, true);
			pItem->SetAnchorPoint(Vec2(0.5, 0.5));
			pItem->SetPosition(pos);
			this->AddChild(pItem);
		}
		return pItem;
	};

	auto btnSize = Size(100, 40);
	Vec2 circleCenter = windowSize / 2;
	float radius = 140;
	
	CHmsUiButton *pItem = nullptr;

	pItem = CreateButton(circleCenter + Vec2(-radius, 0), "Direct     To",
		"res/NavImage/SearchMode/btn_normal.png", "res/NavImage/SearchMode/btn_select.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetRotation(-90);
		pItem->GetLabel()->SetRotation(90);
		pItem->GetLabel()->SetMaxLineWidth(50);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			this->SetVisible(false);
			if (m_pFrame2DRoot)
			{
				m_pFrame2DRoot->DirectTo(m_curIndex);
			}
		});
	}

	pItem = CreateButton(circleCenter + Vec2(0, radius), "Search Mode",
		"res/NavImage/SearchMode/btn_normal.png", "res/NavImage/SearchMode/btn_select.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			this->SetVisible(false);
			if (m_pFrame2DRoot)
			{
				m_pFrame2DRoot->ShowSearchShape(m_curIndex);
			}
		});
	}

	pItem = CreateButton(circleCenter + Vec2(radius, 0), "Set Alt",
		"res/NavImage/SearchMode/btn_normal.png", "res/NavImage/SearchMode/btn_select.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (pItem)
#endif
	{
		pItem->SetRotation(90);
		pItem->GetLabel()->SetRotation(-90);
		pItem->GetLabel()->SetMaxLineWidth(30);
		pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
		{
			this->SetVisible(false);
			if (m_pFrame2DRoot)
			{
				m_pFrame2DRoot->ShowInputAltitude(m_curIndex);
			}
		});
	}
	

	this->RegisterAllNodeChild();

	return true;
}

void CHmsFrame2DToolbox::SetCurSelectIndex(FPLNodeIndex index)
{
	m_curIndex = index;
}

void CHmsFrame2DToolbox::SetFrame2DRoot(CHmsFrame2DRoot *p)
{
	m_pFrame2DRoot = p;
}
