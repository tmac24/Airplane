#include "HmsScratchPad.h"

CHmsScratchPad::CHmsScratchPad()
: HmsUiEventInterface(this)
{

}

CHmsScratchPad::~CHmsScratchPad()
{

}

bool CHmsScratchPad::Init(const HmsAviPf::Size & size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	this->SetContentSize(size);

	m_bgDrawNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_bgDrawNode)
#endif
	{
		m_bgDrawNode->DrawSolidRect(Rect(Vec2(0, 0), size), Color4F::BLACK);
		this->AddChild(m_bgDrawNode);
	}

	m_rightToolPanel = CHmsUiPanel::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_rightToolPanel)
#endif
	{
		m_rightToolPanel->SetContentSize(Size(100, size.height));
		m_rightToolPanel->SetPosition(this->GetContentSize().width - 100, 0);
		this->AddChild(m_rightToolPanel);
	}

	m_rightToolPreview = CHmsUiPanel::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_rightToolPreview)
#endif
	{
		m_rightToolPreview->SetContentSize(Size(100, size.height));
		m_rightToolPreview->SetPosition(this->GetContentSize().width - 100, 0);
		this->AddChild(m_rightToolPreview);
	}

#if _NAV_SVS_LOGIC_JUDGE
	HmsAviPf::Size sizeShow;
	if (m_rightToolPanel)
		sizeShow = size - Size(m_rightToolPanel->GetContentSize().width, 0);
#else
	auto sizeShow = size - Size(m_rightToolPanel->GetContentSize().width, 0);
#endif
	

	m_scratchPadClipping = CHmsScratchPadClipping::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_scratchPadClipping)
#endif
	{
		auto nodeMask = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
		if (nodeMask)
#endif
		{
			nodeMask->DrawSolidRect(Rect(Vec2(), sizeShow));
			m_scratchPadClipping->setStencil(nodeMask);
		}
		m_scratchPadClipping->SetContentSize(sizeShow);

		m_scratchPadDraw = CHmsScratchPadDraw::Create();
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_scratchPadDraw)
			return false;
#endif
		m_scratchPadClipping->AddChildWithEvent(m_scratchPadDraw, true);

		this->AddChild(m_scratchPadClipping);
	}

	InitRightToolButton();

	m_preview = CHmsScratchPadPreview::Create(sizeShow);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_preview)
#endif
	{
		this->AddChild(m_preview);
		m_preview->SetCallbackFunc(std::bind(&CHmsScratchPad::ShowThisPage, this, std::placeholders::_1, ScratchPadPage::PAGE_DRAW));
		m_preview->SetCallbackFuncOnDel(std::bind(&CHmsScratchPad::ShowThisPage, this, ScratchPadInfoStu(), ScratchPadPage::PAGE_PREVIEW));
	}

	m_addDlg = CHmsScratchPadAddDlg::Create(Size(600, 600), 330);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_addDlg)
#endif
	{
		m_addDlg->SetAnchorPoint(0, 0);
		m_addDlg->SetPosition(
			sizeShow.width - m_addDlg->GetContentSize().width,
			sizeShow.height - m_addDlg->GetContentSize().height);
		m_addDlg->SetVisible(false);
		this->AddChild(m_addDlg);
		m_addDlg->SetCallback([=](ScratchPadBgType bgType){

			m_scratchPadDraw->SaveDataToDb();

			ScratchPadInfoStu stu;
			stu.bgType = bgType;
			ShowThisPage(stu, ScratchPadPage::PAGE_DRAW);
		});
	}

	m_colorDlg = CHmsScratchPadColorDlg::Create(Size(600, 700), 230);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_colorDlg)
#endif
	{
		m_colorDlg->SetAnchorPoint(0, 0);
		m_colorDlg->SetPosition(
			sizeShow.width - m_colorDlg->GetContentSize().width,
			sizeShow.height - m_colorDlg->GetContentSize().height - 110);
		m_colorDlg->SetVisible(false);
		this->AddChild(m_colorDlg);
		m_colorDlg->SetCallback([=](int lineWidth, Color4B lineColor){

			m_scratchPadDraw->SetLineWidthColor(lineWidth, lineColor);

		});
	}

	{
		m_pClearAllButton = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
			Size(260, 95), Color3B(0x1e, 0x37, 0x4f), Color3B(0x1e, 0x37, 0x4f));
#if _NAV_SVS_LOGIC_JUDGE
		if (m_pClearAllButton)
#endif
		{
			m_pClearAllButton->AddText("Clear All", 32, Color4B::RED);
			m_pClearAllButton->SetAnchorPoint(Vec2(1, 0.5));
			m_pClearAllButton->SetPosition(Vec2(sizeShow.width - 10, sizeShow.height - 700));
			m_pClearAllButton->SetOnClickedFunc([=](CHmsUiButtonAbstract *pBtn){

				m_scratchPadDraw->ClearDrawContent();
				m_pClearAllButton->SetVisible(false);
			});
			this->AddChild(m_pClearAllButton);

			auto pClearAllBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
			if (!pClearAllBgNode)
				return false;
#endif
			pClearAllBgNode->SetContentSize(Size(10, 16));
			pClearAllBgNode->SetAnchorPoint(Vec2(0, 0.5));

			auto clearAllSize = m_pClearAllButton->GetContentSize();
			pClearAllBgNode->SetPosition(clearAllSize.width, clearAllSize.height / 2);
			std::vector<Vec2> vec;
			vec.push_back(Vec2(0, 0));
			vec.push_back(Vec2(10, 8));
			vec.push_back(Vec2(0, 16));
			pClearAllBgNode->DrawTriangles(vec, Color4F(Color3B(0x1e, 0x37, 0x4f)));
			m_pClearAllButton->AddChild(pClearAllBgNode);
		}
	}

	this->RegisterAllNodeChild();

	ShowThisPage(ScratchPadInfoStu(), ScratchPadPage::PAGE_PREVIEW);

	return true;
}

void CHmsScratchPad::InitRightToolButton()
{
	auto CreateButton = [this](CHmsNode *pParent, int index, const char* str, std::function<void(CHmsUiButtonAbstract*, HMS_UI_BUTTON_STATE)> func)
	{
#if _NAV_SVS_LOGIC_JUDGE
		HmsAviPf::Size toolSize;
		if (pParent)
			toolSize = pParent->GetContentSize();
#else
		auto toolSize = pParent->GetContentSize();
#endif
		
        int btnCount = 9;
        auto s = Size(toolSize.width, toolSize.height / btnCount - 6);
		auto pToolBtn = CHmsFixSizeUiImageButton::CreateWithImage(str, s, func);
#if _NAV_SVS_LOGIC_JUDGE
		if (pToolBtn && pParent)
#endif
		{
			pToolBtn->SetAnchorPoint(Vec2(0.5, 0.5));
			pToolBtn->SetPosition(
				toolSize.width / 2,
				toolSize.height - (toolSize.height / btnCount / 2 + index*toolSize.height / btnCount));

			pParent->AddChild(pToolBtn);
		}

		return pToolBtn;
	};

#define DEF_CALLBACK(btnType) std::bind(&CHmsScratchPad::ToolBtnCallback, this, std::placeholders::_1, std::placeholders::_2, btnType)  

	int index = 0;

	CHmsFixSizeUiImageButton *pButton = nullptr;
#if _NAV_SVS_LOGIC_JUDGE
	if (m_rightToolPanel)
#endif
	{
		CreateButton(m_rightToolPanel, index++, "res/NavImage/close.png", DEF_CALLBACK(ToolBtnEnum::BTN_CLOSE));
		CreateButton(m_rightToolPanel, index++, "res/Scratchpad/userAdd.png", DEF_CALLBACK(ToolBtnEnum::BTN_ADD));
		pButton = CreateButton(m_rightToolPanel, index++, "res/Scratchpad/draw.png", DEF_CALLBACK(ToolBtnEnum::BTN_DRAW));
#if _NAV_SVS_LOGIC_JUDGE
		if (pButton)
#endif	
		pButton->SetToggleBtn(true);
		CreateButton(m_rightToolPanel, index++, "res/NavImage/pencolor.png", DEF_CALLBACK(ToolBtnEnum::BTN_PENCOLOR));
		//pButton = CreateButton(m_rightToolPanel, index++, "res/airport/imagePage/full_screen.png", DEF_CALLBACK(ToolBtnEnum::BTN_FULLSCREEN));
		//pButton->SetToggleBtn(true);
		CreateButton(m_rightToolPanel, index++, "res/NavImage/clear.png", DEF_CALLBACK(ToolBtnEnum::BTN_CLEAR));
		CreateButton(m_rightToolPanel, index++, "res/Scratchpad/undo.png", DEF_CALLBACK(ToolBtnEnum::BTN_UNDO));
		CreateButton(m_rightToolPanel, index++, "res/Scratchpad/redo.png", DEF_CALLBACK(ToolBtnEnum::BTN_REDO));
		CreateButton(m_rightToolPanel, index++, "res/Scratchpad/zoomIn.png", DEF_CALLBACK(ToolBtnEnum::BTN_ZOOMIN));
		CreateButton(m_rightToolPanel, index++, "res/Scratchpad/zoomOut.png", DEF_CALLBACK(ToolBtnEnum::BTN_ZOOMOUT));
		m_rightToolPanel->RegisterAllNodeChild();
	}


#if _NAV_SVS_LOGIC_JUDGE
	if (m_rightToolPreview)
#endif
	{
		CreateButton(m_rightToolPreview, 0, "res/Scratchpad/userAdd.png", DEF_CALLBACK(ToolBtnEnum::BTN_ADD));
		CreateButton(m_rightToolPreview, 1, "res/Scratchpad/delete.png", DEF_CALLBACK(ToolBtnEnum::BTN_EDIT));
		m_rightToolPreview->RegisterAllNodeChild();
	}
}

void CHmsScratchPad::ToolBtnCallback(CHmsUiButtonAbstract *btn, HMS_UI_BUTTON_STATE eState, ToolBtnEnum btnType)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (btn)
#endif
	ChangeBtnColor(btn, eState);

	if (btnType != ToolBtnEnum::BTN_ADD)
	{
#if _NAV_SVS_LOGIC_JUDGE
		if (m_addDlg)
#endif
		m_addDlg->SetVisible(false);
	}
	if (btnType != ToolBtnEnum::BTN_PENCOLOR)
	{
#if _NAV_SVS_LOGIC_JUDGE
		if (m_colorDlg)
#endif
		m_colorDlg->SetVisible(false);
	}
	if (btnType != ToolBtnEnum::BTN_CLEAR)
	{
#if _NAV_SVS_LOGIC_JUDGE
		if (m_pClearAllButton)
#endif
		m_pClearAllButton->SetVisible(false);
	}

	switch (btnType)
	{
	case ToolBtnEnum::BTN_ADD:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_addDlg)
#endif
			m_addDlg->SetVisible(!m_addDlg->IsVisible());
		}
		break;
	case ToolBtnEnum::BTN_DRAW:
		if (eState == HMS_UI_BUTTON_STATE::btn_selected)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadClipping)
#endif
			m_scratchPadClipping->SetEventMode(ClippingEventMode::E_TRANSMIT);
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadDraw)
#endif
			m_scratchPadDraw->SetEnableDraw(true);
		}
		else if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadClipping)
#endif
			m_scratchPadClipping->SetEventMode(ClippingEventMode::E_DRAG);
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadDraw)
#endif
			m_scratchPadDraw->SetEnableDraw(false);
		}
		break;
	case ToolBtnEnum::BTN_PENCOLOR:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			int width;
			Color4B color;
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadDraw)
#endif
			m_scratchPadDraw->GetCurLineWidthAndColor(width, color);
#if _NAV_SVS_LOGIC_JUDGE
			if (m_colorDlg)
#endif
			{
				m_colorDlg->SetLineWidthAndColor(width, color);
				m_colorDlg->SetVisible(!m_colorDlg->IsVisible());
			}
		}
		
		break;
	case ToolBtnEnum::BTN_FULLSCREEN:

		if (eState == HMS_UI_BUTTON_STATE::btn_selected)
		{
			LayoutBySize(m_fullSize);
		}
		else if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			LayoutBySize(m_halfFullSize);
		}
		
		break;
	case ToolBtnEnum::BTN_CLEAR:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_pClearAllButton)
#endif
			m_pClearAllButton->SetVisible(!m_pClearAllButton->IsVisible());
		}
		break;
	case ToolBtnEnum::BTN_UNDO:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadDraw)
#endif
			m_scratchPadDraw->Undo();
		}
		break;
	case ToolBtnEnum::BTN_REDO:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadDraw)
#endif
			m_scratchPadDraw->Redo();
		}
		break;
	case ToolBtnEnum::BTN_CLOSE:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadDraw)
#endif
			m_scratchPadDraw->SaveDataToDb();

			ShowThisPage(ScratchPadInfoStu(), ScratchPadPage::PAGE_PREVIEW);
		}
		break;
	case ToolBtnEnum::BTN_ZOOMIN:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadDraw)
#endif
			m_scratchPadDraw->ZoomIn();
		}
		break;
	case ToolBtnEnum::BTN_ZOOMOUT:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadDraw)
#endif
			m_scratchPadDraw->ZoomOut();
#if _NAV_SVS_LOGIC_JUDGE
			if (m_scratchPadClipping)
#endif
			m_scratchPadClipping->EdgeDetection();
		}
		break;
	case ToolBtnEnum::BTN_EDIT:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
#if _NAV_SVS_LOGIC_JUDGE
			if (m_preview)
#endif
			m_preview->ToggleShowDelButton();
		}
		break;
	default:
		break;
	}
}

void CHmsScratchPad::ChangeBtnColor(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	auto pNode = dynamic_cast<CHmsNode*>(pBtn);
	switch (state)
	{
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_normal:
		if (pNode)
		{
			pNode->SetColor(Color3B::WHITE);
			pNode->SetScale(1.0f);
		}
		break;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected:
		if (pNode)
		{
			pNode->SetColor(Color3B(0, 71, 157));
			pNode->SetScale(1.1f);
		}
		break;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_disable:
		break;
	default:
		break;
	}
}

void CHmsScratchPad::SetHalfAndFullSize(Size half, Size full)
{
	m_halfFullSize = half;
	m_fullSize = full;

    LayoutBySize(full);
}

void CHmsScratchPad::LayoutBySize(Size size)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_scratchPadClipping 
		|| !m_scratchPadDraw 
		|| !m_bgDrawNode
		|| !m_rightToolPanel
		|| !m_pClearAllButton
		|| !m_rightToolPreview
		|| !m_addDlg
		|| !m_colorDlg
		|| !m_preview)
		return;
#endif	
	auto sizeShow = size - Size(m_rightToolPanel->GetContentSize().width, 0);
	auto nodeMask = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (nodeMask)
#endif
	{
		nodeMask->DrawSolidRect(Rect(Vec2(), sizeShow));
		m_scratchPadClipping->setStencil(nodeMask);
	}
	
	m_scratchPadClipping->SetContentSize(sizeShow);

    m_scratchPadDraw->SetScale(sizeShow.height / m_scratchPadDraw->GetContentSize().height);
	m_scratchPadDraw->SetPosition((sizeShow.width - m_scratchPadDraw->GetContentSize().width * m_scratchPadDraw->GetScale()) *0.5, 0);

	this->SetContentSize(size);
	this->SetPositionX(m_fullSize.width - size.width);

	m_bgDrawNode->clear();
	m_bgDrawNode->DrawSolidRect(Rect(Vec2(0, 0), size), Color4F::BLACK);
	m_rightToolPanel->SetPosition(this->GetContentSize().width - 100, 0);

	m_pClearAllButton->SetPosition(Vec2(sizeShow.width - 10, sizeShow.height - 730));

	m_rightToolPreview->SetPosition(this->GetContentSize().width - 100, 0);

	m_scratchPadClipping->EdgeDetection();

	m_addDlg->SetPosition(
		sizeShow.width - m_addDlg->GetContentSize().width, 
		sizeShow.height - m_addDlg->GetContentSize().height);

	m_colorDlg->SetPosition(
		sizeShow.width - m_colorDlg->GetContentSize().width,
		sizeShow.height - m_colorDlg->GetContentSize().height - 110);

	m_preview->LayoutBySize(sizeShow);

// 	if (m_preview->IsVisible())
// 	{
// 		ShowThisPage(ScratchPadInfoStu(), ScratchPadPage::PAGE_PREVIEW);
// 	}
}

void CHmsScratchPad::ShowThisPage(ScratchPadInfoStu stu, ScratchPadPage page)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pClearAllButton
		|| !m_scratchPadClipping
		|| !m_preview
		|| !m_rightToolPanel
		|| !m_rightToolPreview
		|| !m_scratchPadDraw
		|| !m_colorDlg)
		return;
#endif
	m_pClearAllButton->SetVisible(false);

	switch (page)
	{
	case ScratchPadPage::PAGE_DRAW:
	{
		m_scratchPadClipping->SetVisible(true);
		m_preview->SetVisible(false);
		m_rightToolPanel->SetVisible(true);
		m_rightToolPreview->SetVisible(false);

		m_scratchPadDraw->SetScratchPadInfoStu(stu);

		m_colorDlg->SetLineWidthAndColor(stu.lineWidth, stu.lineColor);
	}
		break;
	case ScratchPadPage::PAGE_PREVIEW:
	{
		if (!m_preview->IsVisible())
		{
			m_preview->SetShowDelButton(false);
		}
		m_scratchPadClipping->SetVisible(false);
		m_preview->SetVisible(true);
		m_rightToolPanel->SetVisible(false);
		m_rightToolPreview->SetVisible(true);

		auto sizeShow = this->GetContentSize() - Size(m_rightToolPanel->GetContentSize().width, 0);
		CHmsScratchPadSql &sql = CHmsScratchPadSql::GetInstance();
		std::vector<ScratchPadInfoStu> vec = sql.GetScratchPadInfoStu();
		//m_preview->LayoutBySize(sizeShow);
		m_preview->SetScratchPadInfoStu(vec);
	}
		break;
	default:
		break;
	}
}

void CHmsScratchPad::Resize(Size size)
{
	LayoutBySize(size);
}
