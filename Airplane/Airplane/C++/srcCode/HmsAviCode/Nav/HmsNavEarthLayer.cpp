#include "HmsNavEarthLayer.h"
#include "HmsNavMenuBar.h"
#include "HmsNavEarthFrame.h"
#include "HmsNavComm.h"
#include "../FMS/HmsDuControlData.h"

USING_NS_HMS;

#ifndef HMS_ARRAYSIZE
#define HMS_ARRAYSIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

CHmsNavEarthLayer::CHmsNavEarthLayer()
	: HmsUiEventInterface(this)
	, m_pNavEarthFrame(nullptr)
	, m_pFrame2DRoot(nullptr)
{

}

CHmsNavEarthLayer::~CHmsNavEarthLayer()
{
	HMS_SAFE_RELEASE(m_pNavEarthFrame);
	HMS_SAFE_RELEASE(m_pFrame2DRoot);
}
#define NAV_VIEWER_ID		0x21
#define NAV_INFO_WINDOW		0x22
#define NAV_SCRATCHPAD		0x23
bool CHmsNavEarthLayer::Init(const HmsAviPf::Size & size)
{
	if (!CHmsResizeableLayer::Init())
	{
		return false;
	}
	SetContentSize(size);

	auto & layerSize = size;
	float fBorder = 1.0f;
	//create the menu bar
// 	CHmsNavMenuBar * menuBar = nullptr;
// 	{
// 		menuBar = CHmsNavMenuBar::Create(this, Size(layerSize.width, 64), fBorder);
// 		menuBar->SetPosition(this->GetUiBottomLeft() + Vec2(fBorder, fBorder));
// 		this->AddChild(menuBar);
// 	}
// 	m_menuBar = menuBar;


	auto earthFrame = CHmsNavEarthFrame::Create(Size(layerSize.width - fBorder * 2, layerSize.height - fBorder * 2));
#if _NAV_SVS_LOGIC_JUDGE
	if (!earthFrame)
		return false;
#endif
	//earthFrame->SetPlaneAlwaysCenterCallback(HMS_CALLBACK_1(CHmsNavMenuBar::SetCenterBtnSelect, menuBar));
	earthFrame->SetViewToAft();
	earthFrame->SetPlaneAlwaysCenter(true);
	CHmsGlobal::GetInstance()->SetNavEarthFramePointer(earthFrame);

	CHmsFrame2DRoot *pFrame2D = nullptr;
	pFrame2D = CHmsFrame2DRoot::Create(earthFrame->GetCamera(), earthFrame->GetContentSize());
#if _NAV_SVS_LOGIC_JUDGE
	if (!pFrame2D)
		return false;
#endif
	pFrame2D->SetAnchorPoint(Vec2(0, 0));
	pFrame2D->SetPosition(Vec2(0, 0));
	earthFrame->AddChild(pFrame2D);
	earthFrame->SetFrame2DRoot(pFrame2D);

    m_pScissorNode = CHmsScratchPadClipping::Create();
    this->AddChild(m_pScissorNode);
    m_pScissorNode->SetEventMode(ClippingEventMode::E_TRANSMIT);
    m_pScissorNode->SetContentSize(size);

	auto navNode = earthFrame->CreateNavViewer();
#if _NAV_SVS_LOGIC_JUDGE
	if (!navNode)
		return false;
#endif
	navNode->SetAnchorPoint(Vec2(0, 0));
	navNode->SetPosition(fBorder, fBorder);
	navNode->SetTag(NAV_VIEWER_ID);
	navNode->AddChild(earthFrame);
    m_pScissorNode->AddChildWithEvent(navNode, true);
    //m_pScissorNode->RegisterAllNodeChild();
    //m_pScissorNode->SetClipRect(Rect(0, 0, navNode->GetContentSize().width, navNode->GetContentSize().height));
    {
        auto clipDrawNode = CHmsDrawNode::Create();
        m_pScissorNode->setStencil(clipDrawNode);

        clipDrawNode->DrawSolidRect(0, 0, size.width, size.height);
    }

	m_pFrame2DRoot = pFrame2D;
	m_pFrame2DRoot->retain();

	m_pNavEarthFrame = earthFrame;
	m_pNavEarthFrame->retain();
	
	
	//menuBar->RegisterEarthFrame(earthFrame);

// 	//===============test
// 	auto info = CHmsNavInfoWindow::Create(Size(layerSize.width - 300, layerSize.height*0.3));
// 	info->SetAnchorPoint(0.5, 0);
// 	info->SetPosition(Vec2(layerSize.width / 2, layerSize.height*0.7));
// 	info->SetVisible(false);
// 	info->SetTag(NAV_INFO_WINDOW);
// 	this->AddChild(info);

	
	//===================

// 	auto scratchPad = CHmsScratchPad::Create(layerSize - Size(layerSize.width*0.4,64));
// 	scratchPad->SetPosition(Vec2(layerSize.width*0.4, 64));
// 	scratchPad->SetVisible(false);
// 	scratchPad->SetHalfAndFullSize(layerSize - Size(layerSize.width*0.4, 64), layerSize - Size(0, 64));
// 	scratchPad->SetTag(NAV_SCRATCHPAD);
// 	this->AddChild(scratchPad);
// 	menuBar->RegisterScratchPad(scratchPad);

	InitFlowBtns();

	this->RegisterAllNodeChild();
	return true;
}

void CHmsNavEarthLayer::Resize(const HmsAviPf::Size & size, int nLevel)
{
	SetContentSize(size);

	auto & layerSize = size;

	if (m_menuBar)
	{
		float fBorder = 1.0f;
		auto SizeBar = Size(layerSize.width, 64);
		auto bar = dynamic_cast<CHmsNavMenuBar*>(m_menuBar.get());
#if _NAV_SVS_LOGIC_JUDGE
		if (!bar)
			return;
#endif
		bar->Resize(SizeBar, nLevel, fBorder);

		auto ef = bar->GetRegisterEarthFrame();
		if (ef)
		{
            auto navViewer = dynamic_cast<CHmsNavViewer*>(m_pScissorNode->GetChildByTag(NAV_VIEWER_ID));
            if (navViewer)
            {
                ef->Resize(Size(layerSize.width - fBorder * 2, layerSize.height - fBorder * 2 - 64), nLevel, navViewer);
            }
		}
	}

// 	auto info = dynamic_cast<CHmsNavInfoWindow*>(this->GetChildByTag(NAV_INFO_WINDOW));
// 	if (info)
// 	{
// 		info->SetPosition(Vec2(layerSize.width / 2, layerSize.height*0.7));
// 	}

	auto scratchPad = dynamic_cast<CHmsScratchPad*>(this->GetChildByTag(NAV_SCRATCHPAD));
	if (scratchPad)
	{
		auto size = layerSize;
		size = size - Size(layerSize.width*0.4, 64);
		scratchPad->SetContentSize(size);
		scratchPad->SetPosition(Vec2(layerSize.width*0.4, 64));
		auto sizeHalf = layerSize - Size(layerSize.width*0.4, 64);
		scratchPad->SetHalfAndFullSize(sizeHalf, layerSize - Size(0, 64));
		scratchPad->Resize(sizeHalf);
	}
#if 0

	float fBorder = 1.0f;
	//create the menu bar
	CHmsNavMenuBar * menuBar = nullptr;
	{
		menuBar = CHmsNavMenuBar::Create(this, Size(layerSize.width, 64), fBorder);
		menuBar->SetPosition(this->GetUiBottomLeft() + Vec2(fBorder, fBorder));
		this->AddChild(menuBar);
	}

	auto ef = CHmsNavEarthFrame::Create(Size(layerSize.width - fBorder * 2, layerSize.height - fBorder * 2 - 64));
	//auto navNode = ef->CreateImageFrameNode();
	auto navNode = ef->CreateNavViewer();
	navNode->SetAnchorPoint(Vec2(0, 0));
	navNode->SetPosition(fBorder, fBorder + 64);
#if 0
	this->AddChild(navNode);
	this->AddChild(ef);
#else
	this->AddChild(navNode);
	navNode->AddChild(ef);
#endif

	menuBar->RegisterEarthFrame(ef);

	//===============test
	auto info = CHmsNavInfoWindow::Create(Size(layerSize.width - 300, layerSize.height*0.3));
	info->SetAnchorPoint(0.5, 0);
	info->SetPosition(Vec2(layerSize.width / 2, layerSize.height*0.7));
	info->SetVisible(false);
	this->AddChild(info);

	menuBar->RegisterNavInfoWindow(info);
	//===================

	auto scratchPad = CHmsScratchPad::Create(layerSize - Size(layerSize.width*0.4, 64));
	scratchPad->SetPosition(Vec2(layerSize.width*0.4, 64));
	scratchPad->SetVisible(false);
	scratchPad->SetHalfAndFullSize(layerSize - Size(layerSize.width*0.4, 64), layerSize - Size(0, 64));
	this->AddChild(scratchPad);
	menuBar->RegisterScratchPad(scratchPad);



	this->RegisterAllNodeChild();
#endif
}

void CHmsNavEarthLayer::ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos)
{
	CHmsResizeableLayer::ResetLayerSize(size, pos);

	if (m_pNavEarthFrame)
	{
        auto navViewer = dynamic_cast<CHmsNavViewer*>(m_pScissorNode->GetChildByTag(NAV_VIEWER_ID));
        if (navViewer)
        {
            m_pNavEarthFrame->Resize(size, 0, navViewer);
        }	
	}
    //pScissorNode->SetClipRect(Rect(0, 0, size.width, size.height));
    {
        auto clipDrawNode = CHmsDrawNode::Create();
        m_pScissorNode->setStencil(clipDrawNode);
        m_pScissorNode->SetContentSize(size);

        clipDrawNode->DrawSolidRect(0, 0, size.width, size.height);
    }
}

void CHmsNavEarthLayer::ResetLayerSizeByAction(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos, bool bRemoveSelfAfterAction /*= false*/)
{
    CHmsNavComm::GetInstance()->SetEnableTileLoad(false);

    CHmsResizeableLayer::ResetLayerSizeByAction(size, pos, bRemoveSelfAfterAction);
}

void CHmsNavEarthLayer::ResizeActionFinished(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos)
{
    CHmsResizeableLayer::ResizeActionFinished(size, pos);

    CHmsNavComm::GetInstance()->SetEnableTileLoad(true);
}

void CHmsNavEarthLayer::SetDisplayAdminArea(bool bShow)
{
	if (m_pFrame2DRoot)
	{
		m_pFrame2DRoot->Set2DLayerDisplayByName("2dAdminAreas", bShow);
	}
}

void CHmsNavEarthLayer::SetDisplayAirspace(bool bShow)
{
	if (m_pFrame2DRoot)
	{
		m_pFrame2DRoot->Set2DLayerDisplayByName("2dAirspaceDB", bShow);
	}
}

void CHmsNavEarthLayer::SetDisplayNearAirport(bool bShow)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()
		|| !CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->GetAirportLayer())
		return;
#endif
    CHmsNavComm::GetInstance()->GetCHmsFrame2DRoot()->GetAirportLayer()->SetLayerVisible(bShow);
}


void CHmsNavEarthLayer::SetDisplayAftTrack(bool bShow)
{
    if (m_pFrame2DRoot)
    {
        m_pFrame2DRoot->Set2DLayerDisplayByName("2dAftTrackLayer", bShow);
    }
}

HmsAviPf::CHmsNode * CHmsNavEarthLayer::GetTrackLayer()
{
    CHmsNode * pNode = nullptr;
    if (m_pFrame2DRoot)
    {
        pNode = m_pFrame2DRoot->Get2DLayerByName("2dAftTrackLayer");
    }
    return pNode;
}

#include "ui/HmsUiImagePanel.h"
#include "ToolBar/HmsToolBarFlowBtn.h"
#include "ToolBar/HmsToolBarCombBtn.h"
void CHmsNavEarthLayer::InitFlowBtns()
{
	auto rootModeMenu = CHmsUiPanel::Create();
    if (!rootModeMenu || !m_pNavEarthFrame)
		return;

	float fMargin = 10.0f;
	Size sizeShow(fMargin * 2 + CHmsToolBarFlowBtn::GetDesignBtnSize().width, fMargin);
	Vec2 pos(fMargin, fMargin + sizeShow.height / 2.0f);
	float fMoveY = CHmsToolBarFlowBtn::GetDesignBtnSize().height + fMargin;

	auto CreateBtns = [=, &pos, &sizeShow](bool bToggle, bool bClickNormal, const char * strIconPath, const CHmsUiButtonAbstract::HmsUiBtnCallback & callback)
	{
		auto btn = CHmsToolBarFlowBtn::CreateWithImage(strIconPath, nullptr);

		if (btn)
		{
			btn->SetToggleBtn(bToggle);
			btn->SetClick2Normal(bClickNormal);
			btn->SetAnchorPoint(0, 0.0f);
			btn->SetPosition(pos);
			btn->SetCallbackFunc(callback);

			pos.y += fMoveY;
			sizeShow.height += fMoveY;
			rootModeMenu->AddChild(btn);
		}
		return btn;
	};

	auto SetSpace = [=, &pos, &sizeShow](int nSpaceCnt){
		do
		{
			pos.y += fMoveY;
			sizeShow.height += fMoveY;
			nSpaceCnt--;
		} while (nSpaceCnt > 0);
	};

	SetSpace(1);


    auto btnCenter = CreateBtns(true, true, "res/ToolBarIcon/localIcon.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
		if (m_pNavEarthFrame && (eState == HMS_UI_BUTTON_STATE::btn_selected))
			m_pNavEarthFrame->SetPlaneAlwaysCenter(true);

        if (m_pNavEarthFrame && (eState == HMS_UI_BUTTON_STATE::btn_normal))
            m_pNavEarthFrame->SetPlaneAlwaysCenter(false);
	});

    if (btnCenter)
    {
        btnCenter->Selected();
        m_pNavEarthFrame->SetPlaneAlwaysCenterCallback([=](bool bSelected){
            if (bSelected)
            {
                btnCenter->Selected();
            }
            else
            {
                btnCenter->UnSelected();
            }
        });
    }

	SetSpace(1);

	auto btnZoomOut = CreateBtns(false, false, "res/Scratchpad/zoomOut.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
		if (m_pNavEarthFrame && (eState == HMS_UI_BUTTON_STATE::btn_selected))
			m_pNavEarthFrame->MoveFar();
	});

	auto btnZoomIn = CreateBtns(false, false, "res/Scratchpad/zoomIn.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
		if (m_pNavEarthFrame && (eState == HMS_UI_BUTTON_STATE::btn_selected))
			m_pNavEarthFrame->MoveNear();
	});

	SetSpace(1);

    auto btnAftTrack = CreateBtns(true, true, "res/ToolBarIcon/aftTrackIcon.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
        bool bDisplay = false;
        if (eState == HMS_UI_BUTTON_STATE::btn_selected)
            bDisplay = true;

        SetDisplayAftTrack(bDisplay);
    });
    btnAftTrack->Selected();

	auto btnObs = CreateBtns(true, true, "res/ToolBarIcon/obsIcon.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
		bool bObsOn = false;
		m_ObsBtnState = eState;
		if (m_pNavEarthFrame && (eState == HMS_UI_BUTTON_STATE::btn_selected))
		{
			bObsOn = true;
		}
		CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData()->SetIsOBSMode(bObsOn);
	});

    if (btnObs)
    {
        btnObs->Selected();
        m_ObsBtnState = HMS_UI_BUTTON_STATE::btn_selected;

        CHmsDuControlData *duControlData = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
        if (duControlData)
        {
            duControlData->SubscribeMemberChanged([=](DUControlDataMember member){
                if (member == MEMBER_IS_OBS_MODE)
                {
                    bool bObs = duControlData->IsObsMode();
                    m_ObsBtnState = bObs ? HMS_UI_BUTTON_STATE::btn_selected : HMS_UI_BUTTON_STATE::btn_normal;
                    btnObs->SetState(m_ObsBtnState);
                }
            }, MEMBER_IS_OBS_MODE);
        }
    }

	auto btnAdminArea = CreateBtns(true, true, "res/ToolBarIcon/adminAreaIcon.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
		bool bDisplay = false;
		if (eState == HMS_UI_BUTTON_STATE::btn_selected)
			bDisplay = true;

		SetDisplayAdminArea(bDisplay);
	});

	auto btnAirport = CreateBtns(true, true, "res/ToolBarIcon/aptIcon.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
		bool bDisplay = false;
		if (eState == HMS_UI_BUTTON_STATE::btn_selected)
			bDisplay = true;

		SetDisplayNearAirport(bDisplay);
	});

    auto btnAirspace = CreateBtns(true, true, "res/ToolBarIcon/airspaceIcon.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
        bool bDisplay = false;
        if (eState == HMS_UI_BUTTON_STATE::btn_selected)
            bDisplay = true;

        SetDisplayAirspace(bDisplay);
    });

    if (btnAirspace)
    {
        btnAirspace->SetVisible(false);
        m_pAirspaceBtn = btnAirspace;
    }

	rootModeMenu->SetContentSize(sizeShow);
	rootModeMenu->RegisterAllNodeChild();
	this->AddChild(rootModeMenu, 10);
	this->AddChildInterface(rootModeMenu);


	rootModeMenu->SetAnchorPoint(Vec2(0.0f, 0.0f));
	Vec2 posMenu(0, 0);
	posMenu += Vec2(40 - fMargin, 40 - fMargin);
	rootModeMenu->SetPosition(posMenu);

	//btnCenter->SetSelected(true);

#if 0
	CombBtnItem items[] = {
		{ "", "IFR-HIGT", HMS_CALLBACK_0(CHmsNavEarthLayer::SwitchMapScr, this, "ifr_High") },
		{ "", "IFR-LOW", HMS_CALLBACK_0(CHmsNavEarthLayer::SwitchMapScr, this, "ifr_Low") },
		{ "", "VFR", HMS_CALLBACK_0(CHmsNavEarthLayer::SwitchMapScr, this, "vfr") },
		{ "", "SATELLITE", HMS_CALLBACK_0(CHmsNavEarthLayer::SwitchMapScr, this, "satellite") }
	};
    auto combBox = CHmsToolBarCombBtn::CreateBtns("res/ToolBarIcon/layersIcon.png", items, HMS_ARRAYSIZE(items)/*, Size(240, 90)*/);
#else
    std::vector<CombBtnItem>    vCombItem;
    auto mapItems = CHmsMapTileSql::GetInstance().GetMapItems();
    auto strDefaultName = CHmsMapTileSql::GetInstance().GetDefaultMapDb();
    int nSelectIndex = 0;
    for (auto c:mapItems)
    {
        CombBtnItem item;
        item.strTitle = c.strNameDisplay;
        item.callback = HMS_CALLBACK_0(CHmsNavEarthLayer::SwitchMapScr, this, c.strMapType);
        if (strDefaultName == c.strMapType)
        {
            nSelectIndex = vCombItem.size();
        }
        vCombItem.push_back(item);
    }
    auto combBox = CHmsToolBarCombBtn::CreateBtns("res/ToolBarIcon/layersIcon.png", vCombItem.data(), vCombItem.size()/*, Size(240, 90)*/);
#endif

    if (combBox)
    {
        combBox->SetSelect(nSelectIndex);
        posMenu.setPoint(0, GetContentSize().height);
        posMenu += Vec2(40 - fMargin, fMargin - 40);
        combBox->SetAnchorPoint(0.0f, 1.0f);
        combBox->SetPosition(posMenu);
        this->AddChild(combBox);
        this->AddChildInterface(combBox);
    }
}

void CHmsNavEarthLayer::SwitchMapScr(const std::string & strScr)
{
	CHmsMapTileSql &mapTileSql = CHmsMapTileSql::GetInstance();
	if (mapTileSql.ChangeMapTypeName(strScr))
	{
		if (m_pNavEarthFrame)
			m_pNavEarthFrame->ReloadEarthTile();

		if (m_pFrame2DRoot)
		{
			auto node = m_pFrame2DRoot->Get2DLayerByName("2dAdminAreas");
			if (node)
			{
				Color3B colorSet(Color3B::WHITE);
				if (strScr.find("ifr") != std::string::npos)
				{
					colorSet = Color3B::BLACK;
				}
				node->SetColor(colorSet);
			}
		}

        if (strScr == "satellite")
        {
            if (m_pAirspaceBtn)
            {
                m_pAirspaceBtn->SetVisible(true);
            }
        }
        else
        {
            if (m_pAirspaceBtn)
            {
                m_pAirspaceBtn->UnSelected();
                m_pAirspaceBtn->SetVisible(false);
            }
        }
	}
}
