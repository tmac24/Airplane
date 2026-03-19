#pragma once
#include "HmsWidget.h"
#include "HmsStretchImageNode.h"
#include "base/RefPtr.h"
#include "HmsLabel.h"
#include "base/HmsTimer.h"
#include "base/HmsClippingNode.h"

NS_HMS_BEGIN

class CHmsStretchImageNode;
class CHmsImageNode;
class CHmsDrawNode;

class CHmsLineEidt : public CHmsWidget
{
public:
	CHmsLineEidt();
	~CHmsLineEidt();

	static CHmsLineEidt* Create();

	static CHmsLineEidt* Create(const std::string &bgImage);

	static CHmsLineEidt* Create(const std::string &bgImage,
							  const onClickCallback& callback);

	virtual bool Init() override;

	void SetIcon(CHmsImageNode *icon);

	void SetIcon(const std::string iconPath, const Size &iconSize = Size(16, 16));

	void SetText(const std::string &text);

	void AddChar(const char &c);

	void Backspace();

	void SetMarginLeft(const float &marginLeft);

	void SetMarginRight(const float &marginRight);

	std::string GetText();

	void SetTextLabel(CHmsLabel *label);

	void SetTextColor(const Color4B &color);

	virtual void SetSize(const Size &size) override;

protected:
	std::string m_text;
	Color4B m_textColor;
	float m_marginLeft;
	float m_marginRight;

	RefPtr<CHmsImageNode>			m_pNodeIcon;
	RefPtr<CHmsLabel>				m_pLabelText;
	RefPtr<CHmsStretchImageNode>	m_pImageBackground;
	RefPtr<CHmsDrawNode>			m_pNodeCursor;
	RefPtr<CHmsClippingNode>		m_pNodeClip;
	RefPtr<CHmsDrawNode>			m_pNodeStencil;//用于显示文字蒙板,超出部分不显示

	bool InitWithBackgroundImage(const std::string& bgImage, const onClickCallback& callback);

	bool InitWithCallback(const onClickCallback& callback);

	void SetBackgroundImage(CHmsStretchImageNode* image);

	bool OnPressed(const Vec2 & posParent) override;

	void OnReleased(const Vec2 & posParent) override;

	virtual void ContentLayout();

	virtual void Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags) override;

	void TextChanged(const std::string &newStr, const std::string &oldStr);

	virtual void  EnableChanged();

	virtual void FocusChanged();

private:
	float m_cursorWidth;//光标宽度 由字符大小决定
	float m_textHeight;//文字高度
	CHmsTime *m_time;
	double m_lastTime;

	void UpdateContentState();
};

NS_HMS_END