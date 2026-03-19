#include "HmsAftObjectLayer.h"
//#include "HmsPopDataDef.h"
#include "HmsDataBus_Old.h"
USING_NS_HMS;

CHmsAftObjectLayer::CHmsAftObjectLayer()
	: m_bCurAftCenter(true)
	, m_bNorthMode(false)
{
}


CHmsAftObjectLayer::~CHmsAftObjectLayer()
{
}

bool CHmsAftObjectLayer::Init()
{
	if (!CHmsMapLayerInterface::Init())
	{
		return false;
	}

	m_aftCurInRoot = CHmsImageNode::Create("res/airport/plane.png");//res/Airplane/Dart450Overlook.png
	if (m_aftCurInRoot)
	{
		m_aftCurInRoot->SetColor(Color3B::BLUE);
		this->AddChild(m_aftCurInRoot);
		m_aftCurInRoot->SetVisible(m_bCurAftCenter);
	}


	m_aftCurInMoveable = CHmsImageNode::Create("res/airport/plane.png");
	if (m_aftCurInMoveable)
	{
		m_aftCurInMoveable->SetColor(Color3B::BLUE);
		AddMapLayerChild(m_aftCurInMoveable);

		m_aftCurInMoveable->SetVisible(!m_bCurAftCenter);
	}

	m_nodeDistanceCicle = CHmsNode::Create();
	this->AddChild(m_nodeDistanceCicle);

	return true;
}

void CHmsAftObjectLayer::OnEnter()
{
	CHmsMapLayerInterface::OnEnter();
	this->SetScheduleUpdate();
}

void CHmsAftObjectLayer::OnExit()
{
	CHmsMapLayerInterface::OnExit();
	this->SetUnSchedule();
}

void CHmsAftObjectLayer::Update(float delta)
{
#if 0//todo
	if (!m_bCurAftCenter)
	{
		auto insData = CHmsGlobal::GetInstance()->GetDataBus()->GetInsGnssData();

		auto fHeading = insData.yaw;// dHeading;/*m_fCurHeading + localData.dHeading - m_fCurHeading;*/
		auto pos = GetGooglePosition(insData.Lng, insData.Lat);
		pos *= m_fScale;
		m_aftCurInMoveable->SetPosition(pos);
		m_aftCurInMoveable->SetRotation(fHeading);
		m_nodeDistanceCicle->SetPosition(pos);
	}
	else
	{
		auto insData = CHmsGlobal::GetInstance()->GetDataBus()->GetInsGnssData();
		auto fHeading = insData.yaw;
		if (m_bNorthMode)
		{
			m_aftCurInRoot->SetRotation(fHeading);
		}
		else
		{
			m_aftCurInRoot->SetRotation(m_fCurHeading);
		}
				
	}
	m_nodeDistanceCicle->SetRotation(m_fCurHeading);
#endif
}

void CHmsAftObjectLayer::UpdateLayer()
{
	
}

void CHmsAftObjectLayer::SetCurAftCenter(bool bCenter)
{
	m_bCurAftCenter = bCenter;
	if (m_aftCurInRoot)
	{
		m_aftCurInRoot->SetVisible(m_bCurAftCenter);
	}

	if (m_aftCurInMoveable)
	{
		m_aftCurInMoveable->SetVisible(!m_bCurAftCenter);
	}

	m_nodeDistanceCicle->RemoveFromParent();
	if (m_bCurAftCenter)
	{
		m_nodeDistanceCicle->SetPosition(Vec2());
		this->AddChild(m_nodeDistanceCicle);
	}
	else
	{
		AddMapLayerChild(m_nodeDistanceCicle);
	}
}

void CHmsAftObjectLayer::SetDistanceCicleNode(HmsAviPf::CHmsNode * pNode)
{
	m_nodeDistanceCicle->AddChild(pNode);
	m_nodeDistanceCicle->RemoveFromParent();
	if (m_bCurAftCenter)
	{
		m_nodeDistanceCicle->SetPosition(Vec2());
		this->AddChild(m_nodeDistanceCicle);
	}
	else
	{
		AddMapLayerChild(m_nodeDistanceCicle);
	}
}

void CHmsAftObjectLayer::SetNorthMode(bool bNorth)
{
	m_bNorthMode = bNorth;
}
