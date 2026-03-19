#include "HmsScratchPadPreview.h"

CHmsScratchPadPreview::CHmsScratchPadPreview()
: HmsUiEventInterface(this)
, m_callbackFunc(nullptr)
, c_scale(0.6f)
, c_margin(20.0f)
, m_imageLocalZOrder(1000)
, m_buttonLocalZOrder(2000)
, m_bShowDelButton(false)
, m_curPanelCount(0)
#if _NAV_SVS_LOGIC_JUDGE
, m_callbackFuncOnDel(nullptr)
#endif
{

}

CHmsScratchPadPreview::~CHmsScratchPadPreview()
{

}

bool CHmsScratchPadPreview::Init(const HmsAviPf::Size & size)
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
		m_bgDrawNode->DrawSolidRect(Rect(Vec2(0, 0), size), Color4F(1.0f, 1.0f, 1.0f, 0.3f));
		this->AddChild(m_bgDrawNode);
	}


	m_scratchPadClipping = CHmsScratchPadClipping::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_scratchPadClipping)
#endif
	{
		this->AddChild(m_scratchPadClipping);
		auto nodeMask = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
		if (nodeMask)
#endif
		{
			nodeMask->DrawSolidRect(Rect(Vec2(), size));
			m_scratchPadClipping->setStencil(nodeMask);
		}
		m_scratchPadClipping->SetContentSize(size);
		m_scratchPadClipping->SetEventMode(ClippingEventMode::E_DRAG_CLICK);

		m_previewPanel = CHmsUiPanel::Create();
#if _NAV_SVS_LOGIC_JUDGE
		if (m_previewPanel)
#endif
		{
			m_previewPanel->SetAnchorPoint(Vec2(0, 0));
			m_scratchPadClipping->AddChildWithEvent(m_previewPanel, true);
		}
	}

	this->RegisterAllNodeChild();

	return true;
}

void CHmsScratchPadPreview::SetScratchPadInfoStu(const std::vector<ScratchPadInfoStu> &vecInfoStu)
{
	DetectNeedCreateOrNot(vecInfoStu.size());

	Size panelSize = this->GetContentSize();
	panelSize.height = 0;

	auto temp = CHmsScratchPadDraw::Create();
#if _NAV_SVS_LOGIC_JUDGE
	float width, height;
	if (temp)
	{
		width = temp->GetContentSize().width * c_scale;
		height = temp->GetContentSize().height * c_scale;
	}
#else
	auto width = temp->GetContentSize().width * c_scale;
	auto height = temp->GetContentSize().height * c_scale;
#endif
	int columnCount = panelSize.width / (width + c_margin);
    for (int i = 0; i < (int)vecInfoStu.size(); ++i)
	{
		int column = i % columnCount;
		if (0 == column)
		{
			panelSize.height += height + c_margin;
		}
	}

	int row = -1;
	auto iterButton = m_buttonMap.begin();
    for (int i = 0; i < (int)vecInfoStu.size(); ++i)
	{
		auto &stu = vecInfoStu.at(i);

		auto drawPanel = m_padDrawVector.at(i);
#if _NAV_SVS_LOGIC_JUDGE
		if (drawPanel)
#endif
		{
			drawPanel->SetScratchPadInfoStu(stu);
			drawPanel->SetVisible(true);

			int column = i % columnCount;
			if (0 == column)
			{
				++row;
			}
			drawPanel->SetPosition(Vec2(0, 0));
			drawPanel->SetPosition(column*(width + c_margin) + c_margin, panelSize.height - (height + c_margin)*(row + 1));

			CHmsFixSizeUiImageButton *pButton = iterButton->first;
			iterButton->second = stu.id;
			++iterButton;
			pButton->SetPosition(drawPanel->GetPoistion() + Vec2(10, height - 10));
			if (m_bShowDelButton)
			{
				pButton->SetVisible(true);
			}
		}
	}

#if _NAV_SVS_LOGIC_JUDGE
	if (m_previewPanel)
#endif
	m_previewPanel->SetContentSize(panelSize);
	//m_previewPanel->SetPosition(0, m_scratchPadClipping->GetContentSize().height - panelSize.height);
}

void CHmsScratchPadPreview::LayoutBySize(Size s)
{
	this->SetContentSize(s);

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_bgDrawNode || !m_scratchPadClipping)
		return;
#endif
	m_bgDrawNode->clear();
	m_bgDrawNode->DrawSolidRect(Rect(Vec2(0, 0), s), Color4F(1.0f, 1.0f, 1.0f, 0.3f));

	auto nodeMask = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!nodeMask)
		return;
#endif
	nodeMask->DrawSolidRect(Rect(Vec2(), s));
	m_scratchPadClipping->setStencil(nodeMask);
	m_scratchPadClipping->SetContentSize(s);
}

void CHmsScratchPadPreview::SetCallbackFunc(t_callbackFunc func)
{
	m_callbackFunc = func;
}

void CHmsScratchPadPreview::DetectNeedCreateOrNot(int dstCount)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_previewPanel)
		return;
#endif
	m_curPanelCount = dstCount;

	int needCount = dstCount - m_padDrawVector.size();
	for (int i = 0; i < needCount; ++i)
	{
		auto drawPanel = CHmsScratchPadDraw::Create();
#if _NAV_SVS_LOGIC_JUDGE
		if (drawPanel)
#endif
		{
			drawPanel->SetAnchorPoint(Vec2(0, 0));
			drawPanel->SetScale(c_scale);
			drawPanel->SetDealEventAsButton(true);
			drawPanel->SetCallbackFunc(m_callbackFunc);

			m_padDrawVector.pushBack(drawPanel);

			m_previewPanel->AddChild(drawPanel, m_imageLocalZOrder++);
			auto intf = dynamic_cast<HmsUiEventInterface*>(drawPanel);
			if (intf)
			{
				m_previewPanel->AddChildInterface(intf);
			}
		}
	}
	for (auto &item : m_padDrawVector)
	{
		item->SetVisible(false);
	}

	needCount = dstCount - m_buttonMap.size();
	for (int i = 0; i < needCount; ++i)
	{
		auto pItem = CHmsFixSizeUiImageButton::CreateWithImage("res/Scratchpad/delete.png", Size(50, 50), nullptr);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->SetAnchorPoint(Vec2(0.5, 0.5));
			pItem->SetColor(Color3B::RED);
			pItem->SetCallbackFunc(std::bind(&CHmsScratchPadPreview::OnDelButtonClick, this, pItem));

			m_buttonMap[pItem] = 0;

			m_previewPanel->AddChild(pItem, m_buttonLocalZOrder++);
			auto intf = dynamic_cast<HmsUiEventInterface*>(pItem);
			if (intf)
			{
				m_previewPanel->AddChildInterface(intf);
			}
		}
	}
	for (auto &item : m_buttonMap)
	{
		item.first->SetVisible(false);
	}
}

void CHmsScratchPadPreview::OnDelButtonClick(CHmsFixSizeUiImageButton *pBtn)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!pBtn)
		return;
#endif
	int id = m_buttonMap[pBtn];
	
	CHmsScratchPadSql &sql = CHmsScratchPadSql::GetInstance();
	sql.DelScratchPadInfoStu(id);

	if (m_callbackFuncOnDel)
	{
		m_callbackFuncOnDel();
	}
}

void CHmsScratchPadPreview::SetShowDelButton(bool b)
{
	m_bShowDelButton = b;
}

void CHmsScratchPadPreview::SetCallbackFuncOnDel(t_callbackFuncOnDel func)
{
	m_callbackFuncOnDel = func;
}

void CHmsScratchPadPreview::ToggleShowDelButton()
{
	m_bShowDelButton = !m_bShowDelButton;

	auto iterButton = m_buttonMap.begin();
    for (int i = 0; i < m_curPanelCount && iterButton != m_buttonMap.end(); ++i)
	{
		CHmsFixSizeUiImageButton *pButton = iterButton->first;
#if _NAV_SVS_LOGIC_JUDGE
		if (pButton)
#endif
		{
			pButton->SetVisible(m_bShowDelButton);

			auto pos = pButton->GetPoistion();
			pButton->SetPosition(Vec2(0, 0));
			pButton->SetPosition(pos);
		}
		
		++iterButton;
	}
}