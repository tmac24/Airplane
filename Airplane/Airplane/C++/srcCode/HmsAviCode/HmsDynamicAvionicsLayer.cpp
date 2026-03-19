#include "HmsDynamicAvionicsLayer.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"
#include "ui/HmsUiImagePanel.h"
USING_NS_HMS;

CHmsDynamicAvionicsLayer::CHmsDynamicAvionicsLayer()
: HmsUiEventInterface(this)
{
    m_fSettingPageWidth = 100;
    m_eCurViewMode = DynamicViewMode::NoneMode;
}

bool CHmsDynamicAvionicsLayer::Init(const HmsAviPf::Size & visibleSize)
{
    if (!CHmsResizeableLayer::Init())
    {
        return false;
    }
    SetContentSize(visibleSize);

    m_fSettingPageWidth = 510;

    m_nodeNavEarth = CHmsNavEarthLayer::Create(visibleSize);
    m_nodeSvs = CHmsLayerCombineSvs::Create(visibleSize);
    m_nodeSvs->SetPosition(Vec2(visibleSize.width, 0));

    m_nodeTerrain = CHmsLayerTerrain::Create(Size((int)((1 - 0.618) * 2560), visibleSize.height));
    m_nodeTerrain->SetAnchorPoint(Vec2(0, 0));
    m_nodeTerrain->SetPosition(Vec2(visibleSize.width * 2, 0));

    m_nodeWarning = CHmsWarningLayer::Create(visibleSize);
    m_nodeWarning->SetPosition(Vec2(0, 0));
    m_nodeWarning->SetLocalZOrder(9);

    this->AddChild(m_nodeWarning);

    m_nodeDataViewer = CHmsToolBarDataViewer::create();
    m_nodeDataViewer->SetPosition(Vec2(150, visibleSize.height - 130));
    m_nodeDataViewer->SetLocalZOrder(10);

    InitMenuButtons();

    SetViewModeAndSetBtnState(DynamicViewMode::EarthNavMode);
    //SetViewModeAndSetBtnState(DynamicViewMode::SvsMode);
    return true;
}

void CHmsDynamicAvionicsLayer::SetParallelLayer(CHmsParallelLayer * pLayer)
{
    m_parallelLayer = pLayer;
    if (nullptr == m_parallelLayer)
    {
        return;
    }

    m_parallelLayer->AddScreenAction(HMS_CALLBACK_1(CHmsDynamicAvionicsLayer::ShowSettingPage, this), "showSetting");
    m_parallelLayer->AddScreenAction(HMS_CALLBACK_1(CHmsDynamicAvionicsLayer::ChangeCurrentPlane, this), "setPlane");
    m_parallelLayer->AddScreenAction(HMS_CALLBACK_1(CHmsDynamicAvionicsLayer::SetWarningDistance, this), "setWarningDistance");
    m_parallelLayer->AddScreenAction(HMS_CALLBACK_1(CHmsDynamicAvionicsLayer::SetTawsColorMode, this), "setTawsColorTheme");
    m_parallelLayer->AddScreenAction(HMS_CALLBACK_1(CHmsDynamicAvionicsLayer::SetAttitudeZeroDisplay, this), "attitudeZero");
    m_parallelLayer->AddScreenAction(HMS_CALLBACK_1(CHmsDynamicAvionicsLayer::SetMetricDisplay, this), "metric");
	m_parallelLayer->AddScreenAction(HMS_CALLBACK_1(CHmsDynamicAvionicsLayer::SetPfdTrueTrackDisplay, this), "trueTrack");
    m_parallelLayer->AddScreenAction(HMS_CALLBACK_1(CHmsDynamicAvionicsLayer::SetSvsPfdMode, this), "svsDisplayMode");
    m_parallelLayer->AddScreenAction(HMS_CALLBACK_1(CHmsDynamicAvionicsLayer::SetDeviceDataDisplay, this), "DeviceData");
}

void CHmsDynamicAvionicsLayer::SetVisible(bool visible)
{
    CHmsNode::SetVisible(visible);
    if (m_nodeSvs)
    {
        m_nodeSvs->SetVisible(visible);
    }
}

void CHmsDynamicAvionicsLayer::InitMenuButtons()
{
    auto rootModeMenu = CHmsUiPanel::Create();
    float fMargin = 10.0f;
    Size sizeShow(fMargin, CHmsToolBarFlowBtn::GetDesignBtnSize().height);
    Vec2 pos(fMargin, sizeShow.height / 2.0f);
    float fMoveX = CHmsToolBarFlowBtn::GetDesignBtnSize().width + fMargin;

    auto CreateBtns = [=, &pos, &sizeShow](const char * strModeName, const char * strIconPath, DynamicViewMode eMode)
    {
        auto btn = CHmsToolBarFlowBtn::CreateWithImage(strIconPath, strModeName);
		if (btn)
		{
			btn->SetToggleBtn(true);
			btn->SetClick2Normal(false);
			btn->SetAnchorPoint(0, 0.5f);
			btn->SetPosition(pos);
			btn->SetCallbackFunc(HMS_CALLBACK_2(CHmsDynamicAvionicsLayer::OnModeBtnsChange, this, eMode));

			pos.x += fMoveX;
			sizeShow.width += fMoveX;
			m_mapModeFlowBtn.insert((int)eMode, btn);
			rootModeMenu->AddChild(btn);
		}
        return btn;
    };

    CreateBtns("NavMode", "res/ToolBarIcon/mapIconNav.png", DynamicViewMode::EarthNavMode);
    CreateBtns("NavSvsMode", "res/ToolBarIcon/mapIconNavSvs.png", DynamicViewMode::EarthNavAndSvsMode);
    CreateBtns("SvsMode", "res/ToolBarIcon/mapIconSvs.png", DynamicViewMode::SvsMode);
    CreateBtns("SvsTawsMode", "res/ToolBarIcon/mapIconSvsTaws.png", DynamicViewMode::SvsAndTawsMode);
    CreateBtns("NavTawsMode", "res/ToolBarIcon/mapIconNavTaws.png", DynamicViewMode::EarthNavAndTawsMode);

    rootModeMenu->SetContentSize(sizeShow);
    rootModeMenu->RegisterAllNodeChild();
    this->AddChild(rootModeMenu, 10);
    this->AddChildInterface(rootModeMenu);

    rootModeMenu->SetAnchorPoint(Vec2(1.0f, 1.0f));
    Vec2 posMenu = GetContentSize();
    posMenu += Vec2(-40, -30);
    rootModeMenu->SetPosition(posMenu);
}

void CHmsDynamicAvionicsLayer::OnModeBtnsChange(HmsAviPf::CHmsUiButtonAbstract * btn, HmsAviPf::HMS_UI_BUTTON_STATE eState, DynamicViewMode eMode)
{
    if (eState == HMS_UI_BUTTON_STATE::btn_selected)
    {
        int nTempMode = (int)eMode;

        for (auto c : m_mapModeFlowBtn)
        {
            if (c.first != nTempMode)
            {
                if (c.second->IsSelected())
                {
                    c.second->SetState(HMS_UI_BUTTON_STATE::btn_normal);;
                }
            }
        }

        SetViewMode(eMode);
    }
}

void CHmsDynamicAvionicsLayer::SetViewMode(DynamicViewMode eMode)
{
    if (m_eCurViewMode != eMode)
    {
        m_eCurViewMode = eMode;
    }

    Size fullSize = GetContentSize();
    Size halfSize(fullSize.width / 2.0f, fullSize.height);

    switch (eMode)
    {
    case DynamicViewMode::SvsMode:
    {
		if (m_nodeSvs.get())
		{
			m_nodeSvs->RemoveFromParent();
			this->AddChild(m_nodeSvs);
			m_nodeSvs->ResetLayerSizeByAction(fullSize, Vec2(0, 0));
		}

		if (m_nodeNavEarth.get())
		{
			m_nodeNavEarth->ResetLayerSizeByAction(fullSize, Vec2(-fullSize.width, 0), true);
		}

		if (m_nodeTerrain.get())
		{
			m_nodeTerrain->ResetLayerSizeByAction(m_nodeTerrain->GetContentSize(), Vec2(fullSize.width, 0), true);
		}
        break;
    }
    case DynamicViewMode::EarthNavMode:
    {
		if (m_nodeNavEarth.get())
		{
			m_nodeNavEarth->RemoveFromParent();
			this->AddChild(m_nodeNavEarth);
			m_nodeNavEarth->ResetLayerSizeByAction(fullSize, Vec2(0, 0));
		}

		if (m_nodeSvs.get())
		{
			m_nodeSvs->RemoveFromParent();
			this->AddChild(m_nodeSvs);
			m_nodeSvs->ResetLayerSizeByAction(fullSize, Vec2(fullSize.width, 0), true);
		}
        
		if (m_nodeTerrain.get())
		{
			m_nodeTerrain->ResetLayerSizeByAction(m_nodeTerrain->GetContentSize(), Vec2(fullSize.width * 2, 0), true);
		}
        break;
    }
    case DynamicViewMode::EarthNavAndSvsMode:
    {
		if (m_nodeSvs.get())
		{
			m_nodeSvs->RemoveFromParent();
			this->AddChild(m_nodeSvs);
			m_nodeSvs->ResetLayerSizeByAction(halfSize, Vec2(halfSize.width, 0));
		}
       
		if (m_nodeNavEarth.get())
		{
			m_nodeNavEarth->RemoveFromParent();
			this->AddChild(m_nodeNavEarth);
			m_nodeNavEarth->ResetLayerSizeByAction(halfSize, Vec2(0, 0));
		}
        
		if (m_nodeTerrain.get())
		{
			m_nodeTerrain->ResetLayerSizeByAction(m_nodeTerrain->GetContentSize(), Vec2(halfSize.width * 2, 0), true);
		}
        break;
    }
    case DynamicViewMode::SvsAndTawsMode:
    {
       if (m_nodeSvs.get())
		{
			m_nodeSvs->RemoveFromParent();
			this->AddChild(m_nodeSvs);
		}

        m_nodeTerrain->RemoveFromParent();
        this->AddChild(m_nodeTerrain);
        auto terrainSize = m_nodeTerrain->GetContentSize();
        auto svsSize = Size(fullSize.width - terrainSize.width, fullSize.height);
        auto navEarthSize = m_nodeNavEarth->GetContentSize();

		if (m_nodeTerrain->GetPoistionX() > fullSize.width)
		{
			m_nodeTerrain->SetPosition(fullSize.width, 0);
		}

		if (m_nodeSvs)
		{
			m_nodeSvs->ResetLayerSizeByAction(svsSize, Vec2(0, 0));
		}
        m_nodeNavEarth->ResetLayerSizeByAction(navEarthSize, Vec2(-navEarthSize.width, 0), true);
        m_nodeTerrain->ResetLayerSizeByAction(terrainSize, Vec2(svsSize.width, 0));
        break;
    }
    case DynamicViewMode::EarthNavAndTawsMode:
    {
        m_nodeNavEarth->RemoveFromParent();
        m_nodeTerrain->RemoveFromParent();
        this->AddChild(m_nodeTerrain);
        this->AddChild(m_nodeNavEarth);
        auto terrainSize = m_nodeTerrain->GetContentSize();
        auto navEarthSize = Size(fullSize.width - terrainSize.width, fullSize.height);
        auto svsSize = Size(0, 0);
		if (m_nodeSvs)
		{
			svsSize = m_nodeSvs->GetContentSize();
		}

		if (m_nodeTerrain->GetPoistionX() > fullSize.width)
		{
			m_nodeTerrain->SetPosition(fullSize.width, 0);
		}

		if (m_nodeSvs)
		{
			m_nodeSvs->ResetLayerSizeByAction(svsSize, Vec2(-svsSize.width, 0), true);
		}
        m_nodeNavEarth->ResetLayerSizeByAction(navEarthSize, Vec2(0, 0));
        m_nodeTerrain->ResetLayerSizeByAction(terrainSize, Vec2(navEarthSize.width, 0));
        break;
    }
    default:
        break;
    }

    this->RemoveAllChildInterface();
    this->RegisterAllNodeChild();
}

void CHmsDynamicAvionicsLayer::SetViewModeAndSetBtnState(DynamicViewMode eMode)
{
    if (m_eCurViewMode != eMode)
    {
        auto itFind = m_mapModeFlowBtn.find((int)eMode);
        if (itFind != m_mapModeFlowBtn.end())
        {
            itFind->second->SetState(HMS_UI_BUTTON_STATE::btn_selected);
        }

        //SetViewMode(eMode);
    }

}

void CHmsDynamicAvionicsLayer::SetPfdMode(const std::string & strMode)
{
    if (m_nodeSvs)
    {
        m_nodeSvs->SetPfdMode(strMode);
    }
}

bool CHmsDynamicAvionicsLayer::SetWarningDistance(std::string strDistance)
{
    if (m_nodeSvs)
    {
        int nDistance = atoi(strDistance.c_str());
        auto svsLayer = m_nodeSvs->GetSvsLayer();
        if (svsLayer)
        {
            svsLayer->SetWarnningDistance(nDistance);
        }
    }
    return true;
}

bool CHmsDynamicAvionicsLayer::SetTawsColorMode(std::string strTawsColorMode)
{
    if (m_nodeTerrain)
    {
        int colorMode = atoi(strTawsColorMode.c_str());
        auto terrainLayer = m_nodeTerrain->GetTawsLayer();
        if (terrainLayer)
        {
            if (1 == colorMode)
            {
                terrainLayer->SetTawsColorStyle(CHmsLayerTaws::enTawsColorTheme::EN_TAWS_THEME_0);
            }
            else if (2 == colorMode)
            {
                terrainLayer->SetTawsColorStyle(CHmsLayerTaws::enTawsColorTheme::EN_TAWS_THEME_1);
            }
            else
            {
                std::cout << "color mode error" << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool CHmsDynamicAvionicsLayer::SetMetricDisplay(std::string strOn)
{
    bool bOn = false;
    if (strOn == "on")
        bOn = true;

    auto ctrldata = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
    if (ctrldata)
    {
        ctrldata->SetMetricEnable(bOn);
    }
    return true;
}

bool CHmsDynamicAvionicsLayer::SetPfdTrueTrackDisplay(std::string strOn)
{
	CHmsDuControlData *pDUCtrl = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();

	if (strOn == "on")
	{
		pDUCtrl->SetPfdCompassDataMode(PFD_COMPASS_DATA_MODE::trueTrack);
	}
	else
	{
		pDUCtrl->SetPfdCompassDataMode(PFD_COMPASS_DATA_MODE::magneticHeading);
	}

	return true;
}

bool CHmsDynamicAvionicsLayer::SetSvsPfdMode(std::string strMode)
{
    SetPfdMode(strMode);
    return true;
}

bool CHmsDynamicAvionicsLayer::SetAttitudeZeroDisplay(std::string strOn)
{
    bool bOn = false;
    if (strOn == "on")
        bOn = true;

    CHmsGlobal::GetInstance()->GetDataBus()->SetDataCorrection(bOn);
    return true;
}

bool CHmsDynamicAvionicsLayer::SetDeviceDataDisplay(std::string strOn)
{
    if (m_nodeDataViewer)
    {
        m_nodeDataViewer->RemoveFromParent();
        if (strOn == "on")
        {
            this->AddChild(m_nodeDataViewer);
        }
    }
    return true;
}

bool CHmsDynamicAvionicsLayer::ChangeCurrentPlane(std::string strPlaneName)
{
    if (m_nodeSvs)
    {
        auto svsLayer = m_nodeSvs->GetSvsLayer();
        if (svsLayer)
        {
            svsLayer->SetAircraftModel(strPlaneName);
        }
    }
    //	return SvsSetCurrentAircraft(strPlaneName);
    return true;
}

#include "ToolBar/HmsAvionicsDisplayMgr.h"
bool CHmsDynamicAvionicsLayer::ShowSettingPage(std::string strOn)
{
    CHmsAvionicsDisplayMgr::GetInstance()->SetBarActionDisplaySettingPage(false);
    return true;
}
