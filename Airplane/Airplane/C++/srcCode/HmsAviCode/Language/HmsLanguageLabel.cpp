#include "HmsLanguageLabel.h"
#include "HmsLanguageMananger.h"
#include "display/HmsLabel.h"
#include "HmsStringUtils.h"

CHmsLanguageLabel::CHmsLanguageLabel()
{
}


CHmsLanguageLabel::~CHmsLanguageLabel()
{
}

void CHmsLanguageLabel::UpdateLanguage()
{
	std::string strShowText = CHmsLanguageMananger::GetInstance()->GetCurLanguageText(m_strText);
	if (m_label)
	{
		m_label->SetString(strShowText);
		OnTextChanged(true);
	}
}

CHmsLanguageLabel * CHmsLanguageLabel::CreateLabel(const char * strText, float fSize, bool bold)
{
	auto * pRet = new(std::nothrow) CHmsLanguageLabel(); 
	if (pRet) 
	{ 
		if (bold)
			pRet->InitBold(strText, fSize);
		else
			pRet->Init(strText, fSize);
		pRet->autorelease(); 
		CHmsLanguageMananger::GetInstance()->AddObject(pRet);
		return pRet; 
	}
	else 
	{ 
		delete pRet;
		pRet = nullptr;
		return nullptr; 
	}
}

CHmsLanguageLabel * CHmsLanguageLabel::CreateLabelGbk(const char * strText, float fSize, bool bold)
{
	auto * pRet = new(std::nothrow) CHmsLanguageLabel();
	if (pRet)
	{
		if (bold)
			pRet->InitGbkBold(strText, fSize);
		else
			pRet->InitGbk(strText, fSize);
		pRet->autorelease();
		CHmsLanguageMananger::GetInstance()->AddObject(pRet);
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

void CHmsLanguageLabel::Init(const char * strTextUt8, float fSize)
{
	auto strShowText = CHmsLanguageMananger::GetInstance()->GetCurLanguageText(strTextUt8);
	m_label = HmsAviPf::CHmsLabel::createWithTTF(strShowText, "fonts/msyh.ttc", fSize);
	if (m_label.get())
	{
		m_label->SetUserObject(this);
	}

	m_strText = strTextUt8;
}

void CHmsLanguageLabel::InitBold(const char * strTextUt8, float fSize)
{
	auto strShowText = CHmsLanguageMananger::GetInstance()->GetCurLanguageText(strTextUt8);
	m_label = HmsAviPf::CHmsLabel::createWithTTF(strShowText, "fonts/msyhbd.ttc", fSize);
	if (m_label.get())
	{
		m_label->SetUserObject(this);
	}

	m_strText = strTextUt8;
}

void CHmsLanguageLabel::SetText(HmsAviPf::CHmsLabel * label, const char * strText)
{
	if (label)
	{
		auto strTextUt8 = a2u(strText);
		std::string strShowText;
		auto ll = dynamic_cast<CHmsLanguageLabel*>(label->GetUserObject());
		if (ll)
		{
			strShowText = CHmsLanguageMananger::GetInstance()->GetCurLanguageText(strTextUt8);
			ll->m_strText = strTextUt8;
			ll->OnTextChanged(false);
		}
		else
		{
			strShowText = strTextUt8;
		}

		label->SetString(strShowText);
	}
}

void CHmsLanguageLabel::InitGbk(const char * strText, float fSize)
{
	auto strTextUt8 = a2u(strText);
	auto strShowText = CHmsLanguageMananger::GetInstance()->GetCurLanguageText(strTextUt8);
	m_label = HmsAviPf::CHmsLabel::createWithTTF(strShowText, "fonts/msyh.ttc", fSize);
	if (m_label.get())
	{
		m_label->SetUserObject(this);
	}

	m_strText = strTextUt8;
}

void CHmsLanguageLabel::InitGbkBold(const char * strText, float fSize)
{
	auto strTextUt8 = a2u(strText);
	auto strShowText = CHmsLanguageMananger::GetInstance()->GetCurLanguageText(strTextUt8);
	m_label = HmsAviPf::CHmsLabel::createWithTTF(strShowText, "fonts/msyhbd.ttc", fSize);
	if (m_label.get())
	{
		m_label->SetUserObject(this);
	}

	m_strText = strTextUt8;
}

void CHmsLanguageLabel::setTextColor(const HmsAviPf::Color4B &color)
{
	if (m_label)
	{
		m_label->SetTextColor(color);
	}
}



