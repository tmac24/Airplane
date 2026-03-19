#include "HmsDialog.h"
#include "base/HmsEventListenerTouch.h"
#include "../HmsAviDisplayUnit.h"
#include "HmsDrawNode.h"
#include "Language/HmsLanguageMananger.h"

NS_HMS_BEGIN

CHmsDialog::CHmsDialog()
:m_titleBarHeigth(48)
{

}

CHmsDialog::~CHmsDialog()
{
}

bool CHmsDialog::Init()
{
	//默认宽高 600
	return InitWithSize(Size(600, 600));
}

bool CHmsDialog::InitWithSize(const Size &size)
{
	if (!CHmsUiPanel::Init())
	{
		return false;
	}

	this->SetContentSize(size);

	auto bgMask = CHmsDrawNode::Create();
	bgMask->DrawDisk(2500, 2.0, 10, Color4F(0.0, 0.0, 0.0, 0.5));
	this->AddChild(bgMask);

 	m_pImageBackground = CHmsStretchImageNode::Create("res/display/dialog_bg.png", size); 	
	if (m_pImageBackground)
	{
		m_pImageBackground->SetAnchorPoint(Vec2(0, 0));
	}

	int leftmargin = 12;
	int topmargin = 10;
	m_pImageTitlebar = CHmsStretchImageNode::Create("res/display/dialog_titlebar.png", size.width - leftmargin * 2, m_titleBarHeigth);
	if (m_pImageTitlebar)
	{
		m_pImageTitlebar->SetAnchorPoint(Vec2(0, 1));
		m_pImageTitlebar->SetPosition(Vec2(leftmargin, size.height - topmargin));
		HmsAviPf::Size titbleBarSize = m_pImageTitlebar->GetContentSize();


		m_pLabelTitle = CHmsLabel::createWithTTF("", "fonts/msyh.ttc", 24);
		if (m_pLabelTitle)
		{
			m_pLabelTitle->SetPosition(titbleBarSize.width / 2.0, titbleBarSize.height / 2.0);
			m_pImageTitlebar->AddChild(m_pLabelTitle);
		}
	}


	if (m_pImageBackground)
	{
		this->AddChild(m_pImageBackground);
	}
	if (m_pImageTitlebar)
	{
		this->AddChild(m_pImageTitlebar);
	}

	RegisterTouchEvent(-1);
	return true;
}

void CHmsDialog::SetTitle(const std::string &title)
{
	if (m_pLabelTitle)
	{
		std::string languageTitle = CHmsLanguageMananger::GetInstance()->GetCurLanguageText(title);
		m_pLabelTitle->SetString(languageTitle);
	}
}

std::string CHmsDialog::GetTitle()
{
	if (m_pLabelTitle)
	{
		return m_pLabelTitle->GetString();
	}
	else
	{
		return "";
	}
}

float CHmsDialog::GetTitleHeight()
{
	return m_titleBarHeigth + 20;
}

float CHmsDialog::GetContentsHeigth()
{
	return GetContentSize().height - GetTitleHeight();
}

NS_HMS_END

