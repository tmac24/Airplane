#include "HmsWind.h"
USING_NS_HMS;

CHmsWind::CHmsWind()
:m_boxWidth(72.0f)
, m_boxHeight(56.0f)
{
}


CHmsWind::~CHmsWind()
{
}

bool CHmsWind::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_pBoxNode = CHmsDrawNode::Create();
	m_pBoxNode->SetPosition(m_boxWidth / 2.0, m_boxHeight / 2.0);
	//m_pBoxNode->DrawInnerRect(-m_boxWidth / 2.0, m_boxHeight / 2.0, m_boxWidth / 2.0, -m_boxHeight / 2.0);

	m_pNoDataLabel = CHmsGlobal::CreateLabel("NO WIND\n   DATA", 14);

	float wdWidth = 0.0, degWidth = 0.0, wsWidth = 0.0;
	m_pWindDirLabel = CHmsGlobal::CreateLabel("333", 16);
	if (m_pWindDirLabel)
	{
		m_pWindDirLabel->SetAnchorPoint(Vec2(0, 0));
		m_pWindDirLabel->SetPosition(0, 2);
		wdWidth = m_pWindDirLabel->GetContentSize().width;
	}

	auto degree = CHmsGlobal::CreateLabel("o", 12);
	if (degree)
	{
		degree->SetAnchorPoint(Vec2(0, 0.5));
		if (m_pWindDirLabel)
		{
			degree->SetPosition(wdWidth, m_pWindDirLabel->GetContentSize().height - 2);
		}
		degWidth = degree->GetContentSize().width;
	}

	m_pWindSpdLabel = CHmsGlobal::CreateLabel("/ 21", 16);
	if (m_pWindSpdLabel)
	{
		m_pWindSpdLabel->SetAnchorPoint(Vec2(0, 0));
		m_pWindSpdLabel->SetPosition(wdWidth + degWidth, 2);
		wsWidth = m_pWindSpdLabel->GetContentSize().width;
	}

	m_pWindDataNode = CHmsNode::Create();
	if (m_pWindDirLabel)
	{
		m_pWindDataNode->AddChild(m_pWindDirLabel);
	}

	if (degree)
	{
		m_pWindDataNode->AddChild(degree);
	}
	if (m_pWindSpdLabel)
	{
		m_pWindDataNode->AddChild(m_pWindSpdLabel);
	}
	m_pWindDataNode->SetPosition(-(wdWidth + degWidth + wsWidth) / 2.0, 0);

	m_pWindDirIcon = CHmsImageNode::Create("res/Pfd/Wind/arrow.png");
	if (m_pWindDirIcon)
	{
		m_pWindDirIcon->SetPosition((wdWidth + degWidth + wsWidth) / 2.0, -m_pWindDirIcon->GetContentSize().height / 2.0);
		m_pWindDataNode->AddChild(m_pWindDirIcon);
	}

	if (m_pNoDataLabel)
	{
		m_pBoxNode->AddChild(m_pNoDataLabel);
	}
	m_pBoxNode->AddChild(m_pWindDataNode);
	this->AddChild(m_pBoxNode);

	SetNoData();
	return true;
}

void CHmsWind::SetNoData()
{
	if (m_pNoDataLabel)
	{
		m_pNoDataLabel->SetVisible(true);
	}
	m_pWindDataNode->SetVisible(false);
}

void CHmsWind::SetData(const int &windDir, const int &windSpd)
{
	if (m_pNoDataLabel && m_pNoDataLabel->IsVisible())
	{
		m_pNoDataLabel->SetVisible(false);
	}
	if (!m_pWindDataNode->IsVisible())
	{
		m_pWindDataNode->SetVisible(true);
	}

	std::string strWD = CHmsGlobal::FormatString("%03d", windDir);
	if (m_pWindDirLabel)
	{
		m_pWindDirLabel->SetString(strWD);
	}

	std::string strWS = CHmsGlobal::FormatString("/ %d", windSpd);
	if (m_pWindSpdLabel)
	{
		m_pWindSpdLabel->SetString(strWS);
	}

	if (m_pWindDirIcon)
	{
		m_pWindDirIcon->SetRotation(windDir);
	}
}
