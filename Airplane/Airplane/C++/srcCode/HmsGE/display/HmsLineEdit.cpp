#include "HmsLineEdit.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "HmsImageNode.h"
#include "HmsDrawNode.h"


NS_HMS_BEGIN

CHmsLineEidt::CHmsLineEidt()
: m_textColor(0x00, 0x00, 0x00, 0xFF)
, m_cursorWidth(0.0)
, m_lastTime(0.0)
, m_marginLeft(6.0f)
, m_marginRight(6.0f)
{
	m_time = new CHmsTime;
}

CHmsLineEidt::~CHmsLineEidt()
{
	if (m_time)
	{
		delete m_time;
		m_time = nullptr;
	}
}

CHmsLineEidt* CHmsLineEidt::Create()
{
	CHmsLineEidt *ret = new CHmsLineEidt();
	if (ret && ret->Init())
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsLineEidt* CHmsLineEidt::Create(const std::string &bgImage)
{
	return Create(bgImage, (const onClickCallback&)nullptr);
}

CHmsLineEidt* CHmsLineEidt::Create(const std::string &bgImage, const onClickCallback& callback)
{
	CHmsLineEidt *ret = new STD_NOTHROW CHmsLineEidt();
	if (ret && ret->InitWithBackgroundImage(bgImage, callback))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsLineEidt::Init()
{
	return InitWithBackgroundImage("res/display/lineedit.png", (const onClickCallback&)nullptr);
}

bool CHmsLineEidt::InitWithCallback(const onClickCallback& callback)
{
	SetAnchorPoint(Vec2(0.5f, 0.5f));
	onClicked = callback;
	m_enabled = true;
	return true;
}

bool CHmsLineEidt::InitWithBackgroundImage(const std::string& bgImage, const onClickCallback& callback)
{
	if (!CHmsWidget::Init())
	{
		return false;
	}

	CHmsStretchImageNode *bgImageNode = nullptr;

	if (bgImage.size() > 0)
	{
		bgImageNode = CHmsStretchImageNode::Create(bgImage);
	}

	m_pNodeClip = CHmsClippingNode::create();
	m_pNodeStencil = CHmsDrawNode::Create();
	m_pNodeClip->setStencil(m_pNodeStencil);

	InitWithCallback(callback);
	SetBackgroundImage(bgImageNode);

	m_pNodeCursor = CHmsDrawNode::Create();
	m_pNodeCursor->SetVisible(false);

	auto label = CHmsLabel::createWithTTF("", "fonts/msyh.ttc", 16);
	if (label)
	{
		label->SetTextColor(m_textColor);
		SetTextLabel(label);
	}

	m_pNodeClip->AddChild(m_pNodeCursor);

	this->AddChild(m_pNodeClip);

	if (m_pImageBackground)
	{
		this->SetSize(Size(80, 30));
	}

	this->SetScheduleUpdate();

	return true;
}

void CHmsLineEidt::SetIcon(CHmsImageNode *icon)
{
	if (icon != m_pNodeIcon)
	{
		if (icon)
		{
			this->AddChild(icon);
		}

		if (m_pNodeIcon)
		{
			this->RemoveChild(m_pNodeIcon, true);
		}

		m_pNodeIcon = icon;

		if (!m_enabled) m_pNodeIcon->SetColor(Color3B::GRAY);
	}

	ContentLayout();
}

void CHmsLineEidt::SetIcon(const std::string iconPath, const Size &iconSize /*= Size(16, 16)*/)
{
	auto icon = CHmsImageNode::Create(iconPath);
	float width = icon->GetContentSize().width;
	float height = icon->GetContentSize().height;
	icon->SetScale(iconSize.width / width, iconSize.height / height);
	auto size = icon->GetContentSize();
	SetIcon(icon);
}

void CHmsLineEidt::SetSize(const Size &size)
{
	CHmsWidget::SetSize(size);
	if (m_pImageBackground)
	{
		m_pImageBackground->SetContentSize(size);
	}
	
	ContentLayout();
}

void CHmsLineEidt::SetText(const std::string &text)
{
	m_pLabelText->SetString(text);

    TextChanged(text, m_text);
	m_text = text;
}


void CHmsLineEidt::AddChar(const char &c)
{
	std::string newStr = m_text + c;
	SetText(newStr);
}

void CHmsLineEidt::Backspace()
{
	if (m_text.length() > 0)
	{
		std::string newStr = m_text.substr(0, m_text.length() - 1);

		SetText(newStr);
	}
}

void CHmsLineEidt::SetMarginLeft(const float &marginLeft)
{
	m_marginLeft = marginLeft;

	ContentLayout();
}

void CHmsLineEidt::SetMarginRight(const float &marginRight)
{
	m_marginRight = marginRight;

	ContentLayout();
}

std::string CHmsLineEidt::GetText()
{
	return m_text;
}

void CHmsLineEidt::SetTextColor(const Color4B &color)
{
	m_textColor = color;

	m_pLabelText->SetTextColor(color);
}

void CHmsLineEidt::SetTextLabel(CHmsLabel *label)
{
	if (label != m_pLabelText)
	{
		if (label)
		{
			m_pNodeClip->AddChild(label);
		}

		if (m_pLabelText)
		{
			m_pNodeClip->RemoveChild(m_pLabelText, true);
		}

		m_pLabelText = label;
		m_textColor = Color4B(label->GetColor());
		
		//获取光标宽度
		auto refLabel = CHmsLabel::Create(label->GetTTFConfig(), "MMM");
		if (refLabel)
		{
			m_cursorWidth = refLabel->GetContentSize().width / 3.0;
			m_textHeight = refLabel->GetContentSize().height;
		}

		if (!m_enabled) m_pLabelText->SetTextColor(Color4B::GRAY);

	}

	ContentLayout();
}

void CHmsLineEidt::SetBackgroundImage(CHmsStretchImageNode* image)
{
	if (image != m_pImageBackground)
	{
		if (image)
		{
			this->AddChild(image);
			image->SetAnchorPoint(Vec2(0, 0));
		}

		if (m_pImageBackground)
		{
			this->RemoveChild(m_pImageBackground, true);
		}

		m_pImageBackground = image;
		this->UpdateContentState();
	}
}

bool CHmsLineEidt::OnPressed(const Vec2 & posParent)
{
	if (m_enabled)
	{
	}

	return true;
}

void CHmsLineEidt::OnReleased(const Vec2 & posParent)
{
	if (m_enabled)
	{
		if (onClicked)
		{
			onClicked(this);
		}
	}
}

void CHmsLineEidt::UpdateContentState()
{
	if (m_enabled)
	{
		if (m_pImageBackground)		m_pImageBackground->SetColor(Color3B::WHITE);
		if (m_pNodeIcon)			m_pNodeIcon->SetColor(Color3B::WHITE);
		if (m_pLabelText)			m_pLabelText->SetTextColor(Color4B::WHITE);
		if (m_pNodeCursor)			m_pNodeCursor->SetVisible(true);
	}
	else
	{
		if (m_pImageBackground)	m_pImageBackground->SetColor(Color3B::GRAY);
		if (m_pNodeIcon)		m_pNodeIcon->SetColor(Color3B::GRAY);
		if (m_pLabelText)		m_pLabelText->SetTextColor(Color4B::GRAY);
		if (m_pNodeCursor)		m_pNodeCursor->SetVisible(false);
	}
}

void CHmsLineEidt::ContentLayout()
{
	auto height = this->GetContentSize().height;
	auto width = this->GetContentSize().width;
	if (m_pNodeIcon != nullptr)
	{
		m_pNodeIcon->SetAnchorPoint(Vec2(0, 0.5));
		m_pNodeIcon->SetPosition(6.0, height / 2.0);
	}

	if (m_pNodeStencil)
	{
		m_pNodeStencil->clear();
		m_pNodeStencil->DrawSolidRect(m_marginLeft, 0, width - m_marginRight, height);
	}

	if (m_pLabelText != nullptr)
	{
		m_pLabelText->SetAnchorPoint(0, 0.5);
		m_pLabelText->SetPosition(m_marginLeft, height / 2.0);
	}

	if (m_pNodeCursor)
	{
		m_pNodeCursor->clear();
		m_pNodeCursor->DrawHorizontalLine(0, m_cursorWidth, 0, 2.0, Color4F::WHITE);
		m_pNodeCursor->SetPosition(m_pLabelText->GetPoistionX() + m_pLabelText->GetContentSize().width, (this->GetContentSize().height - m_textHeight) / 2.0);
	}
}

void CHmsLineEidt::Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags)
{
	if (m_enabled && m_focus)
	{
		auto temp = m_time->GetTime();
		if (temp - m_lastTime > 0.5)
		{
			if (!m_pNodeCursor->IsVisible())
			{
				m_pNodeCursor->SetVisible(true);
			}
			else
			{
				m_pNodeCursor->SetVisible(false);
			}

			m_lastTime = temp;
		}
	}
}

void CHmsLineEidt::TextChanged(const std::string &newStr, const std::string &oldStr)
{
	float labelWidth = m_pLabelText->GetContentSize().width;

    if (newStr.length() > 0)
    {
        if (newStr.at(newStr.length() - 1) == 0x20)
        {
            labelWidth += 10;
        }
    }

	float maxLabelWidth = this->GetContentSize().width - m_marginLeft - m_marginRight;
	float nextLabelWidth = labelWidth + m_cursorWidth;
	
	float posX = m_pLabelText->GetPoistionX();
	if (nextLabelWidth > maxLabelWidth)
	{
		float beyondPart = nextLabelWidth - maxLabelWidth;
		posX = m_marginLeft - beyondPart;
		m_pLabelText->SetPositionX(posX);
	}
	else
	{
		if (posX != m_marginLeft)
		{
			posX = m_marginLeft;
			m_pLabelText->SetPositionX(posX);
		}
	}

	m_pNodeCursor->SetPositionX(posX + labelWidth);
}

void CHmsLineEidt::EnableChanged()
{
	UpdateContentState();
}

void CHmsLineEidt::FocusChanged()
{
	if (!m_focus)
	{
		if (m_pNodeCursor)
		{
			m_pNodeCursor->SetVisible(false);
		}
	}
}

NS_HMS_END