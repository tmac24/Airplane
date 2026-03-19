#pragma once

#include "ui/HmsUiStretchButton.h"
#include "display/HmsLineEdit.h"
#include "ui/HmsKeyboard.h"

using namespace HmsAviPf;

class CHmsHistoryRouteInputName : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsHistoryRouteInputName();
	~CHmsHistoryRouteInputName();

	CREATE_FUNC_BY_SIZE(CHmsHistoryRouteInputName)
	virtual bool Init(const HmsAviPf::Size & windowSize);

	void LineEditBlur();
	void ShowDlg(std::string &name, std::vector<int> vecIndex, std::function<void(std::string)> callback);
	void CloseDlg();
protected:
	virtual bool OnPressed(const Vec2 & posOrigin) override;

private:
	void OnKeyboardCallback(char c);

private:
	RefPtr<CHmsLineEidt> m_pLineEdit;
	std::string m_strName;
	std::vector<int> m_vecIndex;
	std::function<void(std::string)> m_callbackFunc;
};

