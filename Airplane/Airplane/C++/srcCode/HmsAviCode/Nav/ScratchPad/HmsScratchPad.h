#pragma once

#include "base/HmsNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiVerticalScrollView.h"
#include "ui/HmsUiImageButton.h"

#include "HmsScratchPadClipping.h"
#include "HmsScratchPadDraw.h"
#include "HmsScratchPadPreview.h"
#include "HmsScratchPadColorDlg.h"

using namespace HmsAviPf;

enum class ToolBtnEnum
{
	BTN_ADD,
	BTN_DRAW,
	BTN_PENCOLOR,
	BTN_FULLSCREEN,
	BTN_CLEAR,
	BTN_UNDO,
	BTN_REDO,
	BTN_CLOSE,
	BTN_ZOOMIN,
	BTN_ZOOMOUT,
	BTN_EDIT
};

enum class ScratchPadPage
{
	PAGE_DRAW,
	PAGE_PREVIEW
};

class CHmsScratchPad : public CHmsNode, public HmsUiEventInterface
{
public:
	CHmsScratchPad();
	~CHmsScratchPad();

	CREATE_FUNC_BY_SIZE(CHmsScratchPad);

	void SetHalfAndFullSize(Size half, Size full);

	void ShowThisPage(ScratchPadInfoStu stu, ScratchPadPage page);

	void Resize(Size size);

protected:
	virtual bool Init(const HmsAviPf::Size & size);

	void InitRightToolButton();

	void ToolBtnCallback(CHmsUiButtonAbstract *btn, HMS_UI_BUTTON_STATE eState, ToolBtnEnum btnType);

	void ChangeBtnColor(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state);

private:

	void LayoutBySize(Size size);

private:

	RefPtr<CHmsDrawNode> m_bgDrawNode;
	RefPtr<CHmsScratchPadAddDlg> m_addDlg;
	RefPtr<CHmsScratchPadColorDlg> m_colorDlg;

	RefPtr<CHmsScratchPadClipping> m_scratchPadClipping;
	RefPtr<CHmsScratchPadDraw> m_scratchPadDraw;
	RefPtr<CHmsUiPanel> m_rightToolPanel;
	RefPtr<CHmsUiPanel> m_rightToolPreview;

	RefPtr<CHmsScratchPadPreview> m_preview;

	Size m_halfFullSize;
	Size m_fullSize;

	RefPtr<CHmsUiStretchButton> m_pClearAllButton;
};