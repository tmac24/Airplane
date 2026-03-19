#include "HmsNavInfoLeft.h"
USING_NS_HMS;

CHmsNavInfoLeft::CHmsNavInfoLeft()
{
}


CHmsNavInfoLeft::~CHmsNavInfoLeft()
{
}

bool CHmsNavInfoLeft::Init()
{
	if (!CHmsNavInfo::Init())
	{
		return false;
	}

	m_pBoxNode->SetTexture("res/Pfd/HSI/leftBox.png");
	if (!m_pBoxNode)
	{
		return false;
	}
	m_pBoxNode->SetPosition(0, 0);
	m_pBoxNode->SetAnchorPoint(Vec2(1, 0));

	float boxWidth = m_pBoxNode->GetContentSize().width;
	float boxHeight = m_pBoxNode->GetContentSize().height;

	m_pNavSrcLabel->SetAnchorPoint(Vec2(0, 0));
	m_pNavSrcLabel->SetPosition(10, 6);
	auto navSrcHeight = m_pNavSrcLabel->GetContentSize().height;

	m_pPointerIcon->SetAnchorPoint(Vec2(1, 0));
	m_pPointerIcon->SetPosition(boxWidth - 30, 6);

	m_pInfo1->SetAnchorPoint(Vec2(0, 0));
	m_pInfo1->SetPosition(10, 6 + navSrcHeight + 4);
	auto radioCodeHeight = m_pInfo1->GetContentSize().height;
	auto radioCodeWidth = m_pInfo1->GetContentSize().width;

	m_pInfo2->SetAnchorPoint(Vec2(0, 0));
	m_pInfo2->SetPosition(10, 6 + navSrcHeight + 4 + radioCodeHeight + 4);

	return true;
}

void CHmsNavInfoLeft::UnfoldBox(bool unfold)
{

	//m_pBoxNode->SetTextureRect(Rect(0, 0, 139, 29));
}

