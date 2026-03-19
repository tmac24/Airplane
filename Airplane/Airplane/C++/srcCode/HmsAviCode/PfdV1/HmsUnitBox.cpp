#include "HmsUnitBox.h"
USING_NS_HMS;

CHmsUnitBox::CHmsUnitBox()
: m_unitBoxWidth(74)
, m_unitBoxHeight(26)
, m_defBorderColor(Color4B::WHITE)
, m_selBorderColor(Color4B::GREEN)
, m_curUnits(inHg)
{
}


CHmsUnitBox::~CHmsUnitBox()
{
}

bool CHmsUnitBox::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_pNodeMetric = CHmsDrawNode::Create();
	m_pNodeMetric->SetPosition(0, 0);
	{
		auto label = CHmsGlobal::CreateLabel("Metric", 14);
		if (label)
		{
			label->SetPosition(m_unitBoxWidth / 2.0, -m_unitBoxHeight / 2.0);
			m_pNodeMetric->AddChild(label);
		}
	}

	m_pNodeInHg = CHmsDrawNode::Create();
	m_pNodeInHg->SetPosition(m_unitBoxWidth + 10, 0);
	{
		auto label = CHmsGlobal::CreateLabel("inHg", 14);
		if (label)
		{
			label->SetPosition(m_unitBoxWidth / 2.0, -m_unitBoxHeight / 2.0);

			m_pNodeInHg->AddChild(label);
		}
	}

	this->AddChild(m_pNodeMetric);
	this->AddChild(m_pNodeInHg);

	ChangeUnits(inHg);

	return true;
}

void CHmsUnitBox::ChangeUnits(Units u)
{
	m_curUnits = u;
	m_pNodeInHg->clear();
	m_pNodeMetric->clear();

	switch (m_curUnits)
	{
	case CHmsUnitBox::inHg:
		m_pNodeInHg->DrawInnerRoundRect(0.0f, 0.0f, m_unitBoxWidth, -m_unitBoxHeight, m_unitBoxHeight / 2.0f, 2.0f, 10, Color4F(m_selBorderColor));
		m_pNodeMetric->DrawInnerRoundRect(0.0f, 0.0f, m_unitBoxWidth, -m_unitBoxHeight, m_unitBoxHeight / 2.0f, 1.4f, 10, Color4F(m_defBorderColor));
		break;
	case CHmsUnitBox::Metric:
		m_pNodeInHg->DrawInnerRoundRect(0.0f, 0.0f, m_unitBoxWidth, -m_unitBoxHeight, m_unitBoxHeight / 2.0f, 1.4f, 10, Color4F(m_defBorderColor));
		m_pNodeMetric->DrawInnerRoundRect(0.0f, 0.0f, m_unitBoxWidth, -m_unitBoxHeight, m_unitBoxHeight / 2.0f, 2.0f, 10, Color4F(m_selBorderColor));
		break;
	default:
		break;
	}
}
