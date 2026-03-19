#pragma once
#include "HmsBaseTable.h"

class CHmsAltmeterV2 : public CHmsBaseTable
{
public:
	CHmsAltmeterV2();
	~CHmsAltmeterV2();

	CREATE_FUNC(CHmsAltmeterV2)

	static CHmsAltmeterV2* Create(HmsAviPf::Size size);

	virtual bool InitWithSize(HmsAviPf::Size size);

	void SetText(const std::string &text);

	void MetricSwitch(bool bOn);

protected:
	virtual void GenDecorate();
	virtual void GenPointer();
	virtual void GenTableBar();
	virtual void GenRollText();
	virtual HmsAviPf::CHmsNode* GenTextTick(bool bNeedText, bool bLong);
	virtual HmsAviPf::CHmsNode* GenMarker(HmsAviPf::Size size, Color4B color);

	virtual void GenBarClip();
	virtual void UpdateValueAfter(float fValue);
	virtual void BeforeGenUnvalid();

	HmsAviPf::CHmsNode* GenFeetFrame(HmsAviPf::Size size, Color4B color);

protected:
	float m_TextLineOffset;
	float m_TickTextSize;

	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>  m_pFeetFrame;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pFeetText;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pShowText;
};

