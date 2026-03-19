#include "HmsNavInfo.h"
USING_NS_HMS;

CHmsNavInfo::CHmsNavInfo()
: m_pointerIconColor(Color3B::WHITE)
, m_pointerIconPath("")
, m_navSrc("--")
, m_info1("--")
, m_info2("--")
{
}


CHmsNavInfo::~CHmsNavInfo()
{
}

bool CHmsNavInfo::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_pBoxNode = CHmsImageNode::Create();

	m_pNavSrcLabel = CHmsGlobal::CreateLabel(m_navSrc.c_str(), 14);

	m_pPointerIcon = CHmsImageNode::Create();

	m_pInfo1 = CHmsGlobal::CreateLabel(m_info1.c_str(), 14);

	m_pInfo2 = CHmsGlobal::CreateLabel(m_info2.c_str(), 14);

	if (m_pNavSrcLabel)
	{
		m_pBoxNode->AddChild(m_pNavSrcLabel);
	}
	m_pBoxNode->AddChild(m_pPointerIcon);
	if (m_pInfo1)
	{
		m_pBoxNode->AddChild(m_pInfo1);
	}
	if (m_pInfo2)
	{
		m_pBoxNode->AddChild(m_pInfo2);
	}

	this->AddChild(m_pBoxNode);

	this->SetNoData();

	return true;
}

void CHmsNavInfo::UnfoldBox(bool unfold)
{

}

void CHmsNavInfo::SetPointerIconColor(const HmsAviPf::Color3B &color)
{
	m_pointerIconColor = color;
	if (m_pPointerIcon)
	{
		m_pPointerIcon->SetColor(m_pointerIconColor);
	}
}

void CHmsNavInfo::SetPointerIconPath(const std::string &path)
{
	if (m_pPointerIcon)
	{
		m_pPointerIcon->SetTexture(path);
	}
}

void CHmsNavInfo::SetNavSrc(const std::string &navSrc)
{
	m_navSrc = navSrc;
	if (m_pNavSrcLabel)
	{
		m_pNavSrcLabel->SetString(navSrc);
	}
}

void CHmsNavInfo::SetInfo1(const std::string &info1)
{
	m_info1 = info1;
	if (m_pInfo1)
	{
		if (!m_pInfo1->IsVisible())
		{
			m_pInfo1->SetVisible(true);

		}
		m_pInfo1->SetString(m_info1);
	}
}

void CHmsNavInfo::SetInfo2(const std::string &info2)
{
	m_info2 = info2;
	if (m_pInfo2)
	{
		if (!m_pInfo2->IsVisible())
		{
			m_pInfo2->SetVisible(true);

		}
		m_pInfo2->SetString(m_info2);
	}
}

void CHmsNavInfo::SetNoData()
{
	if (m_pInfo1)
	{
		m_pInfo1->SetString("NO DATA");
	}
	if (m_pInfo2)
	{
		m_pInfo2->SetVisible(false);
	}
}
