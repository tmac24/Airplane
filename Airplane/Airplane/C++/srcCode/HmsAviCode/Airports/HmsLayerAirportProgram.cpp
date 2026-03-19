#include "HmsLayerAirportProgram.h"
#include "HmsLayerAirports.h"


CHmsLayerAirportProgram* CHmsLayerAirportProgram::GetInstance()
{
    static CHmsLayerAirportProgram *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsLayerAirportProgram;
    }
    if (s_pIns->GetParent())
    {
        s_pIns->CloseThis();
    }
    return s_pIns;
}

CHmsLayerAirportProgram::CHmsLayerAirportProgram()
: HmsUiEventInterface(this)
, m_bIsFullScreen(true)
, m_margin(10)
, m_bEnableModifyFavorite(true)
, m_onCloseCallback(nullptr)
{
}


CHmsLayerAirportProgram::~CHmsLayerAirportProgram()
{
}

bool CHmsLayerAirportProgram::Init(const HmsAviPf::Size & size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(size);

	auto & layerSize = size;

    auto bgDrawNode = CHmsDrawNode::Create();
	if (bgDrawNode)
	{
		bgDrawNode->DrawSolidRect(Rect(0, 0, size.width, size.height), Color4F::BLACK);
		this->AddChild(bgDrawNode);
	}    

	m_toolBottom.width = layerSize.width;
	m_toolBottom.height = 0;

	m_toolRight.width = 100;
    m_toolRight.height = layerSize.height;

	Size windowSize = Size(
		layerSize.width - m_toolRight.width - m_margin * 2,
		layerSize.height - m_toolBottom.height - m_margin
		);
// 	m_pImageScrollView = CHmsImageScrollView::Create(windowSize);
// 	m_pImageScrollView->SetAnchorPoint(Vec2(0.5f, 0.5f));
// 	m_pImageScrollView->SetPosition(windowSize.width / 2 + m_margin, windowSize.height / 2 + m_toolBottom.height);
// 	this->AddChild(m_pImageScrollView);
// 
// 	m_pImageNode = CHmsImageNode::Create();
// 	m_pImageNode->SetAnchorPoint(Vec2(0.5f, 0.5f));
// 	m_pImageNode->SetPosition(windowSize.width / 2, windowSize.height / 2);
// 	m_pImageScrollView->AttachImageNode(m_pImageNode);

	m_scratchPadClipping = CHmsScratchPadClipping::Create();
	if (m_scratchPadClipping)
	{
		m_scratchPadClipping->SetAnchorPoint(Vec2(0.5f, 0.5f));
		m_scratchPadClipping->SetPosition(windowSize.width / 2 + m_margin, windowSize.height / 2 + m_toolBottom.height);
		this->AddChild(m_scratchPadClipping);
		auto nodeMask = CHmsDrawNode::Create();
		nodeMask->DrawSolidRect(Rect(Vec2(), windowSize));
		m_scratchPadClipping->setStencil(nodeMask);
		m_scratchPadClipping->SetContentSize(windowSize);
		m_scratchPadClipping->SetEdgeDetection_0_0(false);

		m_scratchPadDraw = CHmsScratchPadDraw::Create();
		if (m_scratchPadDraw)
		{
			m_scratchPadDraw->SetAnchorPoint(Vec2(0.5f, 0.5f));
			m_scratchPadDraw->SetLineWidthColor(8, Color4B::BLUE);
			m_scratchPadClipping->AddChildWithEvent(m_scratchPadDraw, false);
			m_scratchPadDraw->RegisterMutiTouchEvent(-1);

			m_pImagePlaneTop = CHmsImageNode::Create("res/airport/plane.png");
			if (m_pImagePlaneTop)
			{
				m_pImagePlaneTop->SetColor(Color3B::BLUE);
				m_pImagePlaneTop->SetAnchorPoint(Vec2(0.5f, 0.5f));
				m_scratchPadDraw->AddChild(m_pImagePlaneTop);
			}			

			m_pImagePlaneSide = CHmsImageNode::Create("res/airport/plane_2.png");
			if (m_pImagePlaneSide)
			{
				m_pImagePlaneSide->SetColor(Color3B::BLUE);
				m_pImagePlaneSide->SetAnchorPoint(Vec2(0.5f, 0.5f));
				m_scratchPadDraw->AddChild(m_pImagePlaneSide);
			}			

			m_colorDlg = CHmsScratchPadColorDlg::Create(Size(600, 700), 400);
			if (m_colorDlg)
			{
				m_colorDlg->SetAnchorPoint(0, 0);
				m_colorDlg->SetPosition(
					layerSize.width - m_colorDlg->GetContentSize().width - m_toolRight.width,
					layerSize.height - m_colorDlg->GetContentSize().height - 20);
				m_colorDlg->SetVisible(false);
				this->AddChild(m_colorDlg);
				m_colorDlg->SetCallback([=](int lineWidth, Color4B lineColor){

					m_scratchPadDraw->SetLineWidthColor(lineWidth, lineColor);

				});
			}			
		}
		
	}
	
	//InitBottom();
	InitRight();

	InitEvent();

	SetScheduleUpdate();

	return true;
}

void CHmsLayerAirportProgram::Update(float delta)
{
	m_pImagePlaneTop->SetVisible(false);
	m_pImagePlaneSide->SetVisible(false);
	
    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
    auto aftData = dataBus->GetData();
    auto state = aftData->bLatLonValid;
	if (!state)
	{
		return;
	}
    auto lon = aftData->longitude;
    auto lat = aftData->latitude;
    auto height = aftData->altitudeFt;
    auto yaw = aftData->magneticHeading;

	if (m_starSidStu.firstAreaType == NAV_TOP_VIEM)
	{
		int xIndex = -1;
		int yIndex = -1;
		FindIndexIncToRight(xIndex, yIndex, lon, lat, m_vecFirstAreaXLines, m_vecFirstAreaYLines);
		if (xIndex >= 0 && yIndex >= 0)
		{
			const XYLineStu &xLineStu = m_vecFirstAreaXLines[xIndex];
			const XYLineStu &yLineStu = m_vecFirstAreaYLines[yIndex];
			double pixel_x = (lon - xLineStu.logicValue)*xLineStu.ratio + xLineStu.pixelValue;
			double pixel_y = (lat - yLineStu.logicValue)*yLineStu.ratio + yLineStu.pixelValue;
			m_pImagePlaneTop->SetVisible(true);
			m_pImagePlaneTop->SetPosition(Vec2(pixel_x, pixel_y));
			m_pImagePlaneTop->SetRotation(yaw);
		}
	}
	if (m_starSidStu.secondAreaType == NAV_SIDE_INC_TO_RIGHT)
	{
		//跑道相对于飞机的角度
		double angle = CalculateAngle(lon, lat, m_starSidStu.runwayLon, m_starSidStu.runwayLat) / SIM_Pi * 180;
		double dis = CalculateDistanceNM(lon, lat, m_starSidStu.runwayLon, m_starSidStu.runwayLat);
		double heightFT = height / 0.3048f;
		int xIndex = -1;
		int yIndex = -1;
		if (fabsf(yaw - m_starSidStu.runwayBearing) < 10)
		{
			if (cos(angle / 180 * SIM_Pi) > 0)
			{
				dis = -dis;
			}
			FindIndexIncToRight(xIndex, yIndex, dis, heightFT, m_vecSecondAreaXLines, m_vecSecondAreaYLines);

			if (xIndex >= 0 && yIndex >= 0)
			{
				const XYLineStu &xLineStu = m_vecSecondAreaXLines[xIndex];
				const XYLineStu &yLineStu = m_vecSecondAreaYLines[yIndex];
				double pixel_x = (dis - xLineStu.logicValue)*xLineStu.ratio + xLineStu.pixelValue;
				double pixel_y = (heightFT - yLineStu.logicValue)*yLineStu.ratio + yLineStu.pixelValue;
				m_pImagePlaneSide->SetVisible(true);
				m_pImagePlaneSide->SetPosition(Vec2(pixel_x, pixel_y));
			}
		}
	}
}

void CHmsLayerAirportProgram::FindIndexIncToRight(int &xIndex, int &yIndex, double logicX, double logicY,
	const std::vector<XYLineStu> &vecX, const std::vector<XYLineStu> &vecY)
{
	for (int i = 0; i < (int)vecX.size() - 1; ++i)
	{
		const XYLineStu &curStu = vecX.at(i);
		const XYLineStu &nextStu = vecX.at(i+1);
		if (logicX > curStu.logicValue && logicX <= nextStu.logicValue)
		{
			xIndex = i;
			break;
		}
	}
	for (int i = 0; i < (int)vecY.size() - 1; ++i)
	{
		const XYLineStu &curStu = vecY.at(i);
		const XYLineStu &nextStu = vecY.at(i + 1);
		if (logicY > curStu.logicValue && logicY <= nextStu.logicValue)
		{
			yIndex = i;
			break;
		}
	}
}

void CHmsLayerAirportProgram::InitEvent()
{
	this->RegisterAllNodeChild();
}

void CHmsLayerAirportProgram::InitBottom()
{
	auto CreateButton = [this](int index, const char* str, const char * text, std::function<void(CHmsUiButtonAbstract*, HMS_UI_BUTTON_STATE)> func)
	{
		auto pToolBtn = CHmsNavMenuButton::CreateWithImage(str, text);
		if (pToolBtn)
		{
			pToolBtn->SetAnchorPoint(Vec2(0.5, 0.5));
			pToolBtn->SetPosition(m_toolBottom.width / 8 / 2 + index*m_toolBottom.width / 8, m_toolBottom.height / 2);
			pToolBtn->SetCallbackFunc(func);
			this->AddChild(pToolBtn);
		}	

		return pToolBtn;
	};

#define AP_PRO_CALL_BOTTOM(btnType) std::bind(&CHmsLayerAirportProgram::BottomToolBtnCallback, this, std::placeholders::_1, std::placeholders::_2, btnType)
	
	int index = 0;
	CHmsNavMenuButton * pButtonBack = CreateButton(index++, "res/airport/imagePage/back.png", "Back", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_Back));
	CHmsNavMenuButton * pButtonMap = CreateButton(index++, "res/airport/imagePage/map.png", "Map", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_Map));
	CHmsNavMenuButton * pButtonMove = CreateButton(index++, "res/airport/imagePage/move.png", "Move", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_Move));
	if (pButtonMove)
	{
		pButtonMove->SetToggleBtn(true);
	}
	
	m_pToolBtnTurnLeft = CreateButton(index++, "res/airport/imagePage/turn_left.png", "Turn Left", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_TurnLeft));
	m_pToolBtnTurnRight = CreateButton(index++, "res/airport/imagePage/turn_right.png", "Turn Right", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_TurnRight));
	m_pToolBtnZoomIn = CreateButton(index++, "res/airport/imagePage/zoomIn.png", "Zoom In", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_ZoomIn));
	m_pToolBtnZoomOut = CreateButton(index++, "res/airport/imagePage/zoomOut.png", "Zoom Out", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_ZoomOut));
	CHmsNavMenuButton * pButtonFullScreen = CreateButton(index++, "res/airport/imagePage/full_screen.png", "Fit Width", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_FullScreen));
	if (pButtonFullScreen)
	{
		pButtonFullScreen->SetToggleBtn(true);
		pButtonFullScreen->Selected();
	}
	

	auto mgr = CHmsNavMgr::GetInstance()->GetBtnMgr();
	if (mgr)
	{
		mgr->CreateOrSelectGroup(this);

		//page one
		{
			if (pButtonBack) mgr->AddBtn(pButtonBack);
			if (pButtonMap) mgr->AddBtn(pButtonMap);
			if (pButtonMove) mgr->AddBtn(pButtonMove);
			if (m_pToolBtnTurnLeft) mgr->AddBtn(m_pToolBtnTurnLeft);
			if (m_pToolBtnTurnRight) mgr->AddBtn(m_pToolBtnTurnRight);
			if (m_pToolBtnZoomIn) mgr->AddBtn(m_pToolBtnZoomIn);
			if (m_pToolBtnZoomOut) mgr->AddBtn(m_pToolBtnZoomOut);
			if (pButtonFullScreen) mgr->AddBtn(pButtonFullScreen);
		}
	}
	
	index = 3;

	auto MoveButtonIcon = [](CHmsNavMenuButton *pButton)
	{
		if (pButton)
		{
			auto icon = pButton->GetIconNode();
			if (icon)
			{
				icon->SetPosition(icon->GetPoistion() + Vec2(0, -5));
			}
		}
	};
	m_pToolBtnLeft = CreateButton(index++, "res/airport/arrow_left.png", "", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_Left));
	if (m_pToolBtnLeft)
	{
		m_pToolBtnLeft->SetVisible(false);
		MoveButtonIcon(m_pToolBtnLeft);
	}
	
	m_pToolBtnRight = CreateButton(index++, "res/airport/arrow_right.png", "", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_Right));
	if (m_pToolBtnRight)
	{
		m_pToolBtnRight->SetVisible(false);
		MoveButtonIcon(m_pToolBtnRight);
	}
	
	m_pToolBtnUp = CreateButton(index++, "res/airport/arrow_up.png", "", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_Up));
	if (m_pToolBtnUp)
	{
		m_pToolBtnUp->SetVisible(false);
		MoveButtonIcon(m_pToolBtnUp);
	}
	
	m_pToolBtnDown = CreateButton(index++, "res/airport/arrow_down.png", "", AP_PRO_CALL_BOTTOM(APToolBtnType::AP_Down));
	if (m_pToolBtnDown)
	{
		m_pToolBtnDown->SetVisible(false);

		MoveButtonIcon(m_pToolBtnLeft);
		MoveButtonIcon(m_pToolBtnRight);
		MoveButtonIcon(m_pToolBtnUp);
		MoveButtonIcon(m_pToolBtnDown);
	}

	m_nodePageMove = CHmsNode::Create();
	mgr->CreateOrSelectGroup(m_nodePageMove);

	//page move
	{
		if (pButtonBack) mgr->AddBtn(pButtonBack);
		if (pButtonMap) mgr->AddBtn(pButtonMap);
		if (pButtonMove) mgr->AddBtn(pButtonMove);
		if (m_pToolBtnLeft) mgr->AddBtn(m_pToolBtnLeft);
		if (m_pToolBtnRight) mgr->AddBtn(m_pToolBtnRight);
		if (m_pToolBtnUp) mgr->AddBtn(m_pToolBtnUp);
		if (m_pToolBtnDown) mgr->AddBtn(m_pToolBtnDown);
		if (pButtonFullScreen) mgr->AddBtn(pButtonFullScreen);
	}

	m_nodeCurBtnGroup = this;
	mgr->CreateOrSelectGroup(m_nodeCurBtnGroup);
}

void CHmsLayerAirportProgram::InitRight()
{
	auto CreateButton = [this](int index, const char* str, std::function<void(CHmsUiButtonAbstract*, HMS_UI_BUTTON_STATE)> func)
	{
        int btnCount = 12;
        Size size(m_toolRight.width, m_toolRight.height / btnCount - 6);
		auto pToolBtn = CHmsFixSizeUiImageButton::CreateWithImage(str, size, func);
		if (pToolBtn)
		{
			pToolBtn->SetAnchorPoint(Vec2(0.5, 0.5));
			pToolBtn->SetPosition(
				this->GetContentSize().width - m_toolRight.width / 2,
				this->GetContentSize().height - (m_toolRight.height / btnCount / 2 + index*m_toolRight.height / btnCount));
			this->AddChild(pToolBtn);
		}		
		return pToolBtn;
	};

#define AP_PRO_CALLRIGHT(btnType) std::bind(&CHmsLayerAirportProgram::RightToolBtnCallback, this, std::placeholders::_1, std::placeholders::_2, btnType)

	int index = 0;
	CHmsFixSizeUiImageButton *pButton = nullptr;

    CreateButton(index++, "res/AirportInfo/close.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_Close));
	pButton = CreateButton(index++, "res/AirportInfo/draw.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_Draw));
	if (pButton)
	{
		pButton->SetToggleBtn(true);
	}	
	CreateButton(index++, "res/NavImage/pencolor.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_PenColor));
	CreateButton(index++, "res/NavImage/clear.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_Clear));
	CreateButton(index++, "res/AirportInfo/undo.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_Undo));
	CreateButton(index++, "res/AirportInfo/redo.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_Redo));
	pButton = nullptr;
    pButton = CreateButton(index++, "res/AirportInfo/collect.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_Favorite));
	if (pButton)
	{
		pButton->SetToggleBtn(true);
		m_pRightFavoriteBtn = pButton;
	}    
    
    CreateButton(index++, "res/AirportInfo/turnleft.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_TurnLeft));
    CreateButton(index++, "res/AirportInfo/turnright.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_TurnRight));
    CreateButton(index++, "res/AirportInfo/zoomIn.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_ZoomIn));
    CreateButton(index++, "res/AirportInfo/zoomOut.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_ZoomOut));
	pButton = nullptr;
    pButton = CreateButton(index++, "res/airport/imagePage/full_screen.png", AP_PRO_CALLRIGHT(APToolBtnType::AP_FullScreen));
	if (pButton)
	{
		pButton->SetToggleBtn(true);
		pButton->Selected();
    }	
}

void CHmsLayerAirportProgram::BottomToolBtnCallback(CHmsUiButtonAbstract *btn, HMS_UI_BUTTON_STATE eState, APToolBtnType btnType)
{

#define  IMAGE_SCROLL_DISTANCE 100

	ChangeBtnColor(btn, eState);
	switch (btnType)
	{
	case AP_Back:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			CHmsNavMgr::GetInstance()->ShowNavType(HmsNavType::NavAirport);
		}
		break;
	case AP_Map:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			CHmsNavMgr::GetInstance()->ShowNavType(HmsNavType::NavMap);
		}
		break;
	case AP_Move:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_pToolBtnTurnLeft.get())
			{
				m_pToolBtnTurnLeft->SetVisible(true);
			}
			
			if (m_pToolBtnTurnRight.get())
			{
				m_pToolBtnTurnRight->SetVisible(true);
			}

			if (m_pToolBtnZoomIn.get())
			{
				m_pToolBtnZoomIn->SetVisible(true);
			}
			
			if (m_pToolBtnZoomOut.get())
			{
				m_pToolBtnZoomOut->SetVisible(true);
			}

			if (m_pToolBtnLeft.get())
			{
				m_pToolBtnLeft->SetVisible(false);
			}
			if (m_pToolBtnRight.get())
			{
				m_pToolBtnRight->SetVisible(false);
			}
			if (m_pToolBtnUp.get())
			{
				m_pToolBtnUp->SetVisible(false);
			}
			if (m_pToolBtnDown.get())
			{
				m_pToolBtnDown->SetVisible(false);
			}

			m_nodeCurBtnGroup = this;
			auto mgr = CHmsNavMgr::GetInstance()->GetBtnMgr();
			if (mgr)
			{
				mgr->CreateOrSelectGroup(m_nodeCurBtnGroup);
			}			
		}
		else if (eState == HMS_UI_BUTTON_STATE::btn_selected)
		{
			if (m_pToolBtnTurnLeft.get())
			{
				m_pToolBtnTurnLeft->SetVisible(true);
			}

			if (m_pToolBtnTurnRight.get())
			{
				m_pToolBtnTurnRight->SetVisible(true);
			}

			if (m_pToolBtnZoomIn.get())
			{
				m_pToolBtnZoomIn->SetVisible(true);
			}

			if (m_pToolBtnZoomOut.get())
			{
				m_pToolBtnZoomOut->SetVisible(true);
			}

			if (m_pToolBtnLeft.get())
			{
				m_pToolBtnLeft->SetVisible(false);
			}
			if (m_pToolBtnRight.get())
			{
				m_pToolBtnRight->SetVisible(false);
			}
			if (m_pToolBtnUp.get())
			{
				m_pToolBtnUp->SetVisible(false);
			}
			if (m_pToolBtnDown.get())
			{
				m_pToolBtnDown->SetVisible(false);
			}

			m_nodeCurBtnGroup = m_nodePageMove;
			auto mgr = CHmsNavMgr::GetInstance()->GetBtnMgr();
			if (mgr)
			{
				mgr->CreateOrSelectGroup(m_nodeCurBtnGroup);
			}			
		}
		break;
	case AP_TurnLeft:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			m_scratchPadDraw->SetRotation(m_scratchPadDraw->GetRotation() - 90);
			ReSetImageScale();
		}
		break;
	case AP_TurnRight:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			m_scratchPadDraw->SetRotation(m_scratchPadDraw->GetRotation() + 90);
			ReSetImageScale();
		}
		break;
	case AP_FullScreen:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			m_bIsFullScreen = true;
			ReSetImageScale();
		}
		else if (eState == HMS_UI_BUTTON_STATE::btn_selected)
		{
			m_bIsFullScreen = false;
			ReSetImageScale();
		}
		break;
	case AP_ZoomIn:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			ZoomImage(m_scratchPadDraw->GetScale()*1.2);
		}
		break;
	case AP_ZoomOut:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			ZoomImage(m_scratchPadDraw->GetScale()*0.8);
		}
		break;
	case AP_Left:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_scratchPadClipping.get())
			{
				m_scratchPadClipping->ChildMoveDistance(-IMAGE_SCROLL_DISTANCE, 0);
			}			
		}
		break;
	case AP_Right:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_scratchPadClipping.get())
			{
				m_scratchPadClipping->ChildMoveDistance(IMAGE_SCROLL_DISTANCE, 0);
			}
		}
		break;
	case AP_Up:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_scratchPadClipping.get())
			{
				m_scratchPadClipping->ChildMoveDistance(0, IMAGE_SCROLL_DISTANCE);
			}
		}
		break;
	case AP_Down:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_scratchPadClipping.get())
			{
				m_scratchPadClipping->ChildMoveDistance(0, -IMAGE_SCROLL_DISTANCE);
			}
		}
		break;
	default:
		break;
	}
}
void CHmsLayerAirportProgram::RightToolBtnCallback(CHmsUiButtonAbstract *btn, HMS_UI_BUTTON_STATE eState, APToolBtnType btnType)
{
	ChangeBtnColor(btn, eState);
	switch (btnType)
	{
    case AP_Close:
        if (eState == HMS_UI_BUTTON_STATE::btn_normal)
        {
            CloseThis();
        }
        break;
	case AP_Draw:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_scratchPadClipping.get())
			{
				m_scratchPadClipping->SetEventMode(ClippingEventMode::E_DRAG);
			}
			if (m_scratchPadDraw.get())
			{
				m_scratchPadDraw->SetEnableDraw(false);
			}
		}
		else if (eState == HMS_UI_BUTTON_STATE::btn_selected)
		{
			if (m_scratchPadClipping.get())
			{
				m_scratchPadClipping->SetEventMode(ClippingEventMode::E_TRANSMIT);
			}
			if (m_scratchPadDraw.get())
			{
				m_scratchPadDraw->SetEnableDraw(true);
			}
		}
		break;
	case AP_PenColor:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			int width;
			Color4B color;
			if (m_scratchPadDraw.get())
			{
				m_scratchPadDraw->GetCurLineWidthAndColor(width, color);
			}
			if (m_colorDlg.get())
			{
				m_colorDlg->SetLineWidthAndColor(width, color);
				m_colorDlg->SetVisible(!m_colorDlg->IsVisible());
			}			
		}
		break;
	case AP_Clear:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_scratchPadDraw.get())
			{
				m_scratchPadDraw->ClearDrawContent();
			}
		}
		break;
	case AP_Undo:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_scratchPadDraw.get())
			{
				m_scratchPadDraw->Undo();
			}
		}
		break;
	case AP_Redo:
		if (eState == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_scratchPadDraw.get())
			{
				m_scratchPadDraw->Redo();
			}
		}
		break;
    case AP_Favorite:
        if (m_bEnableModifyFavorite)
        {
            if (eState == HMS_UI_BUTTON_STATE::btn_normal)
            {
                m_starSidStu.isFavorite = false;
            }
            else if (eState == HMS_UI_BUTTON_STATE::btn_selected)
            {
                m_starSidStu.isFavorite = true;
            }
            CHmsNavImageSql::GetInstance().ModProgramFavorite(m_starSidStu);
        }     
        break;
    case AP_TurnLeft:
        if (eState == HMS_UI_BUTTON_STATE::btn_normal)
        {
			if (m_scratchPadDraw.get())
			{
				m_scratchPadDraw->SetRotation(m_scratchPadDraw->GetRotation() - 90);
			}
            ReSetImageScale();
        }
        break;
    case AP_TurnRight:
        if (eState == HMS_UI_BUTTON_STATE::btn_normal)
        {
			if (m_scratchPadDraw.get())
			{
				m_scratchPadDraw->SetRotation(m_scratchPadDraw->GetRotation() + 90);
			}
            ReSetImageScale();
        }
        break;
    case AP_FullScreen:
        if (eState == HMS_UI_BUTTON_STATE::btn_normal)
        {
            m_bIsFullScreen = true;
            ReSetImageScale();
        }
        else if (eState == HMS_UI_BUTTON_STATE::btn_selected)
        {
            m_bIsFullScreen = false;
            ReSetImageScale();
        }
        break;
    case AP_ZoomIn:
        if (eState == HMS_UI_BUTTON_STATE::btn_normal)
        {
			if (m_scratchPadDraw.get())
			{
				ZoomImage(m_scratchPadDraw->GetScale()*1.2);
			}
        }
        break;
    case AP_ZoomOut:
        if (eState == HMS_UI_BUTTON_STATE::btn_normal)
        {
			if (m_scratchPadDraw.get())
			{
				ZoomImage(m_scratchPadDraw->GetScale()*0.8);
			}
        }
        break;
	default:
		break;
	}
}

void CHmsLayerAirportProgram::MapImageToEarth()
{
	if (m_vecFirstAreaXLines.size() >= 2 && m_vecFirstAreaYLines.size() >= 2)
	{
		NavImageTileStu stu;
		auto texSize = m_scratchPadDraw->GetContentSize();
		stu.lonLatLT.x = m_vecFirstAreaXLines[0].logicValue;
		stu.lonLatLT.y = m_vecFirstAreaYLines[m_vecFirstAreaYLines.size() - 1].logicValue;
		stu.pixelLT.x = m_vecFirstAreaXLines[0].pixelValue;
		stu.pixelLT.y = m_vecFirstAreaYLines[m_vecFirstAreaYLines.size() - 1].pixelValue;

		stu.lonLatRB.x = m_vecFirstAreaXLines[m_vecFirstAreaXLines.size() - 1].logicValue;
		stu.lonLatRB.y = m_vecFirstAreaYLines[0].logicValue;
		stu.pixelRB.x = m_vecFirstAreaXLines[m_vecFirstAreaXLines.size() - 1].pixelValue;
		stu.pixelRB.y = m_vecFirstAreaYLines[0].pixelValue;

		stu.imagePath = m_starSidStu.imagePath;

		//把显示区域扩展到整张图
		auto xRatio = (stu.lonLatRB.x - stu.lonLatLT.x) / (stu.pixelRB.x - stu.pixelLT.x);
		auto yRatio = (stu.lonLatLT.y - stu.lonLatRB.y) / (stu.pixelLT.y - stu.pixelRB.y);

		stu.lonLatLT.x -= stu.pixelLT.x * xRatio;
		stu.lonLatRB.x += (texSize.width - stu.pixelRB.x) * xRatio;

		stu.lonLatLT.y += (texSize.height - stu.pixelLT.y) * yRatio;
		stu.lonLatRB.y -= stu.pixelRB.y * yRatio;

		auto temp = CHmsGlobal::GetInstance()->GetNavImageTileMgrPointer();
		if (temp)
		{
			temp->SetNavImageTileInfoStu(stu);

			CHmsNavMgr::GetInstance()->ShowNavType(HmsNavType::NavMap);
		}
	}
}
void CHmsLayerAirportProgram::UnMapImageToEarth()
{
	NavImageTileStu stu;
	auto temp = CHmsGlobal::GetInstance()->GetNavImageTileMgrPointer();
	if (temp)
	{
		temp->SetNavImageTileInfoStu(stu);

		CHmsNavMgr::GetInstance()->ShowNavType(HmsNavType::NavMap);
	}
}

void CHmsLayerAirportProgram::ChangeBtnColor(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	auto pNode = dynamic_cast<CHmsNode*>(pBtn);
	switch (state)
	{
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_normal:
		if (pNode)
		{
			pNode->SetColor(Color3B::WHITE);
			pNode->SetScale(1.0);
		}
		break;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected:
		if (pNode)
		{
			pNode->SetColor(Color3B(0x00, 0xa0, 0xe9));
			pNode->SetScale(1.1f);
		}
		break;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_disable:
		break;
	default:
		break;
	}
}

void CHmsLayerAirportProgram::SetStarSidStu(StarSidStu stu)
{
	m_starSidStu = stu;

    m_bEnableModifyFavorite = false;
	if (m_pRightFavoriteBtn)
	{
		m_pRightFavoriteBtn->UnSelected();
		if (m_starSidStu.isFavorite)
		{
			m_pRightFavoriteBtn->Selected();
		}
	}
   
    m_bEnableModifyFavorite = true;

	auto layerSize = this->GetContentSize();
	
	if (m_scratchPadDraw.get())
	{
		m_scratchPadDraw->ClearDrawContent();
		m_scratchPadDraw->SetScratchPadBgByPath(stu.imagePath);
		m_scratchPadDraw->SetRotation(0);

		if (m_pImagePlaneTop.get())
		{
			m_pImagePlaneTop->SetPosition(m_scratchPadDraw->GetContentSize().width / 2, m_scratchPadDraw->GetContentSize().height / 2);
		}		
	}
	
	if (m_pImagePlaneSide.get())
	{
		if (cos(m_starSidStu.runwayBearing / 180 * SIM_Pi) < 0)
		{
			m_pImagePlaneSide->SetFlippedX(true);
		}
		else
		{
			m_pImagePlaneSide->SetFlippedX(false);
		}
	}	

	auto mgr = CHmsNavMgr::GetInstance()->GetBtnMgr();
	if (mgr)
	{
		mgr->CreateOrSelectGroup(m_nodeCurBtnGroup);
	}	

	ReSetImageScale();

	CalXYTable();
}

void CHmsLayerAirportProgram::SetOnCloseCallback(std::function<void()> func)
{
    m_onCloseCallback = func;
}

void CHmsLayerAirportProgram::CalXYTable()
{
	if (m_starSidStu.firstAreaType != NAV_NONE)
	{
		AreaInfoToVector(m_starSidStu.firstAreaInfo, m_vecFirstAreaXLines, m_vecFirstAreaYLines);
	}
	if (m_starSidStu.secondAreaType != NAV_NONE)
	{
		AreaInfoToVector(m_starSidStu.secondAreaInfo, m_vecSecondAreaXLines, m_vecSecondAreaYLines);
	}
}

void CHmsLayerAirportProgram::AreaInfoToVector(std::string &infoStr, std::vector<XYLineStu> &vecX, std::vector<XYLineStu> &vecY)
{
	vecX.clear();
	vecY.clear();
	std::vector<std::string> vecRet = StrSplit(infoStr, std::string(";"));
	if (vecRet.size() != 2)
	{
		return;
	}
	for (int k = 0; k < (int)vecRet.size(); ++k)
	{
		std::vector<std::string> vec = StrSplit(vecRet[k], std::string(","));
		for (int i = 0; i < (int)vec.size(); ++i)
		{
			std::vector<std::string> vecTemp = StrSplit(vec[i], std::string("_"));
			if (vecTemp.size() != 2)
			{
				continue;
			}
			XYLineStu stu;
			stu.pixelValue = atof(vecTemp[0].c_str());
			stu.logicValue = atof(vecTemp[1].c_str());
			if (0 == k)
			{
				vecX.push_back(stu);
			}
			else
			{
				vecY.push_back(stu);
			}
		}
	}
	//计算比例系数
	for (int i = 0; i < (int)vecX.size() - 1; ++i)
	{
		XYLineStu &curStu = vecX[i];
		XYLineStu &nextStu = vecX[i + 1];
		curStu.ratio = (nextStu.pixelValue - curStu.pixelValue) / (nextStu.logicValue - curStu.logicValue);
	}
	for (int i = 0; i < (int)vecY.size() - 1; ++i)
	{
		XYLineStu &curStu = vecY[i];
		XYLineStu &nextStu = vecY[i + 1];
		curStu.ratio = (nextStu.pixelValue - curStu.pixelValue) / (nextStu.logicValue - curStu.logicValue);
	}
}

void CHmsLayerAirportProgram::ReSetImageScale()
{
	if (!m_scratchPadDraw)
	{
		return;
	}
	auto layerSize = this->GetContentSize();

	auto imageSize = m_scratchPadDraw->GetContentSize();
	float rotation = fabs(m_scratchPadDraw->GetRotation());

	float dstWidth = layerSize.width - m_toolRight.width - m_margin * 2;
	float dstHeight = layerSize.height - m_toolBottom.height - m_margin * 2;

	if ((int)(rotation / 90) % 2 == 0)
	{
		if (imageSize.height > imageSize.width && m_bIsFullScreen)
		{
			ZoomImage(1 / (imageSize.height / dstHeight));
		}
		else
		{
			ZoomImage(1 / (imageSize.width / dstWidth));
		}
	}
	else
	{
		if (imageSize.height > imageSize.width || !m_bIsFullScreen)
		{
			ZoomImage(1 / (imageSize.height / dstWidth));
		}
		else
		{
			ZoomImage(1 / (imageSize.width / dstHeight));
		}
	}

	auto pos = layerSize / 2;
	if (m_scratchPadDraw.get())
	{
		m_scratchPadDraw->SetPosition(pos);
	}
	
}

void CHmsLayerAirportProgram::ZoomImage(float scale)
{
	if (m_scratchPadDraw.get())
	{
		m_scratchPadDraw->ZoomByScale(scale);
		if (m_pImagePlaneTop.get())
		{
			m_pImagePlaneTop->SetScale(1.0 / m_scratchPadDraw->GetScale() * 1.3);
		}
		if (m_pImagePlaneSide)
		{
			m_pImagePlaneSide->SetScale(1.0 / m_scratchPadDraw->GetScale() * 1.3);
		}		
	}
}

std::vector<std::string> CHmsLayerAirportProgram::StrSplit(std::string& s, const std::string& delim)
{
	std::vector<std::string> vecRet;
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		vecRet.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		vecRet.push_back(s.substr(last, index - last));
	}
	return vecRet;
}

void CHmsLayerAirportProgram::CloseThis()
{
    this->RemoveFromParent();
    this->RemoveFromParentInterface();
    if (m_onCloseCallback)
    {
        m_onCloseCallback();
    }
}
