#pragma once

#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiButton.h"
#include "display/HmsLineEdit.h"
#include "ui/HmsKeyboard.h"
#include "RoutePlanData/mcduDef.h"
#include "HmsNavDataStruct.h"

using namespace HmsAviPf;

class CHmsSearchShape : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsSearchShape();
	~CHmsSearchShape();

	CREATE_FUNC_BY_SIZE(CHmsSearchShape)
	virtual bool Init(const HmsAviPf::Size & windowSize);

	void LineEditBlur();
	//传入长度为2的数组
	void ShowDlg(SearchShapeStu shapeStu, std::function<void(SearchShapeStu)> func);
	void CloseDlg();
	void UpdateAllChildPos(CHmsNode *pNode);
protected:
	virtual bool OnPressed(const Vec2 & posOrigin) override;

private:
	void OnKeyboardCallback(char c);

	void OnBtnClick(SearchShapeType type);
	void SelectBtn(SearchShapeType type);
	SearchShapeType GetButtonSelectState();
	void UpdateLineEdit();
	bool GetAltitudeStu();

private:
	RefPtr<CHmsLineEidt> m_pLineEditRadius;
	RefPtr<CHmsLineEidt> m_pLineEditSpace;
	RefPtr<CHmsLineEidt> m_pLineEditAngle;

	RefPtr<CHmsLineEidt> m_pLineEditWidth;
	RefPtr<CHmsLineEidt> m_pLineEditHeight;
	RefPtr<CHmsLineEidt> m_pLineEditPosInSide;
	//RefPtr<CHmsLineEidt> m_pLineEditSpace;
	//RefPtr<CHmsLineEidt> m_pLineEditAngle;

	RefPtr<CHmsUiButton> m_pBtnNull;
	RefPtr<CHmsUiButton> m_pBtnSpiral;
	RefPtr<CHmsUiButton> m_pBtnRectSpiral;
	RefPtr<CHmsUiButton> m_pBtnRect;

	std::map<CHmsLineEidt*, int> m_mapLineEdit;
	CHmsLineEidt *m_pCurInputLineEdit;

	SearchShapeStu m_shapeStu;
	std::function<void(SearchShapeStu)> m_callbackFunc;
};

