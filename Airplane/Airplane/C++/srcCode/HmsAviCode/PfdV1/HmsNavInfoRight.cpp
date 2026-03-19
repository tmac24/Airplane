#include "HmsNavInfoRight.h"
USING_NS_HMS;

CHmsNavInfoRight::CHmsNavInfoRight()
{
}


CHmsNavInfoRight::~CHmsNavInfoRight()
{
}

bool CHmsNavInfoRight::Init()
{
	if (!CHmsNavInfo::Init())
	{
		return false;
	}

	m_pBoxNode->SetTexture("res/Pfd/HSI/rightBox.png");
	if (!m_pBoxNode)
	{
		return false;
	}
	m_pBoxNode->SetPosition(0, 0);
	m_pBoxNode->SetAnchorPoint(Vec2(0, 0));

	float boxWidth = m_pBoxNode->GetContentSize().width;
	float boxHeight = m_pBoxNode->GetContentSize().height;

	if (!m_pNavSrcLabel)
	{
		return false;
	}

	m_pNavSrcLabel->SetAnchorPoint(Vec2(1, 0));
	m_pNavSrcLabel->SetPosition(boxWidth - 10, 6);
	auto navSrcWidth = m_pNavSrcLabel->GetContentSize().width;
	auto navSrcHeight = m_pNavSrcLabel->GetContentSize().height;

	if (!m_pPointerIcon)
	{
		return false;
	}

	m_pPointerIcon->SetAnchorPoint(Vec2(1, 0));
	m_pPointerIcon->SetPosition(boxWidth - 60, 6);

	if (!m_pInfo1)
	{
		return false;
	}
	m_pInfo1->SetAnchorPoint(Vec2(1, 0));
	m_pInfo1->SetPosition(boxWidth - 10, 6 + navSrcHeight + 4);
	auto radioCodeHeight = m_pInfo1->GetContentSize().height;
	auto radioCodeWidth = m_pInfo1->GetContentSize().width;

	if (!m_pInfo2)
	{
		return false;
	}
	m_pInfo2->SetAnchorPoint(Vec2(1, 0));
	m_pInfo2->SetPosition(boxWidth - 10, 6 + navSrcHeight + 4 + radioCodeHeight + 4);

	return true;
}

void CHmsNavInfoRight::UnfoldBox(bool unfold)
{

	//m_pBoxNode->SetTextureRect(Rect(0, 0, 139, 29));
}

