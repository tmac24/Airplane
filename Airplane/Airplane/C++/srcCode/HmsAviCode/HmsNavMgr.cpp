#include "HmsNavMgr.h"
#include "Nav/HmsNavEarthFrame.h"
#include "Airports/HmsLayerAirports.h"
#include "Airports/HmsLayerAirportProgram.h"
#include "ui/HmsUiEventInterface.h"
#include "Nav/HmsNavEarthLayer.h"
#include "Airports/HmsProcedurePreview.h"

CHmsNavMgr * CHmsNavMgr::GetInstance()
{
	static CHmsNavMgr * s_pNavMgr = nullptr;
	if (s_pNavMgr == nullptr)
	{
		s_pNavMgr = new CHmsNavMgr();
	}
	return s_pNavMgr;
}

CHmsNavMgr::CHmsNavMgr()
:m_eType(HmsNavType::NavNone)
, m_lastNavType(HmsNavType::NavNone)
{

}

CHmsNavMgr::~CHmsNavMgr()
{

}

HmsAviPf::CHmsNode * CHmsNavMgr::ShowNavType(HmsNavType eType)
{
	HmsAviPf::CHmsNode * nodeTemp = nullptr;
	if (m_eType != eType)
	{
		auto it = m_mapNavNode.find((int)eType);
		if (it != m_mapNavNode.end())
		{
			if (m_navLayer && it->second)
			{
				auto itLast = m_mapNavNode.find((int)m_eType);
				if (itLast != m_mapNavNode.end())
				{
					if (itLast->second)
					{
						itLast->second->RemoveFromParent();
					}
				}

				nodeTemp = it->second;
				m_navLayer->AddChild(it->second);
				m_navLayer->RemoveAllChildInterface();
				m_navLayer->RegisterAllNodeChild();

				auto mgr = CHmsNavMgr::GetInstance()->GetBtnMgr();
				mgr->CreateOrSelectGroup(nodeTemp);
			}
		}

        m_lastNavType = m_eType;
		m_eType = eType;
	}
	return nodeTemp;
}

HmsAviPf::CHmsNode * CHmsNavMgr::GetNavType(HmsNavType eType)
{
	HmsAviPf::CHmsNode * nodeTemp = nullptr;
	
	auto it = m_mapNavNode.find((int)eType);
	if (it != m_mapNavNode.end())
	{
		if (it->second)
		{
			nodeTemp = it->second;
		}
	}

	return nodeTemp;
}

void CHmsNavMgr::ShowLastNavType()
{
    ShowNavType(m_lastNavType);
}

void CHmsNavMgr::Init(const HmsAviPf::Size & size)
{
	auto navEarth = CHmsNavEarthLayer::Create(size);
	AddNavType(HmsNavType::NavMap, navEarth);

	auto airport = CHmsLayerAirports::Create(size);
	AddNavType(HmsNavType::NavAirport, airport);

	//auto airportProgram = CHmsLayerAirportProgram::Create(size);
	//AddNavType(HmsNavType::NavAirportProgram, airportProgram);

    auto procedure = CHmsProcedurePreview::Create(size);
    AddNavType(HmsNavType::NavAirportProgramPreview, procedure);
}

void CHmsNavMgr::SetNavLayer(CHmsLayerNav * nav)
{
	m_navLayer = nav;
}

void CHmsNavMgr::AddNavType(HmsNavType eType, HmsAviPf::CHmsNode * node)
{
	m_mapNavNode.insert((int)eType, node);
}

void CHmsNavMgr::Resize(const HmsAviPf::Size & size, int nLevel)
{
	auto NavEarthLayer = dynamic_cast<CHmsNavEarthLayer*>(GetNavType(HmsNavType::NavMap));
	if (NavEarthLayer)
	{
		NavEarthLayer->Resize(size, nLevel);
	}
}


