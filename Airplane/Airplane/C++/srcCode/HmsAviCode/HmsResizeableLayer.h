#pragma once
#include "HmsAviPf.h"
#include "base/HmsTimer.h"


class CHmsResizeableLayer : public HmsAviPf::CHmsNode
{
public:
	CHmsResizeableLayer();
	~CHmsResizeableLayer();

	CREATE_FUNC(CHmsResizeableLayer);

	static void SetActionTime(float fTime){ s_fActionTime = fTime; }

	virtual bool Init() override;

	/* reset size immediately */
	virtual void ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos);

	/* reset size by moving */
	virtual void ResetLayerSizeByAction(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos, bool bRemoveSelfAfterAction = false);

	/* call it when the action finished */
	virtual void ResizeActionFinished(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos);

private:
	bool			m_bRemoveSelfAfterAction;
	static float	s_fActionTime;
};

