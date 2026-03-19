#pragma once
#include "HmsAltmeterV2.h"
#include "display/HmsLabel.h"

class CHmsVerticalV2 : public CHmsAltmeterV2
{
public:
	CHmsVerticalV2();
	~CHmsVerticalV2();

	CREATE_FUNC(CHmsVerticalV2)

	static CHmsVerticalV2* Create(HmsAviPf::Size size);

	virtual bool InitWithSize(HmsAviPf::Size size);

protected:
	virtual void GenTableBar();
	virtual void GenRollText();
	virtual void UpdateValueAfter(float fValue);
	virtual HmsAviPf::CHmsNode* GenMarker(HmsAviPf::Size size, Color4B color);

	virtual void GenBarClip();
private:
	HmsAviPf::CHmsLabel *m_pValue;
};

