#pragma once
#include "base/RefPtr.h"
#include "display/HmsStretchImageNode.h"
#include "display/HmsImageNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiButtonAbstract.h"
#include "HmsNavEarthFrame.h"
#include "HmsNavEarthLayer.h"

#include "HmsNavInfoWindow.h"
#include "ScratchPad/HmsScratchPad.h"

namespace HmsAviPf
{
	class CHmsUiButton;
	enum class HMS_UI_BUTTON_STATE;
}

enum class NavMenuBarBtn
{
	Far,
	Near,
	Airport,
	ScratchPad,
	FPL,
	AftCenter,
	MapsBar,
	MapSatellite,
	MapVFR,
	MapIFRLow,
	MapIFRHigh,
	MapBack,
	North
};
class CHmsNavMenuButton;
class CHmsNavMenuBar : public HmsAviPf::CHmsImageNode, public HmsAviPf::HmsUiEventInterface
{
public:
	static CHmsNavMenuBar * Create(CHmsNavEarthLayer *parent, const HmsAviPf::Size & size, float fBorder = 1.0f);

	CHmsNavMenuBar();

	virtual bool Init(CHmsNavEarthLayer *parent, const HmsAviPf::Rect & rect);

	void Resize(const HmsAviPf::Size & size, int nLevel, float fBorder);

	void SetBarSize(const HmsAviPf::Size & size);

	void RegisterEarthFrame(CHmsNavEarthFrame * earthFrame){ m_navEarthFrame = earthFrame; }
	void RegisterNavInfoWindow(CHmsNavInfoWindow * infoWin){ m_navInfoWindow = infoWin; }

	void RegisterScratchPad(CHmsScratchPad *scratchPad){ m_scratchPad = scratchPad; }

	CHmsNavEarthFrame * GetRegisterEarthFrame(){ return m_navEarthFrame.get(); }

	void SetCenterBtnSelect(bool bSelect);

protected:
	void OnBarStateChange(HmsAviPf::CHmsUiButtonAbstract*, HmsAviPf::HMS_UI_BUTTON_STATE eState, NavMenuBarBtn eBtn);
	void SwitchMapScr(const std::string & strScr);

private:
	HmsAviPf::RefPtr<CHmsNavEarthFrame>			m_navEarthFrame;
	HmsAviPf::RefPtr<CHmsNavInfoWindow>			m_navInfoWindow;
	HmsAviPf::RefPtr<CHmsNode>					m_nodeMapsBar;

	HmsAviPf::RefPtr<CHmsScratchPad> m_scratchPad;

	HmsAviPf::RefPtr<CHmsNavMenuButton> m_pCenterBtn;
};


