#pragma once
#include "ui/HmsUiEventInterface.h"
#include "base/HmsNode.h"
#include "HmsFrame2DRoot.h"
#include "HmsResizeableLayer.h"
#include "ToolBar/HmsToolBarFlowBtn.h"
#include "base/HmsScissorNode.h"
#include "base/HmsClippingNode.h"
#include "Nav/ScratchPad/HmsScratchPadClipping.h"

class CHmsNavEarthFrame;
class CHmsFrame2DRoot;

class CHmsNavEarthLayer : public CHmsResizeableLayer, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsNavEarthLayer();
	~CHmsNavEarthLayer();

	CREATE_FUNC_BY_SIZE(CHmsNavEarthLayer);

	virtual bool Init(const HmsAviPf::Size & size);

	void Resize(const HmsAviPf::Size & size, int nLevel);


	virtual void ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos) override;
    /* reset size by moving */
    virtual void ResetLayerSizeByAction(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos, bool bRemoveSelfAfterAction = false) override;
    /* call it when the action finished */
    virtual void ResizeActionFinished(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos) override;

	void SetDisplayAdminArea(bool bShow);
	void SetDisplayAirspace(bool bShow);
	void SetDisplayNearAirport(bool bShow);
    void SetDisplayAftTrack(bool bShow);
    HmsAviPf::CHmsNode * GetTrackLayer();

protected:
	void InitFlowBtns();
	void SwitchMapScr(const std::string & strScr);

private:

	HmsAviPf::RefPtr<CHmsNode>					m_menuBar;
	CHmsNavEarthFrame							* m_pNavEarthFrame;
	CHmsFrame2DRoot								* m_pFrame2DRoot;
	HmsAviPf::HMS_UI_BUTTON_STATE               m_ObsBtnState;
    HmsAviPf::RefPtr<CHmsScratchPadClipping> m_pScissorNode;

    HmsAviPf::RefPtr<CHmsToolBarFlowBtn> m_pAirspaceBtn;
};
