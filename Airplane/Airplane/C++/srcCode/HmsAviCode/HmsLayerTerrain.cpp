#include "HmsLayerTerrain.h"
#include "display/HmsDrawNode.h"
#include "DataInterface/TerrainServer/HmsGeoMapLoadManager.h"
#include "HmsVSDByShader.h"
#include "HmsAftObjectLayer.h"
#include "HmsDataBus_Old.h"
#include "HmsMapDef.h"
#include "Calculate/CalculateZs.h"
#include "Terrain/HmsProfileView.h"

USING_NS_HMS;

CHmsLayerTerrain::CHmsLayerTerrain()
: HmsUiEventInterface(this)
{
}


CHmsLayerTerrain::~CHmsLayerTerrain()
{
}

bool CHmsLayerTerrain::Init(const HmsAviPf::Size & size)
{
	if (!CHmsResizeableLayer::Init())
	{
		return false;
	}
	SetContentSize(size);
	float fMarginTerrain = 1;
	float terrainWidth = m_sizeContent.width - 2 * fMarginTerrain;
	float terainHeight = terrainWidth;

	float fMarginProfile = 8;
	float profileAreaHeight = m_sizeContent.height - m_sizeContent.width;
	float profileWidth = m_sizeContent.width - 2 * fMarginProfile;
	float profileHeight = profileAreaHeight - 2 * fMarginTerrain;
	
	m_pProfileView = CHmsProfileView::Create(profileWidth, profileHeight);
	m_pProfileView->SetBackGroundColor(Color4B(0x16, 0x16, 0x16, 0xFF));
	m_pProfileView->SetPosition(fMarginProfile, fMarginProfile);
	this->AddChild(m_pProfileView);

	m_pTawsView = CHmsLayerTaws::Create();
	m_pTawsView->SetProfileView(m_pProfileView);
	//m_pTawsView->ResizeView(Size(contentWidth, terainHeight));
	m_pTawsView->ResizeLayer(Size(terrainWidth, terainHeight));
	m_pTawsView->SetPosition(fMarginTerrain, profileAreaHeight);
	this->AddChild(m_pTawsView);

	InitFlowBtns();

	return true;
}



void CHmsLayerTerrain::Update(float delta)
{
#if 0//todo
	auto insGnss = CHmsGlobal::GetInstance()->GetDataBus()->GetInsGnssData();
	if (!insGnss.valid || insGnss.insGnssState.navState != 1) return;

	m_pProfileView->SetHeading(insGnss.yaw);
	m_pProfileView->SetLocation(insGnss.gnssLng, insGnss.gnssLat);
#endif
}


#include "ui/HmsUiImagePanel.h"
#include "ToolBar/HmsToolBarFlowBtn.h"
void CHmsLayerTerrain::InitFlowBtns()
{
	auto rootModeMenu = CHmsUiPanel::Create();
	float fMargin = 10.0f;
	Size sizeShow(fMargin * 2 + CHmsToolBarFlowBtn::GetDesignBtnSize().width, fMargin);
	Vec2 pos(fMargin, fMargin + sizeShow.height / 2.0f);
	float fMoveY = CHmsToolBarFlowBtn::GetDesignBtnSize().height + fMargin;

	auto CreateBtns = [=, &pos, &sizeShow](bool bToggle,  bool bClickNormal, const char * strIconPath, const CHmsUiButtonAbstract::HmsUiBtnCallback & callback)
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

	auto btnZoomOut = CreateBtns(false, false, "res/Scratchpad/zoomOut.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
		if (m_pTawsView && (eState == HMS_UI_BUTTON_STATE::btn_selected))
			m_pTawsView->ZoomOut();
	});

	auto btnZoomIn = CreateBtns(false, false, "res/Scratchpad/zoomIn.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
		if (m_pTawsView && (eState == HMS_UI_BUTTON_STATE::btn_selected))
			m_pTawsView->ZoomIn();
	});

	auto btnArc = CreateBtns(true, true, "res/ToolBarIcon/ArcIcon.png", [=](HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState){
		if (m_pTawsView)
		{
			if (eState == HMS_UI_BUTTON_STATE::btn_selected)
			{
				m_pTawsView->SetMode(HMS_TAWS_MODE::Arc);
			}
			else
				m_pTawsView->SetMode(HMS_TAWS_MODE::Center);
		}
	});

	rootModeMenu->SetContentSize(sizeShow);
	rootModeMenu->RegisterAllNodeChild();
	this->AddChild(rootModeMenu, 10);
	this->AddChildInterface(rootModeMenu);

	

	rootModeMenu->SetAnchorPoint(Vec2(1.0f, 0.0f));
	Vec2 posMenu(GetContentSize().width, 0);
	if (m_pTawsView)
	{
		auto viewPos = m_pTawsView->GetPoistion();
		posMenu += viewPos;
	}
	posMenu += Vec2(-40 + fMargin, 40 - fMargin);
	rootModeMenu->SetPosition(posMenu);

	btnArc->SetSelected(false);

}