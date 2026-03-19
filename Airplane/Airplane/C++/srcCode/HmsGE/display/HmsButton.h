#pragma once
#include "HmsWidget.h"
#include "HmsStretchImageNode.h"
#include "base/RefPtr.h"
#include "HmsLabel.h"

NS_HMS_BEGIN

class CHmsStretchImageNode;
class CHmsImageNode;

class CHmsButton : public CHmsWidget
{
public:
	CHmsButton();
	~CHmsButton();

	static CHmsButton* Create();

	static CHmsButton* Create(const std::string &normalImage,
							  const std::string &selectedImage);

	static CHmsButton* Create(const std::string &normalImage,
							  const std::string &selectedImage,
							  const onClickCallback& callback);

	static CHmsButton* Create(const std::string &normalImage,
							  const std::string &selectedImage,
							  const std::string &disableImage,
							  const onClickCallback& callback);

	virtual bool Init() override;

	void SetIcon(CHmsImageNode *icon);

	void SetIcon(const std::string iconPath, const Size &iconSize = Size(16, 16));

	void SetText(const std::string text, const float &size = 12.0f, const Color4B &color = Color4B::BLACK);

	std::string GetText();

	void SetTextLabel(CHmsLabel *label);

	void SetTextColor(const Color4B &color);

	virtual void Selected();

	virtual void UnSelected();

	bool IsSelected();

	virtual void SetSize(const Size &size) override;

protected:
	bool m_selected;
	Color4B m_textColor;

	RefPtr<CHmsImageNode>			m_pNodeIcon;
	RefPtr<CHmsLabel>				m_pLabelText;
	RefPtr<CHmsStretchImageNode>	m_pImageNormal;
	RefPtr<CHmsStretchImageNode>	m_pImageSelected;
	RefPtr<CHmsStretchImageNode>	m_pImageDisable;

	bool InitWithNormalImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const onClickCallback& callback);

	bool InitWithCallback(const onClickCallback& callback);

	void SetPressed(bool selected);

	void SetNormalImage(CHmsStretchImageNode* image);

	void SetSelectedImage(CHmsStretchImageNode* image);

	void SetDisabledImage(CHmsStretchImageNode* image);

	bool OnPressed(const Vec2 & posParent) override;

	void OnReleased(const Vec2 & posParent) override;

	virtual void ContentLayout();

	virtual void EnableChanged() override;

private:
	void UpdateImagesVisibility();
	void UpdateContentColor();
};

NS_HMS_END