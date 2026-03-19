#pragma once
#include "HmsScreenLayout.h"
#include "ui/HmsUiEventInterface.h"

class CHmsLayerNav : public CHmsScreenLayout, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsLayerNav();
	~CHmsLayerNav();

	CREATE_FUNC(CHmsLayerNav);

	virtual bool Init() override;

	virtual void OnEnter() override;

	virtual void OnExit() override;

	virtual void Update(float delta) override;

	virtual void ResetViewer() override;

	virtual void OnLayoutBtnChange(int nLayoutBtnIndex, int nBtnState);

	virtual void OnScreenLayoutChanged(Hms_Screen_Layout_Type eOld, Hms_Screen_Layout_Type eNew);

protected:
	void Test();

};

