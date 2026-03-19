#include "HmsFixedLineEidt.h"
#include "HmsAviDisplayUnit.h"
#include "base/HmsFileUtils.h"

NS_HMS_BEGIN

CHmsFixedLineEidt::CHmsFixedLineEidt()
: m_textColor(0x00, 0x00, 0x00, 0xFF)
, m_lastTime(0.0)
, m_curentIndex(0)
, m_unchangeableChars("")
, m_measureChar("0")
, m_defaltChar("-")
, OnTextChanged(nullptr)
, OnCurrentIndexChanged(nullptr)
{
	m_time = new CHmsTime;
}

CHmsFixedLineEidt::~CHmsFixedLineEidt()
{
	if (m_time)
	{
		delete m_time;
		m_time = nullptr;
	}
}

CHmsFixedLineEidt* CHmsFixedLineEidt::Create(const HmsAviPf::Size &size, const int &length /*= 1*/)
{
	return Create("res/display/lineedit.png", size, length);
}

CHmsFixedLineEidt* CHmsFixedLineEidt::Create(const std::string &bgImage, HmsAviPf::Size size, const int &length /*= 1*/)
{
	CHmsFixedLineEidt *ret = new STD_NOTHROW CHmsFixedLineEidt();
	if (ret && ret->InitWithBackgroundImage(length, bgImage, size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsFixedLineEidt::InitWithBackgroundImage(const int &length, const std::string& bgImage, const HmsAviPf::Size &size)
{
	if (!InitWithFile(bgImage, Rect(0, 0, size.width, size.height)))
	{
		return false;
	}

	m_ttfConfig = CreateTTFConfig();

	m_pNodeCursor = CHmsDrawNode::Create();
	this->AddChild(m_pNodeCursor);

	SetLength(length);

 	this->SetScheduleUpdate();

	return true;
}

int CHmsFixedLineEidt::GetCurrentIndex()
{
	return m_curentIndex;
}

void CHmsFixedLineEidt::SetCurrentIndex(int index)
{
	m_curentIndex = index >= m_textLength ? m_textLength - 1 : index;
	CurentIndexChanged();
}

char CHmsFixedLineEidt::CurentIndexChar()
{
	return m_pLabelTexts.at(m_curentIndex)->GetString()[0];
}

void CHmsFixedLineEidt::SetUnchangeableChars(const std::string &chars)
{
	m_unchangeableChars = chars;
}

void CHmsFixedLineEidt::SetFontSize(const float &fontSize)
{
	m_ttfConfig.fontSize = fontSize;
	for (CHmsLabel *label : m_pLabelTexts)
	{
		label->SetTTFConfig(m_ttfConfig);
	}
	ContentLayout();
}

void CHmsFixedLineEidt::SetText(const std::string &text)
{
	int textLength = text.size();
	for (int i = 0; i < m_textLength; i++)
	{
		CHmsLabel* label = m_pLabelTexts.at(i);
		std::string inputStr = "";
		if (i < textLength)
		{
			inputStr = inputStr + text.at(i);
		}
		else
		{
			inputStr = m_defaltChar;
		}
		
		if (label)
		{
			label->SetString(inputStr);
		}
	}

	if (OnTextChanged)
	{
		OnTextChanged(text);
	}
}

std::string CHmsFixedLineEidt::GetText()
{
	std::string text = "";
	for (int i = 0; i < m_textLength; i++)
	{
		text += m_pLabelTexts.at(i)->GetString();
	}

	return text;
}

void CHmsFixedLineEidt::SetTextColor(const Color4B &color)
{
	m_textColor = color;

	for (CHmsLabel* label : m_pLabelTexts)
	{
		label->SetTextColor(color);
	}
}

void CHmsFixedLineEidt::SetDefaultChar(const char &defaultChar)
{
	std::string c = "";
	c += defaultChar;
	m_defaltChar = c;
	for (CHmsLabel* label : m_pLabelTexts)
	{
		label->SetString(m_defaltChar);
	}
}

void CHmsFixedLineEidt::Backspace()
{
	if (m_pLabelTexts.size() == 0) return;
	CHmsLabel * label = m_pLabelTexts.at(m_curentIndex);
	label->SetString(m_defaltChar);
	ToPreviousIndex();

	if (OnTextChanged)
	{
		OnTextChanged(GetText());
	}
}

void CHmsFixedLineEidt::ReplaceCurentIndexChar(const char &c)
{
	if (m_pLabelTexts.size() == 0) return;

	std::string str = "";
	str += c;

	CHmsLabel * label = m_pLabelTexts.at(m_curentIndex);
	label->SetString(str);

	ToNextIndex();

	if (OnTextChanged)
	{
		OnTextChanged(GetText());
	}
}

void CHmsFixedLineEidt::ToNextIndex()
{
	if (m_curentIndex < m_textLength - 1)
	{
		//判断当前字符是否为不可改变字符，如果是索引再往后移动
		int nextIndex = m_curentIndex + 1;
		bool canMoveToPre = false;
		while (nextIndex <= m_textLength - 1)
		{
			if (!IsUnchangeableChar(nextIndex))
			{
				canMoveToPre = true;
				break;
			}

			nextIndex = nextIndex + 1;
		}

		if (canMoveToPre)
		{
			SetCurrentIndex(nextIndex);
		}
	}
}

void CHmsFixedLineEidt::ToPreviousIndex()
{
	if (m_curentIndex > 0)
	{
		//判断当前字符是否为不可改变字符，如果是索引再往前移动
		int preIndex = m_curentIndex - 1;
		bool canMoveToPre = false;
		while (preIndex >= 0)
		{
			if (!IsUnchangeableChar(preIndex))
			{
				canMoveToPre = true;
				break;
			}

			preIndex = preIndex - 1;
		}

		if (canMoveToPre)
		{
			SetCurrentIndex(preIndex);
		}
	}
}

void CHmsFixedLineEidt::ContentLayout()
{
	auto height = this->GetContentSize().height;
	auto width = this->GetContentSize().width;

	int gap = 2;
	int textsWidth = 0.0;

	CHmsLabel *measureLabel = CHmsLabel::Create(m_ttfConfig, m_measureChar);
	int measureWidth = measureLabel->GetContentSize().width;

	textsWidth = measureWidth * m_textLength + gap * (m_textLength -  1);

	int x = width / 2.0 - textsWidth / 2.0 + measureWidth / 2.0;
	for (CHmsLabel* label : m_pLabelTexts)
	{
		label->SetAnchorPoint(Vec2(0.5, 0.5));
		label->SetPosition(x, height / 2.0);

		x += measureWidth + gap;
	}

	UpdateCursor();
}

void CHmsFixedLineEidt::Update(float delta)
{
	if (IsVisible())
	{
		auto temp = m_time->GetTime();
		if (temp - m_lastTime > 0.5)
		{
			if (!m_pNodeCursor->IsVisible())
			{
				if (m_curentLabel) { m_curentLabel->SetTextColor(Color4B::WHITE); }

				m_pNodeCursor->SetVisible(true);
			}
			else
			{
				if (m_curentLabel) { m_curentLabel->SetTextColor(m_textColor); }

				m_pNodeCursor->SetVisible(false);
			}

			m_lastTime = temp;
		}
	}
}

void CHmsFixedLineEidt::EnableChanged()
{
	UpdateContentState();
}

HmsAviPf::TTFConfig CHmsFixedLineEidt::CreateTTFConfig()
{
	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = CHmsFileUtils::getInstance()->fullPathForFilename("fonts/msyh.ttc");
	ttfConfig.fontSize = 16;
	return ttfConfig;
}

void CHmsFixedLineEidt::UpdateContentState()
{
// 	if (m_enabled)
// 	{
// 		if (m_pImageBackground)		m_pImageBackground->SetColor(Color3B::WHITE);

// 		//if (m_pLabelText)			m_pLabelText->SetTextColor(Color4B::WHITE);
// 		if (m_pNodeCursor)			m_pNodeCursor->SetVisible(true);
// 	}
// 	else
// 	{
// 		if (m_pImageBackground)	m_pImageBackground->SetColor(Color3B::GRAY);

// 		//if (m_pLabelText)		m_pLabelText->SetTextColor(Color4B::GRAY);
// 		if (m_pNodeCursor)		m_pNodeCursor->SetVisible(false);
// 	}
}

void CHmsFixedLineEidt::CurentIndexChanged()
{
	if (m_curentLabel) { m_curentLabel->SetTextColor(m_textColor); }

	m_curentLabel = m_pLabelTexts.at(m_curentIndex);
	UpdateCursor();

	if (OnCurrentIndexChanged)
	{
		OnCurrentIndexChanged(m_curentIndex);
	}
}

void CHmsFixedLineEidt::UpdateCursor()
{
	if (m_pLabelTexts.size() == 0) return;

	if (m_pNodeCursor && m_curentLabel)
	{
		CHmsLabel *label = m_curentLabel;
		float width = label->GetContentSize().width;
		float height = label->GetContentSize().height;
		Vec2 labelPos = label->GetPoistion();
		Vec2 cursorPos(labelPos.x - width / 2.0, labelPos.y - height / 2.0);
		m_pNodeCursor->SetPosition(cursorPos);

		m_pNodeCursor->clear();
		Rect rect(0, 0, width + 3, height);
		m_pNodeCursor->DrawSolidRect(rect, Color4F(0, 0, 0, 1));
	}
}

bool CHmsFixedLineEidt::IsUnchangeableChar(int index)
{
	char c = m_pLabelTexts.at(index)->GetString()[0];
	int pos = m_unchangeableChars.find(c);
	return pos >= 0;
}

void CHmsFixedLineEidt::SetLength(const int &length)
{
	if (length <=0 || m_textLength == length) return;

	m_textLength = length;
	int labelCount = m_pLabelTexts.size();
	if (labelCount < m_textLength)
	{
		for (int i = labelCount; i < m_textLength; i++)
		{
			CHmsLabel *label = CHmsLabel::Create(m_ttfConfig, m_defaltChar);
			label->SetTextColor(m_textColor);
			m_pLabelTexts.pushBack(label);
			this->AddChild(label);
		}

		ContentLayout();
	}
	else if (labelCount > m_textLength)
	{
		for (int i = labelCount-1; i >= m_textLength; i--)
		{
			CHmsLabel *label = m_pLabelTexts.at(i);
			m_pLabelTexts.popBack();
			label->RemoveFromParent();
		}
	}

	m_curentLabel = m_pLabelTexts.at(0);
}

NS_HMS_END
