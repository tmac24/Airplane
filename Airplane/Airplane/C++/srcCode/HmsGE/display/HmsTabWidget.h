#pragma once
#include "HmsWidget.h"
#include "base/RefPtr.h"
#include "HmsImageNode.h"
#include "HmsAbstractButton.h"
#include "HmsDrawNode.h"

NS_HMS_BEGIN

class CHmsTabButton : public CHmsAbstractButton
{
public:
	typedef std::function<void(CHmsTabButton*, bool)> onSelectedChangedCallback;

	CHmsTabButton()
		:m_normalFontSize(16)
		, m_selectedFontSize(18)
		, m_isSelected(false)
		, m_SelectedColor(0x00, 0x00, 0x00, 0xFF)
		, m_UnSelectedColor(0x33, 0x33, 0x33, 0xFF)
		, OnSelectedChanged(nullptr)
	{
	};
	~CHmsTabButton(){};

	CREATE_FUNC(CHmsTabButton);

	virtual bool Init() override
	{
		if (!CHmsAbstractButton::Init())
		{
			return false;
		}

		m_pDrawButton = CHmsDrawNode::Create();

		m_pLabelText = CHmsLabel::createWithTTF("", "fonts/msyh.ttc", m_normalFontSize);
		m_ttfConfig = m_pLabelText->GetTTFConfig();

		this->AddChild(m_pDrawButton);
		this->AddChild(m_pLabelText);

		return true;
	}

	void SetSelectedColor(const Color4B &color)
	{
		m_SelectedColor = color;
	}

	void SetUnSelectedColor(const Color4B &color)
	{
		m_UnSelectedColor = color;
	}

	virtual void SetSize(const Size &size) override
	{
		CHmsAbstractButton::SetSize(size);

		ContentLayout();
	}

	void SetText(const std::string &text)
	{
		if (m_pLabelText) m_pLabelText->SetString(text);
	}

	void SetSelected(bool isSelected)
	{
		m_isSelected = isSelected;

		if (m_pRelatedWidget)
		{
			m_pRelatedWidget->SetVisible(m_isSelected);
		}
		UpdateButton();
	}

	bool IsSelected()
	{
		return m_isSelected;
	}

	void SetRelatedWidget(CHmsWidget *widget)
	{
		m_pRelatedWidget = widget;
	}

	onSelectedChangedCallback OnSelectedChanged;
protected:
	void ContentLayout()
	{
		Size size = this->GetContentSize();
		m_pLabelText->SetPosition(size.width / 2.0, size.height / 2.0);

		UpdateButton();
	}

	void UpdateButton()
	{
		Size size = this->GetContentSize();
		Color4F color = m_isSelected ? Color4F(m_SelectedColor) : Color4F(m_UnSelectedColor);
		float fontSize = m_isSelected ? m_selectedFontSize : m_normalFontSize;

		m_pDrawButton->clear();
		m_pDrawButton->DrawSolidRect(0, 0, size.width, size.height, color);

		m_ttfConfig.fontSize = fontSize;
		m_pLabelText->SetTTFConfig(m_ttfConfig);
	}

	virtual void OnReleased(const Vec2 & posParent) override
	{
		if (!m_isSelected)
		{
			SetSelected(true);

			if (OnSelectedChanged)
			{
				OnSelectedChanged(this, true);
			}
		}
	}

private:
	bool m_isSelected;
	float m_normalFontSize;
	float m_selectedFontSize;
	Color4B m_SelectedColor;
	Color4B m_UnSelectedColor;
	TTFConfig m_ttfConfig;

	RefPtr<CHmsDrawNode>	m_pDrawButton;
	RefPtr<CHmsLabel>		m_pLabelText;
	RefPtr<CHmsWidget>		m_pRelatedWidget;
};

class CHmsTabBar : public CHmsWidget
{
public:
	enum Orientation{Horizontal, Vertical};

	CHmsTabBar()
		:m_buttonHeight(90)
		, m_orientation(Vertical)
		, m_currentIndex(-1)
	{

	}

	~CHmsTabBar(){}

	CREATE_FUNC(CHmsTabBar);

	virtual bool Init() override
	{
		if (!CHmsWidget::Init())
		{
			return false;
		}

		return true;
	}

	virtual void SetSize(const Size &size) override
	{
		CHmsWidget::SetSize(size);

		if (m_orientation == Vertical)
		{
			m_buttonWidth = size.width;
			m_buttonHeight = 90;
		}
		else
		{
			m_buttonWidth = 90;
			m_buttonHeight = size.height;
		}
		

		ContentLayout();
	}

	void SetOrientation(Orientation ori)
	{
		m_orientation = ori;

		ContentLayout();
	}

	CHmsTabButton* AddTabButton(const std::string &text)
	{
		CHmsTabButton *button = CHmsTabButton::Create();
		button->SetText(text);
		button->SetSize(Size(m_buttonWidth, m_buttonHeight));
		button->RegisterTouchEvent();
		button->OnSelectedChanged = HMS_CALLBACK_2(CHmsTabBar::OnSelectedChanged, this);
		this->AddChild(button);

		m_tabButtons.pushBack(button);

		ContentLayout();

		return button;
	}

	bool RemoveTabButton(const int &index)
	{
		if (index < 0 || index > m_tabButtons.size())
		{
			return false;
		}

		CHmsTabButton *node = m_tabButtons.at(index);
		this->RemoveChild(node);
		m_tabButtons.erase(index);

		if (m_tabButtons.size() == 0)
		{
			m_currentIndex = -1;
			return true;
		}

		ContentLayout();

		return true;
	}

	void ContentLayout()
	{
		float x = 0.0;
		float y = this->GetContentSize().height;
		for (CHmsTabButton *node : m_tabButtons)
		{
			CHmsTabButton * button = node;

			button->SetSize(Size(m_buttonWidth, m_buttonHeight));

			if (m_orientation == Horizontal)
			{
				button->SetAnchorPoint(0, 0);
				button->SetPosition(x, y);
				x += m_buttonWidth;
			}

			if (m_orientation == Vertical)
			{
				button->SetAnchorPoint(0, 1);
				button->SetPosition(x, y);
				y -= m_buttonHeight;
			}	
		}
	}

	int CurrentIndex()
	{
		return m_currentIndex;
	}

	void SetCurrentIndex(int index)
	{
		if (index < 0 || index > m_tabButtons.size())
		{
			return;
		}

		CHmsTabButton *button = m_tabButtons.at(index);
		button->SetSelected(true);
		OnSelectedChanged(button, true);
		m_currentIndex = index;
	}

private:
	float m_buttonWidth;
	float m_buttonHeight;
	Orientation m_orientation;
	int m_currentIndex;
	Vector<CHmsTabButton *> m_tabButtons;

	void OnSelectedChanged(CHmsTabButton* sender, bool state)
	{
		if (!state) return;

		for (CHmsTabButton *button : m_tabButtons)
		{
			if (button != sender)
			{
				button->SetSelected(false);
			}
		}
	}
};

class CHmsTabWidget : public CHmsWidget
{
public:
	enum TabPosistion{ North, South, West, East };
	struct Margin
	{
		Margin(float _left, float _right, float _top, float _bottom)
		{
			left = _left;
			right = _right;
			top = _top;
			bottom = _bottom;
		}
		float left;
		float right;
		float top;
		float bottom;
	};

	CHmsTabWidget();
	~CHmsTabWidget();

	CREATE_FUNC(CHmsTabWidget)

	virtual bool Init() override;

	virtual void SetSize(const Size &size) override;

	void SetTabPosition(TabPosistion tabpos);

	Size GetContainerSize();

	void SetContainerMargin(Margin margin);

	int AddTab(CHmsWidget *widget, const std::string &label);
	//int AddTab(CHmsWidget *widget, const CHmsImageNode& icon, const std::string &label);

	bool RemoveTab(int index);

	int CurrentIndex();

	void SetCurrentIndex(int index);
protected:
	TabPosistion m_tabPosition;
	Margin		 m_containerMargin;

	RefPtr<CHmsTabBar>		m_pTabbar;
	RefPtr<CHmsDrawNode>	m_pContainer;
	
	void ContentLayout();

private:
	void AjustContainerWidgets();
	void AjustWidget(CHmsWidget *widget);
};

NS_HMS_END