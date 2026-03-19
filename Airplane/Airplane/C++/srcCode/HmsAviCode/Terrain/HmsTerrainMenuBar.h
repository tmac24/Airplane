#pragma once
#include "base/RefPtr.h"
#include "display/HmsStretchImageNode.h"
#include "display/HmsImageNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiButtonAbstract.h"
#include "ui/HmsUiButtonsManger.h"
#include "../HmsScreenLayer.h"

namespace HmsAviPf
{
	class CHmsUiButton;
	enum class HMS_UI_BUTTON_STATE;
}

enum class TerrainMenuBarBtn
{
	CompassArc,
	CompassHide,
	MapNear,
	MapFar,
};

class CHmsLayerTaws;

class CHmsTerrainMenuBar : public HmsAviPf::CHmsImageNode, public HmsAviPf::HmsUiEventInterface
{
public:
	static CHmsTerrainMenuBar * Create(CHmsScreenLayout *parent, const HmsAviPf::Size & size, float fBorder = 1.0f);

	CHmsTerrainMenuBar();

	virtual bool Init(CHmsScreenLayout *parent, const HmsAviPf::Rect & rect);

	void SetBarSize(const HmsAviPf::Size & size);

	void SetBindData(CHmsLayerTaws * pLayer){ m_pLayerTaws = pLayer; }

	HmsAviPf::CHmsUiButtonsManger * GetBtnManger(){ return &m_btnMgr; }
protected:
	void OnBarStateChange(HmsAviPf::CHmsUiButtonAbstract*, HmsAviPf::HMS_UI_BUTTON_STATE eState, TerrainMenuBarBtn eBtn);
	void SwitchMapScr(const std::string & strScr);

private:
	HmsAviPf::RefPtr<CHmsNode>					m_nodeMapsBar;
	HmsAviPf::CHmsUiButtonsManger				m_btnMgr;
	CHmsLayerTaws  *							m_pLayerTaws;
};


