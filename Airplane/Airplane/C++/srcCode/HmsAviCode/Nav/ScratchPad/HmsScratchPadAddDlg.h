#pragma once

#include "ui/HmsListBox.h"
#include "ui/HmsUiStretchButton.h"

enum class ScratchPadBgType
{
	TYPE_PIREP = 5,
	TYPE_ATIS = 10,
	TYPE_CRAFT = 15,
	TYPE_BLANK = 20
};

class CHmsScratchPadAddDlg : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsScratchPadAddDlg();
	~CHmsScratchPadAddDlg();

    static CHmsScratchPadAddDlg* Create(const HmsAviPf::Size & size, float trianglePosY = 550);

    virtual bool Init(const HmsAviPf::Size & windowSize, float trianglePosY);

	typedef std::function<void(ScratchPadBgType)> t_callbackFunc;

	void SetCallback(t_callbackFunc func){ m_callback = func; }

private:
	void InitListBox(const HmsAviPf::Rect & winsowRect);

private:

	t_callbackFunc m_callback;
};
