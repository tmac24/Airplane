#pragma once
#include "ui/HmsUiButtonAbstract.h"
#include "display/HmsStretchImageNode.h"
#include "display/HmsLabel.h"
#include "base/RefPtr.h"
#include "HmsUiEventInterface.h"

NS_HMS_BEGIN

class CHmsImageNode;

class CHmsUiButton : public CHmsNode, public CHmsUiButtonAbstract
{
public:
	CHmsUiButton();
	virtual ~CHmsUiButton();

	static CHmsUiButton* CreateWithImage(const char * normalImage,
		const char * selectedImage,
		const char * disableImage = nullptr,
		const HmsUiBtnCallback& callback = nullptr);
	static CHmsUiButton* CreateWithStretchImage(const Size & size, 
		const char * normalImage,
		const char * selectedImage,
		const char * disableImage = nullptr,
		const HmsUiBtnCallback& callback = nullptr);
	static CHmsUiButton* CreateWithNode(CHmsNode * normalImage,
		CHmsNode * selectedImage,
		CHmsNode * disableImage = nullptr,
		const HmsUiBtnCallback& callback = nullptr);

	virtual CHmsNode * GetStateNode(HMS_UI_BUTTON_STATE eState);

	void ResetTouchSize(const Size & size);

protected:
	bool InitWithNode(CHmsNode * normalImage, CHmsNode * selectedImage, CHmsNode * disableImage);

	virtual void SetState(HMS_UI_BUTTON_STATE eState) override;

	

protected:
	RefPtr<CHmsNode>				m_pNodeNormal;
	RefPtr<CHmsNode>				m_pNodeSelected;
	RefPtr<CHmsNode>				m_pNodeDisable;
};

NS_HMS_END