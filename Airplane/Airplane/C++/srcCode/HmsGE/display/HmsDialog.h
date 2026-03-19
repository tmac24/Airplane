#pragma once
#include "HmsStretchImageNode.h"
#include "HmsImageNode.h"
#include "HmsLabel.h"
#include "../ui/HmsUiEventInterface.h"
#include "../ui/HmsUiImagePanel.h"

NS_HMS_BEGIN

class CHmsDialog : public CHmsUiPanel//public CHmsNode, public HmsUiEventInterface
{
public:
	CHmsDialog();
	virtual ~CHmsDialog();

	CREATE_FUNC(CHmsDialog);

	static CHmsDialog* Create(int width, int height);

	virtual bool Init() override;

	void SetTitle(const std::string &title);

	std::string GetTitle();

	float GetTitleHeight();
	//获取除标题栏以外的界面高度
	float GetContentsHeigth();

	void SetTitleBarColor(const Color4B &color);

protected:
	virtual bool InitWithSize(const Size &size);

	RefPtr<CHmsStretchImageNode>	m_pImageBackground;
	RefPtr<CHmsStretchImageNode>	m_pImageTitlebar;
	RefPtr<CHmsLabel>				m_pLabelTitle;

private:
	float m_titleBarHeigth;
};

NS_HMS_END