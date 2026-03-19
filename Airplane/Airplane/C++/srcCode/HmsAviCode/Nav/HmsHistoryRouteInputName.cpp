#include "HmsHistoryRouteInputName.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsFrame2DRoot.h"
#include "HmsNavFplLayer.h"

CHmsHistoryRouteInputName::CHmsHistoryRouteInputName()
: HmsUiEventInterface(this)
, m_callbackFunc(nullptr)
{

}

CHmsHistoryRouteInputName::~CHmsHistoryRouteInputName()
{

}

bool CHmsHistoryRouteInputName::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	auto pBgNode = CHmsDrawNode::Create();
	pBgNode->SetAnchorPoint(Vec2(0, 0));
	pBgNode->SetPosition(0, 0);
	this->AddChild(pBgNode);
	pBgNode->DrawSolidRect(Rect(Vec2(0, 0), windowSize), Color4F(Color3B(0x0e, 0x19, 0x25)));
	//画边框
	{
		std::vector<Vec2> vec;
		vec.push_back(Vec2(0, 0));
		vec.push_back(Vec2(windowSize.width, 0));
		vec.push_back(Vec2(windowSize.width, windowSize.height));
		vec.push_back(Vec2(0, windowSize.height));
		vec.push_back(Vec2(0, 0));
		pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
	}

	m_pLineEdit = CHmsLineEidt::Create("res/airport/search_bg.png");
	m_pLineEdit->SetAnchorPoint(0, 0);
	m_pLineEdit->SetPosition(10, windowSize.height - 110);
	m_pLineEdit->SetTextColor(Color4B::WHITE);
	m_pLineEdit->SetSize(Size(windowSize.width - 290, 100));
	//m_pLineEdit->SetMarginRight(60);
	//m_pLineEdit->Focus();
	this->AddChild(m_pLineEdit);

    auto pLabel = CHmsGlobal::CreateUTF8Label("", 32, true);
    m_pLineEdit->SetTextLabel(pLabel);

	auto CreateButton = [=](Size s, Vec2 pos, const char * text)
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
		pItem->AddText(text, 32, Color4B::WHITE, true);
		pItem->SetAnchorPoint(Vec2(0, 0));
		pItem->SetPosition(pos);
		//pItem->SetOnClickedFunc(std::bind(&CHmsRouteNodeWin::ShowPage, this, std::placeholders::_1, WinPageShow::PAGE_ACTION));
		this->AddChild(pItem);
		return pItem;
	};

	auto btnSize = Size(260, 100);
	CHmsUiStretchButton *pItem = nullptr;
    pItem = CreateButton(btnSize, Vec2(windowSize.width - btnSize.width - 10, windowSize.height - btnSize.height - 10), "Clear");
	pItem->SetTextColor(Color4B::RED);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		m_vecIndex.clear();
		m_strName.clear();
		m_pLineEdit->SetText(m_strName);
	});

    pItem = CreateButton(btnSize, Vec2(windowSize.width / 2 - btnSize.width - 100, 10), "Enter");
	pItem->SetTextColor(Color4B::GREEN);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		if (m_strName.size() > 0)
		{
			CloseDlg();
			if (m_callbackFunc)
			{
				m_callbackFunc(m_strName);
			}
		}	
	});
    pItem = CreateButton(btnSize, Vec2(windowSize.width / 2 + 100, 10), "Cancel");
	pItem->SetTextColor(Color4B::GRAY);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		CloseDlg();
	});

	this->RegisterAllNodeChild();

	return true;
}

void CHmsHistoryRouteInputName::LineEditBlur()
{
	if (m_pLineEdit)
	{
		m_pLineEdit->Blur();
	}
}

void CHmsHistoryRouteInputName::ShowDlg(std::string &name, std::vector<int> vecIndex, std::function<void(std::string)> callback)
{
	this->SetVisible(true);
	m_strName = name;
	if (m_strName.size() > 28)
	{
		m_strName = m_strName.substr(0, 28);
		while (m_strName.size() > 0 && (unsigned char)m_strName.back() > 0x7f)
		{
			m_strName.pop_back();
		}
		vecIndex.clear();
	}
	m_vecIndex = vecIndex;
	m_callbackFunc = callback;

	m_pLineEdit->SetText(m_strName);
}

void CHmsHistoryRouteInputName::CloseDlg()
{
	m_pLineEdit->Blur();
	this->SetVisible(false);
    if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
	{
        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false,
			HMS_CALLBACK_1(CHmsHistoryRouteInputName::OnKeyboardCallback, this),
			HMS_CALLBACK_0(CHmsHistoryRouteInputName::LineEditBlur, this));
	}
}

bool CHmsHistoryRouteInputName::OnPressed(const Vec2 & posOrigin)
{
	auto rect = m_pLineEdit->GetRectFromParent();
	if (rect.containsPoint(posOrigin))
	{
		m_pLineEdit->Focus();

        if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
		{
            CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(true,
				HMS_CALLBACK_1(CHmsHistoryRouteInputName::OnKeyboardCallback, this), 
				HMS_CALLBACK_0(CHmsHistoryRouteInputName::LineEditBlur, this));
		}
	}
	else
	{
		m_pLineEdit->Blur();
		HmsUiEventInterface::OnPressed(posOrigin);
	}

	return true;
}

void CHmsHistoryRouteInputName::OnKeyboardCallback(char c)
{
	if ('\b' == c)
	{
		if ((m_vecIndex.size() > 0) && (m_strName.size() > 1) && (m_strName.size() - 1 == m_vecIndex.back()))
		{
			if (m_vecIndex.size() >= 2)
			{
				int lastIndex = m_vecIndex[m_vecIndex.size() - 2];
				m_strName = m_strName.substr(0, lastIndex + 1);
				m_vecIndex.pop_back();
				while (m_strName.size() > 0 && (unsigned char)m_strName.back() > 0x7f)
				{
					m_strName.pop_back();
				}
			}
			else
			{
				m_vecIndex.clear();
				m_strName.clear();
			}
		}
		else
		{
			if (!m_strName.empty())
			{
				m_strName.pop_back();
				while (m_strName.size() > 0 && (unsigned char)m_strName.back() > 0x7f)
				{
					m_strName.pop_back();
				}
			}
		}	
	}
	else
	{
		if (m_strName.size() < 28)
		{
			m_strName.push_back(c);
		}
	}
	
	m_pLineEdit->SetText(m_strName);
}


