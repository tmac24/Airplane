#pragma once

#include "base/HmsNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiVerticalScrollView.h"
#include "ui/HmsUiImageButton.h"
#include "ui/HmsUiStretchButton.h"

#include "HmsScratchPadClipping.h"
#include "HmsScratchPadDraw.h"
#include "HmsScratchPadSql.h"

#include <map>
#include "base/HmsVector.h"

using namespace HmsAviPf;

class CHmsScratchPadPreview : public CHmsNode, public HmsUiEventInterface
{
public:
	CHmsScratchPadPreview();
	~CHmsScratchPadPreview();

	CREATE_FUNC_BY_SIZE(CHmsScratchPadPreview);

	void SetScratchPadInfoStu(const std::vector<ScratchPadInfoStu> &vecInfoStu);

	void LayoutBySize(Size s);

	typedef std::function<void(ScratchPadInfoStu stu)> t_callbackFunc;
	void SetCallbackFunc(t_callbackFunc func);

	typedef std::function<void()> t_callbackFuncOnDel;
	void SetCallbackFuncOnDel(t_callbackFuncOnDel func);

	void SetShowDelButton(bool b);
	void ToggleShowDelButton();

protected:
	virtual bool Init(const HmsAviPf::Size & size);

private:

	void DetectNeedCreateOrNot(int dstCount);

	void OnDelButtonClick(CHmsFixSizeUiImageButton *pBtn);

private:
	RefPtr<CHmsDrawNode> m_bgDrawNode;
	RefPtr<CHmsScratchPadClipping> m_scratchPadClipping;
	RefPtr<CHmsUiPanel> m_previewPanel;

	HmsAviPf::Vector<CHmsScratchPadDraw*> m_padDrawVector;

	std::map<CHmsFixSizeUiImageButton*, int> m_buttonMap;

	t_callbackFunc m_callbackFunc;
	t_callbackFuncOnDel m_callbackFuncOnDel;

	const float c_scale;
	const float c_margin;

	int m_imageLocalZOrder;
	int m_buttonLocalZOrder;

	bool m_bShowDelButton;

	int m_curPanelCount;
};

