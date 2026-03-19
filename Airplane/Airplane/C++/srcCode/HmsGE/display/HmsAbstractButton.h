#pragma once
#include "HmsWidget.h"
#include "HmsStretchImageNode.h"
#include "base/RefPtr.h"
#include "HmsLabel.h"

NS_HMS_BEGIN

class CHmsStretchImageNode;

class CHmsAbstractButton : public CHmsWidget
{
public:
	CHmsAbstractButton();
	~CHmsAbstractButton();

	static CHmsAbstractButton* Create();

	static CHmsAbstractButton* Create(const std::string &normalImage,
							  const std::string &selectedImage);

	static CHmsAbstractButton* Create(const std::string &normalImage,
							  const std::string &selectedImage,
							  const onClickCallback& callback);

	static CHmsAbstractButton* Create(const std::string &normalImage,
							  const std::string &selectedImage,
							  const std::string &disableImage,
							  const onClickCallback& callback);

	virtual bool Init() override;

	virtual void SetSize(const Size &size) override;

protected:
	RefPtr<CHmsStretchImageNode>	m_pImageNormal;
	RefPtr<CHmsStretchImageNode>	m_pImageSelected;
	RefPtr<CHmsStretchImageNode>	m_pImageDisable;

	bool InitWithNormalImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const onClickCallback& callback);

	bool InitWithCallback(const onClickCallback& callback);

	void SetPressed(bool isPressed);

	void SetNormalImage(CHmsStretchImageNode* image);

	void SetSelectedImage(CHmsStretchImageNode* image);

	void SetDisabledImage(CHmsStretchImageNode* image);

	bool OnPressed(const Vec2 & posParent) override;

	void OnReleased(const Vec2 & posParent) override;

	virtual void EnableChanged() override;

private:
	void UpdateImagesVisibility();
	void UpdateContentColor();
};

NS_HMS_END