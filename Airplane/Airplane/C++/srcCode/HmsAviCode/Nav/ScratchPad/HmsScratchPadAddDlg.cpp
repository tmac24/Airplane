#include "HmsScratchPadAddDlg.h"

CHmsScratchPadAddDlg::CHmsScratchPadAddDlg()
: HmsUiEventInterface(this)
, m_callback(nullptr)
{

}

CHmsScratchPadAddDlg::~CHmsScratchPadAddDlg()
{

}

CHmsScratchPadAddDlg* CHmsScratchPadAddDlg::Create(const HmsAviPf::Size & size, float trianglePosY)
{
    CHmsScratchPadAddDlg *pRet = new CHmsScratchPadAddDlg();
    if (pRet && pRet->Init(size, trianglePosY))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool CHmsScratchPadAddDlg::Init(const HmsAviPf::Size & windowSize, float trianglePosY)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	const float triangleHeight = 20;

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
			vec.push_back(Vec2(windowSize.width - triangleHeight, trianglePosY));
			vec.push_back(Vec2(windowSize.width, trianglePosY + triangleHeight * 0.8));
			vec.push_back(Vec2(windowSize.width - triangleHeight, trianglePosY + triangleHeight * 0.8 * 2));

			pBgNode->DrawTriangles(vec, Color4F(Color3B(0x0e, 0x19, 0x25)));
		}
		{
			std::vector<Vec2> vec;
			vec.push_back(Vec2(0, 0));
			vec.push_back(Vec2(windowSize.width - triangleHeight, 0));
			vec.push_back(Vec2(windowSize.width - triangleHeight, trianglePosY));
			vec.push_back(Vec2(windowSize.width, trianglePosY + triangleHeight * 0.8));
			vec.push_back(Vec2(windowSize.width - triangleHeight, trianglePosY + triangleHeight * 0.8 * 2));
			vec.push_back(Vec2(windowSize.width - triangleHeight, windowSize.height));
			vec.push_back(Vec2(0, windowSize.height));
			pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
		}
	}

	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
			Size(260, 95), Color3B(0x1e, 0x37, 0x4f), Color3B::BLACK);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText("Cancel", 32);
			pItem->SetAnchorPoint(Vec2(0.5, 0));
			pItem->SetPosition(Vec2((windowSize.width - triangleHeight)*0.5, 30));
			pItem->SetOnClickedFunc(
				[=](CHmsUiButtonAbstract *btn){
				this->SetVisible(false);
			});
			this->AddChild(pItem);
		}
	}

	InitListBox(Rect(20, 150, windowSize.width - triangleHeight - 40, windowSize.height - 180));

	this->RegisterAllNodeChild();

	return true;
}

void CHmsScratchPadAddDlg::InitListBox(const HmsAviPf::Rect & winsowRect)
{
	auto boxSize = winsowRect.size;

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
			pListBox->SetBoxSize(boxSize);
			pListBox->SetScrollbarWidth(4);
			pListBox->SetBacktopButtonVisible(false);
			pListBox->SetSelectWithCheck(true);
			pListBox->SetItemHeight(95);
			pListBox->SetAnchorPoint(Vec2(0, 0));
			pListBox->SetPosition(winsowRect.origin);
			pListBox->SetItemSelectChangeCallBack([=](CHmsListBoxItem *pItem, int index, bool selected){

				if (m_callback && selected && index >= 0)
				{
					ScratchPadBgType arr[] = {
						ScratchPadBgType::TYPE_PIREP,
						ScratchPadBgType::TYPE_ATIS,
						ScratchPadBgType::TYPE_CRAFT,
						ScratchPadBgType::TYPE_BLANK
					};

					m_callback(arr[index]);

					this->SetVisible(false);
				}
			});
			pListBox->AppendItem("PIREP", true, false, 32, true);
			pListBox->AppendItem("ATIS", true, false, 32, true);
			pListBox->AppendItem("CRAFT", true, false, 32, true);
			pListBox->AppendItem("BLANK", true, false, 32, true);
			pListBox->SelectItem(0);
		}
	}
}


