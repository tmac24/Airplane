#include "HmsLayerCombineSvs.h"
#include "display/HmsDrawNode.h"
#include "HmsConfigXMLReader.h"
USING_NS_HMS;


CHmsLayerCombineSvs::CHmsLayerCombineSvs()
	: HmsUiEventInterface(this)
{
	
}


CHmsLayerCombineSvs::~CHmsLayerCombineSvs()
{

}

bool CHmsLayerCombineSvs::Init(const HmsAviPf::Size & size)
{
	if (!CHmsResizeableLayer::Init())
	{
		return false;
	}

	SetContentSize(size);

	m_nodeSvs = CHmsSvs::Create(size);
	m_nodeHud = CHmsLayerHud::Create(size);
	m_nodePfdV1 = CHmsLayerPfdV1::Create(size);
	m_nodePfdV2 = CHmsLayerPFDV2::Create(size);

	this->AddChild(m_nodeSvs);
	//this->AddChild(m_nodePfdV1);
	//this->AddChild(m_nodeHud);
	//this->AddChild(m_nodePfdV2);
	m_mapPfdMode.insert("hud", m_nodeHud);
	m_mapPfdMode.insert("pfdv1", m_nodePfdV1);
	m_mapPfdMode.insert("pfdv2", m_nodePfdV2);

	SetPfdMode("hud");
	
	RegisterAllNodeChild();
	return true;
}

void CHmsLayerCombineSvs::OnEnter()
{
	CHmsResizeableLayer::OnEnter();

	//SetScheduleUpdate();
}

void CHmsLayerCombineSvs::OnExit()
{
	CHmsResizeableLayer::OnExit();
	//this->SetUnSchedule();
}

void CHmsLayerCombineSvs::Update(float delta)
{
	//CHmsResizeableLayer::Update(delta);
}

void CHmsLayerCombineSvs::ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos)
{
	CHmsResizeableLayer::ResetLayerSize(size, pos);

	if (m_nodeSvs)
	{
		m_nodeSvs->ResetLayerSize(size, Vec2(0, 0));
	}
	if (m_nodePfdV1)
	{
		m_nodePfdV1->ResetLayerSize(size, Vec2(0, 0));
	}
	if (m_nodePfdV2)
	{
		m_nodePfdV2->ResetLayerSize(size, Vec2(0, 0));
	}
	if (m_nodeHud)
	{
		m_nodeHud->ResetLayerSize(size, Vec2(0, 0));
	}
}

void CHmsLayerCombineSvs::SetPfdMode(const std::string & strMode)
{
	for (auto c:m_mapPfdMode)
	{
		auto node = c.second;
		if (node)
		{
			auto uiInterface = dynamic_cast<HmsUiEventInterface*>(node);
			node->RemoveFromParent();
			if (uiInterface)
			{
				uiInterface->RemoveFromParentInterface();
			}
			if (c.first == strMode)
			{
				this->AddChild(node);
				if (uiInterface)
				{
					this->AddChildInterface(uiInterface);
				}
			}
		}
	}
}
