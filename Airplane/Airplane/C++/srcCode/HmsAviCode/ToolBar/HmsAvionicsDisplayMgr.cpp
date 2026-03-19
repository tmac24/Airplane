#include "HmsAvionicsDisplayMgr.h"
#include "../Nav/HmsNavEarthFrame.h"
#include "../Nav/HmsNavEarthLayer.h"
#include "../Nav/ScratchPad/HmsScratchPad.h"
#include "../Airports/HmsLayerAirports.h"
#include "Airports/HmsProcedurePreview.h"
#include "../HmsDynamicAvionicsLayer.h"
#include "ui/HmsUiEventInterface.h"
#include "Svs/HmsSvs.h"
#include "NavSvs/HmsNavSvsLayer.h"
#include "Nav/HmsNavFplLayer.h"

CHmsAvionicsDisplayMgr * CHmsAvionicsDisplayMgr::GetInstance()
{
	static CHmsAvionicsDisplayMgr * s_pNavMgr = nullptr;
	if (s_pNavMgr == nullptr)
	{
		s_pNavMgr = new CHmsAvionicsDisplayMgr();
	}
	return s_pNavMgr;
}

CHmsAvionicsDisplayMgr::CHmsAvionicsDisplayMgr()
:m_eType(HmsPageType::PageNone)
{

}

CHmsAvionicsDisplayMgr::~CHmsAvionicsDisplayMgr()
{

}

HmsAviPf::CHmsNode * CHmsAvionicsDisplayMgr::ShowNavType(HmsPageType eType)
{
	HmsAviPf::CHmsNode * nodeTemp = nullptr;
	if (m_eType != eType)
	{
		auto it = m_mapAviDisplayInfo.find((int)eType);
		if (it != m_mapAviDisplayInfo.end())
		{
			if (m_layerDisplay && it->second)
			{
				auto itLast = m_mapAviDisplayInfo.find((int)m_eType);
				if (itLast != m_mapAviDisplayInfo.end())
				{
					if (itLast->second)
					{
						itLast->second->node->RemoveFromParent();
					}
				}

				nodeTemp = it->second->node;
				m_layerDisplay->AddChild(it->second->node);
				m_layerDisplay->RemoveAllChildInterface();
				m_layerDisplay->RegisterAllNodeChild();

				if (m_toolbarBottom)
				{
					m_toolbarBottom->SetAndShowDynamicBtns(it->second->vDynamicBtns);
				}

// 				auto mgr = CHmsAvionicsDisplayMgr::GetInstance()->GetBtnMgr();
// 				mgr->CreateOrSelectGroup(nodeTemp);
			}
		}
		m_eType = eType;
	}
	return nodeTemp;
}

HmsAviPf::CHmsNode * CHmsAvionicsDisplayMgr::GetNavType(HmsPageType eType)
{
	HmsAviPf::CHmsNode * nodeTemp = nullptr;
	
	auto it = m_mapAviDisplayInfo.find((int)eType);
	if (it != m_mapAviDisplayInfo.end())
	{
		if (it->second)
		{
			nodeTemp = it->second->node;
		}
	}

	return nodeTemp;
}

void CHmsAvionicsDisplayMgr::ShowSettingPage(bool bShow)
{
	if (bShow)
	{
		m_settingPage->RemoveFromParent();
		if (m_layerDisplay)
		{
			m_layerDisplay->AddChild(m_settingPage);
			//m_layerDisplay->AddChildInterface(m_settingPage);
		}
	}
	else
	{
		m_settingPage->RemoveFromParent();
		//m_settingPage->RemoveFromParentInterface();
	}
}

void CHmsAvionicsDisplayMgr::Show2dTawsPage(bool bShow)
{
//     if (bShow)
//     {
//         m_2dTawsPage->RemoveFromParent();
//         if (m_layerDisplay)
//         {
//             m_layerDisplay->AddChild(m_2dTawsPage);
//             m_layerDisplay->AddChildInterface(m_2dTawsPage);
//         }
//     }
//     else
//     {
//         m_2dTawsPage->RemoveFromParent();
//         m_2dTawsPage->RemoveFromParentInterface();
//     }
}

void CHmsAvionicsDisplayMgr::ShowDashBoradBar(bool bShow)
{
	if (bShow)
	{
		m_toolbarDashBoard->RemoveFromParent();
		if (m_layerDisplay)
		{
			m_layerDisplay->AddChild(m_toolbarDashBoard);
		}
	}
	else
	{
		m_toolbarDashBoard->RemoveFromParent();
	}
}

void CHmsAvionicsDisplayMgr::ShowNavFplLayer(bool bShow)
{
    if (bShow)
    {
        m_navFplLayer->RemoveFromParent();
        m_navFplLayer->RemoveFromParentInterface();
        if (m_layerDisplay)
        {
            m_layerDisplay->AddChild(m_navFplLayer);
            m_layerDisplay->AddChildInterface(m_navFplLayer);
        }
    }
    else
    {
        m_navFplLayer->RemoveFromParent();
        m_navFplLayer->RemoveFromParentInterface();
    }
}

void CHmsAvionicsDisplayMgr::SetPfdMode(const std::string & strMode)
{
	auto navEarth = dynamic_cast<CHmsDynamicAvionicsLayer*>(GetNavType(HmsPageType::PageMaps));
	if (navEarth)
	{
		navEarth->SetPfdMode(strMode);
	}
}

void CHmsAvionicsDisplayMgr::SetAviMode(int nModeIndex)
{
	auto navEarth = dynamic_cast<CHmsDynamicAvionicsLayer*>(GetNavType(HmsPageType::PageMaps));
	if (navEarth)
	{
		int nModeSet = nModeIndex;
		if (nModeIndex <= int(DynamicViewMode::NoneMode))
		{
			nModeSet = int(DynamicViewMode::NoneMode) + 1;
		}
		else if (nModeIndex >= int(DynamicViewMode::MaxMode))
		{
			nModeSet = int(DynamicViewMode::MaxMode) - 1;
		}

		DynamicViewMode eMode = (DynamicViewMode)nModeSet;
		navEarth->SetViewModeAndSetBtnState(eMode);
	}
}

HmsAviPf::CHmsNode * CHmsAvionicsDisplayMgr::GetTrackLayer()
{
    HmsAviPf::CHmsNode * node = nullptr;
    auto aviLayer = dynamic_cast<CHmsDynamicAvionicsLayer*>(GetNavType(HmsPageType::PageMaps));
    if (aviLayer)
    {
        auto navEarthLayer = aviLayer->GetNavEarthLayer();
        if (navEarthLayer)
        {
            node = navEarthLayer->GetTrackLayer();
        }
    }
    return node;
}

void CHmsAvionicsDisplayMgr::Init(const HmsAviPf::Size & size)
{
	std::vector<std::string> vActionNames;
	auto airport = CHmsLayerAirports::Create(size);
	vActionNames.clear();
	vActionNames.push_back("menubar.setting");
	AddAvionicsDisplayInfo(HmsPageType::PageAirports, airport, vActionNames);

	auto navEarth = CHmsDynamicAvionicsLayer::Create(size);
	navEarth->EnableUi(true);
	vActionNames.clear();
	vActionNames.push_back("menubar.taws2d");
	vActionNames.push_back("menubar.dashboard");
	vActionNames.push_back("menubar.flyplan");
	vActionNames.push_back("menubar.setting");
	AddAvionicsDisplayInfo(HmsPageType::PageMaps, navEarth, vActionNames);

	auto airportProgram = CHmsProcedurePreview::Create(size);
	vActionNames.clear();
	vActionNames.push_back("menubar.setting");
	AddAvionicsDisplayInfo(HmsPageType::PageAviPrograms, airportProgram, vActionNames);

    auto view3D = CHmsNavSvsLayer::GetInstance();
    view3D->ChangeSize(size);
	vActionNames.clear();
	vActionNames.push_back("menubar.setting");
	AddAvionicsDisplayInfo(HmsPageType::Page3DView, view3D, vActionNames);

	auto scratchPad = CHmsScratchPad::Create(size);
	scratchPad->SetHalfAndFullSize(size, size);
	vActionNames.clear();
	vActionNames.push_back("menubar.setting");
	AddAvionicsDisplayInfo(HmsPageType::PageScratchPad, scratchPad, vActionNames);

	m_settingPage = CHmsSettingPage::Create(Size(814, size.height));
	m_settingPage->SetAnchorPoint(Vec2(0, 0));
	m_settingPage->SetPosition(size.width - 814, 0);
	m_settingPage->SetLocalZOrder(10);

//     m_2dTawsPage = CHmsLayerTerrain::Create();
//     m_2dTawsPage->SetAnchorPoint(Vec2(0, 0));
//     m_2dTawsPage->SetPosition(size.width - 814, 0);
//     m_2dTawsPage->SetLocalZOrder(10);

	m_toolbarDashBoard = CHmsToolBarDisplayFlyData::create(Size(size.width, 126));
	m_toolbarDashBoard->SetPosition(Vec2(0, 0));
	m_toolbarDashBoard->SetLocalZOrder(9);

    m_navFplLayer = CHmsNavFplLayer::Create(size);

	m_parallelLayer = CHmsParallelLayer::Create();
	navEarth->SetParallelLayer(m_parallelLayer);
	m_settingPage->SetParallelLayer(m_parallelLayer);
}

void CHmsAvionicsDisplayMgr::SetToolBarTop(CHmsToolBarTop * bar)
{
	m_toolbarTop = bar;
}

void CHmsAvionicsDisplayMgr::SetDisplayLayer(CHmsAvionicsDisplayLayer * nav)
{
	m_layerDisplay = nav;
}

void CHmsAvionicsDisplayMgr::SetToolBarBottom(CHmsToolBarBottom * bar)
{
	m_toolbarBottom = bar;
}

CHmsToolBarBottom* CHmsAvionicsDisplayMgr::GetToolBarBottom()
{
    return m_toolbarBottom;
}

void CHmsAvionicsDisplayMgr::SetBarActionDisplaySettingPage(bool bShow)
{
	if (m_toolbarBottom)
	{
		m_toolbarBottom->SetDynamicBtnState("menubar.setting", bShow);
	}
}

void CHmsAvionicsDisplayMgr::SetBarActionDispayFlyPlanBox(bool bShow)
{
	if (m_toolbarBottom)
	{
		m_toolbarBottom->SetDynamicBtnState("menubar.flyplan", bShow);
	}
}

void CHmsAvionicsDisplayMgr::SetBarActionDisplayDashboradBar(bool bShow)
{
	if (m_toolbarBottom)
	{
		m_toolbarBottom->SetDynamicBtnState("menubar.dashboard", bShow);
	}
}

void CHmsAvionicsDisplayMgr::RunActions(const std::string & strGroup, const std::string & strAction)
{
	if (m_parallelLayer)
	{
		m_parallelLayer->RunScreenAction(strGroup, strAction);
	}
}

void CHmsAvionicsDisplayMgr::SetRecordDataPower(bool bPowerOn)
{
	if (m_settingPage)
	{
		m_settingPage->SetRecordDataPower(bPowerOn);
	}
}

void CHmsAvionicsDisplayMgr::AddAvionicsDisplayInfo(HmsPageType eType, HmsAviPf::CHmsNode * node)
{
	AvionicsDisplayInfo * info = new AvionicsDisplayInfo();
	info->autorelease();
	info->node = node;

	m_mapAviDisplayInfo.insert((int)eType, info);
}

void CHmsAvionicsDisplayMgr::AddAvionicsDisplayInfo(HmsPageType eType, HmsAviPf::CHmsNode * node, const std::vector<std::string> & vActionName)
{
	AvionicsDisplayInfo * info = new AvionicsDisplayInfo();
	info->autorelease();
	info->node = node;
	info->vDynamicBtns = vActionName;

	m_mapAviDisplayInfo.insert((int)eType, info);
}

void CHmsAvionicsDisplayMgr::Resize(const HmsAviPf::Size & size, int nLevel)
{
// 	auto NavEarthLayer = dynamic_cast<CHmsNavEarthLayer*>(GetNavType(HmsPageType::PageMaps));
// 	if (NavEarthLayer)
// 	{
// 		NavEarthLayer->Resize(size, nLevel);
// 	}
}



