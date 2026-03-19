#pragma once

#include "ui/HmsUiStretchButton.h"
#include "display/HmsLineEdit.h"
#include "ui/HmsKeyboard.h"
#include "RoutePlanData/mcduDef.h"

using namespace HmsAviPf;

class CHmsInputAltitude : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	enum ButtonType
	{
		BUTTON_BLOW = 0,
		BUTTON_EQUAL,
		BUTTON_ABOVE,
		BUTTON_BETWEEN
	};
public:
	CHmsInputAltitude();
	~CHmsInputAltitude();

	CREATE_FUNC_BY_SIZE(CHmsInputAltitude)
	virtual bool Init(const HmsAviPf::Size & windowSize);

	void LineEditBlur();
	//传入长度为2的数组
	void ShowDlg(const FmcAltitude *pAltStu, std::function<void(FmcAltitude altStu[2])> func);
	void CloseDlg();
protected:
	virtual bool OnPressed(const Vec2 & posOrigin) override;

private:
	void OnKeyboardCallbackLeft(char c);
	void OnKeyboardCallbackRight(char c);

	void OnBtnClick(ButtonType type);
	void SelectBtn(ButtonType type);
	ButtonType GetButtonSelectState();
	void UpdateLineEdit();
	void GetAltitudeStu();

private:
	RefPtr<CHmsLineEidt> m_pLineEditLeft;
	RefPtr<CHmsLineEidt> m_pLineEditRight;
	RefPtr<CHmsUiStretchButton> m_pBtnBlow;
	RefPtr<CHmsUiStretchButton> m_pBtnEqual;
	RefPtr<CHmsUiStretchButton> m_pBtnAbove;
	RefPtr<CHmsUiStretchButton> m_pBtnBetween;

	FmcAltitude m_altStu[2];
	std::function<void(FmcAltitude altStu[2])> m_callbackFunc;
};

