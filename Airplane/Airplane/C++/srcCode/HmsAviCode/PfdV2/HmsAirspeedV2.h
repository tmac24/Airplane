#pragma once
#include "HmsBaseTable.h"

class CHmsAirspeedV2 : public CHmsBaseTable
{
public:
	CHmsAirspeedV2();
	~CHmsAirspeedV2();

	CREATE_FUNC(CHmsAirspeedV2)

	static CHmsAirspeedV2* Create(HmsAviPf::Size size);

	virtual bool InitWithSize(HmsAviPf::Size size);

	void SetText(const std::string &text);

	void MetricSwitch(bool bOn);

protected:
	HmsAviPf::CHmsNode* GenFeetFrame(HmsAviPf::Size size, Color4B color);
	HmsAviPf::CHmsNode* GenMarker(HmsAviPf::Size size, Color4B color);
	
	virtual void GenBarClip();
	virtual void UpdateValueAfter(float fValue);
	virtual void BeforeGenUnvalid();

protected:
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>  m_pFeetFrame;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pFeetText;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pShowText;
};

