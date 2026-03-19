#pragma once

#include "ui/HmsListBox.h"
#include "ui/HmsUiStretchButton.h"

class CHmsChartConfigWin : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	enum class ChartCfgPage
	{
		PAGE_CORRIDOR,
		PAGE_ALTITUDE
	};
public:
	CHmsChartConfigWin();
	~CHmsChartConfigWin();

	CREATE_FUNC_BY_SIZE(CHmsChartConfigWin)

	virtual bool Init(const HmsAviPf::Size & windowSize);

	typedef std::function<void(float)> t_CorridorFunc;
	typedef std::function<void(float,float)> t_HazardFunc;

	void SetCorridorCallback(t_CorridorFunc func){ m_corridorCallback = func; }
	void SetHazardCallback(t_HazardFunc func){ m_hazardCallback = func; }

	void ResetValue();

protected:
	virtual bool OnPressed(const Vec2 & posOrigin);

private:
	void InitListBox(const HmsAviPf::Rect & winsowRect);

	void ShowPage(CHmsUiButtonAbstract *pBtn, ChartCfgPage page);

private:
	RefPtr<CHmsListBox> m_pListCorridor;
	RefPtr<CHmsListBox> m_pListAltitude;
	RefPtr<CHmsUiStretchButton> m_pButtonCorridor;
	RefPtr<CHmsUiStretchButton> m_pButtonAltitude;

	t_CorridorFunc m_corridorCallback;
	t_HazardFunc m_hazardCallback;
};